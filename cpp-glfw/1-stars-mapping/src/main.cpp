#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "engine/util.h"
#include "engine/types.h"
#include "engine/render.cpp"

int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        ERROR_EXIT("Couldn't initialize GLFW\n");

    window = glfwCreateWindow(640, 480, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        ERROR_EXIT("Couldn't create window\n");
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    

    u32 shader = create_shader("./shaders/default.vert", "./shaders/default.frag");

    f32 vertices[16] = {
        -1,  -1, 0, 0,
        -1,   1, 0, 1,
         1,  -1, 1, 0,
         1,   1, 1, 1
        };
    i32 elements[6] = {0, 1, 2, 1, 2, 3};
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        int error;

        u32 vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        
        i32 posAttrib = 0;
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        i32 uvAttrib = 1;
        glEnableVertexAttribArray(uvAttrib);
        glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        if ((error = glGetError()) != 0) ERROR_EXIT("OpenGL error: %x\n", error);
        glUseProgram(shader);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}