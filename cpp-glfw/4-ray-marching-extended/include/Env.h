#pragma once
#include "types.h"
#include "util.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "render.h"

#define INPUT_GLFW_KEY_OFFSET_0 32
#define INPUT_GLFW_KEY_OFFSET_1 191

class InputState {
    private:
        bool pressed[160];
    public:
        InputState();
        bool is_pressed(u32 glfw_key_code);
        void set_key(u32 glfw_key_code, bool state);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

class EnvState {
    public:
        RenderState render;
        InputState input;
        bool quit;

        EnvState(int width, int height, const std::string& title, float fps);
        bool should_quit();
        ~EnvState();

};