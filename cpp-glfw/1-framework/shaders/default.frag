#version 330 core
layout (location = 0) in vec2 _uv;

void main() {
    gl_FragColor = vec4(_uv, 1, 1);
}