# 2DComputerGraphics
## PA1: Canvas & Blending
This module turns floating-point colors into premultiplied ARGB pixels, writes them into a bitmap, blends rectangles with full Porter–Duff compositing, and extends the canvas with a convex polygon rasterizer using fast integer math — forming the basic building blocks of a 2D software rasterizer.

### New Features

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

### New Features
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

## PA3: Shaders & Transforms
This module extends the Assignment 2 canvas with a **bitmap shader system**, a **matrix/CTM transform stack**, and **shader-aware drawing paths** for rectangles and convex polygons. 
Together with full Porter–Duff blending and convex polygon rasterization, this provides a flexible, shader-driven pipeline

### New Features

- **Bitmap Shader**
  - Implements `MyShader` for sampling from a bitmap with nearest-neighbor lookup.
  - Supports `setContext(ctm)` with matrix inversion for device-to-local mapping.
  - Provides `shadeRow(x,y,count,row[])` for per-row color generation.
  - Reports opacity via `isOpaque()` for blend-mode simplification.

- **Matrix & Transform Stack**
  - Added `GMatrix` with `Translate`, `Scale`, `Rotate`, `Concat`, `Invert`, and `mapPoints`.
  - Canvas maintains a stack of current transforms (`save`, `restore`, `concat`).
  - Shapes are drawn in device space by mapping points through the CTM.

- **Shader-Aware Drawing**
  - Rectangles and convex polygons:
    - Solid-color fast path when no shader is present.
    - Per-row shader sampling when a shader is provided.
  - Blend modes are preprocessed to skip unnecessary work when possible.

- **Convex Polygon Rasterizer (Refactor)**
  - Edges and clipping factored into `edge.h`.
  - Robust scanline fill with edge sorting and clipping to device bounds.

## PA4: Paths & Gradients
This module introduces **path rendering** with the non-zero winding rule and a **linear gradient shader**, along with stronger edge handling.
### New Features
- **Path Drawing**
  - Added `drawPath` to rasterize arbitrary paths using edge lists and the non-zero winding rule.
  - Path segments (`lineTo`, `moveTo`) are converted into edges and clipped against the device.
  - Integrates with the CTM stack and shaders, so transformed and shaded paths can be filled correctly.

- **Improved Edge Handling**
  - Extended the edge pipeline with explicit **winding numbers** for correct non-zero winding fills.
  - Added robust clipping in both X and Y to keep edges constrained to device bounds.

- **Linear Gradient Shader**
  - Implemented `LinearGradientShader`, supporting both simple 2-color gradients and multi-stop gradients.
  - Computes gradients in local space using inverse transforms of the CTM.
  - Interpolates premultiplied ARGB colors per pixel, reports opacity for blend-mode optimization.

## PA5: Extend Shaders & Blending
This module adds a **generalized shader system** and a **blend mode pipeline**, allowing draw operations to generate pixels through shaders and then combine them with the destination image using configurable blend rules.
### New Features
- **Blend Modes**
  -  Introduced a blending framework (blend.h) with support for standard blend operations (e.g., Src, SrcOver).
  -  All raster operations now pass colors through the blend pipeline before writing to the device.
- **Shader Interface**
  -  Formalized the Shader base class with required methods such as isOpaque() and shadeRow().
  -  Added MyShader (MyShader.cpp) as an example shader implementation.
  -  Shaders now integrate directly with the CTM and supply per-pixel colors to the rasterizer.
- **Canvas Integration**
  -  Updated my_canvas.cpp so all draw calls (paths, rects, gradients) request colors from the active shader and then apply blending.
  -  Ensures consistency between geometric rasterization (edges/paths) and color computation.
## PA6:
This assignment extends PA5 by adding support for triangle meshes, quad patches, and more advanced shader composition. The focus is on **per-triangle transforms, texture mapping, and color interpolation**.
### New Features
- **Triangle Meshes**
  - Renders arbitrary sets of triangles.
  - Three cases supported per triangle:
    - Vertex colors only → shaded via a TriangleShader (barycentric interpolation).
    - Texcoords only → texture mapping through a ProxyShader, which injects a per-triangle device→local transform.
    - Both colors & texcoords → combines a TriangleShader (colors) and proxied texture shader into a composite shader, shaded in one pass.
- **Quad Patches**
  -  Subdivides a 4-point patch into an N×N grid.
  -  Bilinearly interpolates positions, colors, and texcoords across the patch.
  -  Triangulates each cell and draws it using drawMesh.
- **TriangleShader**
  - Uses barycentric interpolation for per-pixel color from triangle vertices.
  - Computes a 2×3 mapping matrix from geometry, inverted with CTM.
  - Reports opacity when all input vertex colors are opaque.
- **ProxyShader**
  - Wraps an existing shader with an extra local transform.
  - Forwards isOpaque and shadeRow calls, but applies ctm * extraTransform.
  - Enables per-triangle texture mapping without modifying the underlying shader.
## Final:
In the final implementation, I introduced 2 new shader: SweepGradientShader and VoronoiShader
### New Features
- **SweepGradientShader**
  - Implements angular (sweep) gradients, where colors are determined by the angle around a central pivot. Extends gradient variety beyond the linear/radial gradients of PA6.
- **VoronoiShader**
  - Introduces Voronoi-based rendering, shading each pixel based on proximity to a set of seed points. Adds a non-linear, distance-based artistic fill capability not present in PA6.
