/**
* @file texture.h
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
#ifndef __L3ENG_TEXTURE_BASE_H__
#define __L3ENG_TEXTURE_BASE_H__

#include "base_obj.h"
#include "l3_ref_ptr.h"

namespace l3eng{

class texture_base : public base_obj
{
public:

	typedef ref_ptr_thread<texture_base> ptr;

	OBJ_TYPE_DEFINE(base_obj, OBJ_TEXTURE_BASE)

public:

	/* @brief 纹理颜色类型(gl内部数据格式) */
	enum e_tex_inter_type
	{
		e_tex_inter_type_none = 0,
		e_tex_inter_type_rgba_u8,    /* 这么调:glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,             width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); */
		e_tex_inter_type_rgba_f16,   /* 这样调:glTexImage2D(GL_TEXTURE_2D ,0, GL_RGBA16F_ARB,      width, height, 0, GL_RGBA, GL_FLOAT,         NULL); */
		e_tex_inter_type_rgba_f32,   /*        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB,      width, height, 0, GL_RGBA, GL_FLOAT,         NULL); */
		e_tex_inter_type_luminance_16, /*      glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE16F_ARB, width, height, 0, GL_RGBA, GL_FLOAT,         NULL); */		
		e_tex_inter_type_luminance_32, /*      glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT,         NULL); */		
		e_tex_inter_type_dep_f16, /* 16bit深度 */
		e_tex_inter_type_dep_f32, /* 32bit深度 */
	};

	/* @brief (传进来的纹理位置参数) */
	enum e_tex_type
	{
		e_tex_type_u8,
		e_tex_type_f32,
	};

	/* @brief 纹理颜色格式(传进来的纹理位置参数) */
	enum e_tex_format
	{
		e_tex_format_rgb,
		e_tex_format_rgba,      //
		e_tex_format_luminance, //灰度
		e_tex_format_dep,       //深度
	};

	/* @brief 纹理过滤 */
	enum e_tex_filter
	{
		e_tex_filter_linear, /* GL_LINEAR */
		e_tex_filter_nearest,/* GL_NEAREST */
	};

	/* @brief 纹理边界处理 */
	enum e_tex_wrap
	{
		e_tex_wrap_clamp,
		e_tex_wrap_clamp_edge,
		e_tex_wrap_repeate,
	};

	/* @brief  */
	enum e_tex_compare
	{
		e_tex_compare_none,
		e_tex_compare_less,
		e_tex_compare_l_equal,
		e_tex_compare_equal,
		e_tex_compare_greater,
		e_tex_compare_no_equal,
		e_tex_compare_g_equal,
		e_tex_compare_always,
	};

public:

	texture_base(){}
	virtual ~texture_base(){}

	virtual l3_uint32 tex_dev_id() = 0;
	virtual l3_int32 tex_dev_bind(const l3_int32 tex_unit, const l3_bool bind_shadow = L3_FALSE) = 0;
	virtual void set_wrap(const texture_base::e_tex_wrap){}
	virtual void set_filter(const texture_base::e_tex_filter){}
	virtual l3_uint32 width(){ return 1; }
	virtual l3_uint32 height(){ return 1; }
	virtual e_tex_inter_type inter_type(){ return e_tex_inter_type_none; }
};

}

#endif
