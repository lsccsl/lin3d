#include "l3_engine.h"
using namespace l3eng;

l3_engine* ge_;
OBJ_ID tex_normal_;
OBJ_ID __water_shader_id_;

OBJ_ID sphere_;

l3_f32 wave_offset_[4] = {0.0f, 0.0f, 0.0f, 0.0f};
l3_f32 wave_speed_[4]  = {9.0f, 4.5f, -8.0f, -3.5f};

l3_bool is_defer = 1;

class test_listerner : public default_listener
{
public:

	virtual void pre_render()override
	{
		if(!is_defer)
		{
			l3_uint64 t1 = this->eng_->t_elapse();
			l3_f32 t11 = (t1 / 1000.0f) * 0.001f;
			wave_offset_[0] = wave_speed_[0] * t11;
			wave_offset_[1] = wave_speed_[1] * t11;
			wave_offset_[2] = wave_speed_[2] * t11;
			wave_offset_[3] = wave_speed_[3] * t11;
			this->eng_->shader_set_param(__water_shader_id_, EN_SHADER_PARAM_RELECT_wave_offset, wave_offset_, sizeof(wave_offset_));

			l3_f32 nml_x;
			l3_f32 nml_y;
			l3_f32 nml_z;
			l3_f32 nml_d;
			this->eng_->robj_plane_get_plane_info(square_plane_,
				nml_x, nml_y, nml_z, nml_d);

			this->eng_->set_reflect_mode(nml_x, nml_y, nml_z, nml_d);
			this->eng_->render_sence(L3_RENDER_GROUP_DEFAULT, water_refl_rtt_);
			this->eng_->set_normal_mode();

			this->eng_->set_refract_mode(nml_x, nml_y, nml_z, nml_d);
			this->eng_->render_sence(L3_RENDER_GROUP_DEFAULT, water_refr_rtt_);
			this->eng_->set_normal_mode();

			this->eng_->rtt_show_tex(water_refl_rtt_, 1, 0.0f, 0.0f, 0.5f, 0.5f);
			this->eng_->rtt_show_tex(water_refr_rtt_, 1, 0.0f, 0.5f, 0.2f, 0.2f);
		}
	}

	virtual void post_render()override
	{
	}

	l3_int32 test_listerner::keyboard_event(const keyboard_data_t& evt)
	{
		l3_bool bset = L3_FALSE;
		switch(evt.key_)
		{
#if 0
		case L3_KEY_DOWN:
			{
				this->eng_->camera_move_toward(-0.1f);
				return 0;
			}
			break;
		case L3_KEY_UP:
			{
				this->eng_->camera_move_toward(0.1f);
				return 0;
			}
			break;
		case L3_KEY_RIGHT:
			{
				this->eng_->camera_move_sideward(0.1f);
				return 0;
			}
			break;
		case L3_KEY_LEFT:
			{
				this->eng_->camera_move_sideward(-0.1f);
				return 0;
			}
			break;
#endif

		case L3_KEY_Z:
			{
				rotate_z_ += 1.0f;
				bset = L3_TRUE;
			}
			break;
		case L3_KEY_X:
			{
				rotate_z_ -= 1.0f;
				bset = L3_TRUE;
			}
			break;

		case L3_KEY_C:
			{
				rotate_x_ += 1.0f;
				bset = L3_TRUE;
			}
			break;
		case L3_KEY_V:
			{
				rotate_x_ -= 1.0f;
				bset = L3_TRUE;
			}
			break;

		case L3_KEY_B:
			{
				this->eng_->robj_move_xyz(sphere_, 0.5f, 0.0f, 0.0f);

				//pln_inter_point_.y(pln_inter_point_.y() + 1.0f);
				bset = L3_TRUE;
			}
			break;
		case L3_KEY_N:
			{
				this->eng_->robj_move_xyz(sphere_, -0.5f, 0.0f, 0.0f);

				//pln_inter_point_.y(pln_inter_point_.y() - 1.0f);
				bset = L3_TRUE;
			}
			break;


		case L3_KEY_F:
			{
				this->eng_->robj_move_xyz(sphere_, 0.0f, 0.5f, 0.0f);

				//pln_inter_point_.y(pln_inter_point_.y() + 1.0f);
				bset = L3_TRUE;
			}
			break;
		case L3_KEY_G:
			{
				this->eng_->robj_move_xyz(sphere_, 0.0f, -0.5f, 0.0f);

				//pln_inter_point_.y(pln_inter_point_.y() - 1.0f);
				bset = L3_TRUE;
			}
			break;

		}

		if(bset)
		{
			ny_ = cos(rotate_x_ * L3_AR_CONVERT);
			float nxz = sin(rotate_x_ * L3_AR_CONVERT);

			nx_ = nxz * cos(rotate_z_ * L3_AR_CONVERT);
			nz_ = nxz * sin(rotate_z_ * L3_AR_CONVERT);

			this->eng_->robj_plane_set_nml_inter_point(square_plane_, vector3(nx_, ny_, nz_), this->pln_inter_point_);
			
			return 0;
		}

		return default_listener::keyboard_event(evt);
	}

	test_listerner(l3_engine * eng):default_listener(eng)
		, nx_(0.0f), ny_(1.0f), nz_(0.0f)
		, pln_inter_point_(0.0f, 0.0f, 0.0f)
		, rotate_z_(0.0f)
	{
	}

	OBJ_ID water_refl_rtt_;
	OBJ_ID water_refr_rtt_;
	OBJ_ID square_water_;
	OBJ_ID square_plane_;

	l3_f32 nx_, ny_, nz_;
	vector3 pln_inter_point_;

	l3_f32 rotate_z_;
	l3_f32 rotate_x_;
};

void test_normal()
{
	OBJ_ID img_nml = ge_->image_load_normal("test_normal.bmp", "../resource/WaterBump.jpg");
	ge_->image_save(img_nml, "test_normal.bmp");
}

void test_read_psd()
{
	OBJ_ID img_psd = ge_->image_load("WaterFresnel.psd", "../resource/WaterFresnel.psd");
	ge_->image_save(img_psd, "fresnel.bmp");
}

int main()
{
	int x_offset_base = 0;

	l3_engine e;
	test_listerner tl(&e);
	e.init_eng(&tl, 0);
	e.enable_debug(1);
	e.enable_hdr(0);
	e.enable_defer_render(is_defer);
	e.enable_hdr(0);
	e.enable_ssao(1);
	e.enable_atmospheric(0);

	ge_ = &e;

	__water_shader_id_ = e.shader_get("_l3eng_inter_reflect_forward");

#if 1
	OBJ_ID light_id = -1;
	light_id = e.light_create();
	e.light_enable(light_id, 1);
	//_eng->light_clr_diffuse(light_id, color(0.5f, (97.0/255.0)* 0.5f, 0.0f));
	e.light_clr_diffuse(light_id, color(0.0f, 1.0f, 1.0f));
	e.light_spot_exponent(light_id, 3.0f);
	e.light_spot_cutoff(light_id, 60.0f);
	e.light_max_len(light_id, 500.0f);
	e.light_set_rotate(light_id, -90.0, 0, 0.0);

	e.light_set_pos(light_id, vector3(0, 35, 0));

	e.light_enable_vol(light_id, 0);
	e.light_type(light_id, 1);
	e.light_enable_light_debug(light_id, 1);
	e.light_enable_shadow(light_id, 1);
	//e.light_enable_vol(light_id, 1);
#endif


	//test_normal();
	test_read_psd();

	//tl.water_refl_rtt_ = e.rtt_fbo_create("test_water", e.dev_window_width()/2, e.dev_window_height()/2);
	//tl.water_refr_rtt_ = e.rtt_fbo_create("test_water", e.dev_window_width()/2, e.dev_window_height()/2);
	tl.water_refl_rtt_ = e.rtt_fbo_create("test_water", e.dev_window_width(), e.dev_window_height());
	tl.water_refr_rtt_ = e.rtt_fbo_create("test_water", e.dev_window_width(), e.dev_window_height());

	if(0)
	{
		OBJ_ID skydome = e.robj_skydome_create(800, 0.7f, 0.9f, 8, 80);
		OBJ_ID tex_id = e.tex_load("../resource/skydome.jpg");
		e.tex_wrap_repeate(tex_id);
		e.robj_set_tex(skydome, tex_id);
		e.shader_defer_render(skydome, 1);

		//e.shader_defer_light(skydome, 1);

	}

	//球
	if(1)
	{
		OBJ_ID sphere = e.robj_sphere_create(10, 100);
		e.robj_move_to_xyz(sphere, x_offset_base + -10, 5, -19);

		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(sphere, tex_id);

		e.shader_defer_render(sphere, 1);

		//sphere_ = sphere;
	}

	//球
	if(1)
	{
		OBJ_ID sphere = e.robj_sphere_create(15, 100);
		e.robj_move_to_xyz(sphere, x_offset_base + 10, 19, -39);

		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(sphere, tex_id);

		e.shader_defer_render(sphere, 0);

		sphere_ = sphere;
	}

	//球
	if(1)
	{
		OBJ_ID sphere = e.robj_sphere_create(10, 100);
		e.robj_move_to_xyz(sphere, x_offset_base + 10, 15, 20);

		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		e.robj_set_tex(sphere, tex_id);

		e.shader_defer_render(sphere, 0);

		sphere_ = sphere;
	}

	//锥
	if(1)
	{
		OBJ_ID obj_cone = e.robj_cone_create(5.0f, 60.0f);
		e.robj_move_to_xyz(obj_cone, x_offset_base + -5, 20, -10);
		e.shader_defer_render(obj_cone, 1);
		//e.material_shininess(obj_cone, 2.0f);
		OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
		//e.robj_set_tex(obj_cone, tex_id);
	}

	////向量
	//if(0)
	//{
	//	OBJ_ID v = e.robj_vector_create(vector3(0.0f, 0.0f, 0.0f), vector3(1.0f, 1.0f, 0.0f), 5.0f);
	//	e.material_enable_depth_test(v, 0);
	//}

	//方块1
	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(10, 10, 10, 1, 255, 255, 255);
		e.robj_move_to_xyz(cube, x_offset_base + -10, 0, 0);
		e.robj_rotate(cube, 0, 0, 180);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall_1.jpg");
		e.robj_set_tex(cube, tex_id);

		e.shader_defer_render(cube, 1);
	}

	//方块1
	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(10, 10, 10, 1, 255, 255, 255);
		e.robj_move_to_xyz(cube, x_offset_base + 30, 7, -19);
		OBJ_ID tex_id = e.tex_load("../resource/WaterBump.jpg");
		e.robj_set_tex(cube, tex_id);

		e.shader_defer_render(cube, 1);
	}

	//if(0)
	//{
	//	OBJ_ID sphere = e.robj_sphere_create(10, 100);
	//	e.robj_move_to_xyz(sphere, 10, 10, -19);
	//	//e.robj_set_sys_shader(sphere, "_l3eng_inter_test_program");
	//	OBJ_ID tex_id = e.tex_load("../resource/eq2_bigmet_01.jpg");
	//	e.robj_set_tex(sphere, tex_id);
	//	e.shader_defer_render(sphere, 1);
	//}

	//方块1
	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(5, 10, 10);
		e.robj_move_to_xyz(cube, x_offset_base + 50, 20, -19);
		e.robj_rotate(cube, 0, 0, 30);
		OBJ_ID tex_id = e.tex_load("../resource/rockwall.jpg");
		e.robj_set_tex(cube, tex_id);

		e.shader_defer_render(cube, 1);
	}

	//方块2
	if(1)
	{
		OBJ_ID cube = e.robj_geometry_create_cube(5, 10, 10, 1, 255, 255, 255);
		e.robj_move_to_xyz(cube, x_offset_base + 60, 30, -19);
		//e.robj_rotate(cube, 0, 0, 45);
		OBJ_ID tex_id = e.tex_load("../resource/WaterBump.jpg");
		e.robj_set_tex(cube, tex_id);

		e.shader_defer_render(cube, 1);
	}


	////背景平面
	//if(0)
	//{
	//	OBJ_ID back_ground = e.robj_geometry_create_square(1000, 1000, 1, 0, 128, 255);
	//	e.robj_move_to_xyz(back_ground, 0, 0, -30);
	//}


#if 1
	//反射平面1
	if(1)
	{
		tl.square_plane_ = e.robj_plane_create(1800, 1800, vector3(0.0f, 1.0f, 0.0f),
			tl.pln_inter_point_, L3_TRUE, vertex_color_t(30, 144, 255));
		e.robj_enable_debug(tl.square_plane_, 1);
		e.shader_defer_render(tl.square_plane_, 0);
		e.shader_defer_light(tl.square_plane_, 0);

		e.robj_set_render_group(tl.square_plane_, L3_RENDER_GROUP_WATER);

		//反射与折射纹理
		e.robj_set_tex(tl.square_plane_, e.rtt_get_tex_clr(tl.water_refl_rtt_), 0);
		e.robj_set_tex(tl.square_plane_, e.rtt_get_tex_clr(tl.water_refr_rtt_), 1);

		//bump纹理
		OBJ_ID img_bump = ge_->image_load_normal("test_normal.bmp", "../resource/WaterBump.jpg", 0.015);
		OBJ_ID tex_bump = e.tex_load_img(img_bump);
		e.tex_wrap_repeate(tex_bump);
		e.robj_set_tex(tl.square_plane_, tex_bump, 2);

		//设置shader
		e.robj_set_sys_shader(tl.square_plane_, "_l3eng_inter_reflect_forward");

		//e.shader_defer_render();
	}
#endif

	//水底平面
	if(1)
	{
		OBJ_ID under_water = e.robj_geometry_create_square(200, 200, 1, 128, 128, 128);
		e.robj_rotate(under_water, -90, 0, 0);
		e.robj_move_to_xyz(under_water, 0, -10, -0);
		OBJ_ID tex_id = e.tex_load("../resource/background.jpg");
		e.robj_set_tex(under_water, tex_id);
		e.shader_defer_render(under_water, 1);
	}
	

	//if(0)
	//{
	//	OBJ_ID square1 = e.robj_geometry_create_square(10, 10, 1, 255, 255, 255);
	//	e.robj_move_to_xyz(square1, -15, 15, 0);
	//	e.robj_set_tex(square1, e.rtt_get_tex_clr(tl.water_rtt_));
	//	e.rob_set_render_group(square1, L3_RENDER_GROUP_WATER);
	//}

	e.camera_set_pers(60.0f, 0.3f, 1000.0f);
	e.camera_set(vector3(0,0,0), vector3(x_offset_base + -9,1,40));
	//e.camera_set(vector3(-23,-54,0), vector3(-88.75, 45.5, 28.4));
	//e.camera_set(vector3(-90,0,0), vector3(30.5, 172, 40.5));

	e.run();


	return 0;
}

