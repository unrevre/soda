#ifndef PATH_H
#define PATH_H

#include <CoreGraphics/CGAffineTransform.h>
#include <CoreGraphics/CGContext.h>

#include <stdint.h>

void render_path(CGContextRef context,
                 uint32_t n, int32_t* x, int32_t* y, uint32_t colour,
                 CGAffineTransform* transform);

void render_box(CGContextRef context,
                int32_t x, int32_t y, int32_t w, int32_t h, uint32_t colour,
                CGAffineTransform* transform);

void render_bar(CGContextRef context,
                int32_t x, int32_t y, int32_t w, int32_t h, int32_t b);

#endif /* PATH_H */
