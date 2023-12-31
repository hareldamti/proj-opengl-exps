#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include "util.h"
#include "types.h"
#include <unordered_map>
#define GLCall(f) {_glClearError();\
    f;\
    _glCatchError(#f, __FILE__, __LINE__);}
#define GLSet(x,f) {_glClearError();\
    x = f;\
    _glCatchError(#f, __FILE__, __LINE__);}

void _glClearError();
char* _glErrorString(u32 error);
bool _glCatchError(const char* func_name, const char* file, int line);
u32 compile_shader(const std::string& glsl_code, u32 shader_type);
u32 create_shader(const std::string& vert_path, const std::string& frag_path);
GLFWwindow* render_init_window(int width, int height, const std::string& title);

class RenderShader {
    private:
        u32 renderer_id;
        u32 vao;
        const std::string name;
        std::unordered_map<std::string, i32> uniform_locations;
        i32 get_uniform_location(const std::string& name);
    public:
        RenderShader(const std::string& vert_path, const std::string& frag_path);
        void RenderShader::set_attributes(int n_attributes,...);
        void bind();
        void unbind();
        void set_uniform(const std::string& name, int x);
        void set_uniform(const std::string& name, float x);
        void set_uniform(const std::string& name, float x, float y);
        void set_uniform(const std::string& name, float x, float y, float z);
        void set_uniform(const std::string& name, float x, float y, float z, float w);
        ~RenderShader();
};

class VertexBuffer {
    private:
        u32 renderer_id;
        u32 m_length;
    public:
        VertexBuffer(u32 length);
        ~VertexBuffer();
        void load(const f32* data);
        void bind();
        void unbind();
        inline u32 length() const { return m_length; }
};

class IndexBuffer {
    private:
        u32 renderer_id;
        u32 m_length;
    public:
        IndexBuffer(const u32* data, u32 length);
        ~IndexBuffer();
        void bind();
        void unbind();
        inline u32 length() const { return m_length; }
};

class Texture {
    public:
        u32 renderer_id;
        u32 width;
        u32 height;
        void* buffer;
        Texture(int width, int height, char* data = 0);
        Texture(int width, int height, float* data = 0);
        void bind(u32 slot = 0);
        void unbind();
        ~Texture();
};

class RenderState {
    public:
        GLFWwindow* window;
        f32 width;
        f32 height;
        u32 shader;
        RenderState(int width, int height, const std::string& title);
        void draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib);
        void draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib, Texture& tex);
        void clear_frame();
        void update_frame();
        ~RenderState();
};