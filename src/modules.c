#include "modules.h"

#include "defines.h"
#include "image.h"
#include "path.h"
#include "system.h"
#include "text.h"

#include <CoreGraphics/CGAffineTransform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char* mon_str[12] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec",
};

static const char* mday_str[10] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
};

static const char* wday_str[7] = {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat",
};

static const char* tofd_str[24] = {
    "4", "4", "4", "4", "4", "4", "0", "0", "0", "1", "1", "1",
    "2", "2", "2", "2", "2", "2", "3", "3", "3", "4", "4", "4",
};

static const char* title[5] = {
    "HDD", "RAM", "CPU", "VOL", "BAT",
};

static CGAffineTransform info_skew = { 1., 0.06, 0., 1., 0., 0. };
static CGAffineTransform info_inv = { 1., -0.06, 0., 1., 0., 0. };

void init_modules(struct module_t* modules) {
    struct info_t* m_info;

    modules[DATE].type = DATE;
    modules[DATE].data = malloc(sizeof(struct date_t));
    memset(modules[DATE].data, 0, sizeof(struct date_t));

    modules[INFO].type = INFO;
    modules[INFO].data = malloc(sizeof(struct info_t));
    memset(modules[INFO].data, 0, sizeof(struct info_t));

    m_info = modules[INFO].data;
    m_info->font = create_font("Roboto Condensed:Bold:27.0");
}

void update_date(struct date_t* date) {
    time_t now;

    now = time(NULL);
    date->time = localtime(&now);
}

void update_info(struct info_t* info) {
    info->hdd = info_hdd();
    info->ram = info_ram();
    info->cpu = info_cpu();
    info->vol = info_vol();
    info->bat = info_bat();
}

void render_date(CGContextRef context, struct date_t* date,
                 int32_t x, int32_t y) {
    int32_t i, mon, mday, wday, hour;
    char buffer[64];

    mon = date->time->tm_mon;
    mday = date->time->tm_mday;
    wday = date->time->tm_wday;
    hour = date->time->tm_hour;

    for (i = 0; i < 3; ++i) {
        sprintf(buffer, "data/month/%s_l%i.png", mon_str[mon], i);
        render_image(context, buffer, x, y + 32);

        sprintf(buffer, "data/date/%s_l%i.png", mday_str[mday / 10], i);
        render_image(context, buffer, x + 64, y + 40);

        sprintf(buffer, "data/date/%s_l%i.png", mday_str[mday % 10], i);
        render_image(context, buffer, x + 116, y + 48);

        sprintf(buffer, "data/day/%s_l%i.png", wday_str[wday], i);
        render_image(context, buffer, x + 40, y);
    }

    sprintf(buffer, "data/time/%s.png", tofd_str[hour]);
    render_image(context, buffer, x + 200, y + 20);
}

void render_info(CGContextRef context, struct info_t* info,
                 int32_t x, int32_t y) {
    int32_t i, b, r, l, w, h;
    int32_t xl, yl;
    int32_t* value;
    char label[5][8];

    b = 10;
    r = 40;
    l = r - b;

    w = 160;
    h = r + l * 4;

    CGContextConcatCTM(context, info_skew);

    render_box(context, x, y, w, h, C_BLK, NULL);

    value = (int32_t*)info;

    for (i = 0; i < 5; ++i)
        render_bar(context, x + w, y + l * i, value[i] * 4, r, b);

    CGContextSetRGBFillColor(context, 1.f, 1.f, 1.f, 1.f);

    for (i = 0; i < 5; ++i) {
        xl = x + 40 + (i % 2) * 12;
        yl = y + l * i + r / 2;

        render_line(context, info->font, title[i], xl, yl, LEFT);
    }

    for (i = 0; i < 5; ++i) {
        snprintf(&label[i][0], 8, "%i", value[i]);

        xl = x + w;
        yl = y + l * i + r / 2;

        render_line(context, info->font, &label[i][0], xl, yl, RIGHT);
    }

    CGContextConcatCTM(context, info_inv);
}

void update_module(struct module_t* mod) {
    switch (mod->type) {
        case DATE:
            update_date(mod->data);
            break;
        case INFO:
            update_info(mod->data);
            break;
        default:
            break;
    }
}

void render_module(struct module_t* mod, CGContextRef context,
                   int32_t x, int32_t y) {
    if (!mod->active)
        return;

    switch (mod->type) {
        case DATE:
            render_date(context, mod->data, x, y);
            break;
        case INFO:
            render_info(context, mod->data, x, y);
            break;
        default:
            break;
    }
}
