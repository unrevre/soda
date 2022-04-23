#include "window.h"

#include "soda.h"

#include "CoreFoundation/CFBase.h"
#include "CoreFoundation/CFDictionary.h"
#include <CoreGraphics/CGDirectDisplay.h>
#include <CoreGraphics/CGError.h>
#include <CoreGraphics/CGWindowLevel.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern int SLSMainConnectionID(void);

extern CGError CGSNewRegionWithRect(CGRect* rect, CFTypeRef* region);
extern CGError SLSNewWindow(int cid, int type, float x, float y,
                            CFTypeRef region, uint32_t* wid);
extern CGError SLSSetWindowTags(int cid, uint32_t wid, uint64_t* tags,
                                int size);
extern CGError SLSClearWindowTags(int cid, uint32_t wid, uint64_t* tags,
                                  int size);
extern CGError SLSSetWindowResolution(int cid, uint32_t wid, double res);
extern CGError SLSSetWindowOpacity(int cid, uint32_t wid, bool opaque);
extern CGError SLSSetWindowAlpha(int cid, uint32_t wid, float alpha);
extern CGError SLSSetWindowLevel(int cid, uint32_t wid, int level);
extern CGContextRef SLWindowContextCreate(int cid, uint32_t wid,
                                          CFDictionaryRef options);

#define kCGSDisableShadowTagBit         (1UL << 3)
#define kCGSOnAllWorkspacesTagBit       (1UL << 11)
#define kCGSIgnoreForExposeTagBit       (1UL << 7) << 32

void init_window(struct soda_t* soda) {
    soda->cid = SLSMainConnectionID();
    soda->win = malloc(sizeof(struct window_t));
    memset(soda->win, 0, sizeof(struct window_t));

    create_window(soda->win, soda->cid, 0.f, 0.f, 1.f, 1.f);
}

void create_window(struct window_t* window, int32_t cid,
                   float x, float y, float w, float h) {
    CGRect bounds;
    int fx, fy, fw, fh;
    uint64_t set_tags, clr_tags;

    bounds = CGDisplayBounds(CGMainDisplayID());

    fx = (int)(bounds.origin.x + (x * bounds.size.width));
    fy = (int)(bounds.origin.y + (y * bounds.size.height));
    fw = (int)(w * bounds.size.width);
    fh = (int)(h * bounds.size.height);

    window->frame = (CGRect) {{fx, fy}, {fw, fh}};

    CFTypeRef region;
    CGSNewRegionWithRect(&window->frame, &region);

    set_tags = kCGSDisableShadowTagBit
        | kCGSOnAllWorkspacesTagBit
        | kCGSIgnoreForExposeTagBit;
    clr_tags = 0x200000000000;

    SLSNewWindow(cid, 2, 0, 0, region, &window->id);
    SLSSetWindowTags(cid, window->id, &set_tags, 64);
    SLSClearWindowTags(cid, window->id, &clr_tags, 64);
    SLSSetWindowResolution(cid, window->id, 2.f);
    SLSSetWindowOpacity(cid, window->id, 0.f);
    SLSSetWindowLevel(cid, window->id, CGWindowLevelForKey(3));

    window->context = SLWindowContextCreate(cid, window->id, 0);
}
