#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include "glad/glad.h"

#include "util.h"
#include "types.h"

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