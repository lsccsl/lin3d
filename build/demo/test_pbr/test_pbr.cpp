#include "l3_engine.h"

using namespace l3eng;

OBJ_ID old_obj_id = -1;
OBJ_ID old_tr_id = -1;

OBJ_ID __light_id;
float light_x = 0.0f;
float light_y = 0.0f;
float light_z = 10.0f;


l3_f32 __light_x_degree = 0.0f;//-6.0f;
l3_f32 __light_y_degree = 0.0f;//-106.0f;

OBJ_ID __pbr_sys_shdr;

l3_f32 __pbr_roughness = 0.2f;
l3_f32 __pbr_metalic = 0.6f;

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
			__light_x_degree += 1.0f;
			eng_->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0);
			return true;
			break;
		case L3_KEY_Z:
			__light_x_degree -= 1.0f;
			eng_->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0);
			return true;
			break;

		case L3_KEY_C:
			__light_y_degree += 1.0f;
			eng_->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0);
			return true;
			break;
		case L3_KEY_V:
			__light_y_degree -= 1.0f;
			eng_->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0);
			return true;
			break;


		case L3_KEY_F:
			light_x += 1.0f;
			eng_->light_set_pos(__light_id, vector3(light_x, light_y, light_z));
			return true;
			break;
		case L3_KEY_G:
			light_x -= 1.0f;
			eng_->light_set_pos(__light_id, vector3(light_x, light_y, light_z));
			return true;
			break;

		case L3_KEY_H:
			light_z += 1.0f;
			eng_->light_set_pos(__light_id, vector3(light_x, light_y, light_z));
			return true;
			break;
		case L3_KEY_J:
			light_z -= 1.0f;
			eng_->light_set_pos(__light_id, vector3(light_x, light_y, light_z));
			return true;
			break;

		case L3_KEY_K:
			light_y += 1.0f;
			eng_->light_set_pos(__light_id, vector3(light_x, light_y, light_z));
			return true;
			break;
		case L3_KEY_L:
			light_y -= 1.0f;
			eng_->light_set_pos(__light_id, vector3(light_x, light_y, light_z));
			return true;
			break;



		case L3_KEY_F1:
			{
				if(keyboard_evt.shift_)
					__pbr_roughness = __pbr_roughness - 0.01f;
				else
					__pbr_roughness = __pbr_roughness + 0.01f;
				if(__pbr_roughness > 1)
					__pbr_roughness = 1;
				if(__pbr_roughness < 0)
					__pbr_roughness = 0;
				eng_->shader_set_param_f32(__pbr_sys_shdr, EN_SHADER_PARAM_PBR_ROUGHNESS, __pbr_roughness);
			}
			break;

		case L3_KEY_F2:
			{
				if(keyboard_evt.shift_)
					__pbr_metalic = __pbr_metalic - 0.01f;
				else
					__pbr_metalic = __pbr_metalic + 0.01f;
				if(__pbr_metalic > 1)
					__pbr_metalic = 1;
				if(__pbr_metalic < 0)
					__pbr_metalic = 0;
				eng_->shader_set_param_f32(__pbr_sys_shdr, EN_SHADER_PARAM_PBR_METALIC, __pbr_metalic);
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

int main()
{
	l3_engine e;
	test_listerner tl(&e);
	e.init_eng(&tl, 0);
	e.enable_defer_render(0);
	e.enable_hdr(0);
	e.enable_debug(1);

	{
		__light_id = e.light_create();

		e.light_type(__light_id, 2);

		e.light_enable(__light_id, 1);
		e.light_clr_diffuse(__light_id, color(1.0f, 1.0f, 1.0f));
		e.light_spot_exponent(__light_id, 2.0f);
		e.light_spot_cutoff(__light_id, 30.0f);
		e.light_spot_cutoff_inner_outer(__light_id, 30.0f, 60.0f);

		e.light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0);
		e.light_set_pos(__light_id, vector3(light_x, light_y, light_z));
		e.light_enable_vol(__light_id, 0);
		e.light_enable_shadow(__light_id, 0);

		e.light_enable_light_debug(__light_id, 1);
	}

	{
		OBJ_ID sphere = e.robj_sphere_create(10, 200);
		e.robj_move_to_xyz(sphere, 0, 0, -19);
		e.robj_set_sys_shader(sphere, "_l3eng_inter_pbr_forward");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(sphere, tex_id);
		e.shader_defer_render(sphere, 1);
	}

	{
		OBJ_ID sphere = e.robj_sphere_create(10, 200);
		e.robj_move_to_xyz(sphere, -25, 0, -19);
		e.robj_set_sys_shader(sphere, "_l3eng_inter_pbr_forward");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(sphere, tex_id);
		e.shader_defer_render(sphere, 1);
	}

	{
		OBJ_ID sphere = e.robj_sphere_create(10, 200);
		e.robj_move_to_xyz(sphere, 25, 0, -19);
		e.robj_set_sys_shader(sphere, "_l3eng_inter_pbr_forward");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(sphere, tex_id);
		e.shader_defer_render(sphere, 1);
	}

	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(100, 100, 100, true, 255, 255, 255);
		e.robj_move_to_xyz(cube, 0, -80, 0);
		e.robj_set_sys_shader(cube, "_l3eng_inter_pbr_forward");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(cube, tex_id);
		e.shader_defer_render(cube, 1);
	}

	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(1000, 1000, 10, true, 255, 255, 255);
		e.robj_move_to_xyz(cube, 0, 0, -100);
		e.robj_set_sys_shader(cube, "_l3eng_inter_pbr_forward");
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(cube, tex_id);
		e.shader_defer_render(cube, 1);
	}

	__pbr_sys_shdr = e.shader_get("_l3eng_inter_pbr_forward");
	e.shader_set_param_f32(__pbr_sys_shdr, EN_SHADER_PARAM_PBR_ROUGHNESS, __pbr_roughness);
	e.shader_set_param_f32(__pbr_sys_shdr, EN_SHADER_PARAM_PBR_METALIC, __pbr_metalic);

	e.camera_set_pers(60, 1, 1000);
	e.run();
	return 0;
}


