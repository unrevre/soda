#ifndef MODULES_H
#define MODULES_H

#include <CoreGraphics/CGContext.h>
#include <CoreText/CTFont.h>

#include <stdint.h>

enum MODULE {
    DATE,
    INFO,
    NMOD
};

struct module_t {
    enum MODULE type;
    int32_t active;

    void* data;
};

struct date_t {
    struct tm* time;
};

struct info_t {
    int32_t hdd;
    int32_t ram;
    int32_t cpu;
    int32_t vol;
    int32_t bat;

    CTFontRef font;
};

void init_modules(struct module_t* modules);

void update_module(struct module_t* mod);
void render_module(struct module_t* mod, CGContextRef context,
                   int32_t x, int32_t y);

#endif /* MODULES_H */
