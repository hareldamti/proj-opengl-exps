#version 410 core

#define M_PI 3.1415926535897932384626433832795
#define CMAP_PURPLE 0
#define CMAP_ORANGE 1

uniform float time_now;
uniform vec2 resolution;
uniform vec3 position;
uniform float[3] spherical_coords_camera;
float threshold = .004;
float max_z = 150;
int iterations = 150;
float power = 4;

float clamp(float x, float a, float b) { return max(a, min(x, b)); }

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5*(a-b)/k, 0.0, 1.0);
    return mix(a, b, h) - k*h*(1.0-h);
}

float atan2(float y, float x) {
    if (x > 0) return atan(y/x);
    if (x < 0) return atan(y/x) + M_PI;
    return sign(y) * M_PI;
}

float fractal(vec3 position) {
    vec3 z = position;
    float dr = 1;
    float r;
    for (int i = 0; i < iterations; i++) {
        r = length(z) + .01;
        if (r > 2) break;
        float theta = acos(clamp(-1,z.z / r,1)) * power;
        float phi = atan2(z.y, z.x) * power;
        float zr = pow(r, power);
        dr = pow(r, power - 1) * power * dr + 1;
        z = zr * vec3( sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        z += position;
    }
    return .5 * log(r) * r / dr;
}

float min_distance(vec3 position) {
    // float r = length(position),
    //       theta = position.z / (r + threshold/10),
    //       phi = atan2(position.y, position.x);
    // r = r * pow(abs(sin(theta)),.3 - .1) ;
    // position = r * vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    
    float r = length(position.xy),
          theta = atan2(position.y, position.x),
          z = position.z;
    r -= 2;
    z = sin(z);
    // theta += z;
    position = vec3(r * cos(theta), r * sin(theta), z);
    
    return fractal(position - vec3(0,  0, 0));
    return smin(
        fractal(position - vec3(0,  1, 0)),
        fractal(position - vec3(0, -1, 0)), .1
    );

}

vec3 colormap(float val, int cmap) {
    const int len = 5;
    vec3 colors[] = {
            vec3(0, 0, 0),
            vec3(.1, .1, 1),
            vec3(.7, 0, 1),
            vec3(1, 1, 1),
            vec3(1, 1, 1)
        };
    if (cmap == CMAP_ORANGE) {
            colors[1] = vec3(1, .4, .1);
            colors[2] = vec3(1, .9, 0);
    }
    val = clamp(0, val, 1);
    for (int i = 0; i < len - 1; i++) 
        if (val < float(i + 1) / len)
            return mix(colors[i], colors[i+1], val * len - float(i));
    
    return colors[len - 1];
}

vec3 march_ray(vec3 camera_pos, mat3 camera_dir, vec2 xy) {
    float zIndex = 2;
    vec3 direction = normalize(vec3(xy, zIndex)), current_position = camera_pos;
    float d_last = min_distance(camera_dir * (current_position - camera_pos) + camera_pos), d = d_last;
    while (d > threshold) {
        current_position += direction * d;
        d_last = d + threshold / 100;
        d = min_distance(camera_dir * (current_position - camera_pos) + camera_pos);
        if (current_position.z > max_z) return vec3(0, 0, 0);
    }
    float surface = sqrt(pow((d_last - d)/(d_last), 2));
    float edge_glow = pow(1 - pow((d_last - d)/(d_last), 2),25);
    
    current_position = camera_dir * (current_position - camera_pos) + camera_pos;
    float theta = current_position.z / (length(current_position) + threshold/10),
          phi = atan2(current_position.y, current_position.x);
    theta = theta/2 + M_PI/4; phi /= 4;
    vec3 hue = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    
    return mix(colormap(edge_glow, CMAP_ORANGE), colormap(edge_glow, CMAP_PURPLE), 0.5 + 0.5 * (sin(5 * current_position.z)));
}

float seed = 0;
vec2 rand2(vec2 st) {
    return vec2(
        fract(sin(dot(st.xy, vec2(12.9898,78.233)) * 43758.5453123 + seed * 2609.276)),
        fract(sin(dot(st.xy, vec2(95.9898,32.233)) * 23591.5453123 + seed * 8149.276)));
    seed = fract(sin(dot(st.xy, vec2(95.9898,32.233)) * 13881.5453123 + seed * 2215.276));
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - resolution)/(vec2(1, 1) * resolution.y);

    float phi = spherical_coords_camera[0], theta = spherical_coords_camera[1], r = spherical_coords_camera[2],
    psi = time_now / 1e4;
    mat3 camera_dir = 
    mat3(
        1, 0, 0,
        0, cos(theta) , sin(theta),
        0, -sin(theta) , cos(theta)
    ) * mat3(
         cos(phi), 0 , sin(phi),
            0  , 1 ,     0 ,
        -sin(phi), 0 , cos(phi)
    ) * mat3(
         cos(psi), sin(psi), 0,
        -sin(psi), cos(psi), 0,
        0, 0, 1
    );

    vec3 camera_pos = camera_dir * vec3(0, 0, r);

    vec3 color = vec3(0,0,0);
    const int samples = 2;
    for (int i = 0; i < samples; i++) color += march_ray(camera_pos, camera_dir, uv + rand2(uv) * 2 / resolution.x) / float(samples);
    gl_FragColor = vec4(color, 1);
}