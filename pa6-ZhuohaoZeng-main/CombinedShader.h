#ifndef COMBINEDSHADER
#define COMBINEDSHADER
#include "blend.h"
#include "include/GMath.h"
#include "include/GPixel.h"
#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"
#include "utils.h"

class CombinedShader : public GShader {
private:
    GShader* Shader0;
    GShader* Shader1;
    GPixel multiplyByPixel(GPixel p0, GPixel p1) {
        int a = GRoundToInt(multiplyPixel(GPixel_GetA(p0), GPixel_GetA(p1)));
        int r = GRoundToInt(multiplyPixel(GPixel_GetR(p0), GPixel_GetR(p1)));
        int g = GRoundToInt(multiplyPixel(GPixel_GetG(p0), GPixel_GetG(p1)));
        int b = GRoundToInt(multiplyPixel(GPixel_GetB(p0), GPixel_GetB(p1)));
        return GPixel_PackARGB(a, r, g, b);

    }

public:
    CombinedShader(GShader* shader0, GShader* shader1) : Shader0(shader0), Shader1(shader1){}
    bool isOpaque() override{ return Shader0->isOpaque() && Shader1->isOpaque();};
    
    bool setContext(const GMatrix& ctm) override {
        return Shader0->setContext(ctm) && Shader1->setContext(ctm);
    }
    
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPixel row0[count];
        GPixel row1[count];

        Shader0->shadeRow(x, y, count, row0);
        Shader1->shadeRow(x, y, count, row1);

        for (int i=0; i<count; i++) {
            row[i] = multiplyByPixel(row0[i], row1[i]);
        }
    }
};

#endif