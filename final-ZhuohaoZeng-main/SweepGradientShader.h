#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"
#include "include/GPoint.h"
#include "utils.h"

class SweepGradientShader: public GShader {
private:
    std::vector<GColor> myColors;
    int myCount;
    GMatrix invTransform;
    GMatrix M;
public:
    SweepGradientShader(GPoint center, float startRadians, const GColor colors[], int count, GMatrix m){
        myCount = count;
        M = m;
        for(int i = 0; i < count; i++) {
            myColors.push_back(colors[i]);
        }
    };
    bool isOpaque() override {
        return false;
    };
    
    bool setContext(const GMatrix& ctm) override {
        auto inv = (ctm * M).invert();
        if (inv.has_value()){
            invTransform = inv.value();
            return true;
        }
        else
            return false;
        return false;
    };
    
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        
    };
};