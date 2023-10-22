#pragma once

#include <stdbool.h>
#include "render.h"
#include "time.h"
#include "io.h"

//#include "input/input.h"
//#include "config/config.h"
//#include "time/time.h"

typedef struct global {
    Render_State render;
    Time_State time;
    
    //Config_State config;
    //Input_State input;
    bool quit;
    float* data; // Temporary, need to think of data passing from main to render
} Global;

extern Global global;