/**
* @file shader_defer_render_light_vol_mix.h
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
#ifndef __L3ENG_SHADER_DEFER_RENDER_LIGHT_VOL_MIX_H__
#define __L3ENG_SHADER_DEFER_RENDER_LIGHT_VOL_MIX_H__

#include "shader.h"
#include "shader_screen_quad.h"
#include "shader_program_inter.h"

namespace l3eng{

class shader_defer_render_light_vol_mix : public shader
{
public:

	typedef ref_ptr_thread<shader_defer_render_light_vol_mix> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_DEFER_RENDER_LIGHT_VOL_MIX)

public:

	shader_defer_render_light_vol_mix(l3_engine * eng):shader(eng),idx_rtt_(0){}
	virtual ~shader_defer_render_light_vol_mix(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_defer_render_light_vol_mix);
		return n;
	}

public:

	void render_screen_quad();

	OBJ_ID current_tex_light_vol_mix()
	{ return this->tex_light_vol_mix_[idx_rtt_]; }

	void tex_vol(OBJ_ID tex)
	{ this->tex_vol_ = tex; }

	void clear_fbo();

	inline void rtt_light_vol_mix1(OBJ_ID rtt)
	{ this->rtt_light_vol_mix_[0] = rtt; }
	inline void rtt_light_vol_mix2(OBJ_ID rtt)
	{ this->rtt_light_vol_mix_[1] = rtt; }

	inline void tex_light_vol_mix1(OBJ_ID tex)
	{ this->tex_light_vol_mix_[0] = tex; }
	inline void tex_light_vol_mix2(OBJ_ID tex)
	{ this->tex_light_vol_mix_[1] = tex; }

	inline l3_uint32 idx_rtt()const
	{ return this->idx_rtt_; }	

private:

	l3_int32 uni_tex_light_vol_loc_; //体积光 uniform sampler2D
	l3_int32 uni_tex_light_mix_loc_; //颜色 uniform sampler2D

	/* @brief 输入 光照阴影纹理 */
	OBJ_ID tex_vol_;

	/* @brief 输出 光照阴影混合 */
	OBJ_ID rtt_light_vol_mix_[2];
	OBJ_ID tex_light_vol_mix_[2];
	l3_uint32 idx_rtt_;

	shader_screen_quad screen_quad_;
};

}

#endif
