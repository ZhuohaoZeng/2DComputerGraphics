#include "include/GPathBuilder.h"
#include "include/GPoint.h"
#include "include/GPath.h"

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


GRect GPath::bounds() const{
    int count = fPts.size();
    if (count == 0)
        return GRect::LTRB(0, 0, 0, 0);
    float xmax = fPts[0].x;
    float xmin = fPts[0].x;
    float ymax = fPts[0].y;
    float ymin = fPts[0].y;
    for (int i = 1; i < count; i++){
        float x_p = fPts[i].x;
        float y_p = fPts[i].y;

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

// std::shared_ptr<GPath> GPath::transform(const GMatrix& matrix) const{
//     std::vector<GPoint> transformedPoints(fPts.size());
//     matrix.mapPoints(transformedPoints.data(), fPts.data(), fPts.size());
//     return std::make_shared<GPath>(transformedPoints, fVbs);
// }