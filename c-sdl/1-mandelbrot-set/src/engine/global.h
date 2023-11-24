#pragma once

#include <stdbool.h>
#include "render.h"
#include "time.h"
#include "input.h"
#include "system.h"

typedef struct global {
    Render_State render;
    Time_State time;
    //Config_State config;
    System_State system;
    Input_State input;
    bool quit;
} Global;

extern Global global;