/**
* @file shader_defer_render_light.h
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
#ifndef __L3ENG_SHADER_DEFER_RENDER_LIGHT_H__
#define __L3ENG_SHADER_DEFER_RENDER_LIGHT_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "light_mgr.h"
#include "shader_screen_quad.h"

namespace l3eng{

class robj_geometry;

class shader_defer_render_light : public shader
{
public:

	typedef ref_ptr_thread<shader_defer_render_light> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_DEFER_RENDER_LIGHT)

public:

	shader_defer_render_light(l3_engine * eng):shader(eng),rtt_light_(base_obj::INVALID_OBJ_ID){}
	virtual ~shader_defer_render_light(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_defer_render_light);
		return n;
	}

public:

	void set_light(const light_mgr::light_info::ptr& l, sence_mgr * sence);
	void un_set_light();
	void render_screen_quad(sence_mgr * sence);

	inline void tex_sence_view_dep(OBJ_ID tex)
	{ this->tex_sence_view_dep_ = tex; }
	inline void tex_sence_view_nml(OBJ_ID tex)
	{ this->tex_sence_view_nml_ = tex; }
	inline void tex_sence_view_clr(OBJ_ID tex)
	{ this->tex_sence_view_clr_ = tex; }

	inline OBJ_ID rtt_light(){ return this->rtt_light_; }
	inline void rtt_light(OBJ_ID rtt){ this->rtt_light_ = rtt; }

private:

	light_mgr::light_info::ptr li_;

	OBJ_ID tex_sence_view_dep_; //�ӿռ����
	OBJ_ID tex_sence_view_nml_; //�ӿռ䷨��
	OBJ_ID tex_sence_view_clr_; //��ɫ

	/* @brief ���render target */
	OBJ_ID rtt_light_;

	l3_int32 uni_tan_half_fov_loc_; //�ӽ�һ���tanֵ uniform float
	l3_int32 uni_dep_range_loc_; //������ uniform vec2
	l3_int32 uni_aspect_loc_; //��Ļ��߱� uniform float

	l3_int32 uni_tex_nml_loc_; //���� uniform sampler2D
	l3_int32 uni_tex_clr_loc_; //��ɫ uniform sampler2D
	l3_int32 uni_tex_view_depth_loc_; //��Ӱ���ռ�������� uniform sampler2D

	l3_int32 uni_light_type_loc_; //������� uniform int
	l3_int32 uni_light_view_spot_dir_loc_; //Spotlight���� uniform vec3
	l3_int32 uni_light_view_pos_loc_; //�ƹ�λ��(w=0ƽ�йⷽ�� w=1���Դλ��) uniform vec4
	l3_int32 uni_light_clr_loc_; // uniform vec3
	l3_int32 uni_light_attenuation_loc_; //������ڴ�ֵ��ʼ˥�� uniform float
	l3_int32 uni_light_max_len_loc_; //������ڴ�ֵ����˥��Ϊ0 uniform float
	l3_int32 uni_light_cutoff_cos_loc_; //�۹��,��׶�������Ե�нǵ�cosֵ uniform float
	l3_int32 uni_light_exponent_loc_; //�۹�����������Ե˥�� uniform float


#if 1
	shader_screen_quad screen_quad_;
#else
	//l3_int32 attr_screen_quad_pos_loc_; //attribute vec2
	//l3_int32 attr_screen_quad_texcoord_loc_; //attribute vec2

	///* @brief screen quad */
	//device_buf_base::ptr pos_buf_;
	//device_buf_base::ptr texcoord_buf_;
	///* @brief screen quad */
	//robj_geometry * geo_screen_quad_;
#endif
};

}

#endif
