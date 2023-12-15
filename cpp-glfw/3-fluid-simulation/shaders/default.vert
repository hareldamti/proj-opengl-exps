#version 460 core
layout (location = 0) attribute vec2 pos;
layout (location = 1) attribute vec2 uv;

layout (location = 0) out vec2 _uv;

void main() {
    gl_Position = vec4(pos, 0, 1);
    _uv = uv;
}