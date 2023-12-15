#include "Render.h"
#include "App.h"

class FluidSim {
    private:
        AppState& app;
        u32 res_w = 20, res_h = 20;
        Texture props1, props2; // each pixel includes the data for [v_x, v_y, pressure, density]
        bool swap = true;
        ComputeShader update_screen;
    public:
        FluidSim(AppState& app):
            app(app),
            props1(res_w + 2, res_h + 2, (f32*)0),
            props2(res_w + 2, res_h + 2, (f32*)0),
            update_screen("./shaders/update_screen.comp") {
            app.render.screen_tex.bind_image();
            
            props2.bind_image();
            for (int i = 0; i < 10; i++) 
                for (int j = 0; j < 10; j++) {
                    props1.set(i, j, 0, 100);
                    props1.set(i, j, 1, 100);
                    props1.set(i, j, 2, 100);
                    props1.set(i, j, 3, 100);
                }
            props1.set_to_gpu();
            props1.get_from_gpu();
            LOG_DEBUG("%f %f %f %f", props1.get(0,0,0),props1.get(0,0,1),props1.get(0,0,2),props1.get(0,0,3));
            props1.bind_image();
            
        }
        void update(){
            //swap = !swap;
            Texture *curr = swap ? &props1 : &props2, *old = swap ? &props2 : &props1;
            curr = &props1;
            update_screen.bind();
            update_screen.set_uniform("uScreenTex", (int)app.render.screen_tex.image);
            update_screen.set_uniform("uCurrProps", (int)curr->image);
            update_screen.execute(res_w, res_h);
        }
        void imgui() {
        }

        void draw(){
            
        }
        ~FluidSim() {

        }
};