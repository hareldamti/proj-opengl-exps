#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "./engine/global.h"

void handle_keypress(SDL_Event event) {
    int sign = event.type == SDL_KEYDOWN ? 1: -1;
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_UP:
            global.data[7] += sign;
            break;
        case SDL_SCANCODE_DOWN:
            global.data[7] -= sign;
            break;
        case SDL_SCANCODE_RIGHT:
            global.data[6] -= sign;
            break;
        case SDL_SCANCODE_LEFT:
            global.data[6] += sign;
            break;
        case SDL_SCANCODE_W:
            global.data[8] += sign;
            break;
        case SDL_SCANCODE_S:
            global.data[8] -= sign;
            break;
    }
    for (int i = 6; i < 9; i++)
        if (abs(global.data[i]) > 0.5)
            global.data[i] /= abs(global.data[i]);
}

int main(int argc, char* argv[]) {
    render_init();
    time_init(50);

    global.data = malloc(9 * sizeof(float));
    for (int i = 0; i < 9; i++) global.data[i] = 0;

    while (!global.quit) {
        glClear(GL_COLOR_BUFFER_BIT);
        SDL_Event event;
        time_update();
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    global.quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    render_init_shader();
                    time_init(50);
                    time_update();
                    printf("Shader reset\n");
                    break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    handle_keypress(event);
                    break;
                //case SDL_KEYUP:
            }
        }
        for (int j = 0; j < 3; j++) {
            for (int i = 1; i >= 0; i--) 
                global.data[j + 3*i] += (j == 2 ? 0.02 : 0.007) *global.data[j + 3*(i+1)];
            global.data[j + 3] *= .994;
            //if (j != 2) global.data[j + 3] -= 0.07*global.data[j];
        }
        
        draw_screen();
        time_delay();
    }
    return 0;
}
