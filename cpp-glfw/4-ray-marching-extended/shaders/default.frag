#version 460 core
layout (location = 0) in vec2 _uv;
layout (binding = 0) uniform sampler2D u_Texture;
void main() {
    gl_FragColor = texture(u_Texture, _uv);
    //gl_FragColor = vec4(_uv, 1, 1);
   
}