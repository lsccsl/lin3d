/**
* @file shader_vol_radial_blur.h
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
#ifndef __L3ENG_SHADER_VOL_RADIAL_BLUR_H__
#define __L3ENG_SHADER_VOL_RADIAL_BLUR_H__


#include "shader.h"
#include "shader_screen_quad.h"
#include "shader_program_inter.h"
#include "light_mgr.h"
#include "l3_type.h"
#include "l3_vector4.h"
#include "shader_screen_quad.h"

namespace l3eng{

class shader_vol_radial_blur : public shader
{
public:

	typedef ref_ptr_thread<shader_vol_radial_blur> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_VOL_RADIAL_BLUR)

public:

	shader_vol_radial_blur(l3_engine * eng):shader(eng){}
	virtual ~shader_vol_radial_blur(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_volume_light_radial_blur);
		return n;
	}

public:

	void render_screen_quad(sence_mgr * sence);

	void vol_rb_tex_src(OBJ_ID tex_obj){
		this->vol_rb_tex_src_ = tex_obj;
	}

	void vol_rb_hl_threshold(l3_f32 f){
		this->vol_rb_hl_threshold_ = f;
	}

private:

	/* @brief screen quad */
	shader_screen_quad screen_quad_;

	l3_int32 uni_vol_radial_blur_stage_loc_;
	l3_int32 uni_tex_src_loc_;
	l3_int32 uni_tex_src1_loc_;
	l3_int32 uni_high_light_threshold_loc_;
	l3_int32 uni_light_pos_loc_;
	l3_int32 uni_offset_loc_;

	OBJ_ID vol_rb_tex_src_;
	l3_f32 vol_rb_hl_threshold_;

	OBJ_ID rtt_vol_rb_hight_ligh_;
	OBJ_ID tex_vol_rb_hight_ligh_;

	OBJ_ID rtt_radial_blur_[2];
	OBJ_ID tex_radial_blur_[2];

	/* @brief 转换到屏幕空间的光源位置(2d) */
	l3_f32 light_pos_x_;
	l3_f32 light_pos_y_;
};

}

#endif
