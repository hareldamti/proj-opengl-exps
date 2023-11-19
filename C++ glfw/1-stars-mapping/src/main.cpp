#define GLEW_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "engine/util.h"
#include "engine/types.h"

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
    gladLoadGL();
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(.05f,.1f,.05f,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBegin(GL_TRIANGLES);
        glVertex2f(-.5f,0.f);
        glVertex2f(0.f,.5f);
        glVertex2f(0.f,-.5f);
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}