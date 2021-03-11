/**
* @file shader_shadowmap_cast.h
*
* @author lin shao chuan (email:czhama@163.com)
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
#ifndef __L3ENG_SHADER_SHADOWMAP_CAST_H__
#define __L3ENG_SHADER_SHADOWMAP_CAST_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "light_mgr.h"

namespace l3eng{

class shader_shadowmap_cast : public shader
{
public:

	typedef ref_ptr_thread<shader_shadowmap_cast> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_SHADOWMAP_CAST)

public:

	shader_shadowmap_cast(l3_engine * eng):shader(eng),rtt_light_depth_(base_obj::INVALID_OBJ_ID)
	{}
	virtual ~shader_shadowmap_cast(){}

	virtual void init() override;

	virtual void pre_frame(sence_mgr * sence) override;
	virtual void post_frame(sence_mgr * sence) override;
	virtual void prepare(robj_base& robj) override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_shadowmap_cast);
		return n;
	}

public:

	void set_light_info(light_mgr::light_info::ptr li);
	void unset_light_info();

	inline const OBJ_ID rtt_light_depth()
	{ return this->rtt_light_depth_; }
	inline void rtt_light_depth(const OBJ_ID rtt)
	{ this->rtt_light_depth_ = rtt; }

	inline const l3_f32 offset_x()
	{ return this->offset_x_; }
	inline const l3_f32 offset_y()
	{ return this->offset_y_; }

private:

	void _set_shader_loc();

private:

	/* @brief 分辨率 */
	l3_f32 offset_x_;
	l3_f32 offset_y_;
	/* @brief 输出深度缓存 */
	OBJ_ID rtt_light_depth_;

	/* @brief 当前灯光 */
	light_cam::ptr l_cam_;

	l3_int32 uni_world_mtx_loc_; // uniform mat4
	l3_int32 uni_light_dep_range_loc_; //光源的depth range uniform float
	l3_int32 uni_light_view_mtx_loc_; //光源观察 uniform mat4
	l3_int32 uni_light_proj_mtx_loc_; //光源投影 uniform mat4
};

}

#endif






