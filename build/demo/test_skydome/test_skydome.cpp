#include "l3_engine.h"
#include "l3_math.h"
#include "l3_enum_define.h"
#include "l3_log.h"
#include <windows.h>

using namespace l3eng;

OBJ_ID __shader_id_;
OBJ_ID __shader_id_defer_;
float __degree_ = 150.0f;//166.0f;
float __sea_level = -150.0f;
float __g = -0.46f;
float __g_sky = -0.46f;//-0.9f;
float __sun = 40.0f;
float __Kr_ = 0.025f;
float __Km_ = 0.01f;
//float __scale_ = 0.0013f;
float __scale_ = 0.004f;
float __attenuation_final_ = 0.1f;

l3_engine * _eng;


OBJ_ID __light_id;
l3_f32 __light_x_degree = 20.0f;//-6.0f;
l3_f32 __light_y_degree = -90.0f;//-106.0f;
vector3 __light_pos(-10.0, 15.5f, 10);

l3_f32 __light_virtual_width = 0.2f;
l3_f32 __light_virtual_height = 0.3f;
l3_f32 __light_virtual_cam = 0.02f;


class test_listerner : public default_listener
{
public:

	test_listerner(l3_engine * eng):default_listener(eng)
	{}

	virtual void pre_render()
	{
	}
	virtual void post_render()
	{
	}

	virtual l3_int32 keyboard_event(const keyboard_data_t& evt)override
	{
		bool bset = false;
		switch(evt.key_)
		{
		case L3_KEY_Z:
			{
				__degree_ += 1.0f;
				if(__degree_ > 200.0f)
					__degree_ = 180.0f;
				vector3 v_light_dir(cos(__degree_ * L3_AR_CONVERT), sin(__degree_ * L3_AR_CONVERT), 0.0f);
				this->eng_->shader_set_param_vec3(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, v_light_dir);

				this->eng_->shader_set_param_vec3(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, v_light_dir);				

				__light_x_degree = -(180 - __degree_);
				_eng->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);

				bset = true;
			}
			break;
		case L3_KEY_X:
			{
				__degree_ -= 1.0f;
				if(__degree_ < 0.0f)
					__degree_ = 0.0f;
				vector3 v_light_dir(cos(__degree_ * L3_AR_CONVERT), sin(__degree_ * L3_AR_CONVERT), 0.0f);
				this->eng_->shader_set_param_vec3(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, v_light_dir);

				this->eng_->shader_set_param_vec3(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, v_light_dir);

				__light_x_degree = -(180 - __degree_);
				_eng->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);

				bset = true;
			}
			break;


		case L3_KEY_C:
			{
				__sea_level += 1.0f;
				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_sea_level, __sea_level);

				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_sea_level, __sea_level);

				bset = true;
			}
			break;
		case L3_KEY_V:
			{
				__sea_level -= 1.0f;
				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_sea_level, __sea_level);

				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_sea_level, __sea_level);

				bset = true;
			}
			break;


		case L3_KEY_F1:
			{
				__g -= 0.01f;
				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_g, __g);

				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_g, __g);

				bset = true;
			}
			break;
		case L3_KEY_F2:
			{
				__g += 0.01f;
				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_g, __g);

				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_g, __g);

				bset = true;
			}
			break;


		case L3_KEY_F3:
			{
				__sun -= 1.0f;
				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_esun, __sun);

				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_esun, __sun);

				bset = true;
			}
			break;
		case L3_KEY_F4:
			{
				__sun += 1.0f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_esun, __sun);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_esun, __sun);

				bset = true;
			}
			break;

		case L3_KEY_F5:
			{
				__Kr_ += 0.01f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_Kr, __Kr_);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_Kr, __Kr_);

				bset = true;
			}
			break;
		case L3_KEY_F6:
			{
				__Kr_ -= 0.01f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_Kr, __Kr_);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_Kr, __Kr_);

				bset = true;
			}
			break;

		case L3_KEY_F7:
			{
				__Km_ += 0.01f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_Km, __Km_);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_Km, __Km_);

				bset = true;
			}
			break;
		case L3_KEY_F8:
			{
				__Km_ -= 0.01f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_Km, __Km_);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_Km, __Km_);

				bset = true;
			}
			break;


		case L3_KEY_P:
			{
				char actemp[256] = {};
				_snprintf_s(actemp, sizeof(actemp) - 1, "sun dir:%f sun pow:%f sea:%f g:%f",
					__degree_, __sun, __sea_level, __g);
				L3_LOG_ENG_DEBUG((actemp));
			}
			break;


		case L3_KEY_1:
			{
				__light_virtual_width -= 0.1f;
				_eng->light_dir_virtual_width(__light_id, __light_virtual_width);
			}
			break;
		case L3_KEY_2:
			{
				__light_virtual_width += 0.1f;
				_eng->light_dir_virtual_width(__light_id, __light_virtual_width);
			}
			break;

		case L3_KEY_3:
			{
				__light_virtual_cam -= 0.01f;
				_eng->light_dir_virtual_cam(__light_id, __light_virtual_cam);
				bset = true;
			}
			break;
		case L3_KEY_4:
			{
				__light_virtual_cam += 0.01f;
				_eng->light_dir_virtual_cam(__light_id, __light_virtual_cam);
				bset = true;
			}
			break;
		//case L3_KEY_5:
		//	{
		//		__light_virtual_height -= 0.1f;
		//		_eng->light_dir_virtual_height(__light_id, __light_virtual_height);
		//	}
		//	break;
		//case L3_KEY_6:
		//	{
		//		__light_virtual_height += 0.1f;
		//		_eng->light_dir_virtual_height(__light_id, __light_virtual_height);
		//	}
		//	break;
		case L3_KEY_7:
			{
				__g_sky -= 0.01f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_g_sky, __g_sky);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_g_sky, __g_sky);

				bset = true;
			}
			break;
		case L3_KEY_8:
			{
				__g_sky += 0.01f;

				this->eng_->shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_g_sky, __g_sky);
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_g_sky, __g_sky);

				bset = true;
			}
			break;
		case L3_KEY_9:
			{
				__scale_ += 0.0001f;
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_scale, __scale_);
			}
			break;
		case L3_KEY_0:
			{
				__scale_ -= 0.0001f;
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_scale, __scale_);
			}
			break;
		case L3_KEY_I:
			{
				__attenuation_final_ += 0.02f;
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_attenuation_final, __attenuation_final_);
			}
			break;
		case L3_KEY_O:
			{
				__attenuation_final_ -= 0.02f;
				this->eng_->shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_attenuation_final, __attenuation_final_);
			}
			break;


		case L3_KEY_SHIFT:
			{
				static int _at = 0;
				_eng->enable_atmospheric((_at ++) %2);
			}
			break;

		case L3_KEY_CONTROL:
			{
				static int _vol = 0;
				_eng->light_enable_vol(__light_id, (_vol ++) %2);
			}
			break;

		case L3_KEY_TAB:
			{
				static int __hdr = 0;
				_eng->enable_hdr((__hdr ++) %2);
			}
			break;




		//case L3_KEY_NUMPAD1:
		//	{
		//		__light_pos.z(__light_pos.z() - 1.0f);
		//		this->eng_->light_set_pos(__light_id, __light_pos);
		//	}
		//	break;
		//case L3_KEY_NUMPAD3:
		//	{
		//		__light_pos.z(__light_pos.z() + 1.0f);
		//		this->eng_->light_set_pos(__light_id, __light_pos);
		//	}
		//	break;
		//case L3_KEY_NUMPAD5:
		//	{
		//		__light_pos.y(__light_pos.y() - 1.0f);
		//		this->eng_->light_set_pos(__light_id, __light_pos);
		//	}
		//	break;
		//case L3_KEY_NUMPAD2:
		//	{
		//		__light_pos.y(__light_pos.y() + 1.0f);
		//		this->eng_->light_set_pos(__light_id, __light_pos);
		//	}
		//	break;
		//case L3_KEY_NUMPAD4:
		//	{
		//		__light_pos.x(__light_pos.x() - 1.0f);
		//		this->eng_->light_set_pos(__light_id, __light_pos);
		//	}
		//	break;
		//case L3_KEY_NUMPAD6:
		//	{
		//		__light_pos.x(__light_pos.x() + 1.0f);
		//		this->eng_->light_set_pos(__light_id, __light_pos);
		//	}
		//	break;

		case L3_KEY_DIVIDE:
			{
				__light_x_degree -= 1.0f;
				_eng->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);
			}
			break;
		case L3_KEY_NUMPAD8:
			{
				__light_x_degree += 1.0f;
				_eng->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);
			}
			break;
		case L3_KEY_NUMPAD7:
			{
				__light_y_degree -= 1.0f;
				_eng->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);
			}
			break;
		case L3_KEY_NUMPAD9:
			{
				__light_y_degree += 1.0f;
				_eng->light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);
			}
			break;

//	_eng->light_set_rotate(light_id, __light_x_degree, __light_y_degree, 0.0);

		}

		if(bset)
		{
			char actemp[256] = {};
			_snprintf_s(actemp, sizeof(actemp) - 1, "sun dir:%f sun pow:%f sea:%f g:%f g_sky:%f virtual_cam:%f km:%f kr:%f scale:%f",
				__degree_, __sun, __sea_level, __g, __g_sky, __light_virtual_cam, __Km_, __Kr_, __scale_);

			this->eng_->set_title(actemp);
			L3_LOG_ENG_DEBUG((actemp));
		}

		return default_listener::keyboard_event(evt);
	}
};

void set_atmospheric(l3_engine& e)
{
	
	//e.shader_set_param_f32();
}

float atmospheric_cal_scale(float fCos)//fcos为0时,最大,为1时
{
	float x = 1.0 - fCos;

	return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void create_high_wall(l3_engine& e)
{
	int loop = 3;
	int height = 500;
	float step = 1.0 / loop;
	float width = 10;
	for(int i = 0; i < loop; i ++)
	{
		//float width = atmospheric_cal_scale(i * step);
		//width *= 20.0f;

		OBJ_ID obj = e.robj_geometry_create_cube(abs(width), height, 2, true, 255, 255, 255);

		e.robj_move_to_xyz(obj,
			0,//width / 2.0f,
			i * height + 20,
			-100);

		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(obj, tex_id);

		//e.shader_defer_render(obj, 1);
		//e.robj_set_sys_shader(obj, "_l3eng_inter_atmospheric_ground");
	}

	for(int i = 0; i < loop; i ++)
	{
		OBJ_ID obj = e.robj_geometry_create_cube(abs(width), height, 2, true, 255, 255, 255);

		e.robj_move_to_xyz(obj,
			-200,//width / 2.0f,
			i * height + 20,
			-100);

		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(obj, tex_id);

		//e.shader_defer_render(obj, 1);
		//e.robj_set_sys_shader(obj, "_l3eng_inter_atmospheric_ground");
	}
}

const float base_height = 0.0f;

void build_ground()
{
#if 1
	OBJ_ID obj_ground = _eng->robj_geometry_create_cube(9000, 1, 9000, true);
	_eng->robj_move_to_xyz(obj_ground, 0, base_height, 0);
	OBJ_ID tex_id = _eng->tex_load("../resource/terrain_detail.jpg");
	_eng->robj_set_tex(obj_ground, tex_id);
	_eng->shader_defer_render(obj_ground, 1);
	_eng->material_enable_reflect(obj_ground, 1);
#endif

	//水面
	if(0)
	{
		OBJ_ID obj_water = _eng->robj_plane_create(9000, 9000, vector3(0.0f, 1.0f, 0.0f),
			vector3(0.0f, base_height + 1.0f, 0.0f), 1, vertex_color_t(30, 144, 255));
		_eng->robj_enable_debug(obj_water, 1);

		_eng->robj_set_render_group(obj_water, L3_RENDER_GROUP_WATER);

		//bump纹理
		OBJ_ID img_bump = _eng->image_load_normal("test_normal.bmp", "../resource/WaterBump.jpg", 0.015);

		OBJ_ID tex_bump = _eng->tex_load_img(img_bump);
		_eng->tex_wrap_repeate(tex_bump);
		_eng->robj_set_tex(obj_water, tex_bump, 2);

	}
}

void build()
{
	if(0)
	{
		OBJ_ID obj_build1 = _eng->robj_geometry_create_cube(10, 10, 10, true);
		_eng->robj_move_to_xyz(obj_build1, -50, 15.0f, 0);
		_eng->robj_scale(obj_build1, 3.0, 3.0, 3.0);
		_eng->material_poly_mode(obj_build1, 1);;
		OBJ_ID tex_id = _eng->tex_load("../resource/rockwall.jpg");
		_eng->robj_set_tex(obj_build1, tex_id);
		//_eng->shader_defer_render(obj_build1, 1);
	}

	for(int j = 0; j < 30; j ++)
	//for(int j = 29; j >= 0; j --)
	{
		float z_start = -10;
		for(int i = 0; i < 10 + j; i ++)
		{
			float height = rand() % 30;
			float width = rand() % 30;
			float deep = 5;//rand() % 30;

			if(height < 10)
				height = 10;
			if(width > 10)
				width = 10;
			else if(width < 3)
				width = 3;

			height += j * 10;

			if(1)
			{
				OBJ_ID obj_build1 = _eng->robj_geometry_create_cube(deep, height, width, true);
				_eng->robj_move_to_xyz(obj_build1, -(30 + j * (deep * 9)), height / 2.0f + base_height, z_start + width / 2.0f);
				OBJ_ID tex_id = _eng->tex_load("../resource/rockwall_1.jpg");
				_eng->robj_set_tex(obj_build1, tex_id);
				_eng->shader_defer_render(obj_build1, 1);
			}
			else
			{
				OBJ_ID sphere = _eng->robj_sphere_create(3);
				_eng->robj_move_to_xyz(sphere, -(30 + j * (deep * 9)), height / 2.0f + base_height, z_start + width / 2.0f);
				//_eng->robj_move_to_xyz(sphere, -20.0, 15, 15);
				_eng->shader_defer_render(sphere, 1);

				OBJ_ID tex_id = _eng->tex_load("../resource/eq2_bigmet_01.jpg");
				_eng->robj_set_tex(sphere, tex_id);
			}


			z_start += width + rand() % 10;
		}
	}
}

void build_city()
{
	build_ground();
	build();
}

void create_light()
{
	{//255	97
		OBJ_ID light_id = _eng->light_create();
		_eng->light_enable(light_id, 1);
		//_eng->light_clr_diffuse(light_id, color(0.5f, (97.0/255.0)* 0.5f, 0.0f));
		_eng->light_clr_diffuse(light_id, color(0.2f, 0.2f, 0.2f));
		_eng->light_spot_exponent(light_id, 3.0f);
		_eng->light_spot_cutoff(light_id, 10.0f);
		_eng->light_max_len(light_id, 500.0f);

		_eng->light_set_rotate(light_id, __light_x_degree, __light_y_degree, 0.0);
		_eng->light_set_pos(light_id, __light_pos);

		_eng->light_enable_vol(light_id, 1);

		_eng->light_type(light_id, 0);
		_eng->light_enable_light_debug(light_id, 1);

		__light_id = light_id;
	}

	if(0)
	{
		OBJ_ID light_id = _eng->light_create();
		_eng->light_enable(light_id, 1);
		_eng->light_clr_diffuse(light_id, color(0.0f, 0.6f, 1.0f));
		_eng->light_spot_exponent(light_id, 3.0f);
		_eng->light_spot_cutoff(light_id, 10.0f);
		_eng->light_max_len(light_id, 500.0f);

		_eng->light_set_rotate(light_id, __light_x_degree, __light_y_degree, 0.0);
		_eng->light_set_pos(light_id, __light_pos);

		_eng->light_enable_vol(light_id, 1);

		//_eng->light_type(light_id, 0);

		//__light_id = light_id;
	}
}

int main()
{
	char actemp[256] = {0};
	::GetCurrentDirectory(256, actemp);

	l3_engine e;
	_eng = &e;
	test_listerner tl(&e);
	e.init_eng(&tl, 0);
	e.enable_defer_render(1);
	e.enable_atmospheric(1);
	e.enable_debug(1);
	e.enable_hdr(1);
	e.enable_ssao(1);

	create_light();

	__shader_id_ = e.shader_get("_l3eng_inter_atmospheric_ground");
	vector3 v_light_dir(cos(__degree_ * L3_AR_CONVERT), sin(__degree_ * L3_AR_CONVERT), 0.0f);
	e.shader_set_param_vec3(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, v_light_dir);
	e.shader_set_param_f32(__shader_id_, EN_SHADER_ATMOSPHERIC_PARAM_g, __g);

	__shader_id_defer_ = e.shader_get("_l3eng_inter_defer_render_atmospheric");
	e.shader_set_param_vec3(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, v_light_dir);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_g, __g);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_g_sky, __g_sky);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_scale, __scale_);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_esun, __sun);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_attenuation_final, __attenuation_final_);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_sea_level, __sea_level);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_Kr, __Kr_);
	e.shader_set_param_f32(__shader_id_defer_, EN_SHADER_ATMOSPHERIC_PARAM_Km, __Km_);

	{
		int shader_id = e.shader_get("_l3eng_inter_defer_render_final");
		e.shader_set_param_f32(shader_id, EN_SHADER_PARAM_FINAL_DIFF_SCALE, 0.01f);
	}

	__light_x_degree = -(180 - __degree_);
	e.light_set_rotate(__light_id, __light_x_degree, __light_y_degree, 0.0);

	if(1)
	{
		OBJ_ID skydome = e.robj_skydome_create(8000, 0.7f, 0.9f, 8, 80);
		OBJ_ID tex_id = e.tex_load("../resource/skydome.jpg");
		e.tex_wrap_repeate(tex_id);
		e.robj_set_tex(skydome, tex_id);
		e.shader_defer_render(skydome, 1);
		//e.shader_defer_light(skydome, 0);
		//e.robj_set_sys_shader(skydome, "_l3eng_inter_atmospheric_ground");
	}

	//create_high_wall(e);
	build_city();

	if(0)
	{
		OBJ_ID obj_build1 = _eng->robj_geometry_create_cube(30, 30, 30, true);
		_eng->robj_move_to_xyz(obj_build1, 0, 0.0f, 0);
		_eng->material_poly_mode(obj_build1, 1);;
		OBJ_ID tex_id = _eng->tex_load("../resource/rockwall.jpg");
		_eng->robj_set_tex(obj_build1, tex_id);
		//_eng->shader_defer_render(obj_build1, 1);
	}
	//e.camera_set_ortho(792/2.0, 573/2.0, 0.1f, 8000.0f); e.camera_set(vector3(0.0, 0.0, 0.0), vector3(0.0, 0, 1000));

	//e.camera_set(vector3(-37.0, 90.0, 0.0), vector3(0.0, 98.5, 600));
	//e.camera_set(vector3(0.0, 90.0, 0.0), vector3(0.0, 1.5, -35));
	//e.camera_set(vector3(-10.0, 90.0, 0.0), vector3(0.0, 30 + base_height, 50));

	{
		OBJ_ID sphere = e.robj_sphere_create(1, 100);
		e.material_shininess(sphere, 20);
		e.shader_defer_render(sphere, 1);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_move_to_xyz(sphere, -10.0, 15, 15);
		e.robj_set_tex(sphere, tex_id);
	}

	{
		OBJ_ID cube = e.robj_geometry_create_cube(1, 1, 1, true);
		e.material_shininess(cube, 20);
		e.shader_defer_render(cube, 1);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_move_to_xyz(cube, -10.0, 15, 5);
		e.robj_set_tex(cube, tex_id);
	}

#if 0
	e.camera_set_pers(60.0f, 0.3f, 1000.0f);
	e.camera_set(vector3(0,0,0), vector3(-9,1,40));
#else
	e.camera_set(vector3(0.0, 90.0, 0.0), vector3(-17.5f, base_height + 25.0f, 45.0f));
	e.camera_set_pers(90.0f, 0.1f, 8010.0f);
#endif

	e.run();

	return 0;
}

