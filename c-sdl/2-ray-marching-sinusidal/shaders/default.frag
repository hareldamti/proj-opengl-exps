#version 410 core

uniform float time_now;
uniform vec2 resolution;
uniform vec3 position;

float threshold = .01;
float max_z = 100.;

float clamp(float a, float x, float b) { return min(max(a, x), b); }
float smin(float a, float b, float k) { 
    float h = clamp(0.5 + 0.5*(a - b)/k, 0.0, 1.0);
    return mix(a, b, h) - k * h * (1.0 - h);
}

float min_distance(vec3 position) {
    /* // 3 META BALLS
    vec3 ball1 = vec3(cos(time_now/1e3), sin(time_now/1e3 * 2), 10 + cos(time_now/1e3 * 2)),
         ball2 = vec3(cos(time_now/1e3 * 2), sin(time_now/1e3), 10 + cos(time_now/1e3 * 2)),
         rect3 = vec3(cos(time_now/1e3 * 3), sin(time_now/1e3 * 2), 10 + cos(time_now/1e3 * 2));
    return smin(smin(length(position - ball1) - 1, length(position - ball2) - 1, 1), length(position - rect3) - 1,1);
    */
    
    //return sin(position.x - .5) + cos(position.y) + sin(position.z + time_now / 1e3);
    
    vec3 p = vec3(position.xy, pow(position.z,1.8)/100) + vec3(0, 0, time_now / 1e3);
    float angle = time_now / 1e4 + position.z / 40;
    p = mat3(cos(angle), sin(angle), 0, -sin(angle), cos(angle), 0, 0, 0, 1) * p;
    return pow((cos(p.x) + cos(p.y) + sin(p.z)) / 3, 1) + .3;
}

float march_ray(vec2 xy) {
    float zIndex = 2;
    vec3 direction = normalize(vec3(xy, zIndex)), current_position = vec3(0, 0, 0);
    float d_last = 0, d = min_distance(current_position);
    while (d > threshold) {
        current_position += direction * d;
        d_last = d;
        d = min_distance(current_position);
        if (current_position.z > max_z) return 0;
    }
    return sqrt(pow((d_last - d)/(d_last), 2)) + pow(1 - pow((d_last - d)/(d_last), 2),50);
}

vec3 colormap(float val) {
    const int len = 6;
    vec3 colors[] = {
        vec3(0, 0, 0),
        vec3(0, 0, 0),
        vec3(.1, .1, 1),
        vec3(.7, 0, 1),
        vec3(1, 1, 1),
        vec3(1, 1, 1)
    };
    val = clamp(0, val, 1);
    for (int i = 0; i < len - 1; i++) {
        if (val < float(i + 1) / len) return mix(colors[i], colors[i+1], val * len - float(i));
    }
    return colors[len - 1];
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - resolution)/(vec2(1, 1) * resolution.y);
    
    gl_FragColor = vec4(colormap(march_ray(uv)),1);
}