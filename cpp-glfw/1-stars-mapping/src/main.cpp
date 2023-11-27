#include <thread>

#include "util.h"
#include "types.h"
#include "Render.h"
#include "App.h"


#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

AppState* _app;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    _app->input.set_key(key, action);
}

int main(int argc, char** argv)
{
    AppState app(800, 600, argv[0], 20);
    _app = &app;
    glfwSetKeyCallback(app.render.window, key_callback);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(app.render.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    f32 vertices[16] = {
        -1,-1, 0, 0,
        -1, 1, 0, 1,
         1,-1, 1, 0,
         1, 1, 1, 1
    };
    u32 elements[6] = {0, 1, 2, 1, 2, 3};
    
    VertexBuffer vb(16);
    IndexBuffer ib(elements, 6);
    Shader sh("./shaders/default.vert", "./shaders/default.frag");
    sh.set_attributes(2, "pos", 2, "uv", 2);

    while (!app.should_quit())
    {
        vb.load(vertices);

        app.render.clear_frame();
        app.render.draw_triangles(sh, vb, ib);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        app.render.update_frame();
        glfwPollEvents();
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}