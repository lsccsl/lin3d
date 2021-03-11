/**
* @file shader_reflect.cpp
*
* @author lin shao chuan (email:czhama@163.com)
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
#include "shader_reflect_forward.h"
#include "material.h"
#include "robj_base.h"
#include "camera.h"
#include "sence_mgr.h"
#include "win_device.h"
#include "config.h"
#include "texture_mgr.h"

namespace l3eng{

void shader_reflect_forward::init()
{
	this->uni_tex1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex1");
	this->uni_tex2_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex2");
	this->uni_bump_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_bump");
	this->uni_tex_fresnel_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_fresnel");

	this->uni_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");
	this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
	this->uni_cam_world_pos_loc_ = this->shdr_prg_->get_uniform_loc("uni_cam_world_pos");

	this->uni_wave_offset_loc_  = this->shdr_prg_->get_uniform_loc("uni_wave_offset");
	this->uni_wave_scale_loc_   = this->shdr_prg_->get_uniform_loc("uni_wave_scale");
	this->uni_refl_distort_loc_ = this->shdr_prg_->get_uniform_loc("uni_refl_distort");

	this->uni_reflect_scale_loc_ = this->shdr_prg_->get_uniform_loc("uni_reflect_scale");
	this->uni_refract_scale_loc_ = this->shdr_prg_->get_uniform_loc("uni_refract_scale");
	this->uni_color_scale_loc_ = this->shdr_prg_->get_uniform_loc("uni_color_scale");

	this->uni_need_refract_loc_ = this->shdr_prg_->get_uniform_loc("uni_need_refract");


	this->wave_scale_[0] = 0.08f;
	this->wave_scale_[1] = 0.08f;
	this->wave_scale_[2] = 0.032f;
	this->wave_scale_[3] = 0.036f;
	
	this->wave_offset_[0] = 0.0f;
	this->wave_offset_[1] = 0.0f;
	this->wave_offset_[2] = 0.0f;
	this->wave_offset_[3] = 0.0f;

	this->refl_distort_ = 0.7f;

	this->reflect_scale_ = 0.4f;
	this->refract_scale_ = 0.4f;
	this->color_scale_ = 0.2f;

	this->need_refract_ = 1;

	this->load_default_fresnel_tex();
}

void shader_reflect_forward::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	this->shdr_prg_->active();

}

void shader_reflect_forward::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	this->shdr_prg_->inactive();
}

void shader_reflect_forward::prepare(robj_base& robj)
{
	{
		material& mtrl = robj.get_material();

		OBJ_ID tex1 = mtrl.get_tex_obj_id(0);

		if(tex1 != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_tex1_loc_, tex1);
	}

	{
		material& mtrl = robj.get_material();

		OBJ_ID tex2 = mtrl.get_tex_obj_id(1);

		if(tex2 != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_tex2_loc_, tex2);
	}

	{
		material& mtrl = robj.get_material();

		OBJ_ID bump = mtrl.get_tex_obj_id(2);

		if(bump != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_bump_loc_, bump);
	}

	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_fresnel_loc_, this->tex_fresnel_);		
	}

	this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
		robj.get_world_matrix());

	{
		camera * cam = robj.sence()->cam_cur();

		this->shdr_prg_->uni_bind_mat4(this->uni_proj_mtx_loc_,
			cam->proj_mtx());
		this->shdr_prg_->uni_bind_mat4(this->uni_view_mtx_loc_,
			cam->view_mtx());
		
		this->shdr_prg_->uni_bind_float_vec3(this->uni_cam_world_pos_loc_,
			cam->x_pos(),
			cam->y_pos(),
			cam->z_pos());
	}


	this->shdr_prg_->uni_bind_float_vec4(this->uni_wave_scale_loc_,
		this->wave_scale_[0], this->wave_scale_[1], this->wave_scale_[2], this->wave_scale_[3]);

	this->shdr_prg_->uni_bind_float_vec4(this->uni_wave_offset_loc_,
		this->wave_offset_[0], this->wave_offset_[1], this->wave_offset_[2], this->wave_offset_[3]);

	this->shdr_prg_->uni_bind_float(this->uni_refl_distort_loc_, this->refl_distort_);

	this->shdr_prg_->uni_bind_float(this->uni_reflect_scale_loc_, this->reflect_scale_);
	this->shdr_prg_->uni_bind_float(this->uni_refract_scale_loc_, this->refract_scale_);
	this->shdr_prg_->uni_bind_float(this->uni_color_scale_loc_, this->color_scale_);

	this->shdr_prg_->uni_bind_int(this->uni_need_refract_loc_, this->need_refract_);
}

void shader_reflect_forward::load_default_fresnel_tex()
{
	std::string tex_file_path = this->eng_->cfg()->get_default_fresnel_tex();

	texture_base::ptr tex;
	this->tex_fresnel_ = this->eng_->tex_mgr()->load_tex(tex_file_path, tex);
}

void shader_reflect_forward::set_param(const l3_int32 param_type, const void * param, const l3_uint32 param_sz)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_RELECT_wave_scale:
		{
			if(param_sz < sizeof(this->wave_scale_))
				return;

			l3_f32 * param_wave_scale = (l3_f32 *)param;

			this->wave_scale_[0] = param_wave_scale[0];
			this->wave_scale_[1] = param_wave_scale[1];
			this->wave_scale_[2] = param_wave_scale[2];
			this->wave_scale_[3] = param_wave_scale[3];
		}
		break;

	case EN_SHADER_PARAM_RELECT_wave_offset:
		{
			if(param_sz < sizeof(this->wave_offset_))
				return;

			l3_f32 * param_wave_offset = (l3_f32 *)param;

			this->wave_offset_[0] = param_wave_offset[0];
			this->wave_offset_[1] = param_wave_offset[1];
			this->wave_offset_[2] = param_wave_offset[2];
			this->wave_offset_[3] = param_wave_offset[3];
		}
		break;

	case EN_SHADER_PARAM_RELECT_refl_distort:
		{
			if(param_sz < sizeof(l3_f32))
				return;

			this->refl_distort_ = *((l3_f32*)param);
		}
		break;
	}
}

}

