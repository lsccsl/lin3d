/**
* @file shader_hdr.h
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
#ifndef __L3ENG_SHADER_HDR_H__
#define __L3ENG_SHADER_HDR_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "shader_screen_quad.h"

#define HDR_LEVEL 4
#define HDR_GLARE_COEFF_COUNT 16
#define HDR_STREAK_DIR 4
#define HDR_EXP_BUFF 4

namespace l3eng{

class shader_hdr : public shader
{
public:

	enum HDR_STAGE{
		HDR_STAGE_downsample4x       = 0,
		HDR_STAGE_downsample         = 1,
		HDR_STAGE_extract_high_lihgt = 2,
		HDR_STAGE_blur_high_light    = 3,
		HDR_STAGE_glare              = 4,
		HDR_STAGE_compose            = 5,
		HDR_STAGE_tone_mapping       = 6,
	};

public:

	typedef ref_ptr_thread<shader_hdr> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_HDR)

public:

	shader_hdr(l3_engine * eng);
	virtual ~shader_hdr(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_hdr);
		return n;
	}

public:

	void hdr_stage(const HDR_STAGE stage)
	{ this->hdr_stage_ = stage;}	

	void tex_final(OBJ_ID tex)
	{ this->tex_final_ = tex; }
	void pix_offset_x(l3_f32 offset)
	{ this->pix_offset_x_ = offset; }
	void pix_offset_y(l3_f32 offset)
	{ this->pix_offset_y_ = offset; }	

	OBJ_ID rtt_blur1(const l3_uint32 idx)
	{ assert(idx < HDR_LEVEL); return this->rtt_blur1_[idx]; }

	OBJ_ID rtt_compose_buffer(const l3_uint32 idx)
	{ assert(idx < HDR_LEVEL); return this->rtt_compose_buffer_[idx]; }

	OBJ_ID rtt_streak(const l3_uint32 idx)
	{ assert(idx < HDR_STREAK_DIR); return this->rtt_streak_[idx]; }

	OBJ_ID rtt_streak_tmp()
	{ return this->rtt_streak_tmp_; }

	OBJ_ID tex_exp_buff(const l3_uint32 idx)
	{ assert(idx < HDR_EXP_BUFF); return this->tex_exp_buff_[idx]; }

	void render_screen_quad();

private:

	void _init_rtt();
	void _init_shdr_param();

	void _hdr_horizontal_streak(const l3_f32 x_offset, const l3_f32 y_offset, const l3_int32 idx);
	void _hdr_start_streak(const l3_f32 x_offset, const l3_f32 y_offset, const l3_int32 idx);

	void _hdr_down_sample4x(OBJ_ID tex_src, OBJ_ID rtt_dest, const l3_f32 offset_x, const l3_f32 offset_y);
	void _hdr_down_sample4x_and_read(OBJ_ID tex_src, OBJ_ID rtt_dest, const l3_f32 offset_x, const l3_f32 offset_y, l3_f32& lum);
	void _hdr_extract_high_light();
	void _hdr_blur_high_light();
	void _hdr_compose();
	void _hdr_tone_mapping();

private:

	l3_int32 uni_hdr_stage_loc_;            //hdr处理流程 uniform int 
	l3_int32 uni_high_light_threshold_loc_; //高亮阀值 uniform float 
	l3_int32 uni_src_pix_offset_loc_;       //输入像素点偏移 uniform vec2 

	l3_int32 uni_tex_src_loc_;
	l3_int32 uni_tex_blur1_loc_;   //uniform sampler2D 
	l3_int32 uni_tex_blur2_loc_;   //uniform sampler2D 
	l3_int32 uni_tex_blur3_loc_;   //uniform sampler2D 
	l3_int32 uni_tex_blur4_loc_;   //uniform sampler2D 
	l3_int32 uni_tex_streak1_loc_; //uniform sampler2D 
	l3_int32 uni_tex_streak2_loc_; //uniform sampler2D 
	l3_int32 uni_tex_streak3_loc_; //uniform sampler2D 
	l3_int32 uni_tex_streak4_loc_; //uniform sampler2D 
	l3_int32 uni_tex_final_loc_; //uniform sampler2D 

	l3_int32 uni_coeff1_loc_; //uniform vec4 
	l3_int32 uni_coeff2_loc_; //uniform vec4 
	l3_int32 uni_coeff3_loc_; //uniform vec4 
	l3_int32 uni_coeff4_loc_; //uniform vec4 

	l3_int32 uni_step_offset_loc_; //每个采样的偏移 uniform vec2 

	l3_int32 uni_ave_lum_loc_; //原始场景像素lum均值 uniform float 



	/* @brief tex source */
	OBJ_ID tex_final_;
	HDR_STAGE hdr_stage_;
	l3_f32 hl_threshold_;
	l3_f32 pix_offset_x_;
	l3_f32 pix_offset_y_;
	l3_f32 ave_lum_;

	
	OBJ_ID rtt_exp_buff_[HDR_EXP_BUFF];
	OBJ_ID tex_exp_buff_[HDR_EXP_BUFF];

	OBJ_ID rtt_blur1_[HDR_LEVEL];
	OBJ_ID tex_blur1_[HDR_LEVEL];
	//OBJ_ID rtt_blur2_[HDR_LEVEL];
	//OBJ_ID tex_blur2_[HDR_LEVEL];

	OBJ_ID rtt_compose_buffer_[HDR_LEVEL];
	OBJ_ID tex_compose_buffer_[HDR_LEVEL];

	OBJ_ID rtt_streak_[HDR_STREAK_DIR];
	OBJ_ID tex_streak_[HDR_STREAK_DIR];
	OBJ_ID rtt_streak_tmp_;
	OBJ_ID tex_streak_tmp_;

	/* @brief screen quad */
	shader_screen_quad screen_quad_;


	/* @brief 高亮光晕参数 */
	l3_f32 hori_modulation1st_[16];
	l3_f32 hori_modulation2nd_[16];
	l3_f32 hori_modulation3rd_[16];

	l3_f32 star_modulation1st_[16];
	l3_f32 star_modulation2nd_[16];
	l3_f32 star_modulation3rd_[16];
};

}

#endif
