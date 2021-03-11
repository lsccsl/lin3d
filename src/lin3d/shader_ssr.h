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

	/* @brief Զ����������(��պ�) */
	void set_reflect_far();
	/* @brief ������������(��պ�) */
	void set_reflect_near();

private:

	OBJ_ID rtt_ssr_;
	OBJ_ID tex_ssr_;

	OBJ_ID tex_reflect_src_;
	OBJ_ID tex_final_;
	OBJ_ID tex_nml_;
	OBJ_ID tex_clr_;
	OBJ_ID tex_dep_;

	l3_int32 uni_tex_reflect_src_loc_; //����Դ uniform sampler2D 
	l3_int32 uni_tex_final_loc_; //��ɫ uniform sampler2D 
	l3_int32 uni_tex_nml_loc_; //���� uniform sampler2D 
	l3_int32 uni_tex_clr_loc_; //��ɫ uniform sampler2D 
	l3_int32 uni_tex_view_depth_loc_; //��Ӱ���ռ�������� uniform sampler2D 

	l3_int32 uni_tan_half_fov_loc_; //�ӽ�һ���tanֵ uniform float 
	l3_int32 uni_dep_range_loc_; //������ uniform vec2 
	l3_int32 uni_aspect_loc_; //��Ļ��߱� uniform float 

	l3_int32 uni_proj_mtx_loc_; //ͶӰ���� uniform mat4 
	l3_int32 uni_screen_x_pix_loc_; //uniform int x�������ظ���,����
	l3_int32 uni_screen_y_pix_loc_; //uniform int y�������ظ���,����

	l3_int32 uni_sample_num_loc_;        //�������� uniform int 

	l3_int32 uni_step_max_len_loc_;      //���������� uniform float 
	l3_int32 uni_first_step_count_loc_;  //������������� uniform int 
	l3_int32 uni_second_step_count_loc_; //ϸ������������� uniform int 

	l3_int32 uni_trace_start_loc_;     //��������ʼ uniform float 
	l3_int32 uni_trace_step_loc_;      //�������� uniform float 
	l3_int32 uni_trace_count_loc_;     //�������� uniform int 
	l3_int32 uni_trace_sec_count_loc_; // ÿ������ϸ���������� uniform int 


	l3_uint32 sample_count_;

	l3_f32 step_max_len_;
	l3_uint32 first_step_count_;
	l3_uint32 second_step_count_;
	
	l3_f32 trace_start_;        //��������ʼ uniform float 
	l3_f32 trace_step_;         //�������� uniform float 
	l3_uint32 trace_count_;     //�������� uniform int 
	l3_uint32 trace_sec_count_; // ÿ������ϸ���������� uniform int 


	shader_screen_quad screen_quad_;
};

}

#endif
