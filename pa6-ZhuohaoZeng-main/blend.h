/*
 *  Copyright 2024 Zhuohao Zeng
 */

#ifndef _g_blend_h_
#define _g_blend_h_

#include "include/GBlendMode.h"
#include "include/GCanvas.h"
#include "include/GPixel.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "include/GMath.h"
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <iostream>
//fast multiplaction
static inline unsigned int multiplyPixel(int Lnumber, int Rnumber) {
    return ((Lnumber * Rnumber + 128) * 257) >> 16;
}

//clear

static inline GPixel blend_Clear(const GPixel& source, const GPixel& dest){
    return GPixel_PackARGB(0,0,0,0);
}

//source
static inline GPixel blend_Src(const GPixel& source, const GPixel& dest) {
    return source;
}

//destination
static inline GPixel blend_Dst(const GPixel& source, const GPixel& dest) {
    return dest;
}
 
//source-over
static inline GPixel blend_SrcOver(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    if (S_a == 255) {return source;}
    int a = GPixel_GetA(source) + multiplyPixel((255 - S_a), GPixel_GetA(dest));
    int r = GPixel_GetR(source) + multiplyPixel((255 - S_a), GPixel_GetR(dest));
    int g = GPixel_GetG(source) + multiplyPixel((255 - S_a), GPixel_GetG(dest));
    int b = GPixel_GetB(source) + multiplyPixel((255 - S_a), GPixel_GetB(dest));
     //std::cout << "blend_SrcOver" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);
}

//destination-over
static inline GPixel blend_DstOver(const GPixel& source, const GPixel& dest) {
    int D_a = GPixel_GetA(dest);
    if (D_a == 255) {return dest;}
    int a = GPixel_GetA(dest) + multiplyPixel((255 - D_a), GPixel_GetA(source));
    int r = GPixel_GetR(dest) + multiplyPixel((255 - D_a), GPixel_GetR(source));
    int g = GPixel_GetG(dest) + multiplyPixel((255 - D_a), GPixel_GetG(source));
    int b = GPixel_GetB(dest) + multiplyPixel((255 - D_a), GPixel_GetB(source));
     //std::cout << "blend_DstOver" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);
}
//source-in <-- Da * S
static inline GPixel blend_SrcIn(const GPixel& source, const GPixel& dest) {
    int D_a = GPixel_GetA(dest);
    int a = multiplyPixel(D_a, GPixel_GetA(source));
    int r = multiplyPixel(D_a, GPixel_GetR(source));
    int g = multiplyPixel(D_a, GPixel_GetG(source));
    int b = multiplyPixel(D_a, GPixel_GetB(source));
     //std::cout << "blend_SrcIn" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);
}

//destination-in <-- Sa * D
static inline GPixel blend_DstIn(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    int a = multiplyPixel(S_a, GPixel_GetA(dest));
    int r = multiplyPixel(S_a, GPixel_GetR(dest));
    int g = multiplyPixel(S_a, GPixel_GetG(dest));
    int b = multiplyPixel(S_a, GPixel_GetB(dest));
     //std::cout << "blend_DstIn" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);

}

//source-out <--(1 - Da)*S
static inline GPixel blend_SrcOut(const GPixel& source, const GPixel& dest) {
    int D_a = GPixel_GetA(dest);
    int a = multiplyPixel((255 - D_a), GPixel_GetA(source));
    int r = multiplyPixel((255 - D_a), GPixel_GetR(source));
    int g = multiplyPixel((255 - D_a), GPixel_GetG(source));
    int b = multiplyPixel((255 - D_a), GPixel_GetB(source));
     //std::cout << "blend_SrcOut" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);
}

//destination-out <-- (1 - Sa)*D
static inline GPixel blend_DstOut(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    int a = multiplyPixel((255 - S_a), GPixel_GetA(dest));
    int r = multiplyPixel((255 - S_a), GPixel_GetR(dest));
    int g = multiplyPixel((255 - S_a), GPixel_GetG(dest));
    int b = multiplyPixel((255 - S_a), GPixel_GetB(dest));
     //std::cout << "blend_DstOut" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);
}

//source a top <-- Da*S + (1 - Sa)*D
static inline GPixel blend_SrcATop(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    int D_a = GPixel_GetA(dest);
    //calculating the left part of the funtion
    //int l_a = multiplyPixel(D_a, GPixel_GetA(source));
    int l_r = multiplyPixel(D_a, GPixel_GetR(source));
    int l_g = multiplyPixel(D_a, GPixel_GetG(source));
    int l_b = multiplyPixel(D_a, GPixel_GetB(source));
    
    //calculating the right part of the function
    //int r_a = multiplyPixel((255 - S_a), GPixel_GetA(dest));
    int r_r = multiplyPixel((255 - S_a), GPixel_GetR(dest));
    int r_g = multiplyPixel((255 - S_a), GPixel_GetG(dest));
    int r_b = multiplyPixel((255 - S_a), GPixel_GetB(dest));

    //add them together
    unsigned a = GPixel_GetA(dest);
    int r = l_r + r_r;
    int g = l_g + r_g;
    int b = l_b + r_b;
     //std::cout << "blend_SrcATop" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);

}

//destination a top <--Sa*D + (1 - Da)*S
static inline GPixel blend_DstATop(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    int D_a = GPixel_GetA(dest);

    //calculating the left part of the funtion
    int l_a = multiplyPixel(S_a, GPixel_GetA(dest));
    int l_r = multiplyPixel(S_a, GPixel_GetR(dest));
    int l_g = multiplyPixel(S_a, GPixel_GetG(dest));
    int l_b = multiplyPixel(S_a, GPixel_GetB(dest));
    
    //calculating the right part of the function
    int r_a = multiplyPixel((255 - D_a), GPixel_GetA(source));
    int r_r = multiplyPixel((255 - D_a), GPixel_GetR(source));
    int r_g = multiplyPixel((255 - D_a), GPixel_GetG(source));
    int r_b = multiplyPixel((255 - D_a), GPixel_GetB(source));

    //add them together
    int a = l_a + r_a;
    int r = l_r + r_r;
    int g = l_g + r_g;
    int b = l_b + r_b;
     //std::cout << "blend_DstATop" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);

}

// Xor <--(1 - Sa)*D + (1 - Da)*S
static inline GPixel blend_Xor(const GPixel& source, const GPixel& dest) {
    int S_a = GPixel_GetA(source);
    int D_a = GPixel_GetA(dest);

    //calculating the left part of the funtion
    int l_a = multiplyPixel((255 - S_a), GPixel_GetA(dest));
    int l_r = multiplyPixel((255 - S_a), GPixel_GetR(dest));
    int l_g = multiplyPixel((255 - S_a), GPixel_GetG(dest));
    int l_b = multiplyPixel((255 - S_a), GPixel_GetB(dest));

    //calculating the right part of the function
    int r_a = multiplyPixel((255 - D_a), GPixel_GetA(source));
    int r_r = multiplyPixel((255 - D_a), GPixel_GetR(source));
    int r_g = multiplyPixel((255 - D_a), GPixel_GetG(source));
    int r_b = multiplyPixel((255 - D_a), GPixel_GetB(source));

    //add them together
    int a = l_a + r_a;
    int r = l_r + r_r;
    int g = l_g + r_g;
    int b = l_b + r_b;
    //std::cout << "blend_Xor" << std::endl;
     //std::cout<<"a:" << a <<"r:" << r << "g:"<< g<<"b"<<b<<std::endl;
    return GPixel_PackARGB(a, r, g, b);
}

static inline GBlendMode blendPreprocess(int S_a, GBlendMode blend_mode) {
    if (S_a == 0){ // mode 0
        switch (blend_mode) {
            case GBlendMode::kClear:    return GBlendMode::kClear;
            case GBlendMode::kSrc:      return GBlendMode::kClear;
            case GBlendMode::kDst:      return GBlendMode::kDst;
            case GBlendMode::kSrcOver:  return GBlendMode::kDst;
            case GBlendMode::kDstOver:  return GBlendMode::kDst;
            case GBlendMode::kSrcIn:    return GBlendMode::kClear;
            case GBlendMode::kDstIn:    return GBlendMode::kClear;
            case GBlendMode::kSrcOut:   return GBlendMode::kClear;
            case GBlendMode::kDstOut:   return GBlendMode::kDst;
            case GBlendMode::kSrcATop:  return GBlendMode::kDst;
            case GBlendMode::kDstATop:  return GBlendMode::kClear;
            case GBlendMode::kXor:      return GBlendMode::kDst;
        }
    }
    if (S_a == 255){ // mode 1
        switch (blend_mode) {
            case GBlendMode::kClear:    return GBlendMode::kClear;
            case GBlendMode::kSrc:      return GBlendMode::kSrc;
            case GBlendMode::kDst:      return GBlendMode::kDst;
            case GBlendMode::kSrcOver:  return GBlendMode::kSrc;
            case GBlendMode::kDstOver:  return GBlendMode::kDstOver;
            case GBlendMode::kSrcIn:    return GBlendMode::kSrcIn;
            case GBlendMode::kDstIn:    return GBlendMode::kDst;
            case GBlendMode::kSrcOut:   return GBlendMode::kSrcOut;
            case GBlendMode::kDstOut:   return GBlendMode::kClear;
            case GBlendMode::kSrcATop:  return GBlendMode::kSrcIn;
            case GBlendMode::kDstATop:  return GBlendMode::kDstATop;
            case GBlendMode::kXor:      return GBlendMode::kXor;
        }
    }
    return blend_mode;
}

//Helper preprocess the input and decided the blend mode to simplify drawing.
static inline GPixel blend(const GPixel& source, const GPixel& dest, const GBlendMode& blend_mode) {
    int S_a = GPixel_GetA(source);
    GBlendMode processed_blend_mode = blendPreprocess(S_a, blend_mode);
    switch(processed_blend_mode){
        case GBlendMode::kClear: return blend_Clear(source, dest);
        case GBlendMode::kSrc: return blend_Src(source, dest); 
        case GBlendMode::kDst: return blend_Dst(source, dest);
        case GBlendMode::kSrcOver: return blend_SrcOver(source, dest);
        case GBlendMode::kDstOver: return blend_DstOver(source, dest);
        case GBlendMode::kSrcIn: return blend_SrcIn(source, dest);
        case GBlendMode::kDstIn: return blend_DstIn(source, dest);
        case GBlendMode::kSrcOut: return blend_SrcOut(source, dest);
        case GBlendMode::kDstOut: return blend_DstOut(source, dest);
        case GBlendMode::kSrcATop: return blend_SrcATop(source, dest);
        case GBlendMode::kDstATop: return blend_DstATop(source, dest);
        case GBlendMode::kXor: return blend_Xor(source, dest);
        default:
            return blend_Src(source, dest);
    }
}



#endif
