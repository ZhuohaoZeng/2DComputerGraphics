#include "include/GColor.h"
#include "include/GShader.h"
#include "include/GMatrix.h"
#include "utils.h"

class TriangleShader : public GShader {
private:
    std::vector<GColor> shadeColors;
    GMatrix invTransform;
    GMatrix M;
    
public:
    TriangleShader(const GPoint pts[], const GColor colors[]):
    shadeColors(colors, colors + 3) {
        GPoint V1 = pts[1] - pts[0];
        GPoint V2 = pts[2] - pts[0];
        M = { V1.x, V2.x, pts[0].x, V1.y, V2.y, pts[0].y};
    }

    bool isOpaque() override{
        return shadeColors[0].a == 1.0f && shadeColors[1].a == 1.0f && shadeColors[2].a == 1.0f;
    };

    bool setContext(const GMatrix& ctm) override{
        auto inv = (ctm * M).invert();
        if (inv.has_value()) {
            invTransform = inv.value();
            return true;
        }
        return false;
    };

    void shadeRow(int x, int y, int count, GPixel row[]) override{
        for (int i = 0; i < count; ++i) {
            GPoint point = {x + 0.5f, y + 0.5f};
            GPoint p = invTransform * point;
            GColor c = p.x * shadeColors[1] + p.y * shadeColors[2] + (1 - p.x - p.y) * shadeColors[0];
            row[i] = colorToPixel(c);
            x += 1;
        }
    };

    std::unique_ptr<GShader> GCreateTriangleShader(const GPoint pts[3], const GColor colors[]){
        return std::unique_ptr<GShader>(new TriangleShader(pts, colors));
    };

};
