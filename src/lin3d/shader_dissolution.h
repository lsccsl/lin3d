/**
* @file shader_solution.h
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
#ifndef __L3ENG_SHADER_SOLUTION_H__
#define __L3ENG_SHADER_SOLUTION_H__

#include "base_obj.h"
#include "shader.h"
#include "shader_program_inter.h"

namespace l3eng{

class l3_engine;

class shader_dissolution : public shader
{ 
public:

	typedef ref_ptr_thread<shader_dissolution> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_DISSOLUTION)

public:

	shader_dissolution(l3_engine * eng):shader(eng){}
	virtual ~shader_dissolution(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_dissolution);
		return n;
	}

public:

	virtual void set_param_obj(const l3_int32 param_type, const OBJ_ID param_val)override;
	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val)override;

private:

	l3_int32 uni_tex1_loc_; //纹理1 uniform sampler2D
	l3_int32 uni_proj_mtx_loc_; //投影 uniform mat4 
	l3_int32 uni_view_mtx_loc_; //观察 uniform mat4
	l3_int32 uni_world_mtx_loc_; //世界变换 uniform mat4
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2
	l3_int32 uni_rand_tex_loc_; //随机纹理
	l3_int32 uni_dissolution_threshold_loc_; // 溶解阀值
	l3_int32 uni_dissolution_transition_loc_; // 熔解边缘过渡

	OBJ_ID tex_rand_;
	l3_f32 dissolution_threshold_;
	l3_f32 dissolution_transition_;
};

}

#endif
