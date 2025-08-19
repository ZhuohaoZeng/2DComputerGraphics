#include "linearGradientShader.h"
#include "include/GColor.h"
#include "utils.h"

bool LinearGradientShader::isOpaque() {
    return fIsOpaque;
}

bool LinearGradientShader::setContext(const GMatrix& ctm){
    auto inv = (ctm * M).invert();
    if (inv.has_value()){
        invTransform = inv.value();
        return true;
    }
    else
        return false;
}

float applyTileMode(float t, GTileMode tileMode, int numColor) {
    switch (tileMode) {
        case GTileMode::kClamp:
            t = std::min(std::max(t, 0.0f), 1.0f);
            break;
        case GTileMode::kRepeat:
            t = fmod(t, 1.0f);
            t = (t < 0) ? 1 + t : t;
            break;
        case GTileMode::kMirror:
            t = fmod(t, 2.0f);
            t = (t < 0) ? 2 + t : t;
            t = (t > 1) ? 2 - t : t;
            break;
    }
    return t * (numColor - 1);
}

void LinearGradientShader::shadeRow(int x, int y, int count, GPixel row[]) {
    GPoint p_t = invTransform * GPoint{x + 0.5f, y + 0.5f};
    float px = p_t.x;
    float dpx = invTransform.e0().x;

    if (numColor == 1) {
        GColor c_t = shadeColors[0];
        GPixel src = colorToPixel(c_t);
        std::fill_n(row, count, src);
        return;
    }

    if (numColor == 2) {
        GColor c0 = shadeColors[0];
        GColor c1 = shadeColors[1];

        GColor dc = c1 - c0;

        for (int i = 0; i < count; i++, px += dpx) {
            float t = applyTileMode(px, tileMode, 2);
            GColor c_t = c0 + t * dc;
            row[i] = colorToPixel(c_t);
        }
        return;
    }

    for (int i = 0; i < count; i++, px += dpx) {
        float t = applyTileMode(px, tileMode, numColor);
        int k = GRoundToInt(floor(t));
        float frac = t - k;

        if (k >= numColor - 1) {
            k = numColor - 2;
            frac = 1.0f;
        }

        GColor c0 = shadeColors[k];
        GColor c1 = shadeColors[k + 1];
        GColor c_t = (1 - frac) * c0 + frac * c1;
        row[i] = colorToPixel(c_t);
    }
}

std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode mode) {
    if (count >= 1) {
        int dx = GRoundToInt(p1.x - p0.x);
        int dy = GRoundToInt(p1.y - p0.y);
        GMatrix m = GMatrix(dx, -dy, p0.x, dy, dx, p0.y);
        return std::unique_ptr<GShader>(new LinearGradientShader(p0, p1, colors, m, count, mode));
    }
    else
        return nullptr;
}
