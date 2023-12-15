#include <thread>

#include "Render.h"
#include "App.h"


int main(int argc, char** argv)
{
    AppState app(800, 600, argv[0], 50);
    
    ImGuiUtil imgui_util(app.render.window);

    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    f32 default_vertices[16] = {
        -1,-1, 0, 0,
        -1, 1, 0, 1,
         1,-1, 1, 0,
         1, 1, 1, 1
    };
    u32 default_elements[6] = {0, 1, 2, 1, 2, 3};
    VertexBuffer vb(16);
    IndexBuffer ib(default_elements, 6);
    RenderShader sh("./shaders/default.vert", "./shaders/default.frag");
    sh.set_attributes(2, "pos", 2, "uv", 2);

    while (!app.should_quit())
    {
        glfwPollEvents();
        app.render.clear_frame();
        imgui_util.render_begin();

        vb.load(default_vertices);
        app.render.draw_triangles(sh, vb, ib);

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / imgui_util.io().Framerate, imgui_util.io().Framerate);
            ImGui::End();
        }

        imgui_util.render_end();
        app.render.update_frame();
        
    }

    return 0;
}