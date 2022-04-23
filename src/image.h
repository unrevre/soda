#ifndef IMAGE_H
#define IMAGE_H

#include <CoreGraphics/CGContext.h>

#include <stdint.h>

void render_image(CGContextRef context, const char* path,
                  int32_t x, int32_t y);

#endif /* IMAGE_H */
