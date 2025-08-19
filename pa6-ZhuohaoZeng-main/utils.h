#ifndef UTILS
#define UTILS

#include "blend.h"
#include "include/GBlendMode.h"
#include "include/GCanvas.h"
#include "include/GPixel.h"
#include "include/GRect.h"
#include <deque>

//Helper function that only been used in my_canvas clear, Perhaps, we can delete it?.
inline GPixel colorToPixel(const GColor& color) {
    int a = GRoundToInt(color.a * 255);
    int r = GRoundToInt(color.r * color.a * 255);
    int g = GRoundToInt(color.g * color.a * 255);
    int b = GRoundToInt(color.b * color.a * 255);
    return GPixel_PackARGB(a, r, g, b);
}

//Limit the shape of the rectangle to the canvas size
inline GIRect intersect(const GRect& src_rect, const GIRect& dest_rect){
    GIRect src_rect_int = src_rect.round();
    int top = std::max(src_rect_int.top, dest_rect.top);
    int bottom = std::min(src_rect_int.bottom, dest_rect.bottom);
    int left = std::max(src_rect_int.left, dest_rect.left);
    int right = std::min(src_rect_int.right, dest_rect.right);
    GIRect result = GIRect::LTRB(left, top, right, bottom);
    return result;
}

inline void blendRow(int x0, int y0, int width, GPixel src, GBlendMode blend_mode, GBitmap fd) {
    //std::cout << "start blendrowGet" <<"x0: " <<x0 << " width:" <<width << std::endl;
    if (width < 1) return;

    //without following 2 line will causes the dbench to abort
    unsigned sa = GPixel_GetA(src);
    blend_mode = blendPreprocess(sa, blend_mode);
    //
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
            GPixel* addr = fd.getAddr(x, y0);
            *addr = blend(src, *addr, blend_mode);
            
        }
}

//Extra helpers from PA5
inline std::vector<GPoint> quadSegment(GPoint A, GPoint B, GPoint C) {
    float tolerance = 0.25;
    GPoint E = (A - 2 * B + C) * 0.25;
    int num_seg = std::max(GRoundToInt(ceil(sqrt(E.length() / tolerance))), 1);
    float dt = 1.0f / num_seg;

    // forward differencing
    GPoint P = A;
    GPoint dP = 2 * dt * (B - A) + dt * dt * (A - 2 * B + C);
    GPoint ddP = 2 * dt * dt * (A - 2 * B + C);

    std::vector<GPoint> points = {P};
    for (int i = 1; i < num_seg; ++i) {
        P = P + dP;
        points.push_back(P);
        dP = dP + ddP;
    }
    points.push_back(C);
    return points;
}

inline std::vector<GPoint> cubeSegment(GPoint A, GPoint B, GPoint C, GPoint D) {
    float tolerance = 0.25;
    GPoint E0 = A - 2 * B + C;
    GPoint E1 = B - 2 * C + D;
    GPoint E = {std::max(std::abs(E0.x), std::abs(E1.x)), std::max(std::abs(E0.y), std::abs(E1.y))};
    int num_seg = std::max(GRoundToInt(ceil(sqrt((3 * E.length()) / (4 * tolerance)))), 1);
    float dt = 1.0f / num_seg;

    // forward differencing
    GPoint P = A;
    GPoint dP = dt * (3 * (B - A) + dt * (3 * (A - 2 * B + C) - dt * (A - 3 * B + 3 * C - D)));
    GPoint ddP = dt * dt * (6 * (A - 2 * B + C) + 6 * dt * (-1 * A + 3 * B - 3 * C + D));
    GPoint dddP = dt * dt * dt * 6 * (-1 * A + 3 * B - 3 * C + D);

    std::vector<GPoint> points = {P};
    for (int i = 1; i < num_seg; ++i) {
        P = P + dP;
        points.push_back(P);
        dP = dP + ddP;
        ddP = ddP + dddP;
    }
    points.push_back(D);
    return points;
}

inline GPoint interpolatePoint(const GPoint pts[4], float u, float v) {
    return (1 - v) * ((1 - u) * pts[0] + u * pts[1]) +  v * ((1 - u) * pts[3] + u * pts[2]);
}

inline GColor interpolateColor(const GColor colors[4], float u, float v) {
    float a = (1 - v) * ((1 - u) * colors[0].a + u * colors[1].a) + v * ((1 - u) * colors[3].a + u * colors[2].a);
    float r = (1 - v) * ((1 - u) * colors[0].r + u * colors[1].r) + v * ((1 - u) * colors[3].r + u * colors[2].r);
    float g = (1 - v) * ((1 - u) * colors[0].g + u * colors[1].g) + v * ((1 - u) * colors[3].g + u * colors[2].g);
    float b = (1 - v) * ((1 - u) * colors[0].b + u * colors[1].b) + v * ((1 - u) * colors[3].b + u * colors[2].b);
    return GColor::RGBA(r, g, b, a);
}

#endif