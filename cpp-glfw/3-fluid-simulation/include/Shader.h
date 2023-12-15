#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include "util.h"
#include "types.h"
#include <unordered_map>

u32 compile_shader(const std::string& glsl_code, u32 shader_type);
u32 link_shader(u32 shader_id1, u32 shader_id2 = 0);

class Shader {
    protected:
        u32 renderer_id;
        std::unordered_map<std::string, i32> uniform_locations;
        i32 get_uniform_location(const std::string& name);
    
    public:
        const std::string name;
        Shader(const std::string& name);
        virtual void bind();
        void unbind();
        void set_uniform(const std::string& name, int x);
        void set_uniform(const std::string& name, float x);
        void set_uniform(const std::string& name, float x, float y);
        void set_uniform(const std::string& name, float x, float y, float z);
        void set_uniform(const std::string& name, float x, float y, float z, float w);
        ~Shader();
};

class RenderShader : public Shader {
    private:
        u32 vao;
    public:
        RenderShader(const std::string& vert_path, const std::string& frag_path);
        void RenderShader::set_attributes(int n_attributes,...);
        virtual void bind();
};

class ComputeShader : public Shader {
    private:
    public:
        ComputeShader(const std::string& comp_path);
        void execute(u32 width, u32 height, u32 depth = 1, u32 barrier = GL_ALL_BARRIER_BITS);
};
