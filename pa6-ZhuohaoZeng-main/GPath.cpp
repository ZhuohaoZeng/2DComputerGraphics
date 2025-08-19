#include "include/GPathBuilder.h"
#include "include/GPoint.h"
#include "include/GPath.h"

// void GPath::transform(const GMatrix& matrix) const{
    
//     int count = fPts.size();
//     std::vector<GPoint> ptsCopy(fPts.begin(), fPts.end());
//     matrix.mapPoints(ptsCopy.data(), count);
//     return transformedPath;
// }
void GPathBuilder::addRect(const GRect& rect, GPathDirection dir){
    this->moveTo(rect.left, rect.top);

    if (dir == GPathDirection::kCW) {
        this->lineTo(rect.right, rect.top);
        this->lineTo(rect.right, rect.bottom);
        this->lineTo(rect.left, rect.bottom);
    } else {
        this->lineTo(rect.left, rect.bottom);
        this->lineTo(rect.right, rect.bottom);
        this->lineTo(rect.right, rect.top);
    }

}

void GPathBuilder::addPolygon(const GPoint pts[], int count) {
    if (count < 3) return;
    this->moveTo(pts[0]);

    for (int i = 1; i < count; ++i) {
        this->lineTo(pts[i]);
    }

}

void GPathBuilder::addCircle(GPoint center, float radius, GPathDirection dir){
    const float kTanPiOver8 = tan(M_PIf32 / 8.0f);
    const float kSqrt2Over2 = sqrt(2.0f) / 2; 
    GMatrix transform = GMatrix::Translate(center.x, center.y) * GMatrix::Scale(radius, radius);
    this->moveTo(transform * GPoint{1, 0});
    if (dir == GPathDirection::kCW) {
        this->quadTo(transform * GPoint{1, kTanPiOver8}, transform * GPoint{kSqrt2Over2, kSqrt2Over2});
        this->quadTo(transform * GPoint{kTanPiOver8, 1}, transform * GPoint{0, 1});
        this->quadTo(transform * GPoint{-kTanPiOver8, 1}, transform * GPoint{-kSqrt2Over2, kSqrt2Over2});
        this->quadTo(transform * GPoint{-1, kTanPiOver8}, transform * GPoint{-1, 0});
        this->quadTo(transform * GPoint{-1, -kTanPiOver8}, transform * GPoint{-kSqrt2Over2, -kSqrt2Over2});
        this->quadTo(transform * GPoint{-kTanPiOver8, -1}, transform * GPoint{0, -1});
        this->quadTo(transform * GPoint{kTanPiOver8, -1}, transform * GPoint{kSqrt2Over2, -kSqrt2Over2});
        this->quadTo(transform * GPoint{1, -kTanPiOver8}, transform * GPoint{1, 0});
    } else {
        this->quadTo(transform * GPoint{1, -kTanPiOver8}, transform * GPoint{kSqrt2Over2, -kSqrt2Over2});
        this->quadTo(transform * GPoint{kTanPiOver8, -1}, transform * GPoint{0, -1});
        this->quadTo(transform * GPoint{-kTanPiOver8, -1}, transform * GPoint{-kSqrt2Over2, -kSqrt2Over2});
        this->quadTo(transform * GPoint{-1, -kTanPiOver8}, transform * GPoint{-1, 0});
        this->quadTo(transform * GPoint{-1, kTanPiOver8}, transform * GPoint{-kSqrt2Over2, kSqrt2Over2});
        this->quadTo(transform * GPoint{-kTanPiOver8, 1}, transform * GPoint{0, 1});
        this->quadTo(transform * GPoint{kTanPiOver8, 1}, transform * GPoint{kSqrt2Over2, kSqrt2Over2});
        this->quadTo(transform * GPoint{1, kTanPiOver8}, transform * GPoint{1, 0});
    }
};
//helpers to find the bound




//PA4 bound
GRect localBounds(std::vector<GPoint> Pts) {
    int count = Pts.size();
    if (count == 0)
        return GRect::LTRB(0, 0, 0, 0);


    float xmax = Pts[0].x;
    float xmin = Pts[0].x;
    float ymax = Pts[0].y;
    float ymin = Pts[0].y;
    for (int i = 1; i < count; i++){
        float x_p = Pts[i].x;
        float y_p = Pts[i].y;

        if (x_p > xmax)
            xmax = x_p;
        if (x_p < xmin)
            xmin = x_p;

        if (y_p > ymax)
            ymax = y_p;
        if (y_p < ymin)
            ymin = y_p;
    }
    return GRect::LTRB(xmin, ymin, xmax, ymax);
}

static GPoint interpolateQ(GPoint A, GPoint B, GPoint C, float t) {
    return A * std::pow((1 - t), 2.0f) + 2 * B * t * (1 - t) + C * std::pow(t, 2.0f);
}

static GPoint interpolateC(GPoint A, GPoint B, GPoint C, GPoint D, float t) {
    return A * std::pow((1 - t), 3.0f) + 3 * B * t * std::pow((1 - t), 2.0f) +
           3 * C * std::pow(t, 2.0f) * (1 - t) + D * std::pow(t, 3.0f);
}

//finding Extrema
std::vector<float> QuadraticExtrema(float A, float B, float C) {
    std::vector<float> extrema;
    float a = 2 * C - 4 * B + 2 * A;
    float b = -2 * A + 2 * B;
    if (a != 0) {
        float t = -b / a;
        if (t > 0 && t < 1) {
            extrema.push_back(t);
        }
    }
    return extrema;
}

//find extrema of cubu
std::vector<float> CubicExtrema(float A, float B, float C, float D) {
    std::vector<float> extrema;
    float a = -3 * A + 9 * B - 9 * C + 3 * D;
    float b = 6 * A - 12 * B + 6 * C;
    float c = -3 * A + 3 * B;

    if (a == 0) {
        if (b != 0) {
            float t = -c / b;
            if (t > 0 && t < 1) {
                extrema.push_back(t);
            }
        }
    } else {
        float discriminant = b*b - 4*a*c;
        if (discriminant >= 0) {
            discriminant = sqrt(discriminant);
            float t1 = (-b + discriminant) / (2*a);
            float t2 = (-b - discriminant) / (2*a);

            if (t1 > 0 && t1 < 1) {
                extrema.push_back(t1);
            }
            if (t2 > 0 && t2 < 1 && discriminant > 0) { 
                extrema.push_back(t2);
            }
        }
    }
    return extrema;
}
//PA5 New bounds 
GRect GPath::bounds() const {
    if (fPts.empty())
        return GRect::LTRB(0, 0, 0, 0);
    GPoint points[GPath::kMaxNextPoints];
    GPath::Edger edger(*this);
    std::vector<GPoint> pts;
    std::vector<float> extremaX;
    std::vector<float> extremaY;

    while(auto verb = edger.next(points)) {
        switch(verb.value()) {
            case kLine:
                pts.push_back(points[0]);
                pts.push_back(points[1]);
                break;
            case kQuad:
                extremaX = QuadraticExtrema(points[0].x, points[1].x, points[2].x);
                extremaY = QuadraticExtrema(points[0].y, points[1].y, points[2].y);
                for (float t : extremaX) {
                    pts.push_back(interpolateQ(points[0], points[1], points[2], t));
                }
                for (float t : extremaY) {
                    pts.push_back(interpolateQ(points[0], points[1], points[2], t));
                }
                pts.push_back(points[0]);
                pts.push_back(points[2]);
                break;
            case kCubic:
                extremaX = CubicExtrema(points[0].x, points[1].x, points[2].x, points[3].x);
                extremaY = CubicExtrema(points[0].y, points[1].y, points[2].y, points[3].y);
                for (float t : extremaX) {
                    pts.push_back(interpolateC(points[0], points[1], points[2], points[3], t));
                }
                for (float t : extremaY) {
                    pts.push_back(interpolateC(points[0], points[1], points[2], points[3], t));
                }
                pts.push_back(points[0]);
                pts.push_back(points[3]);
                break;
            }
    }
    return localBounds(pts);

}

void GPath::ChopQuadAt(const GPoint *src, GPoint *dst, float t){
    GPoint AB = src[0] + (src[1] - src[0]) * t;
    GPoint BC = src[1] + (src[2] - src[1]) * t;
    GPoint ABC = AB + (BC - AB) * t;

    dst[0] = src[0];
    dst[1] = AB;
    dst[2] = ABC;
    dst[3] = BC;
    dst[4] = src[2];

};

void GPath::ChopCubicAt(const GPoint *src, GPoint *dst, float t){
    GPoint AB = src[0] + (src[1] - src[0]) * t;
    GPoint BC = src[1] + (src[2] - src[1]) * t;
    GPoint CD = src[2] + (src[3] - src[2]) * t;

    GPoint ABC = AB + (BC - AB) * t;
    GPoint BCD = BC + (CD - BC) * t;

    GPoint ABCD = ABC + (BCD - ABC) * t;

    dst[0] = src[0];
    dst[1] = AB;
    dst[2] = ABC;
    dst[3] = ABCD;
    dst[4] = BCD;
    dst[5] = CD;
    dst[6] = src[3];
};

