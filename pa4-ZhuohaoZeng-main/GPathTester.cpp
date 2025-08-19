// #include <iostream>
// #include "include/GCanvas.h" // Assuming you have the relevant headers
// #include "include/GPath.h"
// #include "include/GPaint.h"
// #include "include/GBitmap.h"
// #include "include/GPathBuilder.h"

// void test_drawPath() {
//     // Create a simple bitmap to draw on
//     const int w = 4, h = 4;
//     GPixel pixels[w*h];
//     GBitmap bm(w, h, w * sizeof(GPixel), pixels, false);
    
//     // Create the canvas object
//     auto canvas = GCreateCanvas(bm);

//     // Create a simple path (e.g., a line)
//     GPathBuilder path;
//     path.moveTo(0, 0);
//     path.lineTo(1, 1);

//     // Create a basic paint object
//     GPaint paint;

//     // Call drawPath directly
//     canvas->drawPath(path, paint);

//     // Optionally check the results in pixels, or just verify no crash occurs
//     std::cout << "drawPath test completed" << std::endl;
// }

// int main() {
//     test_drawPath();
//     return 0;
// }

// void nodraw() {
//     const int w = 4, h = 4;
//     GPixel pixels[w*h];
//     GBitmap bm(w, h, w*4, pixels, false);
//     auto canvas = GCreateCanvas(bm);

//     const std::shared_ptr<GPath> nodrawpaths[] = {
//         GPathBuilder().detach(),
//         [](){ GPathBuilder p; p.moveTo(1,1); return p.detach(); }(),
//         [](){ GPathBuilder p; p.moveTo(1,1); p.lineTo(2,2); return p.detach(); }(),
//         [](){ GPathBuilder p; p.moveTo(1,1); p.lineTo(2,2); p.lineTo(1,1); return p.detach(); }(),
//     };
//     const GPaint paint;

//     for (const auto& p : nodrawpaths) {
//         memset(pixels, 0, sizeof(pixels));
//         canvas->drawPath(*p, paint);
//         EXPECT_TRUE(stats, all_zeros(pixels, sizeof(pixels)));
//     }
// }
