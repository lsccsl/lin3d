/**
* @file render_target_mgr.h
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
#ifndef __L3ENG_RENDER_TARGET_MGR_H__
#define __L3ENG_RENDER_TARGET_MGR_H__

#include <string>
#include <map>
#include "l3_type.h"
#include "render_target_base.h"
#include "l3_color.h"

namespace l3eng{

class l3_engine;

class render_target_mgr
{
public:

	render_target_mgr(l3_engine * eng):eng_(eng)
	{}
	
	~render_target_mgr(){}

	l3_int32 get_render_target(const OBJ_ID obj_id, render_target_base::ptr& rtt);
	OBJ_ID get_rtt_tex(const OBJ_ID obj_id, const l3_uint32 idx);

	OBJ_ID create_fbo_empty(render_target_base::ptr& rtt,
		const l3_int32 width,
		const l3_int32 height,
		const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f),
		const l3_f32 clear_dep = 1.0f);

	//OBJ_ID create_fbo(const l3_int32 width,
	//	const l3_int32 height,
	//	const l3_int32 clr_attach_count = 1,
	//	const render_target_base::e_rtt_color_format format = render_target_base::e_rtt_color_format_rgba,
	//	const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f),
	//	const render_target_base::e_rtt_depth_format depth_format = render_target_base::e_rtt_depth_format_16,
	//	const texture_base::e_tex_compare dep_cmp = texture_base::e_tex_compare_none);

	//OBJ_ID create_fbo_no_dep(const l3_int32 width,
	//	const l3_int32 height,
	//	const l3_int32 clr_attach_count = 1,
	//	const render_target_base::e_rtt_color_format format = render_target_base::e_rtt_color_format_rgba,
	//	const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f));

private:

	std::map<OBJ_ID, render_target_base::ptr> map_rrt_;

	l3_engine * eng_;
};

}

#endif
