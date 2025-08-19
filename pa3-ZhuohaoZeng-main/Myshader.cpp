#include "include/GShader.h"
#include "MyShader.h"
bool MyShader::isOpaque() {
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

bool MyShader::setContext(const GMatrix& ctm){
    auto inv = (ctm * localMatrix).invert();
    if (inv.has_value()){
        invTransform = inv.value();
        return true;
    }
    else
        return false;
};

void MyShader::shadeRow(int x, int y, int count, GPixel row[]){
    GPoint p_src = invTransform * GPoint{x + 0.5f, y + 0.5f};
    const float dx = invTransform.e0().x;
    const float dy = invTransform.e0().y;
    const int maxWidth = bitmap.width() - 1;
    const int maxHeight = bitmap.height() - 1;

    for (int i = 0; i < count; i++, p_src.x += dx, p_src.y += dy) {
        int ix = std::max(0, std::min(static_cast<int>(floor(p_src.x)), maxWidth));
        int iy = std::max(0, std::min(static_cast<int>(floor(p_src.y)), maxHeight));
        row[i] = *bitmap.getAddr(ix, iy);
    }

};

std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bm, const GMatrix& localMatrix){
    return std::unique_ptr<GShader>(new MyShader(bm, localMatrix));
};
