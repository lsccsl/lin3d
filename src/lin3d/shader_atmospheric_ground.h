/**
* @file shader_atmospheric_ground.h
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
#ifndef __L3ENG_SHADER_ATMOSPHERIC_GROUND_H__
#define __L3ENG_SHADER_ATMOSPHERIC_GROUND_H__

#include "shader.h"
#include "shader_program_inter.h"

namespace l3eng{

class shader_atmospheric_ground : public shader
{
public:

	typedef ref_ptr_thread<shader_atmospheric_ground> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_ATMOSPHERIC_GROUND)

public:

	shader_atmospheric_ground(l3_engine * eng);
	virtual ~shader_atmospheric_ground(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_atmospheric_ground);
		return n;
	}

public:

	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val)override;
	virtual void set_param_vec3(const l3_int32 param_type, const vector3& param_val)override;

private:

	l3_int32 uni_sea_level_loc_; // uniform float
	l3_int32 uni_rayleigh_scale_height_loc_; // uniform float
	l3_int32 uni_scale_loc_; // uniform float
	l3_int32 uni_vlight_dir_loc_; // uniform vec3
	l3_int32 uni_g_loc_; // uniform float 
	l3_int32 uni_esun_loc_; // uniform float 
	l3_int32 uni_kr_loc_; // uniform float 
	l3_int32 uni_kr4PI_loc_; // uniform float 
	l3_int32 uni_km_loc_; // uniform float 
	l3_int32 uni_km4PI_loc_; // uniform float 
	l3_int32 uni_inv_array_fWavelength4_loc_; // uniform vec3 

	l3_int32 uni_proj_mtx_loc_; //ͶӰ uniform mat4 
	l3_int32 uni_view_mtx_loc_; //�۲� uniform mat4 
	l3_int32 uni_world_mtx_loc_; //����任 uniform mat4 

	l3_int32 uni_cam_pos_loc_; // uniform vec3 


	l3_f32 sea_level_;
	l3_f32 rayleigh_scale_height_;
	l3_f32 scale_;

	vector3 vLightDirection_;

	l3_f32 g_;
	l3_f32 esun_;
	l3_f32 Kr_, Kr4PI_;
	l3_f32 Km_, Km4PI_;
	vector3 inv_array_fWavelength4_;
};

}

#endif
