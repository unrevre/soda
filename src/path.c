#include "path.h"

#include "defines.h"

#include <CoreFoundation/CFBase.h>
#include <CoreGraphics/CGPath.h>

#include <stddef.h>

void render_path(CGContextRef context,
                 uint32_t n, int32_t* x, int32_t* y, uint32_t colour,
                 CGAffineTransform* transform) {
    int32_t i;

    if (n < 1 || n > 7)
        return;

    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, transform, x[0], y[0]);
    for (i = 1; i < n; ++i)
        CGPathAddLineToPoint(path, transform, x[i], y[i]);

    CGContextSetRGBFillColor(
        context,
        ((colour >> 0x18) & 0xff) / 255.,
        ((colour >> 0x10) & 0xff) / 255.,
        ((colour >> 0x08) & 0xff) / 255.,
        ((colour >> 0x00) & 0xff) / 255.);
    CGContextAddPath(context, path);
    CGContextFillPath(context);

    CFRelease(path);
}

void render_box(CGContextRef context,
                int32_t x, int32_t y, int32_t w, int32_t h, uint32_t colour,
                CGAffineTransform* transform) {
    int32_t x_[8], y_[8];

    x_[0] = x;      y_[0] = y;
    x_[1] = x + w;  y_[1] = y;
    x_[2] = x + w;  y_[2] = y + h;
    x_[3] = x;      y_[3] = y + h;
    x_[4] = x;      y_[4] = y;

    render_path(context, 5, x_, y_, colour, transform);
}

void render_bar(CGContextRef context,
                int32_t x, int32_t y, int32_t w, int32_t h, int32_t b) {
    render_box(context,     x,     y, w + 2 * b,         h, C_BLK, NULL);
    render_box(context, x + b, y + b,         w, h - 2 * b, C_RED, NULL);
}
