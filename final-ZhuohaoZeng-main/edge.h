#ifndef EDGE_H
#define EDGE_H

#include <cmath>
#include "include/GPoint.h"
#include "include/GBitmap.h"

typedef struct Edge{
    float m;
    float b;
    int top;
    int bottom;
    int x;
    int winding;

    bool isValid(int y) const {
        return y >= top && y < bottom;
    }

    bool isValue(int y, int value) const{
        return (y == value);
    }
 
}Edge;

bool clipY(GPoint& p0, GPoint& p1, float m, float b, const GBitmap& bm);

void clipX(GPoint& p0, GPoint& p1, float m, float b, int w, std::vector<Edge>& edges, const GBitmap& bm);

static bool compareEdges(const Edge& e1, const Edge& e2) {
    if (e1.top < e2.top) return true;
    if (e1.top > e2.top) return false;
    return e1.x < e2.x;
}

static void sortEdges(std::vector<Edge>& edges) {
    std::sort(edges.begin(), edges.end(), compareEdges);
}

std::vector<Edge> makeEdges(const GPoint* points, int count, const GBitmap& bm);

std::vector<Edge> verbEdge(std::vector<GPoint> points, int count, const GBitmap& bm);



#endif