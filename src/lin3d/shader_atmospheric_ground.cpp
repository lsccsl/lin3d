/**
* @file shader_atmospheric_ground.cpp
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
#include "shader_atmospheric_ground.h"
#include "camera.h"
#include "sence_mgr.h"
#include "l3_enum_define.h"

namespace l3eng{

shader_atmospheric_ground::shader_atmospheric_ground(l3_engine * eng):shader(eng),
	sea_level_(0.0f),
	rayleigh_scale_height_(1.000f),
	scale_(0.01f),
	vLightDirection_(0.0f, 1.0f, 5.0f),
	esun_(50.0f),
	g_(-0.98999f)
{
	this->vLightDirection_.normalise();

	this->Kr_ = 0.025f;		// Rayleigh scattering constant
	this->Kr4PI_ = Kr_* 4.0f * L3_PI;
	this->Km_ = 0.01f;		// Mie scattering constant
	this->Km4PI_ = Km_ * 4.0f * L3_PI;

	this->inv_array_fWavelength4_.x(1.0f / powf(0.650f, 4.0f));
	this->inv_array_fWavelength4_.y(1.0f / powf(0.570f, 4.0f));
	this->inv_array_fWavelength4_.z(1.0f / powf(0.475f, 4.0f));
}

void shader_atmospheric_ground::init()
{
	this->uni_sea_level_loc_              = this->shdr_prg_->get_uniform_loc("uni_sea_level");
	this->uni_rayleigh_scale_height_loc_  = this->shdr_prg_->get_uniform_loc("uni_rayleigh_scale_height");
	this->uni_scale_loc_                  = this->shdr_prg_->get_uniform_loc("uni_scale");
	this->uni_vlight_dir_loc_             = this->shdr_prg_->get_uniform_loc("uni_vlight_dir");
	this->uni_g_loc_                      = this->shdr_prg_->get_uniform_loc("uni_g");
	this->uni_esun_loc_                   = this->shdr_prg_->get_uniform_loc("uni_esun");
	this->uni_kr_loc_                     = this->shdr_prg_->get_uniform_loc("uni_kr");
	this->uni_kr4PI_loc_                  = this->shdr_prg_->get_uniform_loc("uni_kr4PI");
	this->uni_km_loc_                     = this->shdr_prg_->get_uniform_loc("uni_km");
	this->uni_km4PI_loc_                  = this->shdr_prg_->get_uniform_loc("uni_km4PI");
	this->uni_inv_array_fWavelength4_loc_ = this->shdr_prg_->get_uniform_loc("uni_inv_array_fWavelength4");

	this->uni_proj_mtx_loc_  = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");
	this->uni_view_mtx_loc_  = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");

	this->uni_cam_pos_loc_ = this->shdr_prg_->get_uniform_loc("uni_cam_pos");
}

void shader_atmospheric_ground::pre_frame(sence_mgr * sence)
{}

void shader_atmospheric_ground::post_frame(sence_mgr * sence)
{}

void shader_atmospheric_ground::prepare(robj_base& robj)
{
	this->shdr_prg_->uni_bind_float(this->uni_sea_level_loc_, this->sea_level_);
	this->shdr_prg_->uni_bind_float(this->uni_rayleigh_scale_height_loc_, this->rayleigh_scale_height_);
	this->shdr_prg_->uni_bind_float(this->uni_scale_loc_, this->scale_);
	this->shdr_prg_->uni_bind_float_vec3(this->uni_vlight_dir_loc_,
		this->vLightDirection_.x(), this->vLightDirection_.y(), this->vLightDirection_.z());
	this->shdr_prg_->uni_bind_float(this->uni_g_loc_, this->g_);
	this->shdr_prg_->uni_bind_float(this->uni_esun_loc_, this->esun_);
	this->shdr_prg_->uni_bind_float(this->uni_kr_loc_, this->Kr_);
	this->shdr_prg_->uni_bind_float(this->uni_kr4PI_loc_, this->Kr4PI_);
	this->shdr_prg_->uni_bind_float(this->uni_km_loc_, this->Km_);
	this->shdr_prg_->uni_bind_float(this->uni_km4PI_loc_, this->Km4PI_);
	this->shdr_prg_->uni_bind_float_vec3(this->uni_inv_array_fWavelength4_loc_,
		this->inv_array_fWavelength4_.x(), this->inv_array_fWavelength4_.y(), this->inv_array_fWavelength4_.z());

	camera * cam = robj.sence()->cam_cur();
	{		
		this->shdr_prg_->uni_bind_mat4(this->uni_view_mtx_loc_, cam->view_mtx());

		this->shdr_prg_->uni_bind_mat4(this->uni_proj_mtx_loc_, cam->proj_mtx());

		this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_, robj.get_world_matrix());

#if 1
		this->shdr_prg_->uni_bind_float_vec3(this->uni_cam_pos_loc_,
			cam->x_pos(), cam->y_pos(), cam->z_pos());
#else
		this->shdr_prg_->uni_bind_float_vec3(this->uni_cam_pos_loc_,
			0, 0, 0);
#endif
	}
}

void shader_atmospheric_ground::set_param_f32(const l3_int32 param_type, const l3_f32 param_val)
{
	switch(param_type)
	{
	case EN_SHADER_ATMOSPHERIC_PARAM_sea_level:
		this->sea_level_ = param_val;
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_rayleigh_scale_height:
		this->rayleigh_scale_height_ = param_val;
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_scale:
		this->scale_ = param_val;
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_g:
		this->g_ = param_val;
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_esun:
		this->esun_ = param_val;
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_Kr:
		this->Kr_ = param_val;
		this->Kr4PI_ = Kr_* 4.0f * L3_PI;
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_Km: // l3_f32 
		this->Km_ = param_val;
		this->Km4PI_ = Km_ * 4.0f * L3_PI;
		break;
	}


}

void shader_atmospheric_ground::set_param_vec3(const l3_int32 param_type, const vector3& param_val)
{
	switch(param_type)
	{
	case EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection:
		this->vLightDirection_ = param_val;
		this->vLightDirection_.normalise();
		break;

	case EN_SHADER_ATMOSPHERIC_PARAM_inv_array_fWavelength4:
		this->inv_array_fWavelength4_ = param_val;
		break;
	}
}

}
