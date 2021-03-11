#include "l3_engine.h"

using namespace l3eng;

float dissolution_threshold = 0.5f;
float dissolution_transition = 0.3f;

class test_listerner : public default_listener
{
public:

	test_listerner(l3_engine * eng):default_listener(eng)
	{}

	virtual l3_int32 keyboard_event(const keyboard_data_t& keyboard_evt)
	{		
		switch(keyboard_evt.key_)
		{
		case L3_KEY_X:
			{
				dissolution_threshold += 0.03f;
				OBJ_ID shdr_dis = this->eng_->shader_get("_l3eng_inter_dissolution");
				this->eng_->shader_set_param_f32(shdr_dis, EN_SHADER_PARAM_DISSOLUTION_THRESHOLD, dissolution_threshold);
			}
			break;
		case L3_KEY_Z:
			{
				dissolution_threshold -= 0.03f;
				OBJ_ID shdr_dis = this->eng_->shader_get("_l3eng_inter_dissolution");
				this->eng_->shader_set_param_f32(shdr_dis, EN_SHADER_PARAM_DISSOLUTION_THRESHOLD, dissolution_threshold);
			}
			break;

		case L3_KEY_C:
			{
				dissolution_transition += 0.03f;
				OBJ_ID shdr_dis = this->eng_->shader_get("_l3eng_inter_dissolution");
				this->eng_->shader_set_param_f32(shdr_dis, EN_SHADER_PARAM_DISSOLUTION_TRANSITION, dissolution_transition);
			}
			break;
		case L3_KEY_V:
			{
				dissolution_transition -= 0.03f;
				OBJ_ID shdr_dis = this->eng_->shader_get("_l3eng_inter_dissolution");
				this->eng_->shader_set_param_f32(shdr_dis, EN_SHADER_PARAM_DISSOLUTION_TRANSITION, dissolution_transition);
			}
			break;
		}

		return default_listener::keyboard_event(keyboard_evt);
	}

	virtual l3_int32 mouse_event(const mouse_data_t& mouse_evt)override
	{
		switch(mouse_evt.mouse_event_)
		{
		case L3_MOUSE_MOVE:
			break;

		case L3_MOUSE_LBUTTONUP:
			break;
		}

		return 0;
	}

};

int main()
{
	l3_engine e;

	test_listerner tl(&e);
	e.init_eng(&tl, 0);
	e.enable_defer_render(0);
	e.enable_hdr(0);
	e.enable_debug(1);

	OBJ_ID shdr_dis = e.shader_get("_l3eng_inter_dissolution");
	OBJ_ID tex_id = e.tex_load("../resource/terrain-heightmap.bmp");
	e.shader_set_param_obj(shdr_dis, EN_SHADER_PARAM_DISSOLUTION_RAND_TEX, tex_id);

	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(100, 100, 1, true, 255, 255, 255);
		e.robj_move_to_xyz(cube, 0, 0, -50);
		//e.robj_set_sys_shader(cube, "_l3eng_inter_pbr_forward");
		OBJ_ID tex_id = e.tex_load("../resource/skydome2.jpg");
		e.robj_set_tex(cube, tex_id);
		//e.shader_defer_render(cube, 1);
	}


	{
		OBJ_ID sphere = e.robj_sphere_create(10, 200);
		e.robj_move_to_xyz(sphere, 0, 0, -25);
		e.robj_set_sys_shader(sphere, "_l3eng_inter_dissolution");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(sphere, tex_id);
	}


	e.camera_set_pers(60, 1, 1000);
	e.run();

	return 0;
}

