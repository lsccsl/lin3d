/**
* @file shader_easy_hdr.cpp
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
#include "shader_easy_hdr.h"
#include "win_device.h"
#include "render_target_mgr.h"

namespace l3eng{

void shader_easy_hdr::init()
{
	this->uni_tex_final_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_final");

	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);

	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

	//this->rtt_out_put_ = rtt_mgr->create_fbo_no_dep((l3_int32)dev->width(), (l3_int32)dev->height(),
	//	1,
	//	render_target_base::e_rtt_color_format_rgba_f16,
	//	color(0.0f, 0.0f, 0.0f, 0.0f));

#else

	render_target_base::ptr rtt_ptr;
	this->rtt_out_put_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height(),
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

#endif
}

void shader_easy_hdr::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_out_put_);
	dev->clear_fbo(this->rtt_out_put_);

	this->shdr_prg_->active();
}

void shader_easy_hdr::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_out_put_);

	this->shdr_prg_->inactive();
}

void shader_easy_hdr::render_screen_quad()
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_final_loc_, this->tex_final_);	

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

}
