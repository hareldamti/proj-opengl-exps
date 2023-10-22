#version 410 core

#define M_PI 3.1415926535897932384626433832795
#define CMAP_PURPLE 0
#define CMAP_ORANGE 1

uniform float time_now;
uniform vec2 resolution;
uniform vec3 position;
uniform float[3] spherical_coords_camera;
float threshold = .0006;
float max_z = 150;
int iterations = 15;
float power = 8;
float zIndex = 2.5;
float clamp(float x, float a, float b) { return max(a, min(x, b)); }

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5*(a-b)/k, 0.0, 1.0);
    return mix(a, b, h) - k*h*(1.0-h);
}

float atan2(vec2 v) {
    if (v.x > 0) return atan(v.y/v.x);
    if (v.x < 0) return atan(v.y/v.x) + M_PI;
    return sign(v.y) * M_PI;
}

vec3 colormap(float val) {
    const int len = 3;
    vec3 colors[] = {
            vec3(1, 0, 0),
            vec3(1, 1, 1),
            vec3(0, 0.3, 1),
        };
    val = clamp(0, val, 1);
    for (int i = 0; i < len - 1; i++) 
        if (val < float(i + 1) / (len - 1))
            return mix(colors[i], colors[i+1], val * (len - 1) - float(i));
    
    return colors[len - 1];
}


float seed = 0;
vec2 rand2(vec2 st) {
    return vec2(
        fract(sin(dot(st.xy, vec2(12.9898,78.233)) * 43758.5453123 + seed * 2609.276)),
        fract(sin(dot(st.xy, vec2(95.9898,32.233)) * 23591.5453123 + seed * 8149.276)));
    seed = fract(sin(dot(st.xy, vec2(95.9898,32.233)) * 13881.5453123 + seed * 2215.276));
}

// vec3( f, df/dx, df/dy )
vec3 standing_wave(vec2 polar_coords, int n, int m) {
    float f = sin(M_PI * polar_coords.x * n) * sin(polar_coords.y * m);
    vec2 f_partial_pol = vec2(
        M_PI * n * cos(M_PI * polar_coords.x * n) * sin(polar_coords.y * m),
        m * cos (polar_coords.y * m) * sin(M_PI * polar_coords.x * n)
    );
    
    vec2 dpol_dx = vec2(cos(polar_coords.y), -tan(polar_coords.y) / (polar_coords.x * cos(polar_coords.y) * (1 + pow(tan(polar_coords.y), 2))) ),
         dpol_dy = vec2(sin(polar_coords.y),        1.0           / (polar_coords.x * cos(polar_coords.y) * (1 + pow(tan(polar_coords.y), 2))) ),
         diff = vec2( dot(f_partial_pol, dpol_dx), dot(f_partial_pol, dpol_dy));
    return vec3(f, diff);
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - resolution)/(vec2(1, 1) * resolution.y);
    if (length(uv) > 1) { gl_FragColor = vec4(0, 0, 0, 1); return; }
    vec2 pol = vec2(min(length(uv), 1), atan2(uv));
    vec3 f = vec3(0, 0, 0);
    float m, n;
    f += standing_wave(pol + vec2(0, 0), 2, 3);
    f += standing_wave(pol + vec2(0, 0), 5, 7) / 2;

    vec3 normal = normalize(vec3(-f.yz, 1.)) * .5 + .5;

    float intensity;
    //intensity = (1/(pow(length(f.yz),2) + .01)); // gradient ^ -2 (maxima & minima)
    intensity = (.01/(pow(length(f.x),2) + .01)); // height ^ -2 (intersection w\ y plane)
    //intensity = f.x * .5 + .5;
    //intensity = f.y * f.z;
    gl_FragColor = vec4(normal, 1); // normal lighting
    gl_FragColor = vec4(colormap(intensity), 1);
    gl_FragColor = vec4(vec3(1,1,1) * intensity, 1);
    
}