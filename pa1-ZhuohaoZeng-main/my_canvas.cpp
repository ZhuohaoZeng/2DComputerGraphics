/*
 *  Copyright 2024 Zhuohao Zeng
 */

#include "starter_canvas.h"
#include "include/GMath.h"
#include "include/GPixel.h"
#include <algorithm>

//fast multiplaction
unsigned int div255(int number) {
    return ((number + 128) * 257) >> 16;
}

//source-over blend
GPixel blend(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    if (S_a == 255) {return source;}
    int a = GPixel_GetA(source) + div255((255 - S_a) * GPixel_GetA(dest));
    int r = GPixel_GetR(source) + div255((255 - S_a) * GPixel_GetR(dest));
    int g = GPixel_GetG(source) + div255((255 - S_a) * GPixel_GetG(dest));
    int b = GPixel_GetB(source) + div255((255 - S_a) * GPixel_GetB(dest));
    return GPixel_PackARGB(a, r, g, b);
}
//Just to ensure that my score of rects_blend is indeed doubled
GPixel slow_blend(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    if (S_a == 255) {return source;}
    int a = GPixel_GetA(source) + (255 - S_a) * GPixel_GetA(dest) / 255;
    int r = GPixel_GetR(source) + (255 - S_a) * GPixel_GetR(dest) / 255;
    int g = GPixel_GetG(source) + (255 - S_a) * GPixel_GetG(dest) / 255;
    int b = GPixel_GetB(source) + (255 - S_a) * GPixel_GetB(dest) / 255;
    return GPixel_PackARGB(a, r, g, b);
}

//Helper function get the premultiplied color.
GPixel colorToPixel(const GColor& color) {
    int a = GRoundToInt(color.a * 255);
    int r = GRoundToInt(color.r * color.a * 255);
    int g = GRoundToInt(color.g * color.a * 255);
    int b = GRoundToInt(color.b * color.a * 255);
    return GPixel_PackARGB(a, r, g, b);
}

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

void MyCanvas::fillRect(const GRect& rect, const GColor& color) {
    GPixel source = colorToPixel(color);
    GIRect src_rect_round = rect.round();
    int top = std::max(src_rect_round.top, 0);
    int bottom = std::min(src_rect_round.bottom, fDevice.height());
    int left = std::max(src_rect_round.left, 0);
    int right = std::min(src_rect_round.right, fDevice.width()); 

    //Make sure this go to the right address
    for (int y = top; y < bottom; y++) {
        for (int x = left; x < right; x++) {
            GPixel* addr = fDevice.getAddr(x, y);
            *addr = blend(source, *addr);
        }
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


