#include "Render.h"
#include "Env.h"
#include "../src/App.cpp"

int main(int argc, char** argv) {
    int width = 800, height = 600;
    if (argc == 3) {
        width = std::stoi(argv[1]);
        height = std::stoi(argv[2]);
    }
    EnvState env(width, height, argv[0], 50);
    ImGuiUtil imgui_util(env.render.window);
    f32 default_vertices[16] = {
        -1,-1, 0, 0,
        -1, 1, 0, 1,
         1,-1, 1, 0,
         1, 1, 1, 1
    };
    u32 default_elements[6] = {0, 1, 2, 1, 2, 3};
    VertexBuffer vb(16);
    vb.load(default_vertices);
    IndexBuffer ib(default_elements, 6);
    
    
    App app(env);
    while (!env.should_quit()){
        glfwPollEvents();
        app.update();
        env.render.clear_frame();
        app.draw();
        env.render.draw_triangles(vb, ib);
        imgui_util.render_begin();
        app.imgui();
        imgui_util.render_end();
        env.render.update_frame();
    }
    return 0;
}
