#include "text.h"

#include <CoreFoundation/CFAttributedString.h>
#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CFString.h>
#include <CoreGraphics/CGBase.h>
#include <CoreGraphics/CGGeometry.h>
#include <CoreGraphics/CGPath.h>
#include <CoreText/CTFontDescriptor.h>
#include <CoreText/CTFrame.h>
#include <CoreText/CTFramesetter.h>
#include <CoreText/CTLine.h>
#include <CoreText/CTStringAttributes.h>

#include <stdio.h>
#include <string.h>

CTFontRef create_font(char* name) {
    float size;
    char props[2][255];

    size = 12.0f;
    memset(props, 0, sizeof(props));

    sscanf(name, "%254[^:]:%254[^:]:%f", props[0], props[1], &size);
    CFStringRef font_family_name = CFStringCreateWithCString(NULL, props[0],
        kCFStringEncodingUTF8);
    CFStringRef font_style_name = CFStringCreateWithCString(NULL, props[1],
        kCFStringEncodingUTF8);
    CFNumberRef font_size = CFNumberCreate(NULL, kCFNumberFloat32Type, &size);

    const void* keys[] = {
        kCTFontFamilyNameAttribute,
        kCTFontStyleNameAttribute,
        kCTFontSizeAttribute,
    };

    const void* values[] = {
        font_family_name,
        font_style_name,
        font_size,
    };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values,
        (sizeof(keys) / sizeof(keys[0])), &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
    CTFontDescriptorRef desc = CTFontDescriptorCreateWithAttributes(dict);
    CTFontRef font = CTFontCreateWithFontDescriptor(desc, 0.0, NULL);

    CFRelease(desc);
    CFRelease(dict);
    CFRelease(font_size);
    CFRelease(font_style_name);
    CFRelease(font_family_name);

    return font;
}

void render_line(CGContextRef context, CTFontRef font, const char* text,
                 float x, float y, enum ALIGN xalign) {
    const void* keys[] = {
        kCTFontAttributeName,
        kCTForegroundColorFromContextAttributeName
    };

    const void* values[] = {
        font,
        kCFBooleanTrue
    };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values,
        (sizeof(keys) / sizeof(keys[0])), &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
    CFStringRef string = CFStringCreateWithCString(NULL, text,
        kCFStringEncodingUTF8);
    CFAttributedStringRef attrs = CFAttributedStringCreate(NULL, string, dict);
    CTLineRef line = CTLineCreateWithAttributedString(attrs);

    CGRect bounds = CTLineGetBoundsWithOptions(line,
        kCTLineBoundsUseGlyphPathBounds);

    switch (xalign) {
        case LEFT:
            break;
        case CENTRE:
            x = x - bounds.size.width / 2;
            break;
        case RIGHT:
            x = x - bounds.size.width;
            break;
    }

    CGFloat ascent, descent;
    CTLineGetTypographicBounds(line, &ascent, &descent, NULL);

    y = y - (ascent - descent) / 2;

    CGContextSetTextPosition(context, x, y);
    CTLineDraw(line, context);

    CFRelease(line);
    CFRelease(attrs);
    CFRelease(string);
    CFRelease(dict);
}

void render_frame(CGContextRef context, CTFontRef font, const char* text,
                  uint32_t n, int32_t* x, int32_t* y,
                  CGAffineTransform* transform) {
    int32_t i;

    if (n < 1 || n > 7)
        return;

    CGMutablePathRef path = CGPathCreateMutable();
    CGPathMoveToPoint(path, transform, x[0], y[0]);
    for (i = 1; i < n; ++i)
        CGPathAddLineToPoint(path, transform, x[i], y[i]);

    const void* keys[] = {
        kCTFontAttributeName,
        kCTForegroundColorFromContextAttributeName
    };

    const void* values[] = {
        font,
        kCFBooleanTrue
    };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values,
        (sizeof(keys) / sizeof(keys[0])), &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
    CFStringRef string = CFStringCreateWithCString(NULL, text,
        kCFStringEncodingUTF8);
    CFAttributedStringRef attrs = CFAttributedStringCreate(NULL, string, dict);

    CTFramesetterRef setter = CTFramesetterCreateWithAttributedString(attrs);
    CTFrameRef frame = CTFramesetterCreateFrame(setter, CFRangeMake(0, 0),
        path, NULL);

    CTFrameDraw(frame, context);

    CFRelease(frame);
    CFRelease(setter);
    CFRelease(attrs);
    CFRelease(string);
    CFRelease(dict);
    CFRelease(path);
}
