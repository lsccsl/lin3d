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

	OBJ_ID tex_sence_view_dep_; //视空间深度
	OBJ_ID tex_sence_view_nml_; //视空间法线
	OBJ_ID tex_sence_view_clr_; //颜色

	/* @brief 输出render target */
	OBJ_ID rtt_light_;

	l3_int32 uni_tan_half_fov_loc_; //视角一半的tan值 uniform float
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2
	l3_int32 uni_aspect_loc_; //屏幕宽高比 uniform float

	l3_int32 uni_tex_nml_loc_; //法线 uniform sampler2D
	l3_int32 uni_tex_clr_loc_; //颜色 uniform sampler2D
	l3_int32 uni_tex_view_depth_loc_; //摄影机空间深度纹理 uniform sampler2D

	l3_int32 uni_light_type_loc_; //光的类型 uniform int
	l3_int32 uni_light_view_spot_dir_loc_; //Spotlight方向 uniform vec3
	l3_int32 uni_light_view_pos_loc_; //灯光位置(w=0平行光方向 w=1点光源位置) uniform vec4
	l3_int32 uni_light_clr_loc_; // uniform vec3
	l3_int32 uni_light_attenuation_loc_; //距离大于此值开始衰减 uniform float
	l3_int32 uni_light_max_len_loc_; //距离大于此值光线衰减为0 uniform float
	l3_int32 uni_light_cutoff_cos_loc_; //聚光灯,光锥轴线与边缘夹角的cos值 uniform float
	l3_int32 uni_light_exponent_loc_; //聚光灯由中心向边缘衰减 uniform float


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
