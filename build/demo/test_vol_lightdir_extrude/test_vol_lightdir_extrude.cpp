#include "l3_engine.h"

using namespace l3eng;
float light_x = -40.0f;

OBJ_ID old_obj_id = -1;
OBJ_ID old_tr_id = -1;


class test_listerner : public default_listener
{
public:

	test_listerner(l3_engine * eng):default_listener(eng)
	{}


	virtual l3_int32 mouse_event(const mouse_data_t& mouse_evt)override
	{
		switch(mouse_evt.mouse_event_)
		{
		case L3_MOUSE_MOVE:
			break;

		case L3_MOUSE_LBUTTONUP:
			{
				float r_start_x;
				float r_start_y;
				float r_start_z;
				float r_dir_x;
				float r_dir_y;
				float r_dir_z;
				eng_->cam_get_screen_ray(mouse_evt.x_, mouse_evt.y_,
					r_start_x, r_start_y, r_start_z,
					r_dir_x, r_dir_y, r_dir_z);

				float tr_p1x;
				float tr_p1y;
				float tr_p1z;

				float tr_p2x;
				float tr_p2y;
				float tr_p2z;

				float tr_p3x;
				float tr_p3y;
				float tr_p3z;

				//eng_->render_obj_material_force_light_off(old_obj_id, 0);
				OBJ_ID obj_id;
				l3_f32 t = 0;
				if(eng_->ray_pickup(r_start_x, r_start_y, r_start_z,
					r_dir_x, r_dir_y, r_dir_z,
					obj_id,
					t,
					tr_p1x, tr_p1y, tr_p1z,
					tr_p2x, tr_p2y, tr_p2z,
					tr_p3x, tr_p3y, tr_p3z))
				{
					//eng_->render_obj_material_force_light_off(obj_id, 1);
					old_obj_id = obj_id;

					OBJ_ID tr_id = eng_->robj_geometry_create_triangle(tr_p1x, tr_p1y, tr_p1z,
						tr_p2x, tr_p2y, tr_p2z,
						tr_p3x, tr_p3y, tr_p3z);

					eng_->obj_del(old_tr_id);
					old_tr_id = tr_id;
				}

			}
			break;
		}

		return 0;
	}

};


void add_sky_dome(l3_engine& e)
{
	OBJ_ID skydome = e.robj_skydome_create(2000, 0.7f, 0.9f, 8, 8);
	OBJ_ID tex_id = e.tex_load("../../../resource/skydome.jpg");
	e.tex_wrap_repeate(tex_id);
	e.robj_set_tex(skydome, tex_id);
	//e.robj_set_sys_shader(skydome, "_l3eng_inter_atmospheric_ground");

	e.shader_defer_render(skydome, 1);
	e.shader_defer_light(skydome, 1);
}

int main()
{
	l3_engine e;
	test_listerner tl(&e);
	e.init_eng(&tl, 0);
	e.enable_atmospheric(0);
	e.enable_hdr(0);
	e.enable_debug(0);
	e.enable_defer_render(0);

	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(100, 200, 10,
			1, 0, 0, 0);
		e.robj_move_to_xyz(cube, 0, 0, -50);
		//e.robj_set_sys_shader(cube, "_l3eng_inter_test_program");
		//e.robj_add_sys_shader_pass(cube, "_l3eng_inter_volume_lightdir_extrude");
		//OBJ_ID tex_id = e.tex_load("../resource/WaterFresnel.psd");
		//e.robj_set_tex(cube, tex_id);
		//e.shader_defer_render(cube, 1);
	}

	if(0)
	{
		OBJ_ID sphere = e.robj_sphere_create(10, 20);
		e.robj_move_to_xyz(sphere, 0, 0, 0);
		e.robj_add_sys_shader_pass(sphere, "_l3eng_inter_volume_lightdir_extrude");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(sphere, tex_id);
	}

	if(1)
	{
		//OBJ_ID tex_id = e.tex_load("../resource/dwarf.jpg");
		OBJ_ID obj_ske  = e.robj_skeleton_create("../resource/dwarf.x");
		e.robj_scale(obj_ske, 0.2f, 0.2f, 0.2f);
		e.robj_move_to_xyz(obj_ske, -10.0f, 0.0f, 0.0f);
		e.robj_add_sys_shader_pass(obj_ske, "_l3eng_inter_volume_lightdir_extrude");

		//e.robj_set_tex(obj_ske, tex_id);
		//e.robj_set_sys_shader(obj_ske, "_l3eng_inter_test_program");
	}


	e.camera_set_pers(50.0f, 0.1f, 2000.0f);
	e.camera_move_toward(-60.0f);

	e.run();

	return 0;
}