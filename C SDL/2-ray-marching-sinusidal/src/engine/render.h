#pragma once
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <linmath.h>

#include "types.h"

typedef struct render_state {
    SDL_Window* window;
    f32 width;
    f32 height;
    u32 shader;
} Render_State;

int render_init();

SDL_Window* render_init_window(u32 width, u32 height);

u32 create_shader(const char *path_vert, const char *path_frag);

u32 compile_shader(const char* path, GLenum type);

void draw_screen();