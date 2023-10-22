#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "engine/global.h"
#include "engine/config/config.h"
#include "engine/input/input.h"
#include "engine/time/time.h"

static bool quit = false;
static vec2 pos;

static void input_handle() {
    if (global.input.left == KS_PRESSED || global.input.left == KS_HELD)
        pos[0] -= 500 * global.time.delta;
    if (global.input.right == KS_PRESSED || global.input.right == KS_HELD)
        pos[0] += 500 * global.time.delta;
    if (global.input.escape == KS_PRESSED)
        quit = true;
}

int main(int argc, char* argv[]) {
    render_init();
    config_init();

    pos[0] = global.render.width /2;
    pos[1] = global.render.height /2;

    while (!quit) {
        time_update();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                default:
                    break;
            }
        }
        

        input_update();
        input_handle();
        printf("%f\n", pos[0]);
        render_begin();
        render_quad(
            pos,
            (vec2){50, 50},
            (vec4){0, 1, 0, 1});
        render_quad(
            (vec2){global.render.width * .75, global.render.height * .25},
            (vec2){50, 50},
            (vec4){1, 0, 0, 1});
        render_end();
        time_update_late();
    }
    return 0;
}