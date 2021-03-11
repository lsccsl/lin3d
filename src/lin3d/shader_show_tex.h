/**
* @file shader_show_tex.h
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
#ifndef __L3ENG_SHADER_SHOW_TEX_H__
#define __L3ENG_SHADER_SHOW_TEX_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "shader_screen_quad.h"

namespace l3eng{

class shader_show_tex : public shader
{
public:

	typedef ref_ptr_thread<shader_show_tex> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_SHOW_TEX)

public:

	shader_show_tex(l3_engine * eng);
	virtual ~shader_show_tex(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_show_tex);
		return n;
	}

public:

	void render_debug_rtt(OBJ_ID tex,
		const l3_f32 x_start = 0.0f, const l3_f32 y_start = 0.0f,
		const l3_f32 x_len = 2.0f, const l3_f32 y_len = 2.0f,
		const l3_f32 dep = 0.0f);

private:

	l3_int32 uni_tex_loc_; // uniform sampler2D
	l3_int32 uni_dep_loc_; //uniform l3_f32 


	shader_screen_quad screen_quad_;
};


}

#endif
