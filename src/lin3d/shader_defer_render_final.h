/**
* @file shader_defer_render_final.h
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
#ifndef __L3ENG_SHADER_DEFER_RENDER_FINAL_H__
#define __L3ENG_SHADER_DEFER_RENDER_FINAL_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "shader_screen_quad.h"

namespace l3eng{

class shader_defer_render_final : public shader
{
public:

	typedef ref_ptr_thread<shader_defer_render_final> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_DEFER_RENDER_FINAL)

public:

	shader_defer_render_final(l3_engine * eng):shader(eng)
		,rtt_final_output_(base_obj::INVALID_OBJ_ID)
		,diff_scale_(0.01f)
		,atmospheric_scale_(1.0f)
		,has_ssao_(0), has_light_vol_(0),has_atmospheric_(0)
	{}

	virtual ~shader_defer_render_final(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_defer_render_final);
		return n;
	}

public:

	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val)override;

public:

	void render_screen_quad();

	inline void tex_light_shadow(OBJ_ID tex)
	{ this->tex_light_shadow_ = tex; }
	inline void tex_light_vol(OBJ_ID tex)
	{ this->tex_light_vol_ = tex; }
	inline void tex_sence_clr(OBJ_ID tex)
	{ this->tex_sence_clr_ = tex; }
	inline void tex_view_depth(OBJ_ID tex)
	{ this->tex_view_depth_ = tex; }
	inline void tex_view_proj_depth(OBJ_ID tex)
	{ this->tex_view_proj_depth_ = tex; }
	inline void tex_ssao(OBJ_ID tex)
	{ this->tex_ssao_ = tex; }
	inline void tex_atmospheric(OBJ_ID tex)
	{ this->tex_atmospheric_ = tex; }
	inline void diff_scale(l3_f32 ds)
	{ this->diff_scale_ = ds; }
	inline void atmospheric_scale(l3_f32 as)
	{ this->atmospheric_scale_ = as; }
	inline void has_ssao(l3_int32 i)
	{ this->has_ssao_ = i; }
	inline void has_light_vol(l3_int32 i)
	{ this->has_light_vol_ = i; }
	inline void has_atmospheric(l3_int32 i)
	{ this->has_atmospheric_ = i; }

	inline OBJ_ID rtt_final_output()
	{ return this->rtt_final_output_; }
	inline void rtt_final_output(OBJ_ID rtt)
	{ this->rtt_final_output_ = rtt; }

private:

	OBJ_ID tex_light_shadow_;
	OBJ_ID tex_light_vol_;
	OBJ_ID tex_sence_clr_;
	OBJ_ID tex_view_depth_;
	OBJ_ID tex_view_proj_depth_;
	OBJ_ID tex_ssao_; //uniform sampler2D
	OBJ_ID tex_atmospheric_;
	l3_f32 diff_scale_;
	l3_f32 atmospheric_scale_;
	l3_int32 has_ssao_; //是否开启ssao
	l3_int32 has_light_vol_; //是否开启体积光
	l3_int32 has_atmospheric_; //是否开启大气散射


	OBJ_ID rtt_final_output_;


	l3_int32 uni_tex_light_shadow_loc_; //光照阴影 uniform sampler2D
	l3_int32 uni_tex_light_vol_loc_; // 体积光 uniform sampler2D
	l3_int32 uni_tex_clr_loc_; //颜色 uniform sampler2D
	l3_int32 uni_tex_view_depth_loc_; //摄影机空间线性深度纹理 uniform sampler2D
	l3_int32 uni_tex_view_proj_depth_loc_; //摄影机空间深度纹理 uniform sampler2D 
	l3_int32 uni_tex_ssao_loc_; // ssao uniform sampler2D 
	l3_int32 uni_tex_atmospheric_loc_; // uniform sampler2D 
	l3_int32 uni_diff_scale_loc_;
	l3_int32 uni_atmospheric_scale_loc_;
	l3_int32 uni_has_ssao_loc_; //是否开启ssao
	l3_int32 uni_has_light_vol_loc_; //是否开启体积光
	l3_int32 uni_has_atmospheric_loc_; //是否开启大气散射

	shader_screen_quad screen_quad_;
};

}

#endif
