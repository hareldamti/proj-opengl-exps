#version 460
layout (location = 0) in vec2 _uv;

// environment uniforms
uniform float time;
uniform float screen_ratio;
uniform vec3 cam_pos;
mat3 cam_orientation;
uniform float cam_z_index;
uniform float z_max_culling;
uniform float marching_min_distance;
uniform vec3 color;
uniform float glow;
// configuration uniforms

vec3 pov(vec3 position);
vec3 background (vec2 screen_point);
float min_distance(vec3 position, vec3 direction);
float[3] march_ray(vec2 screen_point);
float[3] compute_attributes(float[3] ray);
float mandelbulb(vec3 center, mat3 orientation, vec3 ray_pos, float power, int iterations, float threshold);
mat3 rot_matrix(vec3 axis, float alpha);
float plane(vec3 center, vec3 normal, vec3 ray_pos);

#define n_ 6
float min_distance(vec3 position) {
    float[n_] elements= float[] (
        mandelbulb(
            vec3(4,0,0 + 5),
            rot_matrix(vec3(1,0,0), time),
            position, 2, 50, 0.001
            ),
        mandelbulb(
            vec3(-4,0,0 + 5),
            rot_matrix(vec3(0,1,0), time),
            position, 3, 50, 0.001
            ),
        mandelbulb(
            vec3(0,4,0 + 5),
            rot_matrix(vec3(0,1,0), time),
            position, 4, 50, 0.001
            ),
        mandelbulb(
            vec3(0,-4,0 + 5),
            rot_matrix(vec3(0,1,0), time),
            position, 5, 50, 0.001
            ),
        mandelbulb(
            vec3(0,0,4 + 5),
            rot_matrix(vec3(0,1,0), time),
            position, 6, 50, 0.001
            ),
        mandelbulb(
            vec3(0,0,-4 + 5),
            rot_matrix(vec3(0,1,0), time),
            position, 7, 50, 0.001
            )
    );

    float min_d = elements[0];
    for (int i = 1; i < n_; i++) {
        if (elements[i] < min_d) min_d = elements[i];
    }
    return min_d;
}

 
void main() {
    if (time == 0) {}

    vec2 screen_point = (2 * _uv - 1) * vec2(screen_ratio, 1);
    float[3] ray = march_ray(screen_point);
    float[3] attributes = compute_attributes(ray);

    vec3 color_ = vec3(0);
    if (ray[0] == -1) color_ += background(screen_point);

    color_ += color * attributes[0] + (vec3(1) - color) * pow(attributes[1],2) + vec3(.1 * glow) * attributes[2];
    gl_FragColor = vec4(color_, 1);
}