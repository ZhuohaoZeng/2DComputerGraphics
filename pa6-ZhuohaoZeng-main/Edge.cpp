#include "edge.h"

bool clipY(GPoint& p0, GPoint& p1, float m, float b, const GBitmap& bm) {
    /*
     * Clipping with the bitmap boarder
     */
    if (p0.y > p1.y) std::swap(p0, p1);

    if (p1.y <= 0) return false;

    if (p0.y < 0) {
        p0.x = b;
        p0.y = 0;
    }

    if (p0.y >= bm.height()) return false;

    if (p1.y > bm.height()) {
        p1.x = m * bm.height() + b;
        p1.y = bm.height();
    }

    return true;
}

void clipX(GPoint& p0, GPoint& p1, float m, float b, int w, std::vector<Edge>& edges, const GBitmap& bm) {
     /*
     * Clipping with the bitmap boarder
     */
    if (p0.x > p1.x) std::swap(p0, p1);
    if (p1.x < 0) {
        int top = GRoundToInt(std::min(p0.y, p1.y));
        int bottom = GRoundToInt(std::max(p0.y, p1.y));
        if (top < bottom)
            edges.push_back({0, 0,
                             top,
                             bottom,
                             0,
                             w});
        return;
    }

    if (p0.x < 0) {
        float newY = - b / m;
        int top = GRoundToInt(std::min(p0.y, newY));
        int bottom = GRoundToInt(std::max(p0.y, newY));
        if (top < bottom) {
            edges.push_back({0,
                             0,
                             top,
                             bottom,
                             0,
                             w});
        }
        p0.x = 0;
        p0.y = newY;
    }

    if(p0.x > bm.width()) {
        int top = GRoundToInt(std::min(p0.y, p1.y));
        int bottom = GRoundToInt(std::max(p0.y, p1.y));
        if (top < bottom){
            edges.push_back({0,
                             static_cast<float>(bm.width()),
                             top,
                             bottom,
                             bm.width(),
                             w});
            return;
        }
    } 

    if (p1.x > bm.width()) {
        float newY = (bm.width() - b) / m;
        int top = GRoundToInt(std::min(p1.y, newY));
        int bottom = GRoundToInt(std::max(p1.y, newY));
        if (top < bottom){
            edges.push_back({0,
                             static_cast<float>(bm.width()),
                             top,
                             bottom,
                             bm.width(),
                             w});
        }
        p1.x = bm.width();
        p1.y = newY;
    }

    if (p0.x >= 0 && p1.x <= bm.width()) {
        int top = GRoundToInt(std::min(p0.y, p1.y));
        int bottom = GRoundToInt(std::max(p0.y, p1.y));
        if (top < bottom)
            edges.push_back({m, b,
                             top,
                             bottom,
                             static_cast<int>(std::min(p0.x,p1.x)),
                             w});
    }
}

std::vector<Edge> makeEdges(const GPoint* points, int count, const GBitmap& bm) {
    if (count < 2) return {};
    std::vector<Edge> edges;
    for (int i = 0; i < count; ++i) {
        GPoint current = points[i];
        GPoint next = points[(i + 1) % count];
        int w = next.y >= current.y ? -1 : 1;
        int top = GRoundToInt(std::min(current.y, next.y));
        int bottom = GRoundToInt(std::max(current.y, next.y));

        if (top < bottom) {
            float m = (next.x - current.x) / (next.y - current.y);
            float b = current.x - m * current.y;

            if (clipY(current, next, m, b, bm))
                clipX(current, next, m, b, w, edges, bm);
        }
    }
    sortEdges(edges);
    return edges;
}

std::vector<Edge> verbEdge(std::vector<GPoint> points, int count, const GBitmap& bm){
    if (count < 2) return {};
    std::vector<Edge> edges;

    for (int i = 0; i < count - 1; ++i) {
        GPoint p0 = points[i];
        GPoint p1 = points[i + 1];

        int w = p1.y > p0.y ? -1 : 1;

        int top = GRoundToInt(std::min(p0.y, p1.y));
        int bottom = GRoundToInt(std::max(p0.y, p1.y));

        if (top < bottom) {
            float m = (p1.x - p0.x) / (p1.y - p0.y);
            float b = p0.x - m * p0.y;
            if (clipY(p0, p1, m, b, bm)){
                clipX(p0, p1, m, b, w, edges, bm);
            }
        }
    }
    return edges;
}