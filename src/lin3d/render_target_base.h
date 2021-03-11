/**
* @file render_target_base.h
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
#ifndef __L3ENG_RENDER_TARGET_BASE_H__
#define __L3ENG_RENDER_TARGET_BASE_H__

#include "l3_ref_ptr.h"
#include "base_obj.h"
#include "texture_base.h"
#include "l3_color.h"

namespace l3eng{

class l3_engine;

class render_target_base : public base_obj
{
public:

	/* @brief 只提供三种组合,简化处理 */
	enum e_rtt_color_format
	{
		e_rtt_color_format_rgba,    /* 这么调:glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); */
		e_rtt_color_format_rgba_f16,/* 这样调:glTexImage2D(GL_TEXTURE_2D ,0, GL_RGBA16F_ARB, width, height, 0, GL_RGBA, GL_FLOAT,         NULL); */
		e_rtt_color_format_rgba_f32, /*       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT,         NULL); */
		e_rtt_color_format_luminance_f16,
		e_rtt_color_format_luminance_f32,
	};

	enum e_rtt_depth_format
	{
		e_rtt_depth_format_16,
		e_rtt_depth_format_32,
	};

public:

	typedef ref_ptr_thread<render_target_base> ptr;
	OBJ_TYPE_DEFINE(base_obj, OBJ_RENDER_TARGET_BASE)

	render_target_base(l3_engine * eng):eng_(eng),
		width_(800),
		height_(600)
	{}
	virtual ~render_target_base(){}

public:

	virtual l3_int32 enable(l3_bool need_clear = L3_TRUE){ return -1; }
	virtual l3_int32 disable(){ return -1; }
	virtual l3_int32 clear_fbo(){ return -1; }
	virtual l3_uint32 fbo(){ return -1; }

	virtual texture_base::ptr clr_tex_1(){ return texture_base::ptr(); }
	virtual texture_base::ptr clr_tex_2(){ return texture_base::ptr(); }
	virtual texture_base::ptr clr_tex_n(l3_uint32 n){ return texture_base::ptr(); }

	virtual texture_base::ptr depth_tex(){ return texture_base::ptr(); }


	virtual l3_int32 init_empty(const l3_int32 width, const l3_int32 height,
		const color& clear_clr = color(1.0f, 1.0f, 1.0f, 0.0f),
		const l3_f32 clear_dep = 1.0f)
	{ return -1; }

	virtual l3_int32 add_tex_clr(
		//const l3_int32 width,
		//const l3_int32 height,
		const render_target_base::e_rtt_color_format format = render_target_base::e_rtt_color_format_rgba)
	{ return -1; }
	virtual l3_int32 add_tex_clr(texture_base::ptr& tex_ptr)
	{ return -1; }
	virtual l3_int32 add_tex_clr(OBJ_ID tex_id)
	{ return -1; }
	virtual l3_int32 del_tex_clr(texture_base::ptr& tex_ptr)
	{ return -1; }
	virtual l3_int32 del_all_tex_clr()
	{ return -1; }

	virtual l3_int32 set_tex_dep(
		//const l3_int32 width,
		//const l3_int32 height,
		const render_target_base::e_rtt_depth_format depth_format,
		const texture_base::e_tex_compare dep_cmp,
		const l3_f32 clear_dep)
	{ return -1; }
	virtual l3_int32 set_tex_dep(texture_base::ptr& tex_ptr)
	{ return -1; }
	virtual l3_int32 set_tex_dep(OBJ_ID tex_id)
	{ return -1; }
	virtual l3_int32 unset_tex_dep()
	{ return -1; }

public:

	inline l3_uint32 width() const
	{ return this->width_; }
	inline l3_uint32 height() const
	{ return this->height_; }
	inline l3_f32 pix_offset_x() const
	{ return this->pix_offset_x_; }
	inline l3_f32 pix_offset_y() const
	{ return this->pix_offset_y_; }

protected:

	/* @brief 缓站区的宽高 */
    l3_uint32 width_;
	l3_uint32 height_;
	l3_f32 pix_offset_x_;
	l3_f32 pix_offset_y_;

	l3_engine * eng_;
};

}

#endif









