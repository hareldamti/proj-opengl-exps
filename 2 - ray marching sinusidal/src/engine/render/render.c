#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "../render.h"
#include "../global.h"
#include "../io.h"
#include "../utils.h"

int render_init() {
    global.render.width = 1280;
    global.render.height = 720;
    global.render.window = render_init_window(global.render.width, global.render.height);
    render_init_shader();
    return 0;
}

int render_init_shader() {
    global.render.shader = create_shader("./shaders/default.vert", "./shaders/default.frag");
    glUseProgram(global.render.shader);
    i32 resolution = glGetUniformLocation(global.render.shader, "resolution");
    glUniform2f(resolution, global.render.width, global.render.height);
    return 0;
}

SDL_Window* render_init_window(u32 width, u32 height) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
        ERROR_EXIT("Couldn't init SDL: %s\n", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(
        "Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_OPENGL
    );
    
    if (!window) 
        ERROR_EXIT("Failed to init window: %s\n", SDL_GetError());
    
    

    SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
        ERROR_EXIT("Failed to load GL: %s\n", SDL_GetError());
    

    printf("OpenGL Loaded\n");
    printf("Vendor:   %s\nRenderer: %s\nVersion:  %s\n",
    glGetString(GL_VENDOR),
    glGetString(GL_RENDERER),
    glGetString(GL_VERSION));
    return window;
}

u32 compile_shader(const char* path, GLenum type) {
    File file = io_file_read(path);
    if (!file.is_valid) 
        ERROR_EXIT("Error reading shader: %s\n", path);
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char *const *)&file.data, NULL);
    int success;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char* msg = (char*)alloca( length * sizeof(char));
        glGetShaderInfoLog(shader, length, &length, msg);
        ERROR_EXIT("Error compiling GLenum(%d) shader. %s\n", type, msg);
    }
    free(file.data);
    return shader;
}

u32 create_shader(const char *path_vert, const char *path_frag) {
    int success;
    char log[512];
    u32 vs = compile_shader(path_vert, GL_VERTEX_SHADER),
        fs = compile_shader(path_frag, GL_FRAGMENT_SHADER);

    u32 program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, log);
        ERROR_EXIT("Error linking shader. %s\n", log);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void draw_screen() {
    float vertices[] = {
        -1,  1, 1, 0,
         1,  1, 1, 1,
         1, -1, 0, 1,
        -1, -1, 0, 0
    };
    i32 elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    //for (int i = 0; i < sizeof(vertices)/sizeof(float); i++) vertices[i] *= .9;
    
    u32 vao, vbo, ebo;
    int error;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    i32 posAttrib = glGetAttribLocation(global.render.shader, "a_pos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    i32 timeUni = glGetUniformLocation(global.render.shader, "time_now");
    glUniform1f(timeUni, global.time.now);


    float points[5];
    for (int i = 0; i < 5; i++) points[i] = global.time.now / 1e3 + i * 2 * M_PI / 5;
    i32 pointsUni = glGetUniformLocation(global.render.shader, "points");
    glUniform1fv(pointsUni, 5, points);

    if ((error = glGetError()) != 0) ERROR_EXIT("after unif- OpenGL error: %x\n", error);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if ((error = glGetError()) != 0) ERROR_EXIT("after draw- OpenGL error: %x\n", error);

    SDL_GL_SwapWindow(global.render.window);
}