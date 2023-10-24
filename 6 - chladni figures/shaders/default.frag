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
    if (v.x > 0) return atan(v.y/v.x) + ( v.y < 0 ? 2 * M_PI : 0 );
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
mat3 standing_wave(vec2 polar_coords, float n, float m, vec2 offset) {
    vec2 polar_coords_ = polar_coords - offset;
    float f = cos(M_PI * polar_coords_.x * n) * cos(polar_coords_.y * m),
          n_ = n * M_PI;
    
    // df_polar gradient
    vec2 df_pol = vec2(
        - n_ * sin(polar_coords_.x * n_) * cos(polar_coords_.y * m),
        - m  * cos(polar_coords_.x * n_) * sin(polar_coords_.y * m) 
    );
    
    // ddf_polar hessian
    vec3 hes_pol = vec3(
        - n_ * n_ * cos(polar_coords_.x * n_) * cos(polar_coords_.y * m ),
        - m  * m  * cos(polar_coords_.x * n_) * cos(polar_coords_.y * m ),
          n_ * m  * sin(polar_coords_.x * n_) * sin(polar_coords_.y * m )
    );
    
    // dpolar gradient
    vec2 dpol_dx = vec2(cos(polar_coords_.y), -sin(polar_coords_.y) / polar_coords.x ),
         dpol_dy = vec2(sin(polar_coords_.y),  cos(polar_coords_.y) / polar_coords.x ),
         dr      = vec2(dpol_dx.x ,dpol_dy.x),
         dtheta  = vec2(dpol_dx.y, dpol_dy.y),
    
    // ddpolar hessian     
         ddpol_dxx = vec2(1,  2 * cos(polar_coords_.y) * sin(polar_coords_.y) / polar_coords_.x ) /polar_coords_.x,
         ddpol_dyy = vec2(1, -2 * cos(polar_coords_.y) * sin(polar_coords_.y) / polar_coords_.x) / polar_coords_.x,
         ddpol_dxy = vec2(0,  (pow(sin(polar_coords_.y),2) - pow(cos(polar_coords_.y),2)) / pow(polar_coords_.x, 2)),
    
    // df gradient
        grad = vec2( dot(df_pol, dpol_dx), dot(df_pol, dpol_dy));
    
    // ddf hessian
    vec3 hes = vec3(
        dot(hes_pol, vec3(dpol_dx * dpol_dx, dpol_dx.x * dpol_dx.y)) + dot(df_pol, ddpol_dxx),
        dot(hes_pol, vec3(dpol_dy * dpol_dy, dpol_dy.x * dpol_dy.y)) + dot(df_pol, ddpol_dyy),
        dot(hes_pol, vec3(dpol_dx * dpol_dy, dot(dr, dtheta.yx))) + dot(df_pol, ddpol_dxy)
    );
    return mat3(f, grad, hes, vec3(0, 0, 0));
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - resolution)/(vec2(1, 1) * resolution.y);
    if (length(uv) > 1) { gl_FragColor = vec4(0, 0, 0, 1); return; }
    vec2 pol = vec2(min(length(uv), 1), atan2(uv));
    mat3 f = mat3(0);

    int MODE = 1;

    switch (MODE) {
        case 0:
            f += standing_wave(pol + vec2(0, 0), 3, 5, vec2(1 + time_now/1e4, + time_now/1e4)) / 2;
            f += standing_wave(pol + vec2(0, 0), 6, 4, vec2(1 + time_now/1e4 * 2, + time_now/1e4)) / 2;
            break;
        case 1:
            f += standing_wave(pol + vec2(0, 0), 3, 8, vec2(- time_now/1e5, 0)) / 2;
            f += standing_wave(pol + vec2(0, 0), 2, 6, vec2(0, 0)) / 2;
            break;
        case 2:
            f += standing_wave(pol + vec2(0, 0), 1, 2, vec2(- time_now/1e5, 0)) / 2;
            f += standing_wave(pol + vec2(0, 0), 2, 3, vec2(0, time_now/1e4)) / 2;
            break;
        default:
            break;
    }

    vec2 grad       = normalize(f[0].yz),
         cross_grad = normalize(f[0].zy * vec2(-1, 1));
    vec3 normal = normalize(vec3(-f[0].yz, 1)) * .5 + .5;
    float h_det = f[1].x * f[1].y - pow(f[1].z, 2),
          df_cross_grad = dot(f[0].yz, cross_grad),
          ddf_grad = dot(f[1], vec3(pow(grad.x, 2), pow(grad.y, 2), 2 * grad.x * grad.y)), 
          ddf_cross_grad = dot(f[1], vec3(pow(cross_grad.x, 2), pow(cross_grad.y, 2), 2 * cross_grad.x * cross_grad.y)),
          ddf_grad_cross_grad = dot(f[1], vec3( vec2(-1, 1) * grad.x * grad.y, pow(grad.x, 2) - pow(grad.y, 2)));


    float contours, drains, height;
    //intensity = (1/(pow(length(f[0].yz),2) + .01)); // gradient ^ -2 (maxima & minima)
    //intensity = (.01/(pow(length(f[0].x),2) + .01)); // height ^ -2 (intersection w\ y plane)
    //intensity = f[0].x * .5 + .5;
    
    height = f[0].x * .5 + .5;
    contours = pow(cos(10* M_PI * height), 10);
    drains =  1 / (1 + exp( -(ddf_cross_grad-50)));

    
    //gl_FragColor = vec4(colormap(intensity), 1);
    switch (MODE) {
        case 0:
            gl_FragColor = vec4(
                vec3(-1,-1,-1) * contours * height +
                vec3(1,.7,.3) * 1.1 * drains + 
                vec3(.6,.8,1) * 1.5 * height + 
                0,
            1);
        case 1:
            drains = sin(sqrt(h_det) / 20 + time_now/1e3);
            gl_FragColor = vec4(
                vec3(1,0,0) * drains +
                vec3(0,1,0) * contours + 
                vec3(0,0,1) * height + 
                0
            ,1);
            break;
        case 2:
            contours = pow(cos(20* M_PI * height + time_now/1e3), 10);
            gl_FragColor = vec4(
                mix(normal, vec3(1,1,1), contours)
            ,1);
        default:
            break;
    }
    
    // // normal lighting
    //gl_FragColor = vec4(vec3(1,1,1) * ddf_grad_cross_grad, 1);
}