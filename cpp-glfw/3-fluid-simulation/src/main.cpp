#include "Render.h"
#include "App.h"
#include "../src/FluidSim.cpp"

int main(int argc, char** argv)
{
    AppState app(800,600, argv[0], 50);
    ImGuiUtil imgui_util(app.render.window);
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
    RenderShader sh("./shaders/default.vert", "./shaders/default.frag");
    sh.set_attributes(2, "pos", 2, "uv", 2);
    FluidSim f(app);

    app.render.screen_tex.set(0,0,0,1);
    app.render.screen_tex.set_to_gpu();
    LOG_DEBUG("     %f    \n", app.render.screen_tex.get(0,0,0));
    
    app.render.screen_tex.get_from_gpu();
    LOG_DEBUG("     %f    ", app.render.screen_tex.get(0,0,0));
    return 0;
    while (!app.should_quit())
    {
        glfwPollEvents();
        f.update();
        f.draw();

        app.render.clear_frame();
        imgui_util.render_begin();
        app.render.draw_triangles(sh, vb, ib);
        imgui_util.render_end();
        app.render.update_frame();
    }
    return 0;
}
