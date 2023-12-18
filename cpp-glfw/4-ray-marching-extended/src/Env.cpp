#include "Env.h"
#include "Render.h"

InputState::InputState() {
    memset(pressed, 0, 160 * sizeof(bool));
}

bool InputState::is_pressed(u32 glfw_key_code) {
    if (glfw_key_code <= GLFW_KEY_GRAVE_ACCENT) return pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_0) ];
    if (glfw_key_code >= GLFW_KEY_ESCAPE) return pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_1) ];
    return 0;
}
void InputState::set_key(u32 glfw_key_code, bool state) {
    if (glfw_key_code <= GLFW_KEY_GRAVE_ACCENT) pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_0) ] = state;
    if (glfw_key_code >= GLFW_KEY_ESCAPE) pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_1) ] = state;
}
InputState* curr_input;

void InputState::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    curr_input->set_key(key, action);
}

EnvState::EnvState(int width, int height, const std::string& title, float fps) : render(width, height, title), quit(false) {
    curr_input = &input;
    glfwSetKeyCallback(render.window, &InputState::key_callback);
}

bool EnvState::should_quit() {
 return glfwWindowShouldClose(render.window) || quit;
}

EnvState::~EnvState() {
}