# 2DComputerGraphics
## PA1: Canvas & Blending
This module turns floating-point colors into premultiplied ARGB pixels, writes them into a bitmap, blends rectangles with full Porter–Duff compositing, and extends the PA1 canvas with a convex polygon rasterizer using fast integer math — forming the basic building blocks of a 2D software rasterizer.

### Features

- **Premultiplied ARGB pixels**  
  Convert floating-point colors into premultiplied 8-bit ARGB format for efficient compositing.

- **Source-over (Porter–Duff) blending**  
  Implements alpha blending with:
  - A fast integer-based version using `div255` optimization.  
  - A slower reference version for correctness testing.

- **Canvas operations**  
  - `clear(color)`: fills the entire canvas with a solid color.  
  - `fillRect(rect, color)`: draws an axis-aligned rectangle with alpha blending.

- **Robustness**  
  - Rectangle bounds are clamped to the canvas.  
  - Fully opaque pixels use a fast path to skip blending.

## PA2: Blending & Convex Polygon Rasterization
This module extends the PA1 canvas with full Porter–Duff blending and a convex polygon rasterizer, all in **premultiplied ARGB** using fast integer math.

### Features
- **Blending Modes (Porter–Duff)**
  - `Clear, Src, Dst, SrcOver, DstOver, SrcIn, DstIn, SrcOut, DstOut, SrcATop, DstATop, Xor`
  - Fast integer compositing via `multiplyPixel((x*y)/255)`
  - Mode **preprocessing**: simplifies work when `Sa==0` or `Sa==255`

- **Canvas Operations**
  - `clear(color)`: fill device with a solid premultiplied color
  - `drawRect(rect, paint)`: row-based blitter with fast paths for `Src`, `Clear`, `Dst`

- **Convex Polygon Rasterization**
  - Build/sort edges with slopes and scanline `currentX`
  - Clip edges to device bounds (vertical/horizontal cases handled)
  - Scanline fill by emitting 1-pixel-high rects and blending with the selected mode

## PA3:

## PA4:
## PA5:
## PA6:
## Final:
