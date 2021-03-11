/**
* @file shader_dissolution.cpp
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
#include "shader_dissolution.h"
#include "robj_base.h"
#include "material.h"
#include "camera.h"
#include "sence_mgr.h"
#include "win_device.h"

namespace l3eng{

void shader_dissolution::init()
{
	this->uni_tex1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex1");

	this->uni_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");
	this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
	this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range");
	this->uni_rand_tex_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_rand_tex");
	this->uni_dissolution_threshold_loc_ = this->shdr_prg_->get_uniform_loc("uni_dissolution_threshold");
	this->uni_dissolution_transition_loc_ = this->shdr_prg_->get_uniform_loc("uni_dissolution_transition");	

	this->dissolution_threshold_ = 0.5f;
	this->dissolution_transition_ = 0.3f;
}

void shader_dissolution::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	if(dev)
		dev->blend_mode(L3_BLEND_MODE_ALPHA);

	this->shdr_prg_->active();
}

void shader_dissolution::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	if(dev)
		dev->blend_mode(L3_BLEND_MODE_NONE);

	this->shdr_prg_->inactive();
}

void shader_dissolution::prepare(robj_base& robj)
{
	{
		material& mtrl = robj.get_material();

		OBJ_ID tex1 = mtrl.get_tex_obj_id(0);

		if(tex1 != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_tex1_loc_, tex1);

		this->shdr_prg_->uni_bind_tex(this->uni_rand_tex_loc_, this->tex_rand_);
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

	this->shdr_prg_->uni_bind_float(this->uni_dissolution_threshold_loc_, this->dissolution_threshold_);
	this->shdr_prg_->uni_bind_float(this->uni_dissolution_transition_loc_, this->dissolution_transition_);	
}

void shader_dissolution::set_param_obj(const l3_int32 param_type, const OBJ_ID param_val)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_DISSOLUTION_RAND_TEX:
		this->tex_rand_ = param_val;
		break;
	}
}

void shader_dissolution::set_param_f32(const l3_int32 param_type, const l3_f32 param_val)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_DISSOLUTION_THRESHOLD:
		this->dissolution_threshold_ = param_val;
		break;

	case EN_SHADER_PARAM_DISSOLUTION_TRANSITION:
		this->dissolution_transition_ = param_val;
		if(this->dissolution_transition_ < 0.0f)
			this->dissolution_transition_ = 0.0f;
		if(this->dissolution_transition_ > 1.0f)
			this->dissolution_transition_ = 1.0f;
		break;
	}
}

}
