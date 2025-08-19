#include "blend.h"
#include "include/GBlendMode.h"
#include "include/GCanvas.h"
#include "include/GPixel.h"
#include "include/GRect.h"
#include <deque>

//Limit the shape of the rectangle to the canvas size
inline GIRect intersect(const GRect& src_rect, const GIRect& dest_rect){
    GIRect src_rect_int = src_rect.round();
    int top = std::max(src_rect_int.top, dest_rect.top);
    int bottom = std::min(src_rect_int.bottom, dest_rect.bottom);
    int left = std::max(src_rect_int.left, dest_rect.left);
    int right = std::min(src_rect_int.right, dest_rect.right);
    GIRect result = GIRect::LTRB(left, top, right, bottom);
    return result;
}

inline void blendRow(int x0, int y0, int width, GPixel src, GBlendMode blend_mode, GBitmap fd) {
    //std::cout << "start blendrowGet" <<"x0: " <<x0 << " width:" <<width << std::endl;
    if (width < 1) return;

    //without following 2 line will causes the dbench to abort
    unsigned sa = GPixel_GetA(src);
    blend_mode = blendPreprocess(sa, blend_mode);
    //
    GPixel* addr = fd.getAddr(x0, y0);
    //std::cout << "end blendrowGet" << std::endl;
    if(blend_mode == GBlendMode::kSrc){
            std::fill_n(addr, width, src);
            return;
        }
    if(blend_mode == GBlendMode::kClear){
            std::fill_n(addr, width, GPixel_PackARGB(0, 0, 0, 0));
            return;
        }
    if(blend_mode == GBlendMode::kDst){
            return;
        }
    for (int x = x0; x < x0 + width ; x++) {
            GPixel* addr = fd.getAddr(x, y0);
            *addr = blend(src, *addr, blend_mode);
            
        }
}


