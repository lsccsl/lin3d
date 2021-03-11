/**
* @file shader_reflect.h
*
* @author lin shao chuan (email:czhama@163.com)
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
#ifndef __L3ENG_SHADER_REFLECT_H__
#define __L3ENG_SHADER_REFLECT_H__

#include "shader.h"
#include "shader_program_inter.h"

namespace l3eng{


class shader_reflect_forward : public shader
{
public:

	typedef ref_ptr_thread<shader_reflect_forward> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_REFLECT_FORWARD)

public:

	shader_reflect_forward(l3_engine * eng):shader(eng)
		,tex_fresnel_(base_obj::INVALID_OBJ_ID)
	{}
	virtual ~shader_reflect_forward(){}

	virtual void init() override;

	virtual void pre_frame(sence_mgr * sence) override;
	virtual void post_frame(sence_mgr * sence) override;
	virtual void prepare(robj_base& robj) override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_reflect_forward);
		return n;
	}

public:

	virtual void set_param(const l3_int32 param_type, const void * param, const l3_uint32 param_sz)override;

private:

	void load_default_fresnel_tex();

private:

	l3_int32 uni_tex1_loc_; //纹理1 uniform sampler2D
	l3_int32 uni_tex2_loc_; //纹理1 uniform sampler2D
	l3_int32 uni_bump_loc_; //纹理bump
	l3_int32 uni_tex_fresnel_loc_;

	l3_int32 uni_proj_mtx_loc_; //投影 uniform mat4 
	l3_int32 uni_view_mtx_loc_; //观察 uniform mat4
	l3_int32 uni_world_mtx_loc_; //世界变换 uniform mat4
	l3_int32 uni_cam_world_pos_loc_;

	l3_int32 uni_wave_offset_loc_; //波的偏移
	l3_int32 uni_wave_scale_loc_;  //波的放大
	l3_int32 uni_refl_distort_loc_; //反射扭曲

	l3_int32 uni_need_refract_loc_; //是否需要折射

	l3_int32 uni_reflect_scale_loc_;
	l3_int32 uni_refract_scale_loc_;
	l3_int32 uni_color_scale_loc_;


	l3_f32 wave_scale_[4];
	l3_f32 wave_offset_[4];
	l3_f32 refl_distort_;

	l3_f32 reflect_scale_;
	l3_f32 refract_scale_;
	l3_f32 color_scale_;

	OBJ_ID tex_fresnel_;

	l3_int32 need_refract_;
};

}

#endif
