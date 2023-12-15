#version 460 core
layout (location = 0) in vec2 _uv;
uniform sampler2D uTexture;

void main() {
    gl_FragColor = texture(uTexture, _uv);
}