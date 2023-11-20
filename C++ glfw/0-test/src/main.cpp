#define GLEW_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>
#include "engine/util.h"
#include "engine/types.h"
#include <fstream>
#include <iostream>

u32 compile_shader(const std::string& file_path, u32 shader_type) {
    std::ifstream stream(file_path);
    std::string data((std::istreambuf_iterator<char>(stream)),
                 std::istreambuf_iterator<char>());
    u32 shader = glCreateShader(shader_type);
    const char* src = data.c_str();
    glShaderSource(shader, 1, &src, NULL);
    int success;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char msg[512];
        glGetShaderInfoLog(shader, 512, NULL, msg);
        ERROR_EXIT("Error compiling GLenum(%x) shader. %s\n", shader_type, msg);
    }
    return shader;
}

u32 create_shader(const std::string& vert_path, const std::string& frag_path) {
    int success;
    char log[512];
    u32 vs = compile_shader(vert_path, GL_VERTEX_SHADER),
        fs = compile_shader(frag_path, GL_FRAGMENT_SHADER);

    u32 program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glBindFragDataLocation(program, 0, "fragColor");
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        ERROR_EXIT("Error linking shader. %s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

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

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex2f(-.5,0);
        glVertex2f(0, .5);
        glVertex2f(0,-.5);
        glEnd();
        
       /*
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
        */

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}