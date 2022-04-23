#ifndef TEXT_H
#define TEXT_H

#include <CoreGraphics/CGContext.h>
#include <CoreText/CTFont.h>

enum ALIGN {
    LEFT,
    CENTRE,
    RIGHT,
};

CTFontRef create_font(char* name);

void render_line(CGContextRef context, CTFontRef font, const char* text,
                 float x, float y, enum ALIGN xalign);

#endif /* TEXT_H */
