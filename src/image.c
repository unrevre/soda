#include "image.h"

#include <CoreGraphics/CGColorSpace.h>
#include <CoreGraphics/CGDataProvider.h>
#include <CoreGraphics/CGGeometry.h>
#include <CoreGraphics/CGImage.h>

#include <stdbool.h>
#include <stddef.h>

void render_image(CGContextRef context, const char* path,
                  int32_t x, int32_t y) {
    CGDataProviderRef data;
    CGImageRef image;
    CGRect frame;

    int32_t w, h;

    data = CGDataProviderCreateWithFilename(path);
    image = CGImageCreateWithPNGDataProvider(
        data, NULL, false, kCGRenderingIntentDefault);

    w = CGImageGetWidth(image);
    h = CGImageGetHeight(image);

    frame = (CGRect){{x, y}, {w, h}};
    CGContextDrawImage(context, frame, image);
}
