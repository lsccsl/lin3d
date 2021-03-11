/**
* @file shader_hdr.cpp
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
#include "shader_hdr.h"
#include "win_device.h"
#include "l3_engine.h"
#include "render_target_mgr.h"
#include "win32gl_glee.h"

namespace l3eng{

shader_hdr::shader_hdr(l3_engine * eng):shader(eng),
	hdr_stage_(HDR_STAGE_downsample4x),
	hl_threshold_(1.0f),
	pix_offset_x_(0.01f),
	pix_offset_y_(0.01f)
{}

void shader_hdr::init()
{
	this->uni_hdr_stage_loc_ = this->shdr_prg_->get_uniform_loc("uni_hdr_stage");
	this->uni_high_light_threshold_loc_ = this->shdr_prg_->get_uniform_loc("uni_high_light_threshold");
	this->uni_src_pix_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_src_pix_offset");

	this->uni_tex_src_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_src");

	this->uni_tex_blur1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_blur1");
	this->uni_tex_blur2_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_blur2");
	this->uni_tex_blur3_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_blur3");
	this->uni_tex_blur4_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_blur4");
	this->uni_tex_streak1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_streak1");
	this->uni_tex_streak2_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_streak2");
	this->uni_tex_streak3_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_streak3");
	this->uni_tex_streak4_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_streak4");

	this->uni_tex_final_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_final");

	this->uni_coeff1_loc_ = this->shdr_prg_->get_uniform_loc("uni_coeff1");
	this->uni_coeff2_loc_ = this->shdr_prg_->get_uniform_loc("uni_coeff2");
	this->uni_coeff3_loc_ = this->shdr_prg_->get_uniform_loc("uni_coeff3");
	this->uni_coeff4_loc_ = this->shdr_prg_->get_uniform_loc("uni_coeff4");

	this->uni_step_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_step_offset");

	this->uni_ave_lum_loc_ = this->shdr_prg_->get_uniform_loc("uni_ave_lum");


	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);

	this->_init_rtt();
	this->_init_shdr_param();
}

void shader_hdr::pre_frame(sence_mgr * sence)
{
	this->shdr_prg_->active();
}

void shader_hdr::post_frame(sence_mgr * sence)
{
	this->shdr_prg_->inactive();
}

void shader_hdr::_hdr_down_sample4x(OBJ_ID tex_src, OBJ_ID rtt_dest, const l3_f32 offset_x, const l3_f32 offset_y)
{
	win_device * dev = this->eng_->dev();

	this->hdr_stage_ = HDR_STAGE_downsample4x;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);

	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, tex_src);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, offset_x, offset_y);	

	dev->enable_rtt(rtt_dest);
	this->screen_quad_.render_screen_quad(this->shdr_prg_);
	dev->disable_rtt(rtt_dest);
}

void shader_hdr::_hdr_down_sample4x_and_read(OBJ_ID tex_src, OBJ_ID rtt_dest, const l3_f32 offset_x, const l3_f32 offset_y, l3_f32& lum)
{
	win_device * dev = this->eng_->dev();

	this->hdr_stage_ = HDR_STAGE_downsample4x;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);

	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, tex_src);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, offset_x, offset_y);	

	dev->enable_rtt(rtt_dest);
	this->screen_quad_.render_screen_quad(this->shdr_prg_);

	{
		l3_f32 pix[4] = {0.0f};
		glReadPixels(0,
  			0,
			1,
			1,
			GL_RGBA,
			GL_FLOAT,
			pix);

		lum = 0.27f * pix[0] + 0.67f * pix[1] + 0.06f * pix[2];
	}

	dev->disable_rtt(rtt_dest);
}

void shader_hdr::_hdr_blur_high_light()
{
	win_device * dev = this->eng_->dev();

	// 降采样256x256 --> 32x32 并做简单过滤
	this->hdr_stage_ = HDR_STAGE_blur_high_light;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[0]);
		this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / 256.0f, 1.0f / 256.0f);	

		dev->enable_rtt(this->rtt_blur1_[0]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_blur1_[0]);

		l3_int32 w = 256;
		l3_int32 h = 256;
		for(l3_int32 i = 1; i < HDR_LEVEL; i ++)
		{
			this->hdr_stage_ = HDR_STAGE_downsample;
			this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
			{
				this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[i - 1]);
				this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / w, 1.0f / h);	

				dev->enable_rtt(this->rtt_compose_buffer_[i]);
				this->screen_quad_.render_screen_quad(this->shdr_prg_);
				dev->disable_rtt(this->rtt_compose_buffer_[i]);
			}

			this->hdr_stage_ = HDR_STAGE_blur_high_light;
			this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
			{
				this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[i]);
				this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / w, 1.0f / h);	

				dev->enable_rtt(this->rtt_blur1_[i]);
				this->screen_quad_.render_screen_quad(this->shdr_prg_);
				dev->disable_rtt(this->rtt_blur1_[i]);
			}

			w = w / 2;
			h = h / 2;
		}
	}
}

void shader_hdr::_hdr_extract_high_light()
{
	win_device * dev = this->eng_->dev();

	this->hdr_stage_ = HDR_STAGE_extract_high_lihgt;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_blur1_[0]);
		this->shdr_prg_->uni_bind_float(this->uni_high_light_threshold_loc_, hl_threshold_);
		this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / 256.0f, 1.0f / 256.0f);	

		dev->enable_rtt(this->rtt_compose_buffer_[0]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_compose_buffer_[0]);
	}
}

void shader_hdr::_hdr_compose()
{
	win_device * dev = this->eng_->dev();

	// 合并 blur
	this->hdr_stage_ = HDR_STAGE_compose;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_blur1_loc_, this->tex_blur1_[0]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_blur2_loc_, this->tex_blur1_[1]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_blur3_loc_, this->tex_blur1_[2]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_blur4_loc_, this->tex_blur1_[3]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_streak1_loc_, this->tex_streak_[0]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_streak2_loc_, this->tex_streak_[1]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_streak3_loc_, this->tex_streak_[2]);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_streak4_loc_, this->tex_streak_[3]);

		dev->enable_rtt(this->rtt_compose_buffer_[0]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_compose_buffer_[0]);
	}
}

void shader_hdr::_hdr_tone_mapping()
{
	win_device * dev = this->eng_->dev();

	this->hdr_stage_ = HDR_STAGE_tone_mapping;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_final_loc_, this->tex_final_);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[0]);
		this->shdr_prg_->uni_bind_float(this->uni_ave_lum_loc_, this->ave_lum_);

		this->screen_quad_.render_screen_quad(this->shdr_prg_);
	}
}

void shader_hdr::render_screen_quad()
{
	win_device * dev = this->eng_->dev();

	// 降采样至256x256
	this->_hdr_down_sample4x(this->tex_final_, this->rtt_blur1_[0], this->pix_offset_x_, this->pix_offset_y_);
	//this->hdr_stage_ = HDR_STAGE_downsample4x;
	//this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//{
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_final_);
	//	this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, this->pix_offset_x_, this->pix_offset_y_);	

	//	dev->enable_rtt(this->rtt_blur1_[0]);
	//	this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//	dev->disable_rtt(this->rtt_blur1_[0]);
	//}

	this->_hdr_down_sample4x(this->tex_blur1_[0],    rtt_exp_buff_[0], 1.0f / 256.0f, 1.0f / 256.0f);
	this->_hdr_down_sample4x(this->tex_exp_buff_[0], rtt_exp_buff_[1], 1.0f / 64.0f,  1.0f / 64.0f);
	this->_hdr_down_sample4x(this->tex_exp_buff_[1], rtt_exp_buff_[2], 1.0f / 16.0f,  1.0f / 16.0f);
	l3_f32 lum = 0.0f;
	this->_hdr_down_sample4x_and_read(this->tex_exp_buff_[2], rtt_exp_buff_[3], 1.0f / 4.0f,   1.0f / 4.0f, this->ave_lum_);

	// 拷贝高亮区域
	this->_hdr_extract_high_light();
	//this->hdr_stage_ = HDR_STAGE_extract_high_lihgt;
	//this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//{
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_blur1_[0]);
	//	this->shdr_prg_->uni_bind_float(this->uni_high_light_threshold_loc_, hl_threshold_);
	//	this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / 256.0f, 1.0f / 256.0f);	

	//	dev->enable_rtt(this->rtt_compose_buffer_[0]);
	//	this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//	dev->disable_rtt(this->rtt_compose_buffer_[0]);
	//}

	this->_hdr_blur_high_light();
	//// 降采样256x256 --> 32x32 并做简单过滤
	//this->hdr_stage_ = HDR_STAGE_blur_high_light;
	//this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//{
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[0]);
	//	this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / 256.0f, 1.0f / 256.0f);	

	//	dev->enable_rtt(this->rtt_blur1_[0]);
	//	this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//	dev->disable_rtt(this->rtt_blur1_[0]);

	//	l3_int32 w = 256;
	//	l3_int32 h = 256;
	//	for(l3_int32 i = 1; i < HDR_LEVEL; i ++)
	//	{
	//		this->hdr_stage_ = HDR_STAGE_downsample;
	//		this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//		{
	//			this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[i - 1]);
	//			this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / w, 1.0f / h);	

	//			dev->enable_rtt(this->rtt_compose_buffer_[i]);
	//			this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//			dev->disable_rtt(this->rtt_compose_buffer_[i]);
	//		}

	//		this->hdr_stage_ = HDR_STAGE_blur_high_light;
	//		this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//		{
	//			this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[i]);
	//			this->shdr_prg_->uni_bind_float_ve2(this->uni_src_pix_offset_loc_, 1.0f / w, 1.0f / h);	

	//			dev->enable_rtt(this->rtt_blur1_[i]);
	//			this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//			dev->disable_rtt(this->rtt_blur1_[i]);
	//		}

	//		w = w / 2;
	//		h = h / 2;
	//	}
	//}

	l3_f32 aspect = dev->width() / dev->height();
	l3_f32 w = 0.9f/256.0f;

#if 0
	this->_render_hdr_horizontal_streak( w,  aspect * w, 0);
	this->_render_hdr_horizontal_streak(-w,  -aspect * w, 1);
#else
	this->_hdr_start_streak( w,  aspect * w, 0);
	this->_hdr_start_streak( w, -aspect * w, 1);
	this->_hdr_start_streak(-w,  aspect * w, 2);
	this->_hdr_start_streak(-w, -aspect * w, 3);
#endif

	this->_hdr_compose();
	//// 合并 blur
	//this->hdr_stage_ = HDR_STAGE_compose;
	//this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//{
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_blur1_loc_, this->tex_blur1_[0]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_blur2_loc_, this->tex_blur1_[1]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_blur3_loc_, this->tex_blur1_[2]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_blur4_loc_, this->tex_blur1_[3]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_streak1_loc_, this->tex_streak_[0]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_streak2_loc_, this->tex_streak_[1]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_streak3_loc_, this->tex_streak_[2]);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_streak4_loc_, this->tex_streak_[3]);

	//	dev->enable_rtt(this->rtt_compose_buffer_[0]);
	//	this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//	dev->disable_rtt(this->rtt_compose_buffer_[0]);
	//}

	this->_hdr_tone_mapping();
	//this->hdr_stage_ = HDR_STAGE_tone_mapping;
	//this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	//{
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_final_loc_, this->tex_final_);
	//	this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[0]);

	//	this->screen_quad_.render_screen_quad(this->shdr_prg_);
	//}
}

void shader_hdr::_hdr_start_streak(const l3_f32 x_offset, const l3_f32 y_offset, const l3_int32 idx)
{
	#define DEC (0.90)
	win_device * dev = this->eng_->dev();
	float colorCoeff[16];
	l3_int32 n, s;

	this->hdr_stage_ = HDR_STAGE_glare;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	// 1
	{
		this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
			x_offset, y_offset);

		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[0]);

		n=1;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4] = this->star_modulation1st_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->star_modulation1st_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->star_modulation1st_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->star_modulation1st_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0],  colorCoeff[1],  colorCoeff[2],  colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4],  colorCoeff[5],  colorCoeff[6],  colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8],  colorCoeff[9],  colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_[idx]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_[idx]);
	}

	// 2
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	{
		this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
			4 * x_offset, 4 * y_offset);

		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_streak_[idx]);

		n=2;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4] = this->star_modulation2nd_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->star_modulation2nd_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->star_modulation2nd_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->star_modulation2nd_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0],  colorCoeff[1],  colorCoeff[2],  colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4],  colorCoeff[5],  colorCoeff[6],  colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8],  colorCoeff[9],  colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_tmp_);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_tmp_);
	}

	// 3
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	{
		this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
			16 * x_offset, 16 * y_offset);

		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_streak_tmp_);

		n=3;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4] =  this->star_modulation3rd_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->star_modulation3rd_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->star_modulation3rd_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->star_modulation3rd_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0],  colorCoeff[1],  colorCoeff[2],  colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4],  colorCoeff[5],  colorCoeff[6],  colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8],  colorCoeff[9],  colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_[idx]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_[idx]);
	}

	#undef DEC
}

void shader_hdr::_hdr_horizontal_streak(const l3_f32 x_offset, const l3_f32 y_offset, const l3_int32 idx)
{
	#define DEC (0.96)
	win_device * dev = this->eng_->dev();
	float colorCoeff[16];
	l3_int32 n, s;

	this->hdr_stage_ = HDR_STAGE_glare;
	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
		x_offset, y_offset/*0.9f/256.0f*/);
	// 1
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_compose_buffer_[0]);

		n = 1;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4]   = this->hori_modulation1st_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->hori_modulation1st_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->hori_modulation1st_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->hori_modulation1st_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0],  colorCoeff[1],  colorCoeff[2],  colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4],  colorCoeff[5],  colorCoeff[6],  colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8],  colorCoeff[9],  colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_tmp_);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_tmp_);
	}

	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
		4 * x_offset, 4 * y_offset/*0.9f/256.0f*/);
	// 2
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_streak_tmp_);

		n = 2;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4]   = this->hori_modulation1st_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->hori_modulation1st_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->hori_modulation1st_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->hori_modulation1st_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0],  colorCoeff[1],  colorCoeff[2],  colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4],  colorCoeff[5],  colorCoeff[6],  colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8],  colorCoeff[9],  colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_[idx]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_[idx]);
	}


	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
		16 * x_offset, 16 * y_offset/*0.9f/256.0f*/);
	// 3
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_streak_[idx]);

		n = 3;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4]   = this->hori_modulation2nd_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->hori_modulation2nd_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->hori_modulation2nd_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->hori_modulation2nd_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0], colorCoeff[1], colorCoeff[2], colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4], colorCoeff[5], colorCoeff[6], colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8], colorCoeff[9], colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_tmp_);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_tmp_);
	}


	this->shdr_prg_->uni_bind_int(this->uni_hdr_stage_loc_, this->hdr_stage_);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_step_offset_loc_,
		64 * x_offset, 64 * y_offset/*0.9f/256.0f*/);
	// 4
	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_src_loc_, this->tex_streak_tmp_);

		n = 4;
		for (s=0; s<4; s+=1) {
			colorCoeff[s*4]   = this->hori_modulation3rd_[s*4] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+1] = this->hori_modulation3rd_[s*4+1] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+2] = this->hori_modulation3rd_[s*4+2] * pow(float(DEC), pow(float(4),n-1)*s);
			colorCoeff[s*4+3] = this->hori_modulation3rd_[s*4+3] * pow(float(DEC), pow(float(4),n-1)*s);
		}

		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff1_loc_, colorCoeff[0], colorCoeff[1], colorCoeff[2], colorCoeff[3]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff2_loc_, colorCoeff[4], colorCoeff[5], colorCoeff[6], colorCoeff[7]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff3_loc_, colorCoeff[8], colorCoeff[9], colorCoeff[10], colorCoeff[11]);
		this->shdr_prg_->uni_bind_float_vec4(this->uni_coeff4_loc_, colorCoeff[12], colorCoeff[13], colorCoeff[14], colorCoeff[15]);

		dev->enable_rtt(this->rtt_streak_[idx]);
		this->screen_quad_.render_screen_quad(this->shdr_prg_);
		dev->disable_rtt(this->rtt_streak_[idx]);
	}
}

void shader_hdr::_init_rtt()
{
	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

	l3_int32 w = 256;
	l3_int32 h = 256;

	l3_int32 tmp_w = w;
	l3_int32 tmp_h = h;

	for(l3_int32 i = 0; i < sizeof(this->rtt_blur1_)/sizeof(this->rtt_blur1_[0]); i ++)
	{

#if 0

		//this->rtt_blur1_[i] = rtt_mgr->create_fbo_no_dep(tmp_w, tmp_h,
		//	1,
		//	render_target_base::e_rtt_color_format_rgba_f16,
		//	color(0.0f, 0.0f, 0.0f, 0.0f));

#else

		render_target_base::ptr rtt_ptr;
		this->rtt_blur1_[i] = rtt_mgr->create_fbo_empty(rtt_ptr,
			tmp_w, tmp_h,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
			);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

#endif

		this->tex_blur1_[i] = rtt_mgr->get_rtt_tex(this->rtt_blur1_[i], 1);

		tmp_w = tmp_w / 2;
		tmp_h = tmp_h / 2;
	}

	//tmp_w = w;
	//tmp_h = h;
	//for(l3_int32 i = 0; i < sizeof(this->rtt_blur2_)/sizeof(this->rtt_blur2_[0]); i ++)
	//{

	//	this->rtt_blur2_[i] = rtt_mgr->create_fbo_no_dep(tmp_w, tmp_h,
	//		1,
	//		render_target_base::e_rtt_color_format_rgba_f16,
	//		color(0.0f, 0.0f, 0.0f, 0.0f));

	//	this->tex_blur2_[i] = rtt_mgr->get_rtt_tex(this->rtt_blur2_[i], 1);

	//	tmp_w = tmp_w / 2;
	//	tmp_h = tmp_h / 2;
	//}

	tmp_w = w;
	tmp_h = h;
	for(l3_int32 i = 0; i < sizeof(this->rtt_compose_buffer_)/sizeof(this->rtt_compose_buffer_[0]); i ++)
	{
#if 0

		//this->rtt_compose_buffer_[i] = rtt_mgr->create_fbo_no_dep(tmp_w, tmp_h,
		//	1,
		//	render_target_base::e_rtt_color_format_rgba_f16,
		//	color(0.0f, 0.0f, 0.0f, 0.0f));

#else

		render_target_base::ptr rtt_ptr;
		this->rtt_compose_buffer_[i] = rtt_mgr->create_fbo_empty(rtt_ptr,
			tmp_w, tmp_h,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
			);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

#endif

		this->tex_compose_buffer_[i] = rtt_mgr->get_rtt_tex(this->rtt_compose_buffer_[i], 1);

		tmp_w = tmp_w / 2;
		tmp_h = tmp_h / 2;
	}

	tmp_w = w;
	tmp_h = h;
	for(l3_int32 i = 0; i < sizeof(this->rtt_streak_)/sizeof(rtt_streak_[0]); i ++)
	{
#if 0

		//this->rtt_streak_[i] = rtt_mgr->create_fbo_no_dep(tmp_w, tmp_h,
		//	1,
		//	render_target_base::e_rtt_color_format_rgba_f16,
		//	color(0.0f, 0.0f, 0.0f, 0.0f));

#else

		render_target_base::ptr rtt_ptr;
		this->rtt_streak_[i] = rtt_mgr->create_fbo_empty(rtt_ptr,
			tmp_w, tmp_h,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
			);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

#endif

		this->tex_streak_[i] = rtt_mgr->get_rtt_tex(this->rtt_streak_[i], 1);
	}

	{
#if 0

		//this->rtt_streak_tmp_ = rtt_mgr->create_fbo_no_dep(tmp_w, tmp_h,
		//	1,
		//	render_target_base::e_rtt_color_format_rgba_f16,
		//	color(0.0f, 0.0f, 0.0f, 0.0f));

#else

		render_target_base::ptr rtt_ptr;
		this->rtt_streak_tmp_ = rtt_mgr->create_fbo_empty(rtt_ptr,
			tmp_w, tmp_h,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
			);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

#endif

		this->tex_streak_tmp_ = rtt_mgr->get_rtt_tex(this->rtt_streak_tmp_, 1);
	}

	tmp_w = 64;
	tmp_h = 64;
	for(l3_int32 i = 0; i < sizeof(this->rtt_exp_buff_)/sizeof(rtt_exp_buff_[0]); i ++)
	{
#if 0
		//this->rtt_exp_buff_[i] = rtt_mgr->create_fbo_no_dep(tmp_w, tmp_h,
		//	1,
		//	render_target_base::e_rtt_color_format_rgba_f16,
		//	color(0.0f, 0.0f, 0.0f, 0.0f));
#else

		render_target_base::ptr rtt_ptr;
		this->rtt_exp_buff_[i] = rtt_mgr->create_fbo_empty(rtt_ptr,
			tmp_w, tmp_h,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
			);

		rtt_ptr->add_tex_clr(
			render_target_base::e_rtt_color_format_rgba_f16);

#endif

		this->tex_exp_buff_[i] = rtt_mgr->get_rtt_tex(this->rtt_exp_buff_[i], 1);

		tmp_w = tmp_w / 4;
		tmp_h = tmp_h / 4;
	}
}

void ColorModulation(l3_f32 * color, l3_f32 r, l3_f32 g, l3_f32 b, l3_int32 num)
{
	color[4*num] = color[4*num+0]*r;
	color[4*num+1] = color[4*num+1]*g;
	color[4*num+2] = color[4*num+2]*b;
	//color[4*num+3] = color[4*num]*a;
}

void shader_hdr::_init_shdr_param()
{
	for(l3_int32 i = 0; i < HDR_GLARE_COEFF_COUNT; i ++)
	{
#if 1
		this->star_modulation1st_[i]=0.25;
		this->star_modulation2nd_[i]=0.25;
		this->star_modulation3rd_[i]=0.25;
#else
		this->star_modulation1st_[i]=0.30;
		this->star_modulation2nd_[i]=0.30;
		this->star_modulation3rd_[i]=0.30;
#endif

		this->hori_modulation1st_[i] = 0.5;
		this->hori_modulation2nd_[i] = 0.5;
		this->hori_modulation3rd_[i] = 0.5;
	}

    ColorModulation(this->star_modulation1st_, 1.0f, 0.95f, 0.9f, 0);
	ColorModulation(this->star_modulation1st_, 0.8f, 1.0f, 0.9f,  1);
	ColorModulation(this->star_modulation1st_, 0.9f, 0.9f, 1.0f,  2);
	ColorModulation(this->star_modulation1st_, 0.9f, 1.0f, 0.9f,  3);

	ColorModulation(this->star_modulation2nd_, 1.0f, 0.9f, 0.8f, 0);
    ColorModulation(this->star_modulation2nd_, 1.0f, 0.6f, 0.5f, 1);
	ColorModulation(this->star_modulation2nd_, 0.5f, 1.0f, 0.6f, 2);
	ColorModulation(this->star_modulation2nd_, 0.6f, 0.4f, 1.0f, 3);

	ColorModulation(this->star_modulation3rd_, 1.0f, 0.6f, 0.6f, 1);
	ColorModulation(this->star_modulation3rd_, 0.6f, 1.0f, 0.6f, 2);
	ColorModulation(this->star_modulation3rd_, 0.6f, 0.6f, 1.0f, 3);

	#define BLUE_SHIFT0 1.0f,  1.0f, 1.0f
	#define BLUE_SHIFT1 0.2f,  0.3f, 0.95f
	#define BLUE_SHIFT2 0.1f,  0.2f, 0.9f
	#define BLUE_SHIFT3 0.02f, 0.1f, 0.99f

	//horizontal
    ColorModulation(this->hori_modulation1st_, BLUE_SHIFT1,0);
	ColorModulation(this->hori_modulation1st_, BLUE_SHIFT1,1);
	ColorModulation(this->hori_modulation1st_, BLUE_SHIFT2,2);
	ColorModulation(this->hori_modulation1st_, BLUE_SHIFT1,3);

    ColorModulation(this->hori_modulation2nd_, BLUE_SHIFT1,0);
	ColorModulation(this->hori_modulation2nd_, BLUE_SHIFT2,1);
	ColorModulation(this->hori_modulation2nd_, BLUE_SHIFT3,2);
	ColorModulation(this->hori_modulation2nd_, BLUE_SHIFT3,3);

    ColorModulation(this->hori_modulation3rd_, BLUE_SHIFT0,0);
	ColorModulation(this->hori_modulation3rd_, BLUE_SHIFT0,1);
	ColorModulation(this->hori_modulation3rd_, BLUE_SHIFT0,2);
	ColorModulation(this->hori_modulation3rd_, BLUE_SHIFT0,3);
}

}
