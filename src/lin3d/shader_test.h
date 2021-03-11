/**
* @file shader_test.h
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
#ifndef __L3ENG_SHADER_TEST_H__
#define __L3ENG_SHADER_TEST_H__

#include "base_obj.h"
#include "shader.h"
#include "shader_program_inter.h"

namespace l3eng{

class l3_engine;

class shader_test : public shader
{ 
public:

	typedef ref_ptr_thread<shader_test> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_TEST)

public:

	shader_test(l3_engine * eng):shader(eng){}
	virtual ~shader_test(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_test_program);
		return n;
	}

private:

	l3_int32 uni_tex1_loc_; //纹理1 uniform sampler2D
	l3_int32 uni_proj_mtx_loc_; //投影 uniform mat4 
	l3_int32 uni_view_mtx_loc_; //观察 uniform mat4
	l3_int32 uni_world_mtx_loc_; //世界变换 uniform mat4
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2
};

}

#endif
