#define GLEW_STATIC
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
    
    // Require: OpenGL context's version >= 3.2
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(640, 480, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        ERROR_EXIT("Couldn't create window\n");
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    

    u32 shader = create_shader("./shaders/default.vert", "./shaders/default.frag");
    glUseProgram(shader);

    f32 vertices[] = {
        -.5,  0,
          0, .5,
          0,-.5
    };
    u32 elements[] = {
        0, 1, 2
    };

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        int error;

        glClearColor(.05f,.1f,.05f,1);
        glClear(GL_COLOR_BUFFER_BIT);

        /*
        glBegin(GL_TRIANGLES);
        glVertex2f(-.5,0);
        glVertex2f(0, .5);
        glVertex2f(0,-.5);
        glEnd();
        */
       
        u32 vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        
        i32 posAttrib = glGetAttribLocation(shader, "pos");
        glEnableVertexAttribArray(posAttrib);
        
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        
        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        if ((error = glGetError()) != 0) ERROR_EXIT("OpenGL error: %x\n", error);
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}