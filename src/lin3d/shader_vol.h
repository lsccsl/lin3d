/**
* @file shader_vol.h
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
#ifndef __L3ENG_SHADER_VOL_H__
#define __L3ENG_SHADER_VOL_H__

#include "shader.h"
#include "shader_screen_quad.h"
#include "shader_program_inter.h"
#include "light_mgr.h"

namespace l3eng{

class shader_vol : public shader
{
public:

	typedef ref_ptr_thread<shader_vol> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_VOL)

public:

	shader_vol(l3_engine * eng):shader(eng),rtt_vol_(base_obj::INVALID_OBJ_ID){}
	virtual ~shader_vol(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_volume_light);
		return n;
	}

public:

	void render_screen_quad(sence_mgr * sence);
	void set_light_info(light_mgr::light_info::ptr li);

	void rtt_light_depth(OBJ_ID rtt)
	{ this->rtt_light_depth_ = rtt; }
	void tex_view_dep(OBJ_ID tex)
	{ this->tex_view_dep_ = tex; }
	void tex_sence_clr(OBJ_ID tex)
	{ this->tex_sence_clr_ = tex; }

	OBJ_ID rtt_vol()
	{ return this->rtt_vol_; }
	void rtt_vol(OBJ_ID rtt)
	{ this->rtt_vol_ = rtt; }

private:

	/* @brief �����Դ��Ȼ��� */
	OBJ_ID rtt_light_depth_;
	/* @brief �������� */
	OBJ_ID rtt_vol_;
	/* @brief ������� */
	OBJ_ID tex_view_dep_;
	/* @brief ������ɫ */
	OBJ_ID tex_sence_clr_;

	/* @brief ��ǰ�ƹ� */
	light_mgr::light_info::ptr li_;


	l3_int32 uni_tex_view_depth_loc_; //��Ӱ���ռ�������� uniform sampler2D

	l3_int32 uni_tex_light_depth_tex_loc_; //��Դ�ռ���� uniform sampler2D
	l3_int32 uni_tex_light_depth_shadow_loc_; //ͬһ������,��� uniform sampler2DShadow
	l3_int32 uni_tex_clr_loc_; //��ɫ uniform sampler2D
	l3_int32 uni_light_world_spot_dir_loc_; //Spotlight���� uniform vec3
	l3_int32 uni_light_cutoff_cos_loc_; //�۹��,��׶�������Ե�нǵ�cosֵ uniform float
	l3_int32 uni_light_clr_loc_; //�ƹ���ɫ
	l3_int32 uni_light_type_loc_; //������� uniform int

	l3_int32 uni_tan_half_fov_loc_; //�ӽ�һ���tanֵ uniform float
	l3_int32 uni_aspect_loc_; //��Ļ��߱� uniform float
	l3_int32 uni_dep_range_loc_; //������ uniform vec2

	l3_int32 uni_view_inv_mtx_loc_; //�۲������ uniform mat4

	l3_int32 uni_light_dep_range_loc_; //��Դ��depth range uniform vec2
	l3_int32 uni_light_view_mtx_loc_; //��Դ�۲� uniform mat4
	l3_int32 uni_light_proj_mtx_loc_; //��ԴͶӰ uniform mat4

	l3_int32 uni_eye_world_pos_loc_; //uniform vec3 
	l3_int32 uni_light_world_pos_loc_; //uniform vec3 


	shader_screen_quad screen_quad_;
};

}

#endif
