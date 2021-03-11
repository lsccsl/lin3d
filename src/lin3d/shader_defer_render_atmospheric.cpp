/**
* @file shader_defer_render_atmospheric.cpp
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
#include "shader_defer_render_atmospheric.h"
#include "camera.h"
#include "sence_mgr.h"
#include "l3_engine.h"
#include "l3_enum_define.h"
#include "win_device.h"
#include "render_target_mgr.h"


namespace l3eng{

shader_defer_render_atmospheric::shader_defer_render_atmospheric(l3_engine * eng):shader(eng),
	sea_level_(0.0f),
	rayleigh_scale_height_(1.000f),
	//scale_(0.01f),
	scale_(0.0013f),
	vLightDirection_(0.0f, 1.0f, 5.0f),
	esun_(50.0f),
	//g_(-0.98999f),
	g_(-0.7f),
	//g_sky_(-0.98999f),
	g_sky_(-0.8f),
	skydome_radius_(2000.0f),
	attenuation_final_(0.1f),
	rtt_out_put_(base_obj::INVALID_OBJ_ID)
{
	this->vLightDirection_.normalise();

	this->Kr_ = 0.025f;		// Rayleigh scattering constant
	this->Kr4PI_ = Kr_* 4.0f * L3_PI;
	this->Km_ = 0.01f;		// Mie scattering constant
	this->Km4PI_ = Km_ * 4.0f * L3_PI;

#if 0
	this->inv_array_fWavelength4_.x(1.0f / powf(0.650f, 4.0f));
	this->inv_array_fWavelength4_.y(1.0f / powf(0.570f, 4.0f));
	this->inv_array_fWavelength4_.z(1.0f / powf(0.475f, 4.0f));
#else
	this->inv_array_fWavelength4_.x(1.0f / powf(0.650f, 4.0f));
	this->inv_array_fWavelength4_.y(1.0f / powf(0.570f, 4.0f));
	this->inv_array_fWavelength4_.z(1.0f / powf(0.475f, 4.0f));
#endif
}

void shader_defer_render_atmospheric::init()
{
	this->uni_sea_level_loc_              = this->shdr_prg_->get_uniform_loc("uni_sea_level");
	this->uni_rayleigh_scale_height_loc_  = this->shdr_prg_->get_uniform_loc("uni_rayleigh_scale_height");
	this->uni_scale_loc_                  = this->shdr_prg_->get_uniform_loc("uni_scale");
	this->uni_vlight_dir_loc_             = this->shdr_prg_->get_uniform_loc("uni_vlight_dir");
	this->uni_g_loc_                      = this->shdr_prg_->get_uniform_loc("uni_g");
	this->uni_g_sky_loc_                  = this->shdr_prg_->get_uniform_loc("uni_g_sky");
	this->uni_esun_loc_                   = this->shdr_prg_->get_uniform_loc("uni_esun");
	this->uni_kr_loc_                     = this->shdr_prg_->get_uniform_loc("uni_kr");
	this->uni_kr4PI_loc_                  = this->shdr_prg_->get_uniform_loc("uni_kr4PI");
	this->uni_km_loc_                     = this->shdr_prg_->get_uniform_loc("uni_km");
	this->uni_km4PI_loc_                  = this->shdr_prg_->get_uniform_loc("uni_km4PI");
	this->uni_inv_array_fWavelength4_loc_ = this->shdr_prg_->get_uniform_loc("uni_inv_array_fWavelength4");
	this->uni_skydome_radius_loc_         = this->shdr_prg_->get_uniform_loc("uni_skydome_radius");
	this->uni_attenuation_final_loc_      = this->shdr_prg_->get_uniform_loc("uni_attenuation_final");	

	this->uni_cam_pos_loc_                = this->shdr_prg_->get_uniform_loc("uni_cam_pos");

	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");
	this->uni_tex_clr_loc_        = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr");

	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov");
	this->uni_dep_range_loc_    = this->shdr_prg_->get_uniform_loc("uni_dep_range");
	this->uni_aspect_loc_       = this->shdr_prg_->get_uniform_loc("uni_aspect");
	this->uni_view_inv_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_inv_mtx");


	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

 	render_target_base::ptr rtt_ptr;
	this->rtt_out_put_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height(),
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

#endif


	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);
}

void shader_defer_render_atmospheric::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_out_put_);
	dev->clear_fbo(this->rtt_out_put_);

	this->shdr_prg_->active();
}

void shader_defer_render_atmospheric::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_out_put_);

	this->shdr_prg_->inactive();
}

void shader_defer_render_atmospheric::render_screen_quad()
{
	this->shdr_prg_->uni_bind_float(this->uni_sea_level_loc_, this->sea_level_);
	this->shdr_prg_->uni_bind_float(this->uni_rayleigh_scale_height_loc_, this->rayleigh_scale_height_);
	this->shdr_prg_->uni_bind_float(this->uni_scale_loc_, this->scale_);
	this->shdr_prg_->uni_bind_float_vec3(this->uni_vlight_dir_loc_,
		this->vLightDirection_.x(), this->vLightDirection_.y(), this->vLightDirection_.z());
	this->shdr_prg_->uni_bind_float(this->uni_g_loc_, this->g_);
	this->shdr_prg_->uni_bind_float(this->uni_g_sky_loc_, this->g_sky_);	
	this->shdr_prg_->uni_bind_float(this->uni_esun_loc_, this->esun_);
	this->shdr_prg_->uni_bind_float(this->uni_kr_loc_, this->Kr_);
	this->shdr_prg_->uni_bind_float(this->uni_kr4PI_loc_, this->Kr4PI_);
	this->shdr_prg_->uni_bind_float(this->uni_km_loc_, this->Km_);
	this->shdr_prg_->uni_bind_float(this->uni_km4PI_loc_, this->Km4PI_);
	this->shdr_prg_->uni_bind_float_vec3(this->uni_inv_array_fWavelength4_loc_,
		this->inv_array_fWavelength4_.x(), this->inv_array_fWavelength4_.y(), this->inv_array_fWavelength4_.z());
	this->shdr_prg_->uni_bind_float(this->uni_skydome_radius_loc_, this->skydome_radius_);
	this->shdr_prg_->uni_bind_float(this->uni_attenuation_final_loc_, this->attenuation_final_);


	camera * cam = this->eng_->sence()->cam_cur();
	{		
		matrix4 cam_view_inv_mtx;
		cam->view_mtx().gen_inverse_matrix(cam_view_inv_mtx);

		this->shdr_prg_->uni_bind_mat4(this->uni_view_inv_mtx_loc_,
			cam_view_inv_mtx);

		this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
			cam->z_near(),
			cam->z_far() - cam->z_near());

		this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_,
			cam->aspect());

		this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f)); //todo option here

#if 0
		this->shdr_prg_->uni_bind_float_vec3(this->uni_cam_pos_loc_,
			cam->x_pos(), cam->y_pos(), cam->z_pos());
#else
		this->shdr_prg_->uni_bind_float_vec3(this->uni_cam_pos_loc_,
		cam->x_pos(), 0, cam->z_pos());
#endif
	}

	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_view_depth_);	
	this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->tex_sence_clr_);

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

void shader_defer_render_atmospheric::set_param_f32(const l3_int32 param_type, const l3_f32 param_val)
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

	case EN_SHADER_ATMOSPHERIC_PARAM_g_sky:
		this->g_sky_ = param_val;
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

	case EN_SHADER_ATMOSPHERIC_PARAM_attenuation_final:
		this->attenuation_final_ = param_val;
		break;
	}


}

void shader_defer_render_atmospheric::set_param_vec3(const l3_int32 param_type, const vector3& param_val)
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


void shader_defer_render_atmospheric::clear_fbo()
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_out_put_);
	dev->clear_fbo(this->rtt_out_put_);
	dev->disable_rtt(this->rtt_out_put_);
}

}
