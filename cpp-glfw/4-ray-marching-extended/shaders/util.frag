#version 460 core
#define M_PI 3.1415926535897932384626433832795
#define TINY 0.000001
uniform float time;
uniform float screen_ratio = 1.33; // width / height

uniform vec3 cam_pos = vec3(0);
uniform vec2 cam_angles = vec2(0,0);
uniform float cam_z_index = -2;
uniform float z_max_culling = 1000;
uniform int mode = 0;
uniform float marching_min_distance = .01;

uniform vec3 color = vec3(1);
uniform float glow = .01;

float min_distance(vec3 position);

// MATH FUNCTIONS
float atan2(float y, float x) {
    if (x > 0) return atan(y/x);
    if (x < 0) return atan(y/x) + M_PI;
    return sign(y) * M_PI / 2;
}

mat3 rot_matrix(vec3 axis, float alpha) {
    float phi = atan2(axis.y, axis.x),
    theta = acos(axis.z / (length(axis) + TINY));
    mat3 m_phi = mat3(
        cos(phi), -sin(phi), 0,
        sin(phi), cos(phi), 0,
        0, 0, 1
    ),
        m_theta = mat3(
        cos(theta), 0, -sin(theta),
        0,1,0,
        sin(theta), 0 , cos(theta)
    ), m_alpha = mat3(
        cos(alpha), -sin(alpha), 0,
        sin(alpha), cos(alpha), 0,
        0, 0, 1
    );
    return m_phi * m_theta * m_alpha * transpose(m_theta) * transpose(m_phi);
}

mat3 cam_orientation = rot_matrix(vec3(0,1,0), cam_angles[1]) * rot_matrix(vec3(1,0,0), cam_angles[0]);


// RAY MARCHING
vec3 background(vec2 screen_point) {
    vec3 ray_direction = cam_orientation * normalize(vec3(screen_point, -cam_z_index));
    return mix(vec3(0,.08,.2), vec3(.1,.5,.8), ray_direction.y *.5 + .5);
}

float[3] march_ray(vec2 screen_point) {
    vec3 ray_position = vec3(0, 0, cam_z_index) + cam_pos;
    vec3 ray_direction = cam_orientation * normalize(vec3(screen_point, -cam_z_index));

    float curr_min_distance = min_distance(ray_position),
          prev_min_distance,
          close_peaks_inv_sq_sum = 0;
    bool closing_distance = false, culling = false;

    while (curr_min_distance > marching_min_distance) {
        
        ray_position += ray_direction * curr_min_distance;
        if (mode == 1) {ray_position.z = mod(ray_position.z, 0);}
        if (length(ray_position - cam_pos) > z_max_culling) { curr_min_distance = -1; break;}

        if (curr_min_distance > prev_min_distance) {
            if (closing_distance) close_peaks_inv_sq_sum += pow(prev_min_distance * 20, -2);
            closing_distance = false;
        } else closing_distance = true;

        prev_min_distance = curr_min_distance;
        curr_min_distance = min_distance(ray_position);
    }

    return float[3](
        curr_min_distance,
        prev_min_distance,
        close_peaks_inv_sq_sum
        );
}

float[3] compute_attributes(float[3] ray) {
    float surface = 0,
          edge = 0,
          glow = pow(ray[2], 1);
    if (ray[0] != -1) {
        surface = sqrt(pow((ray[1] - ray[0]) / ray[1],2)),
        edge = pow(1 - pow((ray[1] - ray[0])/ray[1], 2),18);
    }
    return float[3](surface, edge, glow);
}

// ELEMENTS
float mandelbulb(vec3 center, mat3 orientation, vec3 ray_pos, float power, int iterations, float threshold) {
    vec3 add = orientation * (center - ray_pos);
    vec3 z = add;
    float dr = 1;
    float r;
    for (int i = 0; i < iterations; i++) {
        r = length(z) + threshold;
        if (r > 2) break;
        float theta = acos(z.z / r) * power;
        float phi = atan2(z.y, z.x) * power;
        float zr = pow(r, power);
        dr = pow(r, power - 1) * power * dr + 1;
        z = zr * vec3( sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        z += center - ray_pos;
    }
    return (.5) * log(r) * r / dr;
}

float plane(vec3 center, vec3 normal, vec3 ray_pos) {
    float front = dot((ray_pos - center), normal) / length(normal);
    if (front >= 0) return front; return z_max_culling;
}

