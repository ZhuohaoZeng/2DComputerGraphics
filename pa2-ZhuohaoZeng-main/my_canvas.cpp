/*
 *  Copyright 2024 Zhuohao Zeng
 */

#include "blend.h"
#include "include/GBlendMode.h"
#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GRect.h"
#include <algorithm>
#include <deque>
#include <iostream>
#include <utility>


//There is clear in the blend mode, perhaps, we can deleted it?
void MyCanvas::clear(const GColor& color) {
    // your code here
    GPixel pixel = colorToPixel(color);
    int width = fDevice.width();
    int height = fDevice.height();
    
    for (int y = 0; y < height; ++y) {
        GPixel* rowPtr = fDevice.getAddr(0, y);
        for (int x = 0; x < width; ++x) {
            rowPtr[x] = pixel;
        }
    }
}
//Limit the shape of the rectangle to the canvas size
GIRect intersect(const GRect& src_rect, const GIRect& dest_rect){
    GIRect src_rect_int = src_rect.round();
    int top = std::max(src_rect_int.top, dest_rect.top);
    int bottom = std::min(src_rect_int.bottom, dest_rect.bottom);
    int left = std::max(src_rect_int.left, dest_rect.left);
    int right = std::min(src_rect_int.right, dest_rect.right);

    GIRect result = GIRect::LTRB(left, top, right, bottom);

    return result;
}

void blendRow(int x0, int y0, int width, GPixel src, GBlendMode blend_mode, GBitmap fd) {
    //std::cout << "start blendrowGet" <<"x0: " <<x0 << " width:" <<width << std::endl;
    if (width <= 0) return;
    GPixel* addr = fd.getAddr(x0, y0);
    //std::cout << "end blendrowGet" << std::endl;
    if(blend_mode == GBlendMode::kSrc){
            std::fill_n(addr, width, src);
            return;
        }
        if(blend_mode == GBlendMode::kClear){
            std::fill_n(addr, width, GPixel_PackARGB(0, 0, 0, 0));
            return;
        }
        if(blend_mode == GBlendMode::kDst){
            return;
        }
    for (int x = x0; x < x0 + width ; x++) {
            *addr = blend(src, *addr, blend_mode);
            addr++;
        }
}

void MyCanvas::drawRect(const GRect& rect, const GPaint& paint) {
    GPixel src = colorToPixel(paint.getColor());
    GBlendMode blend = paint.getBlendMode();
    GIRect rc = intersect(rect, GIRect::WH(fDevice.width(), fDevice.height()));

    //Draw Rectangle from top to bottom, left to right 
    for (int y = rc.top; y < rc.bottom; y++) {
        //This will causes getAddr go beyond the boundary, why?
        //blend
        blendRow(rc.left, y, rc.width(), src, blend, fDevice);
    }
    
}
struct Edge {
    int topY;
    int bottomY;
    float slope;
    float currentX;

};

bool compareEdge(const Edge& e1, const Edge& e2) {
    if (e1.topY != e2.topY) {
        return e1.topY < e2.topY;  // Prioritize smaller topY
    }
    
    if (e1.currentX != e2.currentX) {
        return e1.currentX < e2.currentX;  // Prioritize smaller currentX
    }

    // If topY and currentX are equal, compare slope as the last tie-breaker
    return e1.slope < e2.slope;
}

//making edges for the input points.
Edge* makeEdge(GPoint p0, GPoint p1) {
    //regularize the p0 to alwasy be the higher point to simplify the calculation
    if (p0.y > p1.y) {
        GPoint temp = p0;
        p0 = p1;
        p1 = temp;
    }
    
    int topY = round(p0.y);
    int bottomY = round(p1.y);
    //Ignored 
    if (topY == bottomY) {
        return nullptr;
    }

    float slope =  (p1.x - p0.x) / (p1.y - p0.y);
    //What exactly do deltaY represents?
    float deltaY = floor(p0.y) - p0.y + 0.5;
    float currentX = p0.x + slope * deltaY;
    
    Edge* newEdge = new Edge();
    newEdge->topY = topY;
    newEdge->bottomY = bottomY;
    newEdge->slope = slope;
    newEdge->currentX = currentX;
    return newEdge;
}

//helper method to calculate the projection of edge to vertical bounds
//directly revise edges to increases running efficiency
void project(float x, float y0, float y1, std::deque<Edge>& edges) {
    GPoint p0 = { static_cast<float>(x), static_cast<float>(y0) };
    GPoint p1 = { static_cast<float>(x), static_cast<float>(y1) };
    Edge* projected_point = makeEdge(p0, p1);
    if (projected_point != nullptr) {
        edges.push_back(*projected_point);
    }
}


//p0 p1 are copuies that we will be using later
void clipping(GPoint p0, GPoint p1, std::deque<Edge>& edges, GRect bounds) {
    //Simpliy calculation by letting p0.y > p1.y
    if (p0.y > p1.y) std::swap(p0, p1);
    //Igonre that edges that both point are above the bounds
    if (p0.y >= bounds.bottom || p1.y <= bounds.top) return;

    //case where higher point is above top bound
    if (p0.y < bounds.top) {
        p0.x = p0.x + (p1.x - p0.x) * (bounds.top - p0.y)/ (p1.y - p0.y);
        p0.y = bounds.top;
    } 
    //case where the lower point is below the bottom bound
    if (p1.y > bounds.bottom) {
        p1.x = p1.x - (p1.x - p0.x) * (p1.y - bounds.bottom) / (p1.y - p0.y);
        p1.y = bounds.bottom;
    }

    //Simpliy calculation by letting p0 always on the left of p1
    if (p1.x < p0.x) {
        GPoint temp = p1;
        p1 = p0;
        p0 = temp;
    }

    //Case where leftmost point is on the left of left bound
    if (p1.x <= bounds.left) {
        project(bounds.left, p0.y, p1.y, edges);
        return;
    }

    //Case where rightmost point is on the right of right bound
    if (p0.x >= bounds.right) {
        project(bounds.right, p0.y, p1.y, edges);
        return;
    }

    //case where left point project to left bound
    if (p0.x < bounds.left ) {
        float newY = p0.y + (bounds.left - p0.x) * (p1.y - p0.y) / (p1.x - p0.x);
        project(bounds.left, p0.y, newY, edges);
        p0.x = bounds.left; p0.y = newY;
    }

    //case where right point project to right bound
    if (p1.x > bounds.right) {
        float newY = p1.y - (p1.x -  bounds.right) * (p1.y - p0.y) / (p1.x - p0.x);
        project( bounds.right, newY, p1.y, edges);
        p1.x = bounds.right; p1.y = newY;
    }


    //adding the clipped edge to the collection
    Edge* edge = makeEdge(p0, p1);
    if (edge != nullptr) {
        edges.push_back(*edge);
    }

}


void MyCanvas::drawConvexPolygon(const GPoint points[], int count, const GPaint& paint){
    std::deque<Edge> edges; 
    GRect bounds = bounds.WH(fDevice.width(), fDevice.height());
    for (int i = 0; i < count; ++i){
        clipping(points[i], points[(i+1) % count], edges, bounds);
    }
    std::sort(edges.begin(), edges.end(), compareEdge);

    if (edges.size() < 2) return; 
    //Edge
    int topPolyY = edges.front().topY;
    int bottomPolyY = edges.back().bottomY;
    Edge edge0 = edges.front(); edges.pop_front();
    Edge edge1 = edges.front(); edges.pop_front();
    float x0 = edge0.currentX;
    float x1 = edge1.currentX;
    float curtY = edge0.topY;

    for(int i = topPolyY; i < bottomPolyY; i ++ ){
        
        GRect rect = rect.LTRB(x0, curtY, x1, curtY + 1);
        drawRect(rect, paint); 

        if (curtY >= edge0.bottomY) {
                assert(!edges.empty());
                edge0 = edges.front(); 
                edges.pop_front();
                x0 = edge0.currentX;
        } 
        else {
                x0 += edge0.slope;
        }

        if (curtY >= edge1.bottomY) {
                assert(!edges.empty());
                edge1 = edges.front(); 
                edges.pop_front();
                x1 = edge1.currentX;
        } 
        else {
                x1 += edge1.slope;
        }
        curtY++;
    }
}



std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device) {
    return std::unique_ptr<GCanvas>(new MyCanvas(device));
}

std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    // as fancy as you like
    // ...
    // canvas->clear(...);
    // canvas->fillRect(...);
    return "tears in rain";
}


