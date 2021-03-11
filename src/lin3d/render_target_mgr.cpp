/**
* @file render_target_mgr.cpp
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
#include "render_target_mgr.h"
#include "l3_engine.h"
#include "render_target_fbo.h"
#include "base_obj_mgr.h"

namespace l3eng{

l3_int32 render_target_mgr::get_render_target(const OBJ_ID obj_id, render_target_base::ptr& rtt)
{
	std::map<OBJ_ID, render_target_base::ptr>::iterator it = this->map_rrt_.find(obj_id);
	if(this->map_rrt_.end() != it)
	{
		rtt = it->second;
		return 0;
	}

	return -1;
}

OBJ_ID render_target_mgr::get_rtt_tex(const OBJ_ID obj_id, const l3_uint32 idx)
{
	render_target_base::ptr rtt;
	this->get_render_target(obj_id, rtt);
	if(rtt.is_null())
		return base_obj::INVALID_OBJ_ID;

	texture_base::ptr tex_ptr = rtt->clr_tex_n(idx);
	if(tex_ptr.is_null())
		return base_obj::INVALID_OBJ_ID;

	return tex_ptr->obj_id();
}

OBJ_ID render_target_mgr::create_fbo_empty(render_target_base::ptr& rtt,
	const l3_int32 width,
	const l3_int32 height,
	const color& clear_clr,
	const l3_f32 clear_dep)
{
	rtt = this->eng_->obj_mgr()->create_obj<render_target_fbo, l3_engine *>(this->eng_);

	rtt->init_empty(width, height, clear_clr, clear_dep);

	this->map_rrt_[rtt->obj_id()] = rtt;

	return rtt->obj_id();
}

//OBJ_ID render_target_mgr::create_fbo(const l3_int32 width,
//	const l3_int32 height,
//	const l3_int32 clr_attach_count,
//	const render_target_base::e_rtt_color_format format,
//	const color& clear_clr,
//	const render_target_base::e_rtt_depth_format depth_format,
//	const texture_base::e_tex_compare dep_cmp)
//{
//	render_target_fbo::ptr rtt = this->eng_->obj_mgr()->create_obj<render_target_fbo, l3_engine *>(this->eng_);
//
//	rtt->init_with_dep(
//		width,
//		height,
//		clr_attach_count,
//		format,
//		clear_clr,
//		depth_format,
//		dep_cmp);
//
//	this->map_rrt_[rtt->obj_id()] = rtt;
//
//	return rtt->obj_id();
//}

//OBJ_ID render_target_mgr::create_fbo_no_dep(const l3_int32 width,
//	const l3_int32 height,
//	const l3_int32 clr_attach_count,
//	const render_target_base::e_rtt_color_format format,
//	const color& clear_clr)
//{
//	render_target_fbo::ptr rtt = this->eng_->obj_mgr()->create_obj<render_target_fbo, l3_engine *>(this->eng_);
//
//	rtt->init_no_dep(width,
//		height,
//		clr_attach_count,
//		format,
//		clear_clr);
//
//	this->map_rrt_[rtt->obj_id()] = rtt;
//
//	return rtt->obj_id();
//}

}
