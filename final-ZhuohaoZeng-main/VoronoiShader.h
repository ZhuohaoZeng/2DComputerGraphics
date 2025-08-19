#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"
#include "include/GPoint.h"
#include "utils.h"

class VoronoiShader: public GShader {
private:
    std::vector<GPoint> myPoints;
    std::vector<GColor> myColors;
    int myCount;
    GMatrix invTransform;
    GMatrix M;
public:
    VoronoiShader(const GPoint points[], const GColor colors[], int count, GMatrix m){
        myCount = count;
        M = m;
        for(int i = 0; i < count; i++) {
            myPoints.push_back(points[i]);
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
        GPoint p_t = invTransform * GPoint{x + 0.5f, y + 0.5f};
        float px = p_t.x;
        float dpx = invTransform.e0().x;


        for (int i = 0; i < count; i++, px += dpx) {
            float minDist = std::numeric_limits<float>::max();
            int minIndex = 0;
            for (int j = 0; j < myCount; j++) {
                float dist = (myPoints[j] - GPoint{px, p_t.y}).length();
                if (dist < minDist) {
                    minDist = dist;
                    minIndex = j;
                }
            }
            row[i] = colorToPixel(myColors[minIndex]);
        }
    };
};