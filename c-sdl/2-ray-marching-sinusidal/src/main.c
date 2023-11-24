#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "./engine/global.h"

int main(int argc, char* argv[]) {
    render_init();
    time_init(50);
    while (!global.quit) {
        SDL_Event event;
        time_update();
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    global.quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    render_init_shader();
                    break;
            }
        }
        draw_screen();
        time_delay();
    }
    return 0;
}
