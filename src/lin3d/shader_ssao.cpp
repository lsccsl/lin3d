/**
* @file shader_ssao.cpp
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
#include "shader_ssao.h"
#include "camera.h"
#include "sence_mgr.h"
#include "win_device.h"
#include "render_target_mgr.h"

namespace l3eng{

void shader_ssao::init()
{
	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");
	this->uni_tex_nml_loc_        = this->shdr_prg_->get_uniform_tex_loc("uni_tex_nml");

	this->uni_tex_dep_pix_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_tex_dep_pix_offset");
	this->uni_tan_half_fov_loc_       = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov");
	this->uni_dep_range_loc_          = this->shdr_prg_->get_uniform_loc("uni_dep_range");
	this->uni_aspect_loc_             = this->shdr_prg_->get_uniform_loc("uni_aspect");


	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

	render_target_base::ptr rtt_ptr;
	this->rtt_ssao_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height(),
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

#endif


	screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);
}

void shader_ssao::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_ssao_);
	dev->clear_fbo(this->rtt_ssao_);

	this->shdr_prg_->active();
}

void shader_ssao::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_ssao_);

	this->shdr_prg_->inactive();
}

void shader_ssao::render_screen_quad(sence_mgr * sence)
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_view_depth_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_nml_loc_, this->tex_view_nml_);

	this->shdr_prg_->uni_bind_float_ve2(this->uni_tex_dep_pix_offset_loc_,
		this->pix_offset_x_,
		this->pix_offset_y_);		

	{
		camera * cam = sence->cam_cur();

		this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
			cam->z_near(),
			cam->z_far() - cam->z_near());

		this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_,
			cam->aspect());

		this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f)); //todo option here
	}

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

}
