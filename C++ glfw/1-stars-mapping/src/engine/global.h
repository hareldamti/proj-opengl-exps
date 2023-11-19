#pragma once
#include "types.h"
#include "GLFW/glfw3.h"

typedef struct render_state {
    GLFWwindow* window;
    f32 width;
    f32 height;
    u32 shader;
} Render_State;

typedef struct global {
    Render_State render;
    bool quit;
} Global;