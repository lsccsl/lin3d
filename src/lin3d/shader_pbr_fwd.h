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
#ifndef __L3ENG_SHADER_PBR_FWD_H__
#define __L3ENG_SHADER_PBR_FWD_H__

#include "base_obj.h"
#include "shader.h"
#include "shader_program_inter.h"
#include "light_mgr.h"

namespace l3eng{

class l3_engine;

class shader_pbr_fwd : public shader
{ 
public:

	typedef ref_ptr_thread<shader_pbr_fwd> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_PBR_FWD)

public:

	shader_pbr_fwd(l3_engine * eng):shader(eng){}
	virtual ~shader_pbr_fwd(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_pbr_forward);
		return n;
	}

public:

	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val);

public:

	void set_light(const light_mgr::light_info::ptr& l, sence_mgr * sence);

private:

	l3_int32 uni_tex1_loc_; //纹理1 uniform sampler2D
	l3_int32 uni_proj_mtx_loc_; //投影 uniform mat4 
	l3_int32 uni_view_mtx_loc_; //观察 uniform mat4
	l3_int32 uni_world_mtx_loc_; //世界变换 uniform mat4
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2

	l3_int32 uni_light_type_loc_; //光的类型 uniform int
	l3_int32 uni_light_view_spot_dir_loc_; //Spotlight方向 uniform vec3
	l3_int32 uni_light_view_pos_loc_; //灯光位置(w=0平行光方向 w=1点光源位置) uniform vec4
	l3_int32 uni_light_clr_loc_; // uniform vec3
	l3_int32 uni_light_attenuation_loc_; //距离大于此值开始衰减 uniform float
	l3_int32 uni_light_max_len_loc_; //距离大于此值光线衰减为0 uniform float
	l3_int32 uni_light_cutoff_inner_cos_loc_; //聚光灯,光锥轴线与边缘夹角的cos值最小(小于此值不衰减)
	l3_int32 uni_light_cutoff_outer_cos_loc_; //聚光灯,光锥轴线与边缘夹角的cos值最大(大于此值衰减为零)
	l3_int32 uni_light_exponent_loc_; //聚光灯由中心向边缘衰减 uniform float

	l3_int32 uni_roughness_loc_; // 粗糙度 0-1之间 1为最粗糙
	l3_int32 uni_metalic_loc_;


	light_mgr::light_info::ptr li_;
	l3_f32 pbr_roughness_;
	l3_f32 pbr_metalic_;
};

}

#endif
