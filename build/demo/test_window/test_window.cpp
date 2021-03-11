//#include "../../../src/lin3d/win_device.h"
#include "l3_engine.h"
#include "l3_image.h"
#include <list>
#include <string>
#include <vector>
#include "l3_log.h"

using namespace l3eng;

OBJ_ID obj_cube_move;
OBJ_ID __light_id = -1;
OBJ_ID __light_id1 = -1;
OBJ_ID __light_id2 = -1;

float light_x = -40.0f;
float light1_z = 0.0f;

OBJ_ID old_obj_id = -1;
OBJ_ID old_tr_id = -1;

class test_listerner : public default_listener
{
public:

	test_listerner(l3_engine * eng):default_listener(eng)
	{}

	virtual l3_int32 keyboard_event(const keyboard_data_t& evt)override
	{
		switch(evt.key_)
		{
		case L3_KEY_Z:
			{
				light_x = light_x - 1.0f;
				eng_->light_set_pos(__light_id, vector3(light_x, 0.0f, -10.0f));

				//this->eng_->robj_move_to_xyz(obj_cube_move, light_x, 0.0f, -10.0f);
				return 0;
			}
			break;

		case L3_KEY_X:
			{
				light_x = light_x + 1.0f;
				eng_->light_set_pos(__light_id, vector3(light_x, 0.0f, -10.0f));

				//this->eng_->robj_move_to_xyz(obj_cube_move, light_x, 0.0f, -10.0f);
				return 0;
			}
			break;

		case L3_KEY_C:
			{
				light1_z = light1_z - 1.0f;
				eng_->light_set_pos(__light_id1, vector3(0.0f, 0.0f, light1_z));
			}
			break;
		case L3_KEY_V:
			{
				light1_z = light1_z + 1.0f;
				eng_->light_set_pos(__light_id1, vector3(0.0f, 0.0f, light1_z));
			}
			break;

		case L3_KEY_TAB:
			{
				static int __hdr = 0;
				eng_->enable_hdr((__hdr ++) %2);
			}
			break;
		}

		return default_listener::keyboard_event(evt);
	}

	//virtual l3_int32 mouse_event(const mouse_data_t& mouse_evt)override
	//{
	//	switch(mouse_evt.mouse_event_)
	//	{
	//	case L3_MOUSE_MOVE:
	//		break;
	//	
	//	case L3_MOUSE_LBUTTONUP:
	//		{
	//			float r_start_x;
	//			float r_start_y;
	//			float r_start_z;
	//			float r_dir_x;
	//			float r_dir_y;
	//			float r_dir_z;
	//			eng_->cam_get_screen_ray(mouse_evt.x_, mouse_evt.y_,
	//				r_start_x, r_start_y, r_start_z,
	//				r_dir_x, r_dir_y, r_dir_z);

	//			float tr_p1x;
	//			float tr_p1y;
	//			float tr_p1z;

	//			float tr_p2x;
	//			float tr_p2y;
	//			float tr_p2z;

	//			float tr_p3x;
	//			float tr_p3y;
	//			float tr_p3z;

	//			//eng_->render_obj_material_force_light_off(old_obj_id, 0);
	//			OBJ_ID obj_id;
	//			if(eng_->ray_pickup(r_start_x, r_start_y, r_start_z,
	//				r_dir_x, r_dir_y, r_dir_z, obj_id,
	//				tr_p1x, tr_p1y, tr_p1z,
	//				tr_p2x, tr_p2y, tr_p2z,
	//				tr_p3x, tr_p3y, tr_p3z))
	//			{
	//				//eng_->render_obj_material_force_light_off(obj_id, 1);
	//				old_obj_id = obj_id;

	//				OBJ_ID tr_id = eng_->robj_geometry_create_triangle(tr_p1x, tr_p1y, tr_p1z,
	//					tr_p2x, tr_p2y, tr_p2z,
	//					tr_p3x, tr_p3y, tr_p3z);

	//				eng_->obj_del(old_tr_id);
	//				old_tr_id = tr_id;
	//			}

	//		}
	//		break;
	//	}

	//	return 0;
	//}
};

void test_image()
{
	l3_image img;

	img.load("../resource/eq2_bigmet_01.jpg");
	//img.save("../../../resource/xxx.jpg");

	l3_image img1;
	img1.load("../resource/AA.JPG");


	l3_image img2;
	img2.load("../resource/rockwall.jpg");
}

void test_list()
{
	std::list<std::string> lst_str;
	for(int i = 0; i < 500000; i ++)
	{
		std::string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaa";
		lst_str.push_back(str);
	}
}

void test_vector()
{
	std::vector<std::string> lst_vec;
	for(int i = 0; i < 500000; i ++)
	{
		std::string str = "aaaaaaaaaaaaaaaaaaaaaaaaaaa";
		lst_vec.push_back(str);
	}
}

void build_wall_with_hole(l3_engine& e)
{
	OBJ_ID tex_id = e.tex_load("../resource/AA.JPG");
	{
		OBJ_ID obj_cube = e.robj_geometry_create_cube(0.5f, 10.0f, 40.0f);
		e.shader_defer_render(obj_cube, 1);

		e.robj_move_to_xyz(obj_cube, -20.0f, 10.0f, -20.0f);
		//e.robj_set_tex(obj_cube, tex_id);
	}

	{
		OBJ_ID obj_cube = e.robj_geometry_create_cube(0.5f, 10.0f, 40.0f);
		e.shader_defer_render(obj_cube, 1);

		e.robj_move_to_xyz(obj_cube, -20.0f, -10.0f, -20.0f);
		//e.robj_set_tex(obj_cube, tex_id);
	}

	{
		OBJ_ID obj_cube = e.robj_geometry_create_cube(0.5f, 12.0f, 10.0f);
		e.shader_defer_render(obj_cube, 1);

		e.robj_move_to_xyz(obj_cube, -20.0f, 0.0f, -5.0f);
		//e.robj_set_tex(obj_cube, tex_id);
	}

	{
		OBJ_ID obj_cube = e.robj_geometry_create_cube(0.5f, 42.0f, 50.0f);
		e.shader_defer_render(obj_cube, 1);

		e.robj_move_to_xyz(obj_cube, -20.0f, 0.0f, -40.0f);
		//e.robj_set_tex(obj_cube, tex_id);
	}
}

void add_sky_dome(l3_engine& e)
{
	OBJ_ID skydome = e.robj_skydome_create(2000, 0.7f, 0.9f, 8, 8);
	OBJ_ID tex_id = e.tex_load("../resource/skydome.jpg");
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
	e.enable_debug(1);
	e.enable_hdr(1);

	//OBJ_ID light_id = -1;

	if( 1 )
	{
		__light_id = e.light_create();
		e.light_enable(__light_id, 1);
		e.light_clr_diffuse(__light_id, color(0.0f, 1.0f, 0.0f));
		e.light_spot_exponent(__light_id, 3.0f);
		e.light_spot_cutoff(__light_id, 20.0f);

		e.light_set_rotate(__light_id, 0, -90, 0);
		e.light_set_pos(__light_id, vector3(light_x, 0.0f, -10.0f));
		e.light_enable_vol(__light_id, 1);
		e.light_enable_shadow(__light_id, 1);
		e.light_enable_light_debug(__light_id, 0);
	}

	if( 1 )
	{
		OBJ_ID light_id = e.light_create();
		e.light_enable(light_id, 1);
		e.light_clr_diffuse(light_id, color(1.0f, 0.0f, 0.0f));
		e.light_spot_exponent(light_id, 3.0f);
		e.light_spot_cutoff(light_id, 60.0f);
		e.light_set_pos(light_id, vector3(0.0f, 0.0f, 15.0f));
		e.light_enable_shadow(light_id, 1);
		e.light_enable_vol(light_id, 0);
		e.light_enable_light_debug(light_id, 0);

		__light_id1 = light_id;
		__light_id2 = light_id;
	}

	if( 1 )
	{
		OBJ_ID light_id = e.light_create();
		e.light_enable(light_id, 1);
		e.light_clr_diffuse(light_id, color(0.0f, 0.0f, 1.0f));
		e.light_spot_exponent(light_id, 3.0f);
		e.light_spot_cutoff(light_id, 60.0f);
		e.light_set_pos(light_id, vector3(-5.0f, 0.0f, 15.0f));
		e.light_enable_shadow(light_id, 1);
		e.light_enable_vol(light_id, 0);
		e.light_enable_light_debug(light_id, 0);

		__light_id1 = light_id;
	}


	//for(int i = 0; i < 5; i ++)
	//{
	//	OBJ_ID light_id1 = e.light_create();
	//	e.light_enable(light_id1, 1);
	//	e.light_clr_diffuse(light_id1, color(0.1f, 0.1f, 0.1f));
	//	e.light_spot_exponent(light_id1, 3.0f);
	//	e.light_spot_cutoff(light_id1, 60.0f);
	//	e.light_set_pos(light_id1, vector3(-1.0f * (i + 1), 0, 0));
	//}

	//for(int i = 0; i < 5; i ++)
	//{
	//	OBJ_ID light_id1 = e.light_create();
	//	e.light_enable(light_id1, 1);
	//	e.light_clr_diffuse(light_id1, color(0.1f * i, 0.0f, 0.0f));
	//	e.light_spot_exponent(light_id1, 3.0f);
	//	e.light_spot_cutoff(light_id1, 60.0f);
	//	e.light_set_pos(light_id1, vector3(1 * (i + 1), 0, 0));
	//}

	//for(int i = 0; i < 5; i ++)
	//{
	//	OBJ_ID light_id1 = e.light_create();
	//	e.light_enable(light_id1, 1);
	//	e.light_clr_diffuse(light_id1, color(0.0f, 0.0f, 0.1f * i));
	//	e.light_spot_exponent(light_id1, 3.0f);
	//	e.light_spot_cutoff(light_id1, 60.0f);
	//	e.light_set_pos(light_id1, vector3(0, 1 * (i + 1), 0));
	//}

	//for(int i = 0; i < 5; i ++)
	//{
	//	OBJ_ID light_id1 = e.light_create();
	//	e.light_enable(light_id1, 1);
	//	e.light_clr_diffuse(light_id1, color(0.0f, 0.1f * i, 0.1f * i));
	//	e.light_spot_exponent(light_id1, 3.0f);
	//	e.light_spot_cutoff(light_id1, 60.0f);
	//	e.light_set_pos(light_id1, vector3(0, -1 * (i + 1), 0));
	//}


	l3_uint8 r = 230;
	l3_uint8 g = 230;
	l3_uint8 b = 230;

	//test_list();

#if 1

	build_wall_with_hole(e);

	add_sky_dome(e);

	// dwarf mesh
	if( 1 )
	{
		OBJ_ID obj_ske  = e.robj_skeleton_create("../resource/dwarf.x");
		e.robj_scale(obj_ske, 0.2f, 0.2f, 0.2f);
		e.shader_defer_render(obj_ske, 1);
		e.robj_move_to_xyz(obj_ske, 6.0f, -14.2f, -21.0f);

		e.robj_set_sys_shader(obj_ske, "_l3eng_inter_test_program");
	}

	if( 1 )
	{
		OBJ_ID obj_cone = e.robj_cone_create(5.0f, 60.0f, 20, vector3(1, -1, 1));
		e.shader_defer_render(obj_cone, 1);
		e.robj_move_to_xyz(obj_cone, 0.0f, -0.0f, -10.0f);
		//e.robj_rotate(obj_cone, -30.0f, 0.0f, 0.0f);
		//e.robj_set_sys_shader(obj_cone, "_l3eng_inter_test_program");
	}

	if( 1 )
	{
		l3_f32 d1 = 100;
		l3_f32 d2 = 1000;
		l3_f32 d3 = 1;
		OBJ_ID obj_cube_back = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_cube_back, 1);
		e.robj_move_to_xyz(obj_cube_back, 25.0, 0, -150.0);
		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(obj_cube_back, tex_id);

		//e.robj_set_rtt(obj_cube, rtt);
		//e.robj_set_sys_shader(obj_cube, "_l3eng_inter_defer_render_gbuffer");
		//e.robj_set_sys_shader(obj_cube, "_l3eng_inter_test_program");
	}

	if( 1 )
	{
		l3_f32 d1 = 40;
		l3_f32 d2 = 10;
		l3_f32 d3 = 5;
		OBJ_ID obj_right_front = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_right_front, 1);
		e.robj_move_to_xyz(obj_right_front, 30, -10, -30);
		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(obj_right_front, tex_id);
	}

	if( 1 )
	{
		l3_f32 d1 = 50;
		l3_f32 d2 = 1;
		l3_f32 d3 = 100;
		OBJ_ID obj_cube_up = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_cube_up, 1);
		e.robj_move_to_xyz(obj_cube_up, 0, 15, -50);
		//OBJ_ID tex_id = e.tex_load("../../../resource/AA.JPG");
		//e.robj_set_tex(obj_cube_up, tex_id);
	}

	if( 1 )
	{
		l3_f32 d1 = 120;
		l3_f32 d2 = 1;
		l3_f32 d3 = 100;
		OBJ_ID obj_cube_down = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_cube_down, 1);
		e.robj_move_to_xyz(obj_cube_down, 0, -15, -50);
		//OBJ_ID tex_id = e.tex_load("../../../resource/AA.JPG");
		//e.robj_set_tex(obj_cube_down, tex_id);
	}

	if( 1 )
	{
		l3_f32 d1 = 1;
		l3_f32 d2 = 35;
		l3_f32 d3 = 1;
		OBJ_ID obj_pillar_left = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_pillar_left, 1);
		e.robj_move_to_xyz(obj_pillar_left, -15, 0, -5);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(obj_pillar_left, tex_id);
	}

	if( 1 )
	{
		l3_f32 d1 = 1;
		l3_f32 d2 = 35;
		l3_f32 d3 = 3;
		OBJ_ID obj_pillar_right = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_pillar_right, 1);
		e.robj_move_to_xyz(obj_pillar_right, 5, 0, -15);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(obj_pillar_right, tex_id);
	}

	if( 1 )
	{
		l3_f32 d1 = 1;
		l3_f32 d2 = 1;
		l3_f32 d3 = 1;
		obj_cube_move = e.robj_geometry_create_cube(d1, d2, d3,
			L3_TRUE, r, g, b);
		e.shader_defer_render(obj_cube_move, 1);
		e.robj_move_to_xyz(obj_cube_move, -5, -14.2f, -10);
		e.robj_rotate(obj_cube_move, 0, 45, 0);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(obj_cube_move, tex_id);

		//e.robj_set_sys_shader(obj_cube_move, "_l3eng_inter_test_program");
	}

	if( 1 )
	{
		OBJ_ID sphere = e.robj_sphere_create(5, 100);
		e.shader_defer_render(sphere, 1);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_move_to_xyz(sphere, 7, 0, -30);
		//e.robj_set_tex(sphere, tex_id);

		e.robj_set_sys_shader(sphere, "_l3eng_inter_test_program");
	}

	if( 1 )
	{
		for(int i = 0; i < 30; i ++)
		{
			l3_f32 d1 = 1;
			l3_f32 d2 = 1;
			l3_f32 d3 = 1;
			OBJ_ID obj_small_cube = e.robj_geometry_create_cube(d1, d2, d3,
				L3_TRUE, r, g, b);
			e.shader_defer_render(obj_small_cube, 1);
			e.robj_move_to_xyz(obj_small_cube, -14, -14.0f, -5 - i * 2);
			OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
			e.robj_set_tex(obj_small_cube, tex_id);
		}
	}

#endif

#if 0
	{
		d = 2;
		l3_int64 o = e.robj_geometry_create_cube(d, d, d);
		e.robj_move_to_xyz(o, 0, 0, 0);
	}
#endif

	if(1)
	{
		OBJ_ID obj_cone = e.robj_cone_create(5.0f, 60.0f);
		e.robj_move_to_xyz(obj_cone, 0, -10, 0);
		e.shader_defer_render(obj_cone, 1);
		//e.robj_set_sys_shader(obj_cone, "_l3eng_inter_test_program");
	}

	if(1)
	{
		OBJ_ID sphere = e.robj_sphere_create(2, 100);
		e.robj_move_to_xyz(sphere, 5, 0, -19);
		//e.robj_set_sys_shader(sphere, "_l3eng_inter_test_program");
		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(sphere, tex_id);
		e.shader_defer_render(sphere, 1);

	//e.shader_defer_render(sphere, 1);
	//e.shader_defer_light(sphere, 0);
	}
	if(1)
	{
		OBJ_ID sphere = e.robj_sphere_create(2, 100);
		e.robj_move_to_xyz(sphere, -5, 0, -19);
		//e.robj_set_sys_shader(sphere, "_l3eng_inter_test_program");
		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(sphere, tex_id);
		e.shader_defer_render(sphere, 1);
	}

	//if(0)
	//{
	//	l3_f32 d1 = 400;
	//	l3_f32 d2 = 200;
	//	l3_f32 d3 = 1;
	//	OBJ_ID obj_cube_back = e.robj_geometry_create_cube(d1, d2, d3,
	//		L3_TRUE, r, g, b);

	//	e.robj_move_to_xyz(obj_cube_back, 0, 0, -100);
	//	OBJ_ID tex_id = e.tex_load("../../../resource/eq2_bigmet_01.jpg");
	//	e.robj_set_tex(obj_cube_back, tex_id);
	//	e.robj_set_sys_shader(obj_cube_back, "_l3eng_inter_test_program");
	//}


#if 0
	OBJ_ID rtt_defer = e.shader_defer_gbuffer();
	OBJ_ID rtt_defer_light = e.shader_defer_rtt_light();
	OBJ_ID rtt_light = e.light_get_rtt(light_id);
	OBJ_ID sence_dep_rtt = e.rtt_get_tex_dep(rtt_defer);
	OBJ_ID rtt_light_shadow = e.shader_defer_rtt_light_shadow();

	if(1)
	{
		obj_cube1 = e.robj_geometry_create_cube(4, 4, 4, true, 255, 255, 255);
		e.robj_move_to_xyz(obj_cube1, 6, 0, 0);

		//OBJ_ID tex_rtt = e.rtt_get_tex_clr(rtt, 1);
		//OBJ_ID tex_rtt = e.rtt_get_tex_clr(rtt_defer, 0);
		OBJ_ID tex_rtt = e.rtt_get_tex_clr(rtt_light_shadow, 0);		
		
		e.robj_set_tex(obj_cube1, tex_rtt);
		//e.robj_set_sys_shader(obj_cube1, "_l3eng_inter_test_program");
		//e.robj_set_sys_shader(obj_cube1, "_l3eng_inter_defer_render_gbuffer");	
	}

	if(0)
	{
		l3_int64 obj_cube2 = e.robj_geometry_create_cube(4, 4, 4, true, 255, 255, 255);
		e.robj_move_to_xyz(obj_cube2, -6, 0, 0);

		//OBJ_ID tex_rtt = e.rtt_get_tex_clr(rtt, 0);
		OBJ_ID tex_rtt = e.rtt_get_tex_clr(rtt_defer, 1);

		e.robj_set_tex(obj_cube2, tex_rtt);
	}

	if(0)
	{
		l3_int64 obj_cube3 = e.robj_geometry_create_square(4, 4, 1);
		
		e.robj_move_to_xyz(obj_cube3, 0, 0, 0);

		OBJ_ID dep_rtt = e.rtt_get_tex_dep(rtt_light);
		//OBJ_ID dep_rtt = e.rtt_get_tex_dep(rtt_defer);

		e.robj_set_tex(obj_cube3, dep_rtt);
	}
#endif

	e.camera_set_pers(90.0f, 0.1f, 2000.0f);
	e.camera_move_toward(-10.0f);

	e.run();

	return 0;
}

