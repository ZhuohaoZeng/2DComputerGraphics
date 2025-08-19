
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "include/GShader.h"

class LinearGradientShader : public GShader {
private:
    GPoint fP0;
    GPoint fP1;
    std::vector<GColor> fColors;
    int fCount;
    float dx, dy, D;
    GMatrix invTransform;

public:
    LinearGradientShader(GPoint p0, GPoint p1, const GColor colors[], int count) 
    : fP0(p0), fP1(p1), fColors(colors, colors + count), fCount(count) {
        dx = p1.x - p0.x;
        dy = p1.y - p0.y;
        D = std::sqrt(dx * dx + dy * dy);
        dx /= D;
        dy /= D;
    }
    
    bool isOpaque() override;

    bool setContext(const GMatrix& ctm) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;
};
 