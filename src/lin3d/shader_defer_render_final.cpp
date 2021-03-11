/**
* @file shader_defer_render_final.cpp
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
#include "shader_defer_render_final.h"
#include "l3_engine.h"
#include "render_target_mgr.h"
#include "win_device.h"

namespace l3eng{

void shader_defer_render_final::init()
{
	this->uni_tex_light_shadow_loc_    = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_shadow");
	this->uni_tex_light_vol_loc_       = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_vol");
	this->uni_tex_clr_loc_             = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr");
	this->uni_tex_view_depth_loc_      = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");
	this->uni_tex_view_proj_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_proj_depth");
	this->uni_tex_ssao_loc_            = this->shdr_prg_->get_uniform_tex_loc("uni_tex_ssao");
	this->uni_tex_atmospheric_loc_     = this->shdr_prg_->get_uniform_tex_loc("uni_tex_atmospheric");
	this->uni_diff_scale_loc_          = this->shdr_prg_->get_uniform_loc("uni_diff_scale");
	this->uni_atmospheric_scale_loc_   = this->shdr_prg_->get_uniform_loc("uni_atmospheric_scale");
	this->uni_has_ssao_loc_            = this->shdr_prg_->get_uniform_loc("uni_has_ssao");
	this->uni_has_light_vol_loc_       = this->shdr_prg_->get_uniform_loc("uni_has_light_vol");
	this->uni_has_atmospheric_loc_     = this->shdr_prg_->get_uniform_loc("uni_has_atmospheric");


	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);


	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

 	render_target_base::ptr rtt_ptr;
	this->rtt_final_output_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height()
		,color(1.1f, 1.1f, 1.1f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

	rtt_ptr->set_tex_dep(
		render_target_base::e_rtt_depth_format_32,
		texture_base::e_tex_compare_none,
		1.0f);

#endif
}

void shader_defer_render_final::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_final_output_);
	dev->clear_fbo(this->rtt_final_output_);

	this->shdr_prg_->active();
}

void shader_defer_render_final::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_final_output_);

	this->shdr_prg_->inactive();
}

void shader_defer_render_final::set_param_f32(const l3_int32 param_type, const l3_f32 param_val)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_FINAL_DIFF_SCALE:
		this->diff_scale_ = param_val;
		break;
	}
}

void shader_defer_render_final::render_screen_quad()
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_shadow_loc_, this->tex_light_shadow_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_vol_loc_, this->tex_light_vol_);	
	this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->tex_sence_clr_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_view_depth_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_proj_depth_loc_, this->tex_view_proj_depth_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_ssao_loc_, this->tex_ssao_);	
	this->shdr_prg_->uni_bind_tex(this->uni_tex_atmospheric_loc_, this->tex_atmospheric_);	
	this->shdr_prg_->uni_bind_float(this->uni_diff_scale_loc_, this->diff_scale_);
	this->shdr_prg_->uni_bind_float(this->uni_atmospheric_scale_loc_, this->atmospheric_scale_);

	this->shdr_prg_->uni_bind_int(this->uni_has_ssao_loc_, this->has_ssao_);
	this->shdr_prg_->uni_bind_int(this->uni_has_light_vol_loc_, this->has_light_vol_);
	this->shdr_prg_->uni_bind_int(this->uni_has_atmospheric_loc_, this->has_atmospheric_);

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

}
