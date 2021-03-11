/**
* @file shader_ssr.h
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
#ifndef __L3ENG_SHADER_SSR_H__
#define __L3ENG_SHADER_SSR_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "shader_screen_quad.h"

namespace l3eng{

class shader_ssr : public shader
{
public:

	typedef ref_ptr_thread<shader_ssr> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_SSR)

public:

	shader_ssr(l3_engine * eng);
	virtual ~shader_ssr(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_ssr);
		return n;
	}

public:

	void tex_reflect_src(OBJ_ID tex)
	{ this->tex_reflect_src_ = tex; }	;
	void tex_final(OBJ_ID tex)
	{ this->tex_final_ = tex; }
	void tex_nml(OBJ_ID tex)
	{ this->tex_nml_ = tex; }
	void tex_clr(OBJ_ID tex)
	{ this->tex_clr_ = tex; }
	void tex_dep(OBJ_ID tex)
	{ this->tex_dep_ = tex; }

	inline OBJ_ID rtt_ssr()
	{ return this->rtt_ssr_; }
	inline OBJ_ID tex_ssr()
	{ return this->tex_ssr_; }

	inline void rtt_ssr(OBJ_ID rtt)
	{ this->rtt_ssr_ = rtt; }

	void render_screen_quad(sence_mgr * sence);

	/* @brief 远景反射搜索(天空盒) */
	void set_reflect_far();
	/* @brief 近景反射搜索(天空盒) */
	void set_reflect_near();

private:

	OBJ_ID rtt_ssr_;
	OBJ_ID tex_ssr_;

	OBJ_ID tex_reflect_src_;
	OBJ_ID tex_final_;
	OBJ_ID tex_nml_;
	OBJ_ID tex_clr_;
	OBJ_ID tex_dep_;

	l3_int32 uni_tex_reflect_src_loc_; //反射源 uniform sampler2D 
	l3_int32 uni_tex_final_loc_; //颜色 uniform sampler2D 
	l3_int32 uni_tex_nml_loc_; //法线 uniform sampler2D 
	l3_int32 uni_tex_clr_loc_; //颜色 uniform sampler2D 
	l3_int32 uni_tex_view_depth_loc_; //摄影机空间深度纹理 uniform sampler2D 

	l3_int32 uni_tan_half_fov_loc_; //视角一半的tan值 uniform float 
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2 
	l3_int32 uni_aspect_loc_; //屏幕宽高比 uniform float 

	l3_int32 uni_proj_mtx_loc_; //投影矩阵 uniform mat4 
	l3_int32 uni_screen_x_pix_loc_; //uniform int x方向像素个数,即宽
	l3_int32 uni_screen_y_pix_loc_; //uniform int y方向像素个数,即高

	l3_int32 uni_sample_num_loc_;        //采样次数 uniform int 

	l3_int32 uni_step_max_len_loc_;      //最大采样距离 uniform float 
	l3_int32 uni_first_step_count_loc_;  //大区间采样次数 uniform int 
	l3_int32 uni_second_step_count_loc_; //细分区间采样次数 uniform int 

	l3_int32 uni_trace_start_loc_;     //搜索的起始 uniform float 
	l3_int32 uni_trace_step_loc_;      //搜索步长 uniform float 
	l3_int32 uni_trace_count_loc_;     //搜索次数 uniform int 
	l3_int32 uni_trace_sec_count_loc_; // 每个步长细分搜索次数 uniform int 


	l3_uint32 sample_count_;

	l3_f32 step_max_len_;
	l3_uint32 first_step_count_;
	l3_uint32 second_step_count_;
	
	l3_f32 trace_start_;        //搜索的起始 uniform float 
	l3_f32 trace_step_;         //搜索步长 uniform float 
	l3_uint32 trace_count_;     //搜索次数 uniform int 
	l3_uint32 trace_sec_count_; // 每个步长细分搜索次数 uniform int 


	shader_screen_quad screen_quad_;
};

}

#endif
