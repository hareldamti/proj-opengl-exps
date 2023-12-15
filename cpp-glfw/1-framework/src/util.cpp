#include "util.h"
#include "render.h"

std::string read_file(const std::string& file_path) {
    std::ifstream stream(file_path);
    std::string data((std::istreambuf_iterator<char>(stream)),
                 std::istreambuf_iterator<char>());
    return data;
}

void _glClearError() {
    while(glGetError() != GL_NO_ERROR);
}
char* _glErrorString(u32 error) {
    switch (error) {
        case (0x0500): return "Invalid enum";
        case (0x0501): return "Invalid value";
        case (0x0502): return "Invalid operation";
        case (0x0503): return "Stack overflow";
        case (0x0504): return "Stack underflow";
        case (0x0505): return "Out of memory";
        case (0x0506): return "Invalid framebuffer operation";
        case (0x0507): return "Context lost";
        case (0x8301): return "Table too large";
        default: return "Unknown error";
    }
}
bool _glCatchError(const char* func_name, const char* file, int line) {
    bool r = true;
    u32 error;
    while((error = glGetError()) != GL_NO_ERROR) {
        LOG_ERROR("%s %s:%d - (GL %d) %s\n", func_name, file, line, error, _glErrorString(error));
        r = false;
    }
    ASSERT(r);
    return r;
}

ImGuiUtil::ImGuiUtil(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}
ImGuiIO& ImGuiUtil::io() { return ImGui::GetIO(); }
void ImGuiUtil::render_begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void ImGuiUtil::render_end() {
    ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImGuiUtil::~ImGuiUtil() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}