/*
 *  Copyright 2024 Zhuohao Zeng
 */
#include "my_canvas.h"
#include "CombinedShader.h"
#include "blend.h"
#include "include/GMath.h"
#include "include/GPaint.h"
#include "include/GPath.h"
#include "include/GPoint.h"
#include "utils.h"
#include "edge.h"
#include <vector>
#include "TriangleShader.h"
#include "ProxyShader.h"

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

void MyCanvas::drawPath(const GPath &path, const GPaint &paint) {
    // Apply Transformation
    GMatrix ctm = matrixStack.front();
    std::shared_ptr<GPath> transformedPath =path.transform(ctm);

    //parameters after transformation
    GRect bound = transformedPath->bounds();
    GBlendMode blendMode = paint.getBlendMode();
    GPixel src = colorToPixel(paint.getColor());
    bool isOpaque;
    GShader* shader = paint.peekShader();
    GPoint points[GPath::kMaxNextPoints];
    std::vector<Edge> edges;
    GPath::Edger edger(*transformedPath);
    std::vector<GPoint> newPoints;
    std::vector<Edge> newEdges;

    if (shader != nullptr){
        shader->setContext(ctm);
        isOpaque = shader->isOpaque();
        if (isOpaque){
            blendMode = blendPreprocess(255, blendMode);
        }
    }

    
    while (auto verb = edger.next(points)) {
        switch (verb.value()) {
            case kLine: {
                newPoints = {points[0], points[1]};
                newEdges = verbEdge(newPoints, static_cast<int>(newPoints.size()), fDevice);
                edges.insert(edges.end(), newEdges.begin(), newEdges.end());
                break;
            }
            case kQuad: {
                newPoints = quadSegment(points[0], points[1], points[2]);
                newEdges = verbEdge(newPoints, static_cast<int>(newPoints.size()), fDevice);
                edges.insert(edges.end(), newEdges.begin(), newEdges.end());
                break;
            }
            case kCubic: {
                newPoints = cubeSegment(points[0], points[1], points[2], points[3]);
                newEdges = verbEdge(newPoints, static_cast<int>(newPoints.size()), fDevice);
                edges.insert(edges.end(), newEdges.begin(), newEdges.end());
                break;
            }
        }
    }


    sortEdges(edges);
    int minY = std::max(GRoundToInt(bound.top), 0);
    int maxY = std::min(GRoundToInt(bound.bottom), fDevice.height());

    if(edges.size() < 2){return;}


    for (int y = minY; y < maxY; ++y) {
        size_t i = 0;
        int winding = 0;
        int startX;
        float ray = y + 0.5f;

        while (i < edges.size() && edges[i].isValid(y)) {
            int x = GRoundToInt(edges[i].m * ray + edges[i].b);
            if (winding == 0)
                startX = x;
            winding += edges[i].winding;

            if (winding == 0) {
                int endX = x;
                if(shader == nullptr)
                    blendRow(startX, y, endX - startX, src, blendMode, fDevice);
                else{
                    GPixel tnp[endX - startX];
                    shader->shadeRow(startX, y, endX-startX, tnp);
                    for(int x_t = startX; x_t < endX; x_t++){
                        GPixel* p = fDevice.getAddr(x_t, y);
                        *p = blend(tnp[x_t - startX], *p, blendMode);
                    }
                }
            }

            if (edges[i].isValid(y + 1)) {
                edges[i].x = GRoundToInt(edges[i].m * (ray + 1) + edges[i].b);
                i += 1;
            } else {
                edges.erase(edges.begin() + i);
            }
        }
        //Sometimes windoing won't end up to be 0, still figuring why.
        //assert(winding == 0);
        while (i < edges.size() && edges[i].isValid(y+1)) {
            edges[i].x = GRoundToInt(edges[i].m * (ray + 1) + edges[i].b);
            i += 1;
        }

        std::sort(edges.begin(), edges.begin() + i, [](const Edge& a, const Edge& b) {
            return a.x < b.x;
        });
    }
}

void MyCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
    int count, const int indices[], const GPaint& paint){
    int n = 0;
    GShader* originalShader = paint.peekShader();
    
    for (int i = 0; i < count; ++i) {
        GPoint trigPts[3] = { verts[indices[n + 0]], verts[indices[n + 1]], verts[indices[n + 2]] };
        
        if (colors != nullptr && texs == nullptr) {
            GColor triangleColors[3] = {colors[indices[n + 0]], colors[indices[n + 1]], colors[indices[n + 2]]};
            //pack
            std::shared_ptr<GShader> shader = std::make_shared<TriangleShader>(trigPts, triangleColors);
            drawConvexPolygon(trigPts, 3, GPaint(shader));
        }
        
        if (colors == nullptr && texs != nullptr && originalShader != nullptr) {
            GPoint trigTexs[3] = {texs[indices[n + 0]], texs[indices[n + 1]], texs[indices[n + 2]]};
            GMatrix P = {trigPts[1].x - trigPts[0].x, trigPts[2].x - trigPts[0].x, trigPts[0].x,
                        trigPts[1].y - trigPts[0].y, trigPts[2].y - trigPts[0].y, trigPts[0].y};
            GMatrix T = {trigTexs[1].x - trigTexs[0].x, trigTexs[2].x - trigTexs[0].x, trigTexs[0].x,
                        trigTexs[1].y - trigTexs[0].y, trigTexs[2].y - trigTexs[0].y, trigTexs[0].y};
            GMatrix invTransform = P * T.invert().value();
            //pack
            std::shared_ptr<GShader> proxy = std::make_shared<ProxyShader>(originalShader, invTransform);
            drawConvexPolygon(trigPts, 3, GPaint(proxy));


        }
        
        if (colors != nullptr && texs != nullptr && originalShader != nullptr) {
            GPoint trigTexs[3] = {texs[indices[n + 0]], texs[indices[n + 1]], texs[indices[n + 2]]};
            GColor trigColors[3] = {colors[indices[n + 0]], colors[indices[n + 1]], colors[indices[n + 2]]};
            GMatrix P = {trigPts[1].x - trigPts[0].x, trigPts[2].x - trigPts[0].x, trigPts[0].x,
                        trigPts[1].y - trigPts[0].y, trigPts[2].y - trigPts[0].y, trigPts[0].y};

            GMatrix T = {trigTexs[1].x - trigTexs[0].x, trigTexs[2].x - trigTexs[0].x, trigTexs[0].x,
                        trigTexs[1].y - trigTexs[0].y, trigTexs[2].y - trigTexs[0].y, trigTexs[0].y};
            GMatrix invTransform = P * T.invert().value();
            TriangleShader triangleShader(trigPts, trigColors);
            ProxyShader proxyShader(originalShader, invTransform);
            //pack
            std::shared_ptr<GShader> combined = std::make_shared<CombinedShader>(&triangleShader, &proxyShader);
            drawConvexPolygon(trigPts, 3, GPaint(combined));
        }
        n += 3;
    }
};

void MyCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
    int level, const GPaint& paint){
    GPoint subVerts[4];
    GColor subColors[4];
    GPoint subTexs[4];

    for (int u = 0; u <= level; u++) {
        float u0 = u / (level + 1.0f);
        float u1 = (u + 1) / (level + 1.0f);

        for (int v = 0; v <= level; v++) {
            float v0 = v / (level + 1.0f);
            float v1 = (v + 1) / (level + 1.0f);

            subVerts[0] = interpolatePoint(verts, u0, v0);
            subVerts[1] = interpolatePoint(verts, u1, v0);
            subVerts[2] = interpolatePoint(verts, u1, v1);
            subVerts[3] = interpolatePoint(verts, u0, v1);

            if (colors != nullptr) {
                subColors[0] = interpolateColor(colors, u0, v0);
                subColors[1] = interpolateColor(colors, u1, v0);
                subColors[2] = interpolateColor(colors, u1, v1);
                subColors[3] = interpolateColor(colors, u0, v1);
            }

            if (texs != nullptr) {
                subTexs[0] = interpolatePoint(texs, u0, v0);
                subTexs[1] = interpolatePoint(texs, u1, v0);
                subTexs[2] = interpolatePoint(texs, u1, v1);
                subTexs[3] = interpolatePoint(texs, u0, v1);
            }

            const int indices[6] = { 0, 1, 3, 1, 2, 3};

            if (colors != nullptr && texs != nullptr) {
                drawMesh(subVerts, subColors, subTexs, 2, indices, paint);
            } else if (texs != nullptr) {
                drawMesh(subVerts, nullptr, subTexs, 2, indices, paint);
             
            } else if (colors != nullptr) {
                drawMesh(subVerts, subColors, nullptr, 2, indices, paint);
            } else {
                drawMesh(subVerts, nullptr, nullptr, 2, indices, paint);
            }
        }
    }

};


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

