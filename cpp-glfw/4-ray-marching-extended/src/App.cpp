#include "Render.h"
#include "Env.h"
#include "unordered_map"
#include <math.h>
using namespace std;

class Vec2 {
    public:
    float x, y;
    Vec2(): x(0), y(0) {}
    Vec2(float x, float y): x(x), y(y) {}
    void operator+=(Vec2 other) {
        x += other.x;
        y += other.y;
    }
    void operator-=(Vec2 other) {
        x -= other.x;
        y -= other.y;
    }
    Vec2 operator*(float c) { return Vec2(x * c, y * c); }
};
class App {
    private:
        EnvState& env;
        unordered_map<std::string, RenderShader> shaders;

        // configuration
        float move_speed = .007, rot_speed = .002;
        Vec2 cam_pos, cam_angles;
        float threshold = .01, z_index = 1, glow = 1;
        float surf_col[3];
        bool repeat = false;
    public:
        App(EnvState& env):
            env(env){
            shaders["default"].init_util("./shaders/default.vert", "./shaders/util.frag", "./shaders/default.frag");
        }
        void update(){
            auto unit = [](float alpha) { return Vec2(cos(alpha), sin(alpha)); };
            if (env.input.is_pressed(GLFW_KEY_SPACE)) shaders["default"].init_util("./shaders/default.vert", "./shaders/util.frag", "./shaders/default.frag");
            if (env.input.is_pressed(GLFW_KEY_UP)) cam_pos += unit(-cam_angles.x + M_PI * .5) * move_speed;
            if (env.input.is_pressed(GLFW_KEY_LEFT)) cam_pos += unit(-cam_angles.x + M_PI) * move_speed;
            if (env.input.is_pressed(GLFW_KEY_DOWN)) cam_pos += unit(-cam_angles.x + M_PI * 1.5) * move_speed;
            if (env.input.is_pressed(GLFW_KEY_RIGHT)) cam_pos += unit(-cam_angles.x) * move_speed;
            if (env.input.is_pressed(GLFW_KEY_W)) cam_angles.y += rot_speed;
            if (env.input.is_pressed(GLFW_KEY_S)) cam_angles.y -= rot_speed;
            if (env.input.is_pressed(GLFW_KEY_D)) cam_angles.x += rot_speed;
            if (env.input.is_pressed(GLFW_KEY_A)) cam_angles.x -= rot_speed;
            }
        void imgui() {
            ImGui::Begin("Ray marching configuration");
            ImGui::SliderFloat("distance threshold", &threshold, 0.001f, 0.1f);
            ImGui::SliderFloat("perspective", &z_index, .2f, 3.f);
            ImGui::SliderFloat("glow", &glow, 0.f, 5.f);
            ImGui::ColorEdit3("surface color", surf_col);
            ImGui::Checkbox("Endless repeat", &repeat);
            ImGui::End();
        }

        void draw(){
            shaders["default"].bind();
            shaders["default"].set_uniform("time", (f32)glfwGetTime());
            shaders["default"].set_uniform("screen_ratio", (f32)(env.render.width / env.render.height));
            shaders["default"].set_uniform("cam_pos", cam_pos.x, 0, cam_pos.y);
            shaders["default"].set_uniform("cam_angles", cam_angles.y, cam_angles.x);
            shaders["default"].set_uniform("marching_min_distance", threshold);
            shaders["default"].set_uniform("cam_z_index", -z_index);
            shaders["default"].set_uniform("mode", repeat ? 1 : 0);
            shaders["default"].set_uniform("color", surf_col[0], surf_col[1], surf_col[2]);
            shaders["default"].set_uniform("glow", glow);
        }
        ~App() {

        }
};