#include "Render.h"

GLFWwindow* render_init_window(int width, int height, const std::string& title) {
    if (!glfwInit())
        ERROR_EXIT("Couldn't initialize GLFW\n");
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        ERROR_EXIT("Couldn't create window\n");
    }
    // Make the window's context current
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    LOG_DEBUG("Vendor: %s", glGetString(GL_VENDOR));
    LOG_DEBUG("Renderer: %s", glGetString(GL_RENDERER));
    LOG_DEBUG("Version: %s", glGetString(GL_VERSION));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));
    return window;
}


RenderState::RenderState(int width, int height, const std::string& title) :
    window(render_init_window(width, height, title)),
    width(width),
    height(height){
    }

void RenderState::draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib) {
    vb.bind();
    ib.bind();
    shader.bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.length(), GL_UNSIGNED_INT, 0));
}

void RenderState::draw_triangles(RenderShader& shader, VertexBuffer& vb, IndexBuffer& ib, Texture& tex) {
    tex.bind(0);
    shader.set_uniform("u_Texture", 0);
    vb.bind();
    ib.bind();
    shader.bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.length(), GL_UNSIGNED_INT, 0));
}


void RenderState::clear_frame() { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

void RenderState::update_frame() { glfwSwapBuffers(window); }

RenderState::~RenderState() {
    glfwDestroyWindow(window);
    glfwTerminate();
}




VertexBuffer::VertexBuffer(u32 length): m_length(length) {
    GLCall(glGenBuffers(1, &renderer_id));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id));
    GLCall(glBufferData(GL_ARRAY_BUFFER, length * sizeof(f32), 0, GL_DYNAMIC_DRAW));
}

void VertexBuffer::load(const f32* data) { GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, m_length * sizeof(f32), data)); }

void VertexBuffer::bind() { GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id)); }

void VertexBuffer::unbind() { GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); }

VertexBuffer::~VertexBuffer() { GLCall(glDeleteBuffers(1, &renderer_id)); }



IndexBuffer::IndexBuffer(const u32* data, u32 length): m_length(length) {
    GLCall(glGenBuffers(1, &renderer_id));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(u32), data, GL_DYNAMIC_DRAW));
}

void IndexBuffer::bind() { GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id)); }

void IndexBuffer::unbind() { GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id)); }

IndexBuffer::~IndexBuffer() { GLCall(glDeleteBuffers(1, &renderer_id)); }



Texture::Texture(int width, int height, char* data) : width(width), height(height) {
    GLCall(glGenTextures(1, &renderer_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id));
    
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));    
    
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GLCall(glBindImageTexture(0, renderer_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI)); 

}

Texture::Texture(int width, int height, float* data) : width(width), height(height) {
    f_buffer = data ? data : new float[width * height * 4];
    GLCall(glGenTextures(1, &renderer_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id));
    
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));    
    
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data));
    GLCall(glBindImageTexture(0, renderer_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F)); 
}

float* Texture::get() {
    GLCall(glGetTexImage(GL_TEXTURE_2D,  0, GL_RGBA, GL_FLOAT,(void*)f_buffer));
    return f_buffer;
}

void Texture::bind(u32 slot) {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id));
}

void Texture::unbind() {
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture(){
    GLCall(glDeleteTextures(1, &renderer_id));
    if (f_buffer) delete[] f_buffer;
}