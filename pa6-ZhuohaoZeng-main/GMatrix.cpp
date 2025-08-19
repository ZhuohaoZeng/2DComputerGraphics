#include "include/GMatrix.h"
#include <cmath>


GMatrix::GMatrix():GMatrix(1, 0, 0, 0, 1,0){};

GMatrix GMatrix::Translate(float tx, float ty){
    return GMatrix(1, 0, tx, 0, 1, ty);
};

GMatrix GMatrix::Scale(float sx, float sy){
    return GMatrix(sx, 0, 0, 0, sy, 0);
};

GMatrix GMatrix::Rotate(float radians){
    return GMatrix(cos(radians), -sin(radians), 0, sin(radians), cos(radians), 0);
};

GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b) {
    return GMatrix(
        a[0] * b[0] + a[2] * b[1],          // a
        a[0] * b[2] + a[2] * b[3],          // c
        a[0] * b[4] + a[2] * b[5] + a[4],   // e
        a[1] * b[0] + a[3] * b[1],          // b
        a[1] * b[2] + a[3] * b[3],          // d
        a[1] * b[4] + a[3] * b[5] + a[5]    // f
    );
};

nonstd::optional<GMatrix> GMatrix::invert() const {
    float determine = fMat[0] * fMat[3] - fMat[1] * fMat[2];
    if (determine == 0) return {};
    float invDet = 1.0f /determine;
    return GMatrix(
        fMat[3] * invDet, // a
        -fMat[2] * invDet,  // c
        (fMat[2] * fMat[5] - fMat[3] * fMat[4]) * invDet, // e
        -fMat[1] * invDet, // b,
        fMat[0] * invDet,  // d
        (fMat[1] * fMat[4] - fMat[0] * fMat[5]) * invDet  // f
     );
};

void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const {
    for (int i = 0; i < count; i++) {
        float x = src[i].x;
        float y = src[i].y;

        dst[i].x = fMat[0] * x + fMat[2] * y + fMat[4];
        dst[i].y = fMat[1] * x + fMat[3] * y + fMat[5];
    }
};