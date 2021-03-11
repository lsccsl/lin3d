#include "l3_engine.h"

using namespace l3eng;

class test_listerner : public default_listener
{
public:

	test_listerner(l3_engine* eng) :default_listener(eng)
	{}

	virtual l3_int32 keyboard_event(const keyboard_data_t& keyboard_evt)
	{
		switch (keyboard_evt.key_)
		{
			case L3_KEY_X:
				return true;
				break;
			case L3_KEY_Z:
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
	e.enable_defer_render(0);

	{
		l3eng::OBJ_ID shader_id = e.shader_get("_l3eng_inter_defer_render_final");
		e.shader_set_param_f32(shader_id, EN_SHADER_PARAM_FINAL_DIFF_SCALE, 0.01f);
	}

	if (1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(1000, 10, 1000);
		e.robj_move_to_xyz(cube, -10, -35, -19);
		//e.robj_set_sys_shader(cube, "_l3eng_inter_test_program");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(cube, tex_id);
		e.shader_defer_render(cube, 1);
	}

	if (1)
	{
		OBJ_ID sphere = e.robj_sphere_create(10, 200);
		e.robj_move_to_xyz(sphere, 0, 0, -19);
		e.robj_set_sys_shader(sphere, "_l3eng_inter_test_program");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(sphere, tex_id);
		e.shader_defer_render(sphere, 1);
	}

	e.camera_set_pers(50.0f, 0.1f, 2000.0f);
	//e.camera_move_toward(-10.0f);
	e.camera_set(vector3(0, 0, 0), vector3(0, 0, 100));

	e.run();

	return 0;
}

