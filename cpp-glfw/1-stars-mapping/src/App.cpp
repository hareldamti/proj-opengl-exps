#include "App.h"
#include "Render.h"

InputState::InputState() {
    memset(pressed, 0, 160 * sizeof(bool));
}

bool InputState::is_pressed(u32 glfw_key_code) {
    if (glfw_key_code <= GLFW_KEY_GRAVE_ACCENT) return pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_0) ];
    if (glfw_key_code >= GLFW_KEY_ESCAPE) return pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_1) ];
}
void InputState::set_key(u32 glfw_key_code, bool state) {
    if (glfw_key_code <= GLFW_KEY_GRAVE_ACCENT) pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_0) ] = state;
    if (glfw_key_code >= GLFW_KEY_ESCAPE) pressed[ (glfw_key_code - INPUT_GLFW_KEY_OFFSET_1) ] = state;
}

AppState::AppState(int width, int height, const std::string& title, float fps) : render(width, height, title), quit(false) {
    render.shader = create_shader("./shaders/default.vert", "./shaders/default.frag");
}

bool AppState::should_quit() {
 return glfwWindowShouldClose(render.window) || quit;
}

AppState::~AppState() {
}