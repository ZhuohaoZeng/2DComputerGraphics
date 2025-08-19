#include "include/GColor.h"
#include "include/GPoint.h"
#include "include/GShader.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"

class BitmapShader : public GShader{
private:
    GBitmap bitmap;
    GMatrix localMatrix;
    GMatrix invTransform;
    GTileMode tileMode;
    
public:
    BitmapShader(const GBitmap& bm, const GMatrix local_matrix, GTileMode mode) : 
    bitmap(bm), localMatrix(local_matrix), tileMode(mode){}

    bool isOpaque() override;

    bool setContext(const GMatrix& ctm) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;


};

