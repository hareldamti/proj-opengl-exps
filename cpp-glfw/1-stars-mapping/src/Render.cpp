#include "Render.h"

u32 compile_shader(const std::string& glsl_code, u32 shader_type) {
    u32 shader = glCreateShader(shader_type);
    const char* src = glsl_code.c_str();
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
    u32 vs = compile_shader(read_file(vert_path), GL_VERTEX_SHADER),
        fs = compile_shader(read_file(frag_path), GL_FRAGMENT_SHADER);

    u32 program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
   
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
    height(height) {
    }

void RenderState::draw_triangles(Shader& shader, VertexBuffer& vb, IndexBuffer& ib) {
    vb.bind();
    ib.bind();
    shader.bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.length(), GL_UNSIGNED_INT, 0));
    //shader.unbind();
    //ib.unbind();
    //vb.unbind();
}

void RenderState::clear_frame() { GLCall(glClear(GL_COLOR_BUFFER_BIT)); }

void RenderState::update_frame() { glfwSwapBuffers(window); }

RenderState::~RenderState() {
    glfwDestroyWindow(window);
    glfwTerminate();
}



Shader::Shader(const std::string& vert_path, const std::string& frag_path):
    renderer_id(create_shader(vert_path, frag_path)),
    name("< " + vert_path + " , " + frag_path + " >") {
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));
}

void Shader::bind() {
    GLCall(glUseProgram(renderer_id));
    GLCall(glBindVertexArray(vao));
}

void Shader::unbind() {
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
}

/**
 * @param (num_attributes, attr1_name, attr1_size, [...])
 */
void Shader::set_attributes(int n_attributes,...) {
    va_list args;
    std::vector<std::string> names(0);
    std::vector<int> lengths(0);
    int offset = 0, stride = 0;
    va_start(args, &n_attributes);
    for (int i = 0; i < n_attributes; i++) {
        names.push_back(va_arg(args, char*));
        int length = va_arg(args, int);
        lengths.push_back(length);
        stride += length;
    }
    for (int i = 0; i < n_attributes; i++) {
        int idx;
        GLSet(idx, glGetAttribLocation(renderer_id, names.at(i).c_str()));
        if (idx == -1) {
            idx = i;
            LOG_ERROR("Can't find attribute %s in shader %s. Using %d", names.at(i).c_str(), name.c_str(), i);
        }
        glEnableVertexAttribArray(idx);
        glVertexAttribPointer(idx, lengths.at(i), GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(sizeof(float) * offset));
        offset += lengths.at(i);
    }
}

i32 Shader::get_uniform_location(std::string& name) {
    i32 res;
    if (uniform_locations.find(name) != uniform_locations.end())
        return uniform_locations[name];
    
    GLSet(res, glGetUniformLocation(renderer_id, name.c_str()));
    if (res == -1) ERROR_EXIT("uniform %s not found in shader %s\n", name.c_str(), this->name.c_str());
    uniform_locations[name] = res;
    return res;
}

void Shader::set_uniform(std::string& name, float x) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform1f(location, x));
}

void Shader::set_uniform(std::string& name, float x, float y) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform2f(location, x, y));
}

void Shader::set_uniform(std::string& name, float x, float y, float z) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform3f(location, x, y, z));
}

void Shader::set_uniform(std::string& name, float x, float y, float z, float w) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform4f(location, x, y, z, w));
}

Shader::~Shader() {
    glDeleteProgram(renderer_id);
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