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
        Mouse down_position, curr;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    global.quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
                        render_init_shader();
                        //time_init(50);
                        //time_update();
                        printf("Shader reset\n");
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    global.input.mouse.down = true;
                    SDL_GetMouseState( &down_position.x, &down_position.y );
                    down_position.x -= global.input.mouse.x;
                    down_position.y -= global.input.mouse.y;
                    break;
                case SDL_MOUSEBUTTONUP:
                    global.input.mouse.down = false;
                    break;
                case SDL_MOUSEMOTION:
                    if (global.input.mouse.down) {
                        SDL_GetMouseState( &curr.x, &curr.y );
                        Mouse new;
                        new.x = curr.x - down_position.x;
                        new.y = curr.y - down_position.y;

                        global.system.x -= (new.x - global.input.mouse.x) / global.system.zoom;
                        global.system.y -= (new.y - global.input.mouse.y) / global.system.zoom;

                        global.input.mouse.x = new.x;
                        global.input.mouse.y = new.y;
                    }
                    break;
            }
        }
        global.system.zoom = exp(global.time.now / 1e4);
        draw_screen();
        time_delay();
    }
    return 0;
}