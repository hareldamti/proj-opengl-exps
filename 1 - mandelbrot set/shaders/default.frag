#version 410 core

#define TINY 1e-5
#define M_PI 3.14159265358979323846
uniform float time_now;
uniform vec2 resolution;
uniform vec2 mouse;
uniform float zoom;
uniform float[5] points;

int iterations = 150;

float atan2(vec2 v) {
    if (v.x > 0) return atan(v.y/v.x);
    if (v.x < 0) return atan(v.y/v.x) + M_PI;
    return sign(v.y) * M_PI;
}

vec2 mult_comp(vec2 z1, vec2 z2) {
    return vec2(z1.x * z2.x - z1.y * z2.y, z1.x * z2.y + z2.x * z1.y);
}

vec2 exp(vec2 z) {
    return vec2(cos(z.y), sin(z.y)) * exp(z.x);
}

vec2 pow_comp(vec2 base, vec2 exponent) {
    return exp(mult_comp(vec2(log(length(base)), atan2(base)), exponent));
}

vec2 pow_comp(vec2 base, float exponent) {
    return exp(vec2(log(length(base)), atan2(base)) * exponent);
}

float fractal(vec2 z, vec2 z0) {
    float intensity = 0;
    for (int i = 0; i < iterations; i++) {
        z0 = pow_comp(z0, 2) + z;
        if (length(z0) > 2) break;
        intensity = i;
    }
    return intensity / iterations;
}

float seed = 0;
vec2 rand2(vec2 st) {
    return vec2(
        fract(sin(dot(st.xy, vec2(12.9898,78.233)) * 43758.5453123 + seed * 2609.276)),
        fract(sin(dot(st.xy, vec2(95.9898,32.233)) * 23591.5453123 + seed * 8149.276)));
    seed = fract(sin(dot(st.xy, vec2(95.9898,32.233)) * 13881.5453123 + seed * 2215.276));
}

void main() {
    vec2 mouse_displacement = (mouse/resolution.x * 2) * vec2(-1,1);
    vec2 uv = (2. * gl_FragCoord.xy/(vec2(1,1) * resolution.x) - vec2(1, 1)) / zoom - mouse_displacement;
    float intensity = 0;
    int anti_aliasing = 10;
    for (int i = 0; i < anti_aliasing; i++) intensity += fractal(uv + rand2(uv) / 2.0 / resolution.x / zoom, exp(vec2(-2, 0 * time_now/1e3))) / anti_aliasing;
    gl_FragColor = vec4(1, 1, 1, 1) * intensity;
}


/*
float fractal(vec2 z) {
    float res = 0;
    for (int i = 0; i < iterations; i++) {
        z = pow(z,)
    }
}

void main() {
    vec2 uv = (2. * gl_FragCoord.xy/(vec2(1,1) * resolution.x) - vec2(1, 1));
    vec2 c = vec2(1.4 + .2*cos(time_now / 1e4), sin(1.4 + .2*time_now / 1e4 * 3)) * .3;
    const int res = 200;
    float intensity = 0;
    for (int i = 0; i < res; i++) {
        uv = vec2(uv.x * uv.x - uv.y * uv.y, 2 * uv.x * uv.y) + c;
        if (dot(uv, uv) > 2) break;
        intensity = i;
    }
    gl_FragColor = vec4(1, 1, 1, 1) * intensity / res;
}
*/