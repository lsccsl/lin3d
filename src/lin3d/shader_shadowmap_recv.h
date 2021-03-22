/**
* @file shader_shadowmap_recv.h
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
#ifndef __L3ENG_SHADER_SHADOWMAP_RECV_H__
#define __L3ENG_SHADER_SHADOWMAP_RECV_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "light_mgr.h"
#include "shader_screen_quad.h"

namespace l3eng{

class robj_geometry;

class shader_shadowmap_recv : public shader
{
public:

	typedef ref_ptr_thread<shader_shadowmap_recv> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_SHADOWMAP_RECV)

public:

	shader_shadowmap_recv(l3_engine * eng):shader(eng),
		sample_loop_(7),
		rtt_light_shadow_(base_obj::INVALID_OBJ_ID)
	{}
	virtual ~shader_shadowmap_recv(){}

	virtual void init() override;

	virtual void pre_frame(sence_mgr * sence) override;
	virtual void post_frame(sence_mgr * sence) override;
	virtual void prepare(robj_base& robj) override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_shadowmap_recv);
		return n;
	}

public:

	void render_screen_quad(sence_mgr * sence);
	void set_light_info(light_mgr::light_info::ptr li);

	inline void set_sence_dep_tex(OBJ_ID sence_dep)
	{ this->sence_dep_tex_ = sence_dep; }
	inline void set_sence_clr_tex(OBJ_ID sence_clr)
	{ this->sence_clr_tex_ = sence_clr; }
	inline void set_sence_light_tex(OBJ_ID sence_light)
	{ this->sence_light_tex_ = sence_light; }

	void set_offset_x(l3_f32 x)
	{ this->offset_x_ = x; }
	void set_offset_y(l3_f32 y)
	{ this->offset_y_ = y; }
	void set_rtt_light_depth(OBJ_ID rtt)
	{ this->rtt_light_depth_ = rtt; }

	inline OBJ_ID rtt_light_shadow()
	{ return this->rtt_light_shadow_; }
	inline void rtt_light_shadow(OBJ_ID rtt)
	{ this->rtt_light_shadow_ = rtt; }

	void sample_loop(l3_uint32 loop)
	{ this->sample_loop_ = loop; }

private:

	/* @brief 分辨率 */
	l3_f32 offset_x_;
	l3_f32 offset_y_;
	/* @brief 输入深度缓存 */
	OBJ_ID rtt_light_depth_;
	/* @brief 输出最终光照阴影 */
	OBJ_ID rtt_light_shadow_;

	/* @brief 当前灯光 */
	light_cam::ptr l_cam_;
	OBJ_ID sence_dep_tex_; //场景深度
	OBJ_ID sence_clr_tex_; //场景纹理
	OBJ_ID sence_light_tex_; //场景光照

	l3_uint32 sample_loop_;


	l3_int32 uni_tex_light_loc_; //光照纹理 uniform sampler2D
	l3_int32 uni_tex_clr_loc_; //颜色 uniform sampler2D
	l3_int32 uni_tex_view_depth_loc_; //摄影机空间深度纹理 uniform sampler2D

	l3_int32 uni_tex_light_depth_tex_loc_; //光源空间深度 uniform sampler2D
	l3_int32 uni_tex_light_depth_shadow_loc_; //同一个纹理,深度 uniform sampler2DShadow

	l3_int32 uni_tan_half_fov_loc_; //视角一半的tan值 uniform float
	l3_int32 uni_aspect_loc_; //屏幕宽高比 uniform float
	l3_int32 uni_dep_range_loc_; //最大深度 uniform vec2

	l3_int32 uni_view_inv_mtx_loc_; //观察逆矩阵 uniform mat4

	l3_int32 uni_light_dep_range_loc_; //光源的depth range uniform vec2
	l3_int32 uni_light_view_mtx_loc_; //光源观察 uniform mat4
	l3_int32 uni_light_proj_mtx_loc_; //光源投影 uniform mat4
	l3_int32 uni_light_dep_pix_offset_loc_; //光源深度纹理分辨率 uniform vec2
	l3_int32 uni_loop_loc_; //uniform int


#if 1
	shader_screen_quad screen_quad_;
#else
	//l3_int32 attr_screen_quad_pos_loc_; //attribute vec2
	//l3_int32 attr_screen_quad_texcoord_loc_; //attribute vec2

	///* @brief screen quad */
	//device_buf_base::ptr pos_buf_;
	//device_buf_base::ptr texcoord_buf_;

	///* @brief screen quad */
	//robj_geometry * geo_screen_quad_;
#endif
};

}

#endif
