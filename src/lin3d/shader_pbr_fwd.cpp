/**
* @file shader_test.cpp
*
* @author lin shao chuan (email:lsccsl@tom.com)
*
* @brief if it works, it was written by lin shao chuan, if not, i don't know who wrote it
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation.  lin shao chuan makes no
* representations about the suitability of this software for any
* purpose.  It is provided "as is" without express or implied warranty.
* see the GNU General Public License  for more detail.
*/
#include "shader_pbr_fwd.h"
#include "robj_base.h"
#include "material.h"
#include "camera.h"
#include "sence_mgr.h"
#include "l3_engine.h"
#include "light_mgr.h"

namespace l3eng{

void shader_pbr_fwd::init()
{
	//shader参数位置初始化
	this->uni_tex1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex1");

	this->uni_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");
	this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
	this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range");

	this->uni_light_type_loc_          = this->shdr_prg_->get_uniform_loc("uni_light_type"); //光的类型 uniform int
	this->uni_light_view_spot_dir_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_spot_dir"); //Spotlight方向 uniform vec3
	this->uni_light_view_pos_loc_      = this->shdr_prg_->get_uniform_loc("uni_light_view_pos"); //灯光位置(w=0平行光方向 w=1点光源位置) uniform vec4
	this->uni_light_clr_loc_           = this->shdr_prg_->get_uniform_loc("uni_light_clr"); // uniform vec3
	this->uni_light_attenuation_loc_   = this->shdr_prg_->get_uniform_loc("uni_light_attenuation"); //距离大于此值开始衰减 uniform float
	this->uni_light_max_len_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_max_len"); //距离大于此值光线衰减为0 uniform float

	this->uni_light_cutoff_inner_cos_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_cutoff_inner_cos"); //聚光灯,光锥轴线与边缘夹角的cos值最小(小于此值不衰减)
	this->uni_light_cutoff_outer_cos_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_cutoff_outer_cos"); //聚光灯,光锥轴线与边缘夹角的cos值最大(大于此值衰减为零)

	this->uni_light_exponent_loc_      = this->shdr_prg_->get_uniform_loc("uni_light_exponent"); //聚光灯由中心向边缘衰减 uniform float

	this->uni_roughness_loc_ = this->shdr_prg_->get_uniform_loc("uni_roughness");
	this->uni_metalic_loc_ = this->shdr_prg_->get_uniform_loc("uni_metalic");


	//其它pbr参数初始化
	this->pbr_roughness_ = 0.35f;
	this->pbr_metalic_ = 0.5;
}

void shader_pbr_fwd::pre_frame(sence_mgr * sence)
{
	this->shdr_prg_->active();
}

void shader_pbr_fwd::post_frame(sence_mgr * sence)
{
	this->shdr_prg_->inactive();
}

void shader_pbr_fwd::prepare(robj_base& robj)
{
	{
		material& mtrl = robj.get_material();

		OBJ_ID tex1 = mtrl.get_tex_obj_id(0);

		if(tex1 != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_tex1_loc_, tex1);
	}

	this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
		robj.get_world_matrix());

	{
		camera * cam = robj.sence()->cam_cur();

		this->shdr_prg_->uni_bind_mat4(this->uni_proj_mtx_loc_,
			cam->proj_mtx());
		this->shdr_prg_->uni_bind_mat4(this->uni_view_mtx_loc_,
			cam->view_mtx());

		this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
			cam->z_near(),
			cam->z_far() - cam->z_near());
	}

	{
		this->shdr_prg_->uni_bind_float(uni_roughness_loc_, this->pbr_roughness_);
		this->shdr_prg_->uni_bind_float(uni_metalic_loc_, this->pbr_metalic_);
	}

	light_mgr * lm = this->eng_->sence()->get_light_mgr();

	const light_mgr::MAP_LIGHT& map_light = lm->get_map_light();

	if(!map_light.empty())
	{
		light_mgr::MAP_LIGHT::const_iterator it = map_light.begin();

		const light_mgr::light_info::ptr& l = it->second;

		set_light(l, robj.sence());
	}
}

void shader_pbr_fwd::set_light(const light_mgr::light_info::ptr& li, sence_mgr * sence)
{
	this->li_ = li;
	light& l = *this->li_->l_;

	vector3 light_pos;

	camera * cam = sence->cam_cur();
	cam->view_mtx().trans_vect3(l.pos(), light_pos, 1);	

	l3_f32 light_x = light_pos.x();
	l3_f32 light_y = light_pos.y();
	l3_f32 light_z = light_pos.z();
	l3_f32 light_w = 1.0f;

	switch(l.light_type())
	{
	case light::E_LIGHT_DIR:
		{
			vector3 light_dir;
			cam->view_mtx().trans_vect3(l.dir(), light_dir, 0);
			light_x = light_dir.x();
			light_y = light_dir.y();
			light_z = light_dir.z();
			light_w = 0.0f;
		}
		break;

	case light::E_LIGHT_SPOT:
		{
			vector3 spot_dir;
			cam->view_mtx().trans_vect3(l.dir(), spot_dir, 0);

			this->shdr_prg_->uni_bind_float_vec3(this->uni_light_view_spot_dir_loc_,
				spot_dir.x(),
				spot_dir.y(),
				spot_dir.z());

			this->shdr_prg_->uni_bind_float(this->uni_light_cutoff_inner_cos_loc_,
				cos(l.cutoff_inner() * L3_AR_CONVERT));
			this->shdr_prg_->uni_bind_float(this->uni_light_cutoff_outer_cos_loc_,
				cos(l.cutoff_outer() * L3_AR_CONVERT));

			this->shdr_prg_->uni_bind_float(this->uni_light_exponent_loc_,
				l.exponent());
		}
		break;

	default:
		break;
	}

	this->shdr_prg_->uni_bind_int(this->uni_light_type_loc_,
		l.light_type());

	this->shdr_prg_->uni_bind_float_vec4(this->uni_light_view_pos_loc_,
		light_x,
		light_y,
		light_z,
		light_w);

	this->shdr_prg_->uni_bind_float_vec3(this->uni_light_clr_loc_,
		l.diffuse().r(),
		l.diffuse().g(),
		l.diffuse().b());

	this->shdr_prg_->uni_bind_float(this->uni_light_attenuation_loc_,
		l.attenuation_begin());

	this->shdr_prg_->uni_bind_float(this->uni_light_max_len_loc_,
		l.light_max_len());
}

void shader_pbr_fwd::set_param_f32(const l3_int32 param_type, const l3_f32 param_val)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_PBR_ROUGHNESS:
		this->pbr_roughness_ = param_val;
		break;

	case EN_SHADER_PARAM_PBR_METALIC:
		this->pbr_metalic_ = param_val;
		break;
	}
}

}
