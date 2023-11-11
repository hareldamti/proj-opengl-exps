#version 410 core

#define M_PI 3.1415926535897932384626433832795

uniform float time_now;
uniform vec2 resolution;
uniform vec3 position;
uniform float[3] spherical_coords_camera;
float threshold = .001;
float max_z = 200;
int iterations = 150;
float power = 7;

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
        r = length(z) + threshold / 10;
        if (r > 2) break;
        float theta = acos(z.z / r) * power;
        float phi = atan2(z.y, z.x) * power;
        float zr = pow(r, power);
        dr = pow(r, power - 1) * power * dr + 1;
        z = zr * vec3( sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        z += position;
    }
    return (.5) * log(r) * r / dr;
}

float min_distance(vec3 position) {
    float r = length(position),
          theta = acos(position.z / (r + threshold/100)),
          phi = atan2(position.y, position.x);
    //r = r * pow(abs(sin(theta)),.3 + .5) * cos(theta) ;
    
    position = r * vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    
    // float r = length(position.xy),
    //       theta = atan2(position.y, position.x),
    //       z = position.z;
    // position = vec3(r * cos(theta), r * sin(theta), z);
    
    return fractal(position - vec3(0,  0, 0));
}

vec3 colormap(float val) {
    const int len = 4;
    vec3 colors[] = {
        vec3(0, 0, 0),
        vec3(.1, .1, 1),
        vec3(.7, 0, 1),
        vec3(1, 1, 1),
    };
    val = clamp(0, val, 1);
    for (int i = 0; i < len - 1; i++) {
        if (val < float(i + 1) / len) return mix(colors[i], colors[i+1], val * len - float(i));
    }
    return colors[len - 1];
}

vec3 march_ray(vec3 camera_pos, mat3 camera_dir, vec2 xy) {
    float zIndex = 2;
    vec3 direction = normalize(vec3(xy, zIndex)), current_position = camera_pos;
    float d_last = 0,
          d = min_distance(camera_dir * (current_position - camera_pos) + camera_pos),
          glow = 0;
    bool bounds = true;
    while (d > threshold && bounds) {
        current_position += direction * d;
        d_last = d + threshold / 100;
        d = min_distance(camera_dir * (current_position - camera_pos) + camera_pos);
        if (d > d_last) glow += pow(d_last * 5000,-2);
        if (current_position.z > max_z) bounds = false;//return vec3(0, 0, 0);
    }
    float surface = sqrt(pow((d_last - d)/(d_last), 2));
    float edge_glow = pow(1 - pow((d_last - d)/(d_last), 2),18);

    current_position = camera_dir * (current_position - camera_pos) + camera_pos;
    float theta = current_position.z / (length(current_position) + threshold/10),
          phi = atan2(current_position.y, current_position.x);
    theta = theta/2 + M_PI/4; phi /= 4;
    vec3 hue = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    
    return bounds ? colormap(edge_glow):vec3(0,0,0);//return bounds ? colormap(glow) : colormap(glow);
}

vec2 rand2(vec2 st) {
    return vec2(
        fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123),
        fract(sin(dot(st.xy, vec2(95.9898,32.233))) * 23591.5453123));
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - resolution)/(vec2(1, 1) * resolution.y);

    float phi = spherical_coords_camera[0], theta = spherical_coords_camera[1], r = spherical_coords_camera[2];
    
    mat3 camera_dir = mat3(
        1, 0, 0,
        0, cos(theta) , sin(theta),
        0, -sin(theta) , cos(theta)
    ) * mat3(
         cos(phi), 0 , sin(phi),
            0  , 1 ,     0 ,
        -sin(phi), 0 , cos(phi)
    );

    vec3 camera_pos = camera_dir * vec3(0, 0, - 3 * exp( -r ));

    vec3 color = vec3(0,0,0);
    const int samples = 1;
    for (int i = 0; i < samples; i++) color += march_ray(camera_pos, camera_dir, uv) / float(samples);
    gl_FragColor = vec4(color, 1);
}