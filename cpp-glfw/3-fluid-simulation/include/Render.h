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
    public:
        u32 width;
        u32 height;
        u32 image;
        f32* f_buffer;
        Texture(u32 width, u32 height, f32* data = 0);
        inline f32 get(u32 i, u32 j, u32 k) {
            u32 idx = 4 * (i * width + j) + k;
            if (idx < 0 || idx >= width * height * 4 || k < 0 || k > 3) ERROR_EXIT("Texture get (%d, %d) with dimensions (%d, %d)\n", i, j, width, height);
            return f_buffer[idx]; 
        };
        inline void set(u32 i, u32 j, u32 k, f32 val) {
            u32 idx = 4 * (i * width + j) + k;
            if (idx < 0 || idx >= width * height * 4 || k < 0 || k > 3) ERROR_EXIT("Texture get (%d, %d) with dimensions (%d, %d)\n", i, j, width, height);
            f_buffer[idx] = val; 
        };
        void bind_image();
        void get_from_gpu();
        void set_to_gpu();
        void bind(u32 slot = 0);
        void unbind();
        ~Texture();
};

class RenderState {
    public:
        GLFWwindow* window;
        f32 width;
        f32 height;
        Texture screen_tex;
        RenderState(int width, int height, const std::string& title);
        void draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib);
        void clear_frame();
        void update_frame();
        ~RenderState();
};