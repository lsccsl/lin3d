/**
* @file shader_vol_lightdir_extrude.h
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
#ifndef __L3ENG_SHADER_VOL_LIGHTDIR_EXTRUDE_H__
#define __L3ENG_SHADER_VOL_LIGHTDIR_EXTRUDE_H__


#include "shader.h"
#include "shader_screen_quad.h"
#include "shader_program_inter.h"
#include "light_mgr.h"
#include "l3_type.h"
#include "l3_vector4.h"

namespace l3eng{

class shader_vol_lightdir_extrude : public shader
{
public:

	typedef ref_ptr_thread<shader_vol_lightdir_extrude> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_VOL_LIGHTDIR_EXTRUDE)

public:

	shader_vol_lightdir_extrude(l3_engine * eng):shader(eng){}
	virtual ~shader_vol_lightdir_extrude(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_volume_lightdir_extrude);
		return n;
	}

public:

	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val)override;
	virtual void set_param_vec3(const l3_int32 param_type, const vector3& param_val)override;
	virtual void set_param_bool(const l3_int32 param_type, const l3_bool b)override;

public:

	void set_light_dir(const vector3 light_pos_dir, const l3_bool is_dir);
	void set_vol_max(const l3_f32 vm);

private:

	l3_int32 uni_light_dir_loc_;
	l3_int32 uni_vol_max_loc_;
	l3_int32 uni_dis_from_light_loc_;

	l3_int32 uni_world_mtx_loc_; //世界变换
	l3_int32 uni_view_mtx_loc_; //观察
	l3_int32 uni_proj_mtx_loc_; //投影矩阵


	vector4 v_light_pos_dir_;
	l3_f32 vol_max_;
	l3_f32 dis_from_light_;
	l3_bool is_dir_light_;
};

}

#endif
