#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "types.h"

typedef struct mouse {
    int x, y;
    bool down;
} Mouse;

typedef struct input_state {
    Mouse mouse;
} Input_State;