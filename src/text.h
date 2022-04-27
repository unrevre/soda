#ifndef TEXT_H
#define TEXT_H

#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGAffineTransform.h>
#include <CoreText/CTFont.h>

#include <stdint.h>

enum ALIGN {
    LEFT,
    CENTRE,
    RIGHT,
};

CTFontRef create_font(char* name);

void render_line(CGContextRef context, CTFontRef font, const char* text,
                 float x, float y, enum ALIGN xalign);
void render_frame(CGContextRef context, CTFontRef font, const char* text,
                  uint32_t n, int32_t* x, int32_t* y,
                  CGAffineTransform* transform);

#endif /* TEXT_H */
