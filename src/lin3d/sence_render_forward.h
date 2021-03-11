/**
* @file sence_render_forward.h
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
#ifndef __L3ENG__SENCE_RENDER_FORWARD_H__
#define __L3ENG__SENCE_RENDER_FORWARD_H__

#include "l3_ref_ptr.h"
#include "l3_enum_define.h"
#include "plane.h"

namespace l3eng{

class sence_mgr;
class robj_base;

class sence_render_forward
{
	enum EN_FORWARD_RENDER_MODE
	{
		EN_FORWARD_RENDER_MODE_NORMAL,
		EN_FORWARD_RENDER_MODE_REFLECT,
		EN_FORWARD_RENDER_MODE_REFRACT,
	};

public:

	typedef ref_ptr_thread<sence_render_forward> ptr;

	sence_render_forward(sence_mgr * sence);
	~sence_render_forward(){}

	void init();

	void render_sence(const L3_RENDER_GROUP render_group);

	void reflect_mode(const plane& p)
	{
		this->frender_mode_ = EN_FORWARD_RENDER_MODE_REFLECT;

		this->reflect_plane_ = p;

		l3_f32 rleng;
		this->reflect_plane_.normalise_normal(&rleng);
		l3_f32 d = this->reflect_plane_.dist();
		this->reflect_plane_.dist(d * rleng);
	}

	void refract_mode(const plane& p)
	{
		this->frender_mode_ = EN_FORWARD_RENDER_MODE_REFRACT;

		this->reflect_plane_ = p;

		l3_f32 rleng;
		this->reflect_plane_.normalise_normal(&rleng);
		l3_f32 d = this->reflect_plane_.dist();
		this->reflect_plane_.dist(d * rleng);
	}

	void normal_mode()
	{
		this->frender_mode_ = EN_FORWARD_RENDER_MODE_NORMAL;
	}

protected:

	void _render_obj(robj_base * obj);

	void _render_obj_shader_pass(robj_base * obj);

	void _debug();

protected:

	sence_mgr * sence_;

	/* @brief 是否是镜像模式 */
	EN_FORWARD_RENDER_MODE frender_mode_;
	plane reflect_plane_;

	/* @brief out door main light(sun) */
	struct outdoor_light
	{
	};
	outdoor_light outdoor_light_;
};

}


#endif
