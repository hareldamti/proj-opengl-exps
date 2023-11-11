#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "./engine/global.h"

int main(int argc, char* argv[]) {
    render_init();
    time_init(50);

    global.data = malloc(9 * sizeof(float));
    for (int i = 0; i < 9; i++) global.data[i] = 0;

    while (!global.quit) {
        SDL_Event event;
        time_update();
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    global.quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                        render_init_shader();
                        time_init(50);
                        time_update();
                        printf("Shader reset\n");
                    }
                    break;
            }
        }
        set_uniform1f("time_now", global.time.now);
        draw_screen();
        time_delay();
    }
    return 0;
}
