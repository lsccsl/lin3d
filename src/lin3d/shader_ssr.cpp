/**
* @file shader_ssr.cpp
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
#include "shader_ssr.h"
#include "win_device.h"
#include "render_target_mgr.h"
#include "camera.h"
#include "sence_mgr.h"

namespace l3eng{

shader_ssr::shader_ssr(l3_engine * eng):shader(eng),rtt_ssr_(base_obj::INVALID_OBJ_ID),
	sample_count_(1),

	first_step_count_(100),
	second_step_count_(40),
	step_max_len_(2000.0f),

#if 1
	trace_start_(0.5f),
	trace_step_(200.0f),
	trace_count_(41),
	trace_sec_count_(35)
#else
	trace_start_(0.1f),
	trace_step_(5.0f),
	trace_count_(100),
	trace_sec_count_(10)
#endif
{}

void shader_ssr::init()
{
	this->uni_tex_reflect_src_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_reflect_src");
	this->uni_tex_final_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_final");
	this->uni_tex_nml_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_nml");
	this->uni_tex_clr_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr");
	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");

	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov");
	this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range");
	this->uni_aspect_loc_ = this->shdr_prg_->get_uniform_loc("uni_aspect");

	this->uni_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");
	this->uni_screen_x_pix_loc_ = this->shdr_prg_->get_uniform_loc("uni_screen_x_pix");
	this->uni_screen_y_pix_loc_ = this->shdr_prg_->get_uniform_loc("uni_screen_y_pix");

	this->uni_sample_num_loc_        = this->shdr_prg_->get_uniform_loc("uni_sample_num");
	
	this->uni_step_max_len_loc_      = this->shdr_prg_->get_uniform_loc("uni_step_max_len");
	this->uni_first_step_count_loc_  = this->shdr_prg_->get_uniform_loc("uni_first_step_count");
	this->uni_second_step_count_loc_ = this->shdr_prg_->get_uniform_loc("uni_second_step_count");

	this->uni_trace_start_loc_     = this->shdr_prg_->get_uniform_loc("uni_trace_start");
	this->uni_trace_step_loc_      = this->shdr_prg_->get_uniform_loc("uni_trace_step");
	this->uni_trace_count_loc_     = this->shdr_prg_->get_uniform_loc("uni_trace_count");
	this->uni_trace_sec_count_loc_ = this->shdr_prg_->get_uniform_loc("uni_trace_sec_count");


	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

	render_target_base::ptr rtt_ptr;
	this->rtt_ssr_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height(),
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

#endif

	this->tex_ssr_ = rtt_mgr->get_rtt_tex(this->rtt_ssr_, 1);


	screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);
}

void shader_ssr::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_ssr_);
	dev->clear_fbo(this->rtt_ssr_);

	this->shdr_prg_->active();
}

void shader_ssr::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_ssr_);

	this->shdr_prg_->inactive();
}

void shader_ssr::render_screen_quad(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	this->shdr_prg_->uni_bind_tex(this->uni_tex_reflect_src_loc_, this->tex_reflect_src_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_final_loc_, this->tex_final_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_nml_loc_, this->tex_nml_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->tex_clr_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_dep_);

	camera * cam = sence->cam_cur();

	{
		this->shdr_prg_->uni_bind_mat4(this->uni_proj_mtx_loc_,
			cam->proj_mtx());

		this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
			cam->z_near(),
			cam->z_far() - cam->z_near());

		this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_,
			cam->aspect());

		this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f)); //todo option here
	}

	this->shdr_prg_->uni_bind_int(this->uni_screen_x_pix_loc_, (l3_int32)dev->width());
	this->shdr_prg_->uni_bind_int(this->uni_screen_y_pix_loc_, (l3_int32)dev->height());

	this->shdr_prg_->uni_bind_int(this->uni_sample_num_loc_, this->sample_count_);

	this->shdr_prg_->uni_bind_float(this->uni_step_max_len_loc_, this->step_max_len_);	
	this->shdr_prg_->uni_bind_int(this->uni_first_step_count_loc_, this->first_step_count_);
	this->shdr_prg_->uni_bind_int(this->uni_second_step_count_loc_, this->second_step_count_);

	this->shdr_prg_->uni_bind_float(this->uni_trace_start_loc_, this->trace_start_);
	this->shdr_prg_->uni_bind_float(this->uni_trace_step_loc_, this->trace_step_);
	this->shdr_prg_->uni_bind_int(this->uni_trace_count_loc_, this->trace_count_);
	this->shdr_prg_->uni_bind_int(this->uni_trace_sec_count_loc_, this->trace_sec_count_);


	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

void shader_ssr::set_reflect_far()
{
	this->trace_start_ = 0.5f;
	this->trace_step_ = 200.0f;
	this->trace_count_ = 41;
	this->trace_sec_count_ = 35;
}

void shader_ssr::set_reflect_near()
{
	this->trace_start_ = 0.1f;
	this->trace_step_ = 5.0f;
	this->trace_count_ = 100;
	this->trace_sec_count_ = 10;
}

}
