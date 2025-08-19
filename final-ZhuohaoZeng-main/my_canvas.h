
#ifndef _g_my_canvas_h_
#define _g_my_canvas_h_

#include "include/GCanvas.h"
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "include/GRect.h"
#include "include/GBitmap.h"
#include "include/GShader.h"
#include "include/GPath.h"


class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device), matrixStack() {
        matrixStack.push_back(GMatrix());
    }

    //drawing
    void clear(const GColor& color) override;
    void drawRect(const GRect& rect, const GPaint& paint) override;
    void drawConvexPolygon(const GPoint* point, int count, const GPaint& paint) override;
    void drawPath(const GPath& path, const GPaint& paint) override;
    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                              int count, const int indices[], const GPaint& paint) override;
    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                              int level, const GPaint&) override;
    //transformation
    void save() override;
    void restore() override;
    void concat(const GMatrix& matrix) override;
    
private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;
    std::vector<GMatrix> matrixStack;
    // Add whatever other fields you need
};
#endif