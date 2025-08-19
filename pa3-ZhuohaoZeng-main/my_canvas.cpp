/*
 *  Copyright 2024 Zhuohao Zeng
 */
#include "my_canvas.h"
#include "blend.h"
#include "utils.h"
#include "edge.h"
//#include <iostream>

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

void MyCanvas::drawRect(const GRect& rect, const GPaint& paint) {
    GMatrix ctm = matrixStack.front();
    GShader* shader = paint.peekShader();
    GBlendMode blendMode = paint.getBlendMode();
    bool isOpaque;
    if (shader != nullptr){
        shader->setContext(ctm);
        // reset the blendMode if is opaque
        isOpaque = shader->isOpaque();
        if (isOpaque){
            blendMode = blendPreprocess(255, blendMode);
        }
    }

    if (ctm != GMatrix()) {
        // There is a transformation, call drawConvexPolygon instead
        GPoint points[4] = {
                {rect.left, rect.top},
                {rect.left, rect.bottom},
                {rect.right, rect.bottom},
                {rect.right, rect.top}
        };
        drawConvexPolygon(points, 4, paint);
        return;
    }

    GIRect rc = intersect(rect, GIRect::WH(fDevice.width(), fDevice.height()));
    int width = rc.width();
    
    if (shader == nullptr) {
        GPixel src = colorToPixel(paint.getColor());
        for (int y = rc.top; y < rc.bottom; y++) {
            blendRow(rc.left, y, width, src, paint.getBlendMode(), fDevice);
        }
    } else {
        // Drawing with shader
        for (int y = rc.top; y < rc.bottom; y++) {
            GPixel tnp[width]; // Ensure width is non-negative
            shader->shadeRow(rc.left, y, width, tnp);
            for (int x = rc.left; x < rc.left + width; x++) {
                GPixel* p = fDevice.getAddr(x, y);
                *p = blend(tnp[x - rc.left], *p, blendMode); // Assuming blend returns GPixel
            }
        }
    }
    
}

void MyCanvas::drawConvexPolygon(const GPoint points[], int count, const GPaint& paint){
    GMatrix ctm = matrixStack.front();
    GPoint* transformedPoints = new GPoint[count];
    ctm.mapPoints(transformedPoints, points, count);
    GBlendMode blendMode = paint.getBlendMode();
    bool isOpaque;

    GShader* shader = paint.peekShader();
    if (shader != nullptr){
        shader->setContext(ctm);
        // reset the blendMode if is opaque
        isOpaque = shader->isOpaque();
        if (isOpaque){
            blendMode = blendPreprocess(255, blendMode);
        }
    }

    std::vector<Edge> edges = makeEdges(transformedPoints, count, fDevice);
    if(edges.size() < 2){return;}

    GPixel src = colorToPixel(paint.getColor());
    int minY = edges.front().top;
    int maxY = edges.back().bottom;

    for (int y = minY; y < maxY; y++){
        if (y >= edges[1].bottom)
            edges.erase(edges.begin() + 1);

        if (y >= edges[0].bottom)
            edges.erase(edges.begin());

        float ray = y + 0.5f;

        int x1 = static_cast<int>(round(edges[0].m * ray + edges[0].b));
        int x2 = static_cast<int>(round(edges[1].m * ray + edges[1].b));

        int x0 = static_cast<int>(std::min(x1, x2));
        int width = std::max(x1, x2) - x0;

        if(shader == nullptr)
            blendRow(x0, y, width, src, paint.getBlendMode(), fDevice);
        else{
            GPixel tnp[width];
            shader->shadeRow(x0, y, width, tnp);
            for(int x = x0; x < x0 + width; x++){
                GPixel* p = fDevice.getAddr(x, y);
                *p = blend(tnp[x - x0], *p, blendMode);
            }
        }
    }
    delete[] transformedPoints;
}

void MyCanvas::save() {
    GMatrix ctm = matrixStack.front();
    matrixStack.insert(matrixStack.begin(), ctm);
};

void MyCanvas::restore() {
    if (matrixStack.size() > 1) {
        matrixStack.erase(matrixStack.begin());
    }
};

void MyCanvas::concat(const GMatrix& matrix) {
    GMatrix& ctm = matrixStack.front();
    ctm = GMatrix::Concat(ctm, matrix);
};


//Below are helper method to play with this program
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


