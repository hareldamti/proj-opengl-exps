#ifndef GLOBAL_H
#define GLOBAL_H

#include "render/render.h"
#include "input/input.h"
#include "config/config.h"
#include "time/time.h"

typedef struct global {
    Render_State render;
    Config_State config;
    Input_State input;
    Time_State time;
} Global;

extern Global global;

#endif;