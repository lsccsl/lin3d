/**
* @file shader_vol_lightdir_extrude.cpp
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
#include "shader_vol_radial_blur.h"
#include "l3_engine.h"
#include "win_device.h"
#include "render_target_mgr.h"
#include "sence_mgr.h"

#define VOL_RADIAL_BLUR_STAGE_get_high_light 1
#define VOL_RADIAL_BLUR_STAGE_radial_blur    2
#define VOL_RADIAL_BLUR_STAGE_test_final     3

namespace l3eng{
	
void shader_vol_radial_blur::init()
{
	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);

	this->uni_vol_radial_blur_stage_loc_ = this->shdr_prg_->get_uniform_loc("uni_vol_radial_blur_stage");
	this->uni_tex_src_loc_               = this->shdr_prg_->get_uniform_tex_loc("uni_tex_src");
	this->uni_tex_src1_loc_              = this->shdr_prg_->get_uniform_tex_loc("uni_tex_src1");
	
	this->uni_high_light_threshold_loc_  = this->shdr_prg_->get_uniform_loc("uni_high_light_threshold");
	this->uni_light_pos_loc_             = this->shdr_prg_->get_uniform_loc("uni_light_pos");
	this->uni_offset_loc_                = this->shdr_prg_->get_uniform_loc("uni_offset");

	this->vol_rb_hl_threshold_ = 1.4f;

	this->light_pos_x_ = 0.0f;
	this->light_pos_y_ = 1.0f;


	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

	l3_int32 w = (l3_int32)dev->width();
	l3_int32 h = (l3_int32)dev->height();

	{
		render_target_base::ptr rtt_ptr;
		this->rtt_vol_rb_hight_ligh_ = rtt_mgr->create_fbo_empty(rtt_ptr,
			w, h);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

		this->tex_vol_rb_hight_ligh_ = rtt_mgr->get_rtt_tex(this->rtt_vol_rb_hight_ligh_, 1);
	}

	{
		render_target_base::ptr rtt_ptr;
		this->rtt_radial_blur_[0] = rtt_mgr->create_fbo_empty(rtt_ptr,
			w, h);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

		this->tex_radial_blur_[0] = rtt_mgr->get_rtt_tex(this->rtt_radial_blur_[0], 1);
	}

	{
		render_target_base::ptr rtt_ptr;
		this->rtt_radial_blur_[1] = rtt_mgr->create_fbo_empty(rtt_ptr,
			w, h);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

		this->tex_radial_blur_[1] = rtt_mgr->get_rtt_tex(this->rtt_radial_blur_[1], 1);
	}
}

void shader_vol_radial_blur::pre_frame(sence_mgr * sence)
{
	this->shdr_prg_->active();
}

void shader_vol_radial_blur::post_frame(sence_mgr * sence)
{
	this->shdr_prg_->inactive();
}

void shader_vol_radial_blur::prepare(robj_base& robj)
{}

void shader_vol_radial_blur::render_screen_quad(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	l3_f32 w = dev->width();
	l3_f32 h = dev->height();

	// 绑定光源位置
	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_pos_loc_,
		this->light_pos_x_,
		this->light_pos_y_);

	// 获取高亮
	this->shdr_prg_->uni_bind_int(this->uni_vol_radial_blur_stage_loc_, VOL_RADIAL_BLUR_STAGE_get_high_light);
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->vol_rb_tex_src_);
		this->shdr_prg_->uni_bind_float(this->uni_high_light_threshold_loc_, this->vol_rb_hl_threshold_);

		dev->enable_rtt(this->rtt_vol_rb_hight_ligh_);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_vol_rb_hight_ligh_);
	}

	// 径向模糊
	this->shdr_prg_->uni_bind_int(this->uni_vol_radial_blur_stage_loc_, VOL_RADIAL_BLUR_STAGE_radial_blur);

	l3_f32 offset = 1.0f / w;
	{
		this->shdr_prg_->uni_bind_float(this->uni_offset_loc_, offset);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_vol_rb_hight_ligh_);

		dev->enable_rtt(this->rtt_radial_blur_[0]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_radial_blur_[0]);
	}

	l3_int32 cur_src_idx = 0;
	l3_int32 cur_dst_idx = 1;
	const l3_int32 MAX_RADIAL_BLUR = 3;
	for(l3_int32 i = 1; i <= MAX_RADIAL_BLUR; i ++)
	{
		offset = (i + 1) * offset;
		this->shdr_prg_->uni_bind_float(this->uni_offset_loc_, offset);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_radial_blur_[cur_src_idx]);

//if(i < MAX_RADIAL_BLUR)
		dev->enable_rtt(this->rtt_radial_blur_[cur_dst_idx]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
//if(i < MAX_RADIAL_BLUR)
		dev->disable_rtt(this->rtt_radial_blur_[cur_dst_idx]);

		cur_src_idx = 1 - cur_src_idx;
		cur_dst_idx = 1 - cur_dst_idx;
	}

#if 1
// 与原图混合
this->shdr_prg_->uni_bind_int(this->uni_vol_radial_blur_stage_loc_, VOL_RADIAL_BLUR_STAGE_test_final);
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->vol_rb_tex_src_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_src1_loc_, this->tex_radial_blur_[cur_src_idx]);
	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}
#endif

#if 1
{
	this->eng_->sence()->render_show_tex(this->tex_radial_blur_[cur_src_idx],
		1.f, 1.f, 1.f, 1.f
		//0,0,2,2
		);
}
#endif

}

}
