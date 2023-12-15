#include "Shader.h"

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

u32 link_shader(u32 shader_id1, u32 shader_id2) {
    int success;
    char log[512];

    u32 program = glCreateProgram();
    glAttachShader(program, shader_id1);
    if (shader_id2) glAttachShader(program, shader_id2);
   
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        ERROR_EXIT("Error linking shader. %s\n", log);
    }
    glDeleteShader(shader_id1);
    if (shader_id1) glDeleteShader(shader_id2);
    return program;
}

Shader::Shader(const std::string& name): name(name), renderer_id(0) {}

RenderShader::RenderShader(const std::string& vert_path, const std::string& frag_path) : Shader("< Render: " + vert_path + " , " + frag_path + " >") {
    u32 vert_id = compile_shader(read_file(vert_path), GL_VERTEX_SHADER);
    u32 frag_id = compile_shader(read_file(frag_path), GL_FRAGMENT_SHADER);
    renderer_id = link_shader(vert_id, frag_id);
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));
}

ComputeShader::ComputeShader(const std::string& comp_path) : Shader("< Compute: " + comp_path + " >") {
    u32 comp_id = compile_shader(read_file(comp_path), GL_COMPUTE_SHADER);
    renderer_id = link_shader(comp_id);
}


void Shader::bind() {
    GLCall(glUseProgram(renderer_id));
}

void RenderShader::bind() {
    Shader::bind();
    GLCall(glBindVertexArray(vao));
}


void Shader::unbind() {
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
}


i32 Shader::get_uniform_location(const std::string& name) {
    i32 res;
    if (uniform_locations.find(name) != uniform_locations.end())
        return uniform_locations[name];
    
    GLSet(res, glGetUniformLocation(renderer_id, name.c_str()));
    if (res == -1) ERROR_EXIT("uniform %s not found in shader %s\n", name.c_str(), this->name.c_str());
    uniform_locations[name] = res;
    return res;
}

void Shader::set_uniform(const std::string& name, int x) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform1i(location, x));
}

void Shader::set_uniform(const std::string& name, float x) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform1f(location, x));
}

void Shader::set_uniform(const std::string& name, float x, float y) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform2f(location, x, y));
}

void Shader::set_uniform(const std::string& name, float x, float y, float z) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform3f(location, x, y, z));
}

void Shader::set_uniform(const std::string& name, float x, float y, float z, float w) {
    i32 location = get_uniform_location(name);
    GLCall(glUniform4f(location, x, y, z, w));
}

Shader::~Shader() {
    glDeleteProgram(renderer_id);
}


/**
 * @param (num_attributes, attr1_name, attr1_size, [...])
 */
void RenderShader::set_attributes(int n_attributes,...) {
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

void ComputeShader::execute(u32 width, u32 height, u32 depth, u32 barrier) {
    bind();
    GLCall(glDispatchCompute(width, height, depth));
    GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
    //unbind();
}