#include <cmath>
#include "include/GPoint.h"
#include "include/GBitmap.h"

typedef struct Edge{
    float m;
    float b;
    int top;
    int bottom;
}Edge;

inline bool clipY(GPoint& p0, GPoint& p1, float m, float b, const GBitmap& bm) {
    /*
     * Clipping with the bitmap boarder
     */
    if (p0.y > p1.y) std::swap(p0, p1);
    if (p1.y < 0) return false;
    if (p0.y < 0) {
        p0.x = b;
        p0.y = 0;
    }
    if (p0.y > bm.height()) return false;
    if (p1.y > bm.height()) {
        p1.x = m * bm.height() + b;
        p1.y = bm.height();
    }
    return true;
}

inline void clipX(GPoint& p0, GPoint& p1, float m, float b, std::vector<Edge>& edges, const GBitmap& bm) {
    /*
     * Clipping with the bitmap boarder
     */
    if (p0.x > p1.x) std::swap(p0, p1);
    if (p1.x < 0) {
        edges.push_back({0, 0,
                         static_cast<int>(std::min(p0.y, p1.y)),
                         static_cast<int>(std::max(p0.y, p1.y))});
        return;
    }

    if (p0.x < 0) {
        float newY = - b / m;
        edges.push_back({0,
                         0,
                         static_cast<int>(std::min(p0.y, newY)),
                         static_cast<int>(std::max(p0.y, newY))});
        p0.x = 0;
        p0.y = newY;
    }

    if(p0.x > bm.width()) {
        edges.push_back({0,
                         static_cast<float>(bm.width()),
                         static_cast<int>(std::min(p0.y, p1.y)),
                         static_cast<int>(std::max(p0.y, p1.y))});
        return;
    }

    if (p1.x > bm.width()) {
        float newY = (bm.width() - b) / m;
        edges.push_back({0,
                         static_cast<float>(bm.width()),
                         static_cast<int>(std::min(p1.y, newY)),
                         static_cast<int>(std::max(p1.y, newY))});
        p1.x = bm.width();
        p1.y = newY;
    }

    if (p0.x >= 0 && p1.x <= bm.width()) {
        edges.push_back({m, b,
                         static_cast<int>(std::min(p0.y, p1.y)),
                         static_cast<int>(std::max(p0.y, p1.y))});
    }
}

inline bool compareEdges(const Edge& e1, const Edge& e2) {
    if (e1.top > e2.top) return false;
    if (e1.top < e2.top) return true;
    return e1.bottom < e2.bottom;
}

inline void sortEdges(std::vector<Edge>& edges) {
    std::sort(edges.begin(), edges.end(), compareEdges);
}

inline std::vector<Edge> makeEdges(const GPoint* points, int count, const GBitmap& bm) {
    std::vector<Edge> edges;
    for (int i = 0; i < count; ++i) {
        GPoint current = points[i];
        GPoint next = points[(i + 1) % count];

        float top = std::min(current.y, next.y);
        float bottom = std::max(current.y, next.y);

        if (top < bottom) {
            float m = (next.x - current.x) / (next.y - current.y);
            float b = current.x - m * current.y;

            if (clipY(current, next, m, b, bm))
                clipX(current, next, m, b,edges, bm);
        }
    }
    sortEdges(edges);
    return edges;
}
