#include <thread>

#include "util.h"
#include "types.h"
#include "Render.h"

int main(int argc, char** argv)
{
    GLFWwindow* window = render_init_window(200,200,"a");
    int success;
    char log[512];
    u32 cs = compile_shader(read_file("./shaders/default.comp"), GL_COMPUTE_SHADER);
    u32 program = glCreateProgram();
    glAttachShader(program, cs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        ERROR_EXIT("Error linking shader. %s\n", log);
    }
    GLCall(glDeleteShader(cs));


    f32 v[16] = {-1, -1, 0, 0, -1, 1, 0, 1, 1, -1, 1, 0, 1, 1, 1, 1};
    u32 i[6] = {0, 1, 2, 1, 2, 3};
    VertexBuffer vb(16);
    IndexBuffer ib(i, 6);
    RenderShader sh("./shaders/default.vert", "./shaders/default.frag");
    sh.set_attributes(2, "pos", 2, "uv", 2);
    float x[4 * 40000] = { 0, 0, 1, 1 };
    printf("%f %f %f %f\n",*x, *(x + 1), *(x + 2), *(x + 3));
    
    Texture t(200,200,(float*)0);
    x[2] = .5;
    GLCall(glTextureSubImage2D(t.renderer_id, 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, x));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT,(void*)(x));
    printf("%f %f %f %f\n",*x, *(x + 1), *(x + 2), *(x + 3));
    t.bind(0);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        GLCall(glUseProgram(program));
        i32 uTime;
        GLSet(uTime,glGetUniformLocation(program, "u_Time");)
        GLCall(glUniform1f(uTime, glfwGetTime()));
        i32 uScreen;
        GLSet(uScreen,glGetUniformLocation(program, "screen");)
        GLCall(glUniform1i(uScreen, 1));
        GLCall(glDispatchCompute(200,200,1));
        GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
        //GLCall(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT,(void*)(x));
        printf("%f %f %f %f\n",*x, *(x + 1), *(x + 2), *(x + 3));

        glClear(GL_COLOR_BUFFER_BIT);
        vb.load(v);
        vb.bind();
        ib.bind();
        sh.bind();
        sh.set_uniform("uTexture", 0);
        GLCall(glDrawElements(GL_TRIANGLES, ib.length(), GL_UNSIGNED_INT, 0));
        glfwSwapBuffers(window);
    }
    
    return 0;
}