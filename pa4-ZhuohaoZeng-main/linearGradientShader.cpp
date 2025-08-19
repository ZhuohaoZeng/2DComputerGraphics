#include "linearGradientShader.h"
#include "include/GColor.h"
#include "utils.h"

bool LinearGradientShader::isOpaque() {
     for (int i = 0; i < fCount; ++i) {
            if (fColors[i].a != 1.0f) {
                return false;
            }
        }
        return true;
};

bool LinearGradientShader::setContext(const GMatrix& ctm){
    GMatrix localMatrix(dx, -dy, fP0.x, dy, dx, fP0.y);
    auto inv = (ctm * localMatrix).invert();
    if (inv.has_value()){
        invTransform = inv.value();
        return true;
    }
    else
        return false;
};

void LinearGradientShader::shadeRow(int x, int y, int count, GPixel row[]){
    GPoint p_src = invTransform * GPoint{x + 0.5f, y + 0.5f};
    float px = p_src.x;
    float dx = invTransform.e0().x;
    float dy = invTransform.e0().y;

    if (fCount == 1) {
        GColor color = fColors[0];
        GPixel src = colorToPixel(color);
        std::fill_n(row, count, src);
        return;
    }

    if (fCount == 2) {
        GColor c0 = fColors[0];
        GColor c1 = fColors[1];
        for (int i = 0; i < count; i++, px += dx) {
            float x_t = std::min(std::max(px, 0.0f), 1.0f);
            GColor c_t = (1 - x_t) * c0 + x_t * c1;
            row[i] = colorToPixel(c_t);
        }
    }

    

    for (int i = 0; i < count; i++, p_src.x += dx, p_src.y += dy) {

        float t = std::max(0.0f, std::min(1.0f, p_src.x / D));
        
        int idx = static_cast<int>(t * (fCount - 1));
        float blend = (t * (fCount - 1)) - idx;
        GColor c0 = fColors[idx];
        GColor c1 = fColors[std::min(idx + 1, fCount - 1)];
        GColor interpColor = {
            (1 - blend) * c0.r + blend * c1.r,
            (1 - blend) * c0.g + blend * c1.g,
            (1 - blend) * c0.b + blend * c1.b,
            (1 - blend) * c0.a + blend * c1.a
        };

        row[i] = colorToPixel(interpColor);
    }

};


std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count) {
    if (count < 1) {
        return nullptr;
    }
    return std::unique_ptr<GShader>(new LinearGradientShader(p0, p1, colors, count));
}
