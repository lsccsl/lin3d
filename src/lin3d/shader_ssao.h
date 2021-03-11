/**
* @file shader_ssao.h
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
#ifndef __L3ENG_SHADER_SSAO_H__
#define __L3ENG_SHADER_SSAO_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "shader_screen_quad.h"

namespace l3eng{

class shader_ssao : public shader
{
public:

	typedef ref_ptr_thread<shader_ssao> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_SSAO)

public:

	shader_ssao(l3_engine * eng):shader(eng),rtt_ssao_(base_obj::INVALID_OBJ_ID){}
	virtual ~shader_ssao(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_ssao);
		return n;
	}

public:

	void render_screen_quad(sence_mgr * sence);

	inline void tex_view_depth(OBJ_ID tex)
	{ this->tex_view_depth_ = tex; }
	inline void tex_view_nml(OBJ_ID tex)
	{ this->tex_view_nml_ = tex; }

	inline OBJ_ID rtt_ssao()
	{ return this->rtt_ssao_; }
	inline void rtt_ssao(OBJ_ID rtt)
	{ this->rtt_ssao_ = rtt; }

	inline void pix_offset_x(l3_f32 x)
	{ this->pix_offset_x_ = x; }
	inline void pix_offset_y(l3_f32 y)
	{ this->pix_offset_y_ = y; }

private:

	OBJ_ID rtt_ssao_;
	OBJ_ID tex_view_depth_;
	OBJ_ID tex_view_nml_;
	l3_f32 pix_offset_x_;
	l3_f32 pix_offset_y_;

	l3_int32 uni_tex_view_depth_loc_; //视空间深度纹理 uniform sampler2D
	l3_int32 uni_tex_nml_loc_; //视空间法线 uniform sampler2D

	l3_int32 uni_tex_dep_pix_offset_loc_; //视深度纹理分辨率 uniform vec2
	l3_int32 uni_tan_half_fov_loc_; //视角一半的tan值 uniform float
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2
	l3_int32 uni_aspect_loc_; //屏幕宽高比 uniform float


	shader_screen_quad screen_quad_;
};

}

#endif
