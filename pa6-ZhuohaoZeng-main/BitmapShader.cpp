#include "include/GMatrix.h"
#include "include/GShader.h"
#include <cmath>
#include "BitmapShader.h"
bool BitmapShader::isOpaque() {
    for (int y = 0; y < bitmap.height(); ++y) {
        for (int x = 0; x < bitmap.width(); ++x) {
            GPixel pixel = *bitmap.getAddr(x, y);
            if (GPixel_GetA(pixel) != 255) {
                return false;
            }
        }
    }
    return true;
};

bool BitmapShader::setContext(const GMatrix& ctm){
    auto inv = (ctm * localMatrix).invert();
    if (inv.has_value()){
        invTransform = inv.value();
        return true;
    }
    else
        return false;
};

void BitmapShader::shadeRow(int x, int y, int count, GPixel row[]){
    int ix, iy;
    GPoint p_src = invTransform * GPoint{x + 0.5f, y + 0.5f};
    const float dx = invTransform.e0().x;
    const float dy = invTransform.e0().y;

    for (int i = 0; i < count; i++, p_src.x += dx, p_src.y += dy) {
        
        
        switch (tileMode) {
            case GTileMode::kClamp:
                ix = std::max(0, std::min(static_cast<int>(floor(p_src.x)), bitmap.width() - 1));
                iy = std::max(0, std::min(static_cast<int>(floor(p_src.y)), bitmap.height() - 1));
                break;

            case GTileMode::kRepeat:
                ix = static_cast<int>(floor(p_src.x)) % bitmap.width();
                iy = static_cast<int>(floor(p_src.y)) % bitmap.height();
                ix = (ix < 0) ? bitmap.width() + ix : ix;
                iy = (iy < 0) ? bitmap.height() + iy : iy;
                break;

            case GTileMode::kMirror:
                ix = static_cast<int>(floor(p_src.x)) % (2 * bitmap.width());
                iy = static_cast<int>(floor(p_src.y)) % (2 * bitmap.height());
                ix = (ix >= bitmap.width()) ? 2 * bitmap.width() - 1 - ix : ix;
                iy = (iy >= bitmap.height()) ? 2 * bitmap.height() - 1 - iy : iy;
                ix = (ix < 0) ? -ix - 1 : ix;
                iy = (iy < 0) ? -iy - 1 : iy;
                break;
        }
        row[i] = *bitmap.getAddr(ix, iy);
    }

};

std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bm, const GMatrix& localMatrix, GTileMode mode){
    return std::unique_ptr<GShader>(new BitmapShader(bm, localMatrix, mode));
};



