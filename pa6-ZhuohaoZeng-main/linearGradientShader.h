
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "include/GShader.h"

class LinearGradientShader : public GShader {
private:
    int numColor;
    GPoint fP0;
    GPoint fP1;
    GMatrix M;
    GMatrix invTransform;
    GColor* shadeColors;
    GTileMode tileMode;
    bool fIsOpaque;

public:
    LinearGradientShader(GPoint p0, GPoint p1, const GColor* colors, GMatrix m, int count, GTileMode tilemode) :
    numColor(count), fP0(p0), fP1(p1), M(m), shadeColors(new GColor[count + 2]), tileMode(tilemode){
        shadeColors[0] = colors[0];
        for (int i = 1; i < count; i++){
            shadeColors[i] = colors[i];
        }
        shadeColors[count] = colors[count - 1];
        
        bool isOpaque = true;
        for(int i = 0; i < numColor; i++){
            if (shadeColors[i].a != 255)
                isOpaque = false;
            }
        fIsOpaque = isOpaque;
    }

    bool isOpaque() override;

    bool setContext(const GMatrix& ctm) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;


};
 