#include <thread>

#include "util.h"
#include "types.h"
#include "Render.h"
#include "App.h"

AppState* _app;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{ _app->input.set_key(key, action); }
int main(int argc, char** argv)
{
    AppState app(800, 600, argv[0], 20);
    _app = &app;
    glfwSetKeyCallback(app.render.window, key_callback);

    f32 vertices[16] = {
        -1,-1, 0, 0,
        -1, 1, 0, 1,
         1,-1, 1, 0,
         1, 1, 1, 1
    };
    u32 elements[6] = {0, 1, 2, 1, 2, 3};
    
    VertexBuffer vb(vertices, 16);
    IndexBuffer ib(elements, 6);
    Shader sh("./shaders/default.vert", "./shaders/default.frag");
    sh.set_attributes(2, "pos", 2, "uv", 2);

    while (!app.should_quit())
    {
        app.render.draw_triangles(sh, vb, ib);
        app.render.update_frame();
        glfwPollEvents();
    }
    
    return 0;
}