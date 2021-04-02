/**
* @file shader_defer_render_light_mix.cpp
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
#include "shader_defer_render_light_mix.h"
#include "win_device.h"
#include "render_target_mgr.h"

namespace l3eng{

void shader_defer_render_light_mix::init()
{
	this->uni_tex_light_shadow_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_shadow");
	this->uni_tex_light_vol_loc_    = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_vol");
	this->uni_tex_light_mix_loc_    = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_mix");

	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);

	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();


#if 0
	//for (l3_int32 i = 0; i < sizeof(this->rtt_light_mix_) / sizeof(this->rtt_light_mix_[0]); i++)
	//{

	//	render_target_base::ptr rtt_ptr;
	//	this->rtt_light_mix_[i] = rtt_mgr->create_fbo_empty(rtt_ptr,
	//		(l3_int32)dev->width(), (l3_int32)dev->height(),
	//		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
	//	);

	//	rtt_ptr->add_tex_clr(
	//		render_target_base::e_rtt_color_format_rgba_f16);

	//	render_target_base::ptr rtt;
	//	rtt_mgr->get_render_target(this->rtt_light_mix_[i], rtt);
	//	if (rtt.is_null())
	//		continue;

	//	this->tex_light_mix_[i] = rtt->clr_tex_1()->obj_id();
	//}
#endif
}

void shader_defer_render_light_mix::pre_frame(sence_mgr * sence)
{
	this->idx_mix_ = !this->idx_mix_;
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_light_mix_[this->idx_mix_]);
	dev->clear_fbo(this->rtt_light_mix_[this->idx_mix_]);

	this->shdr_prg_->active();
}

void shader_defer_render_light_mix::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_light_mix_[this->idx_mix_]);

	this->shdr_prg_->inactive();
}

void shader_defer_render_light_mix::render_screen_quad()
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_shadow_loc_, this->tex_light_shadow_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_vol_loc_, this->tex_vol_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_mix_loc_, this->tex_light_mix_[!this->idx_mix_]);

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

void shader_defer_render_light_mix::clear_fbo()
{
	win_device * dev = this->eng_->dev();

	for(l3_int32 i = 0; i < sizeof(this->rtt_light_mix_) / sizeof(this->rtt_light_mix_[0]); i ++)
	{
		dev->enable_rtt(this->rtt_light_mix_[i]);
		dev->clear_fbo(this->rtt_light_mix_[i]);
		dev->disable_rtt(this->rtt_light_mix_[i]);
	}
}

}
