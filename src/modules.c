#include "modules.h"

#include "defines.h"
#include "image.h"
#include "path.h"
#include "system.h"
#include "text.h"
#include "utils.h"

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

static CGAffineTransform subj_skew = { 1., 0., 0.24, 1.08, 0., 0. };
static CGAffineTransform body_skew = { 1., -0.08, 0.24, 1., 0., 0. };

void init_modules(struct module_t* modules) {
    struct info_t* m_info;
    struct note_t* m_note;

    modules[DATE].type = DATE;
    modules[DATE].data = malloc(sizeof(struct date_t));
    memset(modules[DATE].data, 0, sizeof(struct date_t));

    modules[INFO].type = INFO;
    modules[INFO].data = malloc(sizeof(struct info_t));
    memset(modules[INFO].data, 0, sizeof(struct info_t));

    modules[NOTE].type = NOTE;
    modules[NOTE].data = malloc(sizeof(struct note_t));
    memset(modules[NOTE].data, 0, sizeof(struct note_t));

    m_info = modules[INFO].data;
    m_info->font = create_font("Roboto Condensed:Bold:27.0");

    m_note = modules[NOTE].data;
    m_note->sfont = create_font("Helvetica:Bold:15.0");
    m_note->bfont = create_font("Helvetica:Regular:12.0");
}

void update_date(struct date_t* date, void* data) {
    time_t now;

    now = time(NULL);
    date->time = localtime(&now);
}

void update_info(struct info_t* info, void* data) {
    info->hdd = info_hdd();
    info->ram = info_ram();
    info->cpu = info_cpu();
    info->vol = info_vol();
    info->bat = info_bat();
}

void update_note(struct note_t* note, void* data) {
    char* text;

    strncpy(note->buffer, data, 256);

    if (strnlen(note->buffer, 256) == 256) {
        note->buffer[252] = '.';
        note->buffer[253] = '.';
        note->buffer[254] = '.';
        note->buffer[255] = '\0';
    }

    if ((text = split(note->buffer, '|')) == NULL)
        return;

    note->subj = note->buffer;
    note->body = text;

    *(--text) = '\0';
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

void render_note(CGContextRef context, struct note_t* note,
                 int32_t x, int32_t y) {
    int32_t x_[8], y_[8];

    x_[0] = x + 10;     y_[0] = y + 136;
    x_[1] = x + 60;     y_[1] = y + 21;
    x_[2] = x + 175;    y_[2] = y + 40;
    x_[3] = x + 154;    y_[3] = y + 121;
    x_[4] = x + 10;     y_[4] = y + 136;

    render_path(context, 5, x_, y_, 0x000000ff, NULL);

    x_[0] = x + 32;     y_[0] = y + 125;
    x_[1] = x + 72;     y_[1] = y + 36;
    x_[2] = x + 164;    y_[2] = y + 50;
    x_[3] = x + 146;    y_[3] = y + 113;
    x_[4] = x + 32;     y_[4] = y + 125;

    render_path(context, 5, x_, y_, 0xffffffff, NULL);

    x_[0] = x + 41;     y_[0] = y + 111;
    x_[1] = x + 73;     y_[1] = y + 40;
    x_[2] = x + 160;    y_[2] = y + 54;
    x_[3] = x + 145;    y_[3] = y + 101;
    x_[4] = x + 41;     y_[4] = y + 111;

    render_path(context, 5, x_, y_, 0xff0000ff, NULL);

    render_image(context, "data/message/haru.png", x + 40, y + 30);
    render_image(context, "data/message/frame.png", x + 132, y + 16);

    render_image(context, "data/message/light.png", 894, 530);

    CGContextSetRGBFillColor(context, 1.f, 1.f, 1.f, 1.f);

    x_[0] = x + 192;    y_[0] = y + 96;
    x_[1] = x + 192;    y_[1] = y + 72;
    x_[2] = x + 450;    y_[2] = y + 72;
    x_[3] = x + 450;    y_[3] = y + 96;
    x_[4] = x + 192;    y_[4] = y + 96;

    CGContextSetTextMatrix(context, subj_skew);
    render_frame(context, note->sfont, note->subj, 5, x_, y_, NULL);

    x_[0] = x + 180;    y_[0] = y + 75;
    x_[1] = x + 172;    y_[1] = y + 40;
    x_[2] = x + 450;    y_[2] = y + 40;
    x_[3] = x + 442;    y_[3] = y + 75;
    x_[4] = x + 180;    y_[4] = y + 75;

    CGContextSetTextMatrix(context, body_skew);
    render_frame(context, note->bfont, note->body, 5, x_, y_, NULL);

    CGContextSetTextMatrix(context, CGAffineTransformIdentity);
}

void update_module(struct module_t* mod, void* data) {
    switch (mod->type) {
        case DATE:
            update_date(mod->data, NULL);
            break;
        case INFO:
            update_info(mod->data, NULL);
            break;
        case NOTE:
            update_note(mod->data, data);
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
        case NOTE:
            render_note(context, mod->data, x, y);
            break;
        default:
            break;
    }
}
