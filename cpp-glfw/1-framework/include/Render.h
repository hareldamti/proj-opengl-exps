#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "util.h"
#include "types.h"
#include "Shader.h"

GLFWwindow* render_init_window(int width, int height, const std::string& title);

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
    private:
        u32 renderer_id;
        u32 width;
        u32 height;
        float* f_buffer;
    public:
        Texture(int idth, int height, char* data = 0);
        Texture(int idth, int height, float* data = 0);
        float* get();
        void bind(u32 slot = 0);
        void unbind();
        ~Texture();
};

class RenderState {
    public:
        GLFWwindow* window;
        f32 width;
        f32 height;
        RenderState(int width, int height, const std::string& title);
        void draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib);
        void draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib, Texture& tex);
        void clear_frame();
        void update_frame();
        ~RenderState();
};