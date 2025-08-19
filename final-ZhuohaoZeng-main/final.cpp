#include "include/GFinal.h"
#include "VoronoiShader.h"
#include "SweepGradientShader.h"
#include "include/GPoint.h"
#include "include/GPath.h"
#include "include/GPathBuilder.h"

class final : public GFinal {
    std::shared_ptr<GShader> createVoronoiShader(const GPoint points[], const GColor colors[], int count) override{
        GMatrix m = {1, 0, 0,
                     0, 1, 0 };
        
        return std::unique_ptr<GShader>(new VoronoiShader(points, colors, count, m));
    }

    // std::shared_ptr<GShader> createSweepGradient(GPoint center, float startRadians, const GColor colors[], int count) override{
    //     GMatrix m = {1, 0, center.x,
    //                  0, 1, center.y };
    //     return std::unique_ptr<GShader>(new SweepGradientShader(center, startRadians, colors, count, m));
    // }
    //Helper to draw stroke lines
    GPathBuilder strokeLine(GPathBuilder StrokeBuilder, GPoint curr, GPoint next, float width) {
        float len = width / 2.f;
        GPoint norm = {-(next.y - curr.y),(next.x - curr.x)};
        norm = norm * (1 / sqrt(pow(norm.x, 2.f) + pow(norm.y, 2.f)));
        StrokeBuilder.moveTo(curr + norm * len);
        StrokeBuilder.lineTo(curr - norm * len);
        StrokeBuilder.lineTo(next - norm * len);
        StrokeBuilder.lineTo(next + norm * len);

        StrokeBuilder.addCircle(curr, len);
        StrokeBuilder.addCircle(next, len);
        return StrokeBuilder;
    }

    std::shared_ptr<GPath> strokePolygon(const GPoint points[], int count, float width, bool isClosed) override{
        GPathBuilder strokeBuilder;
        for(int i = 0; i < count - 1; i++) {
            GPoint curr = points[i];
            GPoint next = points[i+1];
            strokeBuilder = strokeLine(strokeBuilder, curr, next, width);
        }

        if (isClosed) {
            strokeBuilder = strokeLine(strokeBuilder, points[0], points[count - 1], width);
        }
        return strokeBuilder.detach();
    }


    
};
std::unique_ptr<GFinal> GCreateFinal() {
    return std::unique_ptr<GFinal>(new final);
}
    