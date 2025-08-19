#include "include/GColor.h"
#include "include/GPoint.h"
#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"

class MainShader : public GShader{
private:
    GBitmap bitmap;
    GMatrix localMatrix;
    GMatrix invTransform;
    
public:
    MainShader(const GBitmap& bm, const GMatrix local_matrix) : bitmap(bm), localMatrix(local_matrix){}

    bool isOpaque() override;

    bool setContext(const GMatrix& ctm) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;


};

