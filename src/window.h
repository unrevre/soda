#ifndef WINDOW_H
#define WINDOW_H

#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGGeometry.h>

#include <stdint.h>

struct soda_t;

struct window_t {
    uint32_t id;
    CGContextRef context;
    CGRect frame;
};

void init_window(struct soda_t* soda);

void create_window(struct window_t* window, int32_t cid,
                   float x, float y, float w, float h);

#endif /* WINDOW_H */
