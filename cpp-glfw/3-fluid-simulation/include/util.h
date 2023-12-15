#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "types.h"

#define ERROR_EXIT(...) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define ERROR_RETURN(R, ...) { fprintf(stderr, __VA_ARGS__); return R; }
#define LOG_DEBUG(...) { fprintf(stderr,"DEBUG:\t"); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define LOG_ERROR(...) { fprintf(stderr,"ERROR:\t"); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define ASSERT(x) if(!(x)) ERROR_EXIT("ASSERSION: %s\n",#x)
#define GLCall(f) {_glClearError();\
    f;\
    _glCatchError(#f, __FILE__, __LINE__);}
#define GLSet(x,f) {_glClearError();\
    x = f;\
    _glCatchError(#f, __FILE__, __LINE__);}
#define TIME(f) {f32 start = glfwGetTime();\
    f;\
    LOG_DEBUG("%s: %f seconds", #f ,glfwGetTime() - start);}

void _glClearError();
char* _glErrorString(u32 error);
bool _glCatchError(const char* func_name, const char* file, int line);
std::string read_file(const std::string& file_path);

class ImGuiUtil {
    public:
        ImGuiUtil(GLFWwindow* window);
        ~ImGuiUtil();
        void render_begin();
        void render_end();
        ImGuiIO& io();
};