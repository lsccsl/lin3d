/**
* @file render_target_fbo.h
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
#ifndef __L3ENG_RENDER_TARGET_FBO_H__
#define __L3ENG_RENDER_TARGET_FBO_H__

#include <vector>
#include "render_target_base.h"
#include "l3_color.h"
#include "win32gl_glee.h"
#include "texture_mgr.h"

namespace l3eng{

class l3_engine;

class render_target_fbo : public render_target_base
{
public:

	typedef ref_ptr_thread<render_target_fbo> ptr;
	OBJ_TYPE_DEFINE(render_target_base, OBJ_RENDER_TARGET_FBO)

	render_target_fbo(l3_engine * eng):render_target_base(eng),
		fbo_(-1),
		clear_dep_(1.0)
	{}
	virtual ~render_target_fbo(){}

public:

	virtual l3_int32 enable(l3_bool need_clear = L3_TRUE)override;
	virtual l3_int32 disable()override;
	virtual l3_int32 clear_fbo()override;

	virtual l3_uint32 fbo()override
	{ return this->fbo_; }

	virtual texture_base::ptr clr_tex_1()override
	{ assert(this->v_tex_.size() >= 1); return this->v_tex_[0]; }
	virtual texture_base::ptr clr_tex_2()override
	{ assert(this->v_tex_.size() >= 2); return this->v_tex_[1]; }
	virtual texture_base::ptr clr_tex_n(l3_uint32 n)override
	{ assert(n <= this->v_tex_.size() && n >= 1); return this->v_tex_[n - 1]; }

	virtual texture_base::ptr depth_tex()override
	{ return this->depth_tex_; }


	virtual l3_int32 init_empty(const l3_int32 width, const l3_int32 height,
		const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f),
		const l3_f32 clear_dep = 1.0f)override;

	virtual l3_int32 add_tex_clr(
		//const l3_int32 width,
		//const l3_int32 height,
		const render_target_base::e_rtt_color_format format = render_target_base::e_rtt_color_format_rgba)override;
	virtual l3_int32 add_tex_clr(texture_base::ptr& tex_ptr)override;
	virtual l3_int32 add_tex_clr(OBJ_ID tex_id)override;
	virtual l3_int32 del_tex_clr(texture_base::ptr& tex_ptr)override;
	virtual l3_int32 del_all_tex_clr()override;

	virtual l3_int32 set_tex_dep(
		//const l3_int32 width,
		//const l3_int32 height,
		const render_target_base::e_rtt_depth_format depth_format,
		const texture_base::e_tex_compare dep_cmp,
		const l3_f32 clear_dep)override;
	virtual l3_int32 set_tex_dep(texture_base::ptr& tex_ptr)override;
	virtual l3_int32 set_tex_dep(OBJ_ID tex_id)override;
	virtual l3_int32 unset_tex_dep()override;

public:

	//l3_int32 init_with_dep(const l3_int32 width,
	//	const l3_int32 height,
	//	const l3_int32 clr_attach_count = 1,
	//	const render_target_base::e_rtt_color_format format = render_target_base::e_rtt_color_format_rgba,
	//	const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f),
	//	const render_target_base::e_rtt_depth_format depth_format = render_target_base::e_rtt_depth_format_16,
	//	const texture_base::e_tex_compare dep_cmp = texture_base::e_tex_compare_none,
	//	const l3_f32 clear_dep = 1.0f);

	//l3_int32 init_no_dep(const l3_int32 width,
	//	const l3_int32 height,
	//	const l3_int32 clr_attach_count = 1,
	//	const render_target_base::e_rtt_color_format format = render_target_base::e_rtt_color_format_rgba,
	//	const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f));

private:

	l3_int32 _init_clr(const l3_int32 width,
		const l3_int32 height,
		const l3_int32 clr_attach_count,
		const render_target_base::e_rtt_color_format format,
		const color& clear_clr);
	l3_int32 _init_dep(const l3_int32 width,
		const l3_int32 height,
		const render_target_base::e_rtt_depth_format depth_format,
		const texture_base::e_tex_compare dep_cmp,
		const l3_f32 clear_dep);
	l3_int32 _init_dev_fbo(const l3_int32 width,
		const l3_int32 height);

	l3_int32 _enable_mult(l3_bool need_clear);
	l3_int32 _enable_one(l3_bool need_clear);

private:

	/* @brief fbo¾ä±ú */
	l3_uint32 fbo_;
	std::vector<texture_base::ptr> v_tex_;
	texture_base::ptr depth_tex_;

	/* @brief ¼ÇÂ¼ color attachment */
	std::vector<GLenum> v_attachment_;

	color clear_clr_;
	l3_f32 clear_dep_;
};

}

#endif




