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

void RenderState::draw_triangles(VertexBuffer& vb, IndexBuffer& ib) {
    vb.bind();
    ib.bind();
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

std::unordered_map<u32, bool> texture_screens;

Texture::Texture(u32 width, u32 height, f32* data) : width(width), height(height) {
    f_buffer = data ? data : new f32[width * height * 4];
    GLCall(glGenTextures(1, &renderer_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id));
    
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));    

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data ? data : f_buffer));
	//glTextureStorage2D(renderer_id, 0, GL_RGBA32F, width, height);
    glBindImageTexture(image ,renderer_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F); 
}

void Texture::bind_image() {
    image = 0;
    while (texture_screens[image]){
        image++;
        if (image > 15) ERROR_EXIT("16 images limit reached");
    }
    texture_screens[image] = true;
}

void Texture::get_from_gpu() {
    GLCall(glGetTexImage(GL_TEXTURE_2D,  0, GL_RGBA, GL_FLOAT,(void*)f_buffer));
}

void Texture::set_to_gpu() {
    GLCall(glTextureSubImage2D(renderer_id, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, f_buffer));
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