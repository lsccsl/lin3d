#include "l3_engine.h"

using namespace l3eng;

float _g_light_pos_x = 30;
float _g_light_pos_z = -100;
OBJ_ID _g_light_id = -1;

float _g_light_rotate_x = -90;
float _g_light_rotate_y = 0;
float _g_light_rotate_z = 0;

class test_listerner : public default_listener
{
public:

	test_listerner(l3_engine* eng) :default_listener(eng)
	{
		this->move_speed_ = 3.0f;
		this->rotate_speed_ = 3.0f;
	}

	virtual l3_int32 keyboard_event(const keyboard_data_t& keyboard_evt)
	{
		switch (keyboard_evt.key_)
		{
			case L3_KEY_X:
				_g_light_pos_x -= 0.5;
				this->eng_->light_set_pos(_g_light_id, vector3(_g_light_pos_x, 60, _g_light_pos_z));
				return true;
				break;
			case L3_KEY_Z:
				_g_light_pos_x += 0.5;
				this->eng_->light_set_pos(_g_light_id, vector3(_g_light_pos_x, 60, _g_light_pos_z));
				return true;
				break;
			case L3_KEY_C:
				_g_light_pos_z -= 0.5;
				this->eng_->light_set_pos(_g_light_id, vector3(_g_light_pos_x, 60, _g_light_pos_z));
				return true;
				break;
			case L3_KEY_V:
				_g_light_pos_z += 0.5;
				this->eng_->light_set_pos(_g_light_id, vector3(_g_light_pos_x, 60, _g_light_pos_z));
				return true;
				break;

			case L3_KEY_B:
				_g_light_rotate_x -= 1;
				this->eng_->light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);
				return true;
				break;
			case L3_KEY_N:
				_g_light_rotate_x += 1;
				this->eng_->light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);
				return true;
				break;

			case L3_KEY_F:
				_g_light_rotate_y -= 1;
				this->eng_->light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);
				return true;
				break;
			case L3_KEY_G:
				_g_light_rotate_y += 1;
				this->eng_->light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);
				return true;
				break;

			case L3_KEY_H:
				_g_light_rotate_z -= 1;
				this->eng_->light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);
				return true;
				break;
			case L3_KEY_J:
				_g_light_rotate_z += 1;
				this->eng_->light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);
				return true;
				break;

			case L3_KEY_K:
				this->eng_->debug_enble_csm_gen();
				return true;
				break;
		}

		return default_listener::keyboard_event(keyboard_evt);
	}

	virtual l3_int32 mouse_event(const mouse_data_t& mouse_evt)override
	{
		switch (mouse_evt.mouse_event_)
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
	e.enable_atmospheric(0);
	e.enable_hdr(0);
	e.enable_debug(0);
	e.enable_defer_render(1);
	e.enable_ssao(0);

	if(1)//out door light
	{
		_g_light_id = e.light_create();
		e.light_type(_g_light_id, 3);
		e.light_enable(_g_light_id, 0);

		e.light_clr_diffuse(_g_light_id,
			//color(0.5f, 0.5f, .5f)
			color(0.5f, .1f, .1f)
		);

		e.light_max_len(_g_light_id, 500.0f);
		e.light_set_rotate(_g_light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);

		e.light_set_pos(_g_light_id, vector3(_g_light_pos_x, 60, _g_light_pos_z));

		e.light_enable_vol(_g_light_id, 0);
		e.light_enable_light_debug(_g_light_id, 1);
		e.light_enable_shadow(_g_light_id, 1);

		e.light_set_sun(_g_light_id);
	}

	if(1)//spot light
	{
		OBJ_ID light_id = e.light_create();
		e.light_type(light_id, 1);
		e.light_enable(light_id, 1);

		e.light_clr_diffuse(light_id,
			//color(0.5f, 0.5f, .5f)
			color(0.0f, 0.0f, 1.0f)
		);

		e.light_max_len(light_id, 300.0f);
		e.light_set_rotate(light_id, _g_light_rotate_x, _g_light_rotate_y, _g_light_rotate_z);

		e.light_set_pos(light_id, vector3(120, 60, 0));

		e.light_enable_vol(light_id, 0);
		e.light_enable_light_debug(light_id, 1);
		e.light_enable_shadow(light_id, 1);
	}

	{
		l3eng::OBJ_ID shader_id = e.shader_get("_l3eng_inter_defer_render_final");
		e.shader_set_param_f32(shader_id, EN_SHADER_PARAM_FINAL_DIFF_SCALE, 0.1f);
	}

	if (1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(2000, 10, 2000);
		e.robj_move_to_xyz(cube, -10, -50, -19);
		//e.robj_set_sys_shader(cube, "_l3eng_inter_test_program");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(cube, tex_id);
		e.shader_defer_render(cube, 1);
	}

	//if (1)
	//{
	//	OBJ_ID cube = e.robj_geometry_create_cube(1000, 1000, 10);
	//	e.robj_move_to_xyz(cube, 0, 0, -200);
	//	//e.robj_set_sys_shader(cube, "_l3eng_inter_test_program");
	//	OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
	//	e.robj_set_tex(cube, tex_id);
	//	e.shader_defer_render(cube, 0);
	//}

	{
		OBJ_ID cube = e.robj_geometry_create_cube(10, 10, 2000);
		e.robj_move_to_xyz(cube, -60, -10, 0);
		e.robj_set_sys_shader(cube, "_l3eng_inter_test_program");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(cube, tex_id);
		e.shader_defer_render(cube, 1);
	}

	for(int i = 0; i < 3; i ++)
	{
		int neg = (i % 2) ? 1 : -1;
		for (int j = 0; j < 10; j++)
		{
			OBJ_ID sphere = e.robj_sphere_create(10, 100);
			e.robj_move_to_xyz(sphere, i * 50 + (j * 5 * neg), 0,  -20 -j * 100);
			e.robj_set_sys_shader(sphere, "_l3eng_inter_test_program");
			OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
			e.robj_set_tex(sphere, tex_id);
			e.shader_defer_render(sphere, 1);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		int neg = (i % 2) ? 1 : -1;
		for (int j = 0; j < 20; j++)
		{
			OBJ_ID cube = e.robj_geometry_create_cube(10, 10, 10);
			e.robj_move_to_xyz(cube, -10 + i * 50 + (j * 5 * neg), -10,  100 - j * 20);
			e.robj_set_sys_shader(cube, "_l3eng_inter_test_program");
			OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
			e.robj_set_tex(cube, tex_id);
			e.shader_defer_render(cube, 1);
		}
	}

	e.camera_set_pers(90.0f, 1.0f, 1000.0f);
	e.camera_set(vector3(0, 0, 0), vector3(0, 40, 100));

	//e.camera_set_pers(90.0f, 1.0f, 1000.0f);
	//e.camera_set(vector3(0, 0, 0), vector3(0, 0, 0));

	e.run();

	return 0;
}

