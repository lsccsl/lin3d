/**
* @file shader_defer_render_atmospheric.h
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
#ifndef __L3ENG_SHADER_DEFER_RENDER_ATMOSPHERIC_H__
#define __L3ENG_SHADER_DEFER_RENDER_ATMOSPHERIC_H__

#include "shader.h"
#include "shader_screen_quad.h"
#include "shader_program_inter.h"

namespace l3eng{

class shader_defer_render_atmospheric : public shader
{
public:

	typedef ref_ptr_thread<shader_defer_render_atmospheric> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_DEFER_ATMOSPHERIC)

public:

	shader_defer_render_atmospheric(l3_engine * eng);
	virtual ~shader_defer_render_atmospheric(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_defer_render_atmospheric);
		return n;
	}

public:

	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val)override;
	virtual void set_param_vec3(const l3_int32 param_type, const vector3& param_val)override;

public:

	inline void tex_view_depth(OBJ_ID tex)
	{ this->tex_view_depth_ = tex; }
	inline void tex_sence_clr(OBJ_ID tex)
	{ this->tex_sence_clr_ = tex; }

	inline OBJ_ID rtt_out_put()
	{ return this->rtt_out_put_; }
	inline void rtt_out_put(OBJ_ID rtt)
	{ this->rtt_out_put_ = rtt; }

	void render_screen_quad();

	void clear_fbo();

private:

	/* @brief 场景深度与颜色 */
	OBJ_ID tex_view_depth_;
	OBJ_ID tex_sence_clr_;
	/* @brief 输出缓冲 */
	OBJ_ID rtt_out_put_;


	l3_int32 uni_sea_level_loc_; // uniform float
	l3_int32 uni_rayleigh_scale_height_loc_; // uniform float
	l3_int32 uni_scale_loc_; // uniform float
	l3_int32 uni_vlight_dir_loc_; // uniform vec3
	l3_int32 uni_g_loc_; // uniform float 
	l3_int32 uni_g_sky_loc_; //uniform float
	l3_int32 uni_esun_loc_; // uniform float 
	l3_int32 uni_kr_loc_; // uniform float 
	l3_int32 uni_kr4PI_loc_; // uniform float 
	l3_int32 uni_km_loc_; // uniform float 
	l3_int32 uni_km4PI_loc_; // uniform float 
	l3_int32 uni_inv_array_fWavelength4_loc_; // uniform vec3 
	l3_int32 uni_skydome_radius_loc_; //skydome的半径 uniform float 
	l3_int32 uni_cam_pos_loc_; // uniform vec3 
	l3_int32 uni_attenuation_final_loc_; //最终计算结果衰减 uniform float 

	l3_int32 uni_tex_view_depth_loc_; //摄影机空间深度纹理 uniform sampler2D 
	l3_int32 uni_tex_clr_loc_; //颜色 uniform sampler2D

	l3_int32 uni_tan_half_fov_loc_; //视角一半的tan值 uniform float 
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2 
	l3_int32 uni_aspect_loc_; //屏幕宽高比 uniform float 
	l3_int32 uni_view_inv_mtx_loc_; //观察逆矩阵 uniform mat4 



	l3_f32 sea_level_;
	l3_f32 rayleigh_scale_height_;
	l3_f32 scale_;

	vector3 vLightDirection_;

	l3_f32 g_, g_sky_;
	l3_f32 esun_;
	l3_f32 Kr_, Kr4PI_;
	l3_f32 Km_, Km4PI_;
	vector3 inv_array_fWavelength4_;
	l3_f32 skydome_radius_;
	l3_f32 attenuation_final_;


	/* @brief screen quad */
	shader_screen_quad screen_quad_;
};

}

#endif
