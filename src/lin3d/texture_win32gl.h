/**
* @file texture_win32gl.h
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
#ifndef __L3ENG_TEXTURE_WIN32GL_H__
#define __L3ENG_TEXTURE_WIN32GL_H__

#include "win32gl_glee.h"

#include "texture_base.h"
#include "l3_image.h"

namespace l3eng{

class image;

class texture_win32gl : public texture_base
{

public:

	typedef ref_ptr_thread<texture_win32gl> ptr;

	OBJ_TYPE_DEFINE(texture_base, OBJ_TEXTURE_WIN32GL)

public:

	texture_win32gl();
	virtual ~texture_win32gl();

	virtual l3_uint32 tex_dev_id()override{ return tex_id_; }
	virtual l3_int32 tex_dev_bind(const l3_int32 tex_unit, const l3_bool bind_shadow = L3_FALSE)override;
	virtual void set_wrap(const texture_base::e_tex_wrap)override;
	virtual void set_filter(const texture_base::e_tex_filter)override;
	virtual l3_uint32 width()override{ return this->width_; }
	virtual l3_uint32 height()override{ return this->height_; }
	virtual e_tex_inter_type inter_type()override{ return this->inter_type_; }

public:

	inline l3_int32 create_tex(
		const l3_int32 width,
		const l3_int32 height,
		const texture_base::e_tex_type type = e_tex_type_u8,
		const texture_base::e_tex_format format = e_tex_format_rgb,
		const void * pixels = NULL,
		const texture_base::e_tex_inter_type inter_type = e_tex_inter_type_rgba_u8,
		const texture_base::e_tex_filter filter = e_tex_filter_linear,
		const texture_base::e_tex_wrap wrap = e_tex_wrap_clamp,
		const texture_base::e_tex_compare comp_func = e_tex_compare_none
		)
	{
		return this->_create_tex(
			width,
			height,
			type,
			format,
			pixels,
			inter_type,
			filter,
			wrap,
			comp_func
		);
	}

	inline l3_int32 create_dep_tex(const l3_int32 width,
		const l3_int32 height,
		const l3_bool b32 = L3_FALSE,
		const texture_base::e_tex_compare dep_cmp = e_tex_compare_none
		)
	{
		return _create_tex(
			width,
			height,
			e_tex_type_f32,
			e_tex_format_dep,
			NULL,
			b32 ? e_tex_inter_type_dep_f32 : e_tex_inter_type_dep_f16,
			e_tex_filter_linear,
			e_tex_wrap_clamp_edge,
			dep_cmp
		);
	}

	//l3_int32 create_dep_tex1(
	//	const l3_int32 width,
	//	const l3_int32 height,
	//	const texture_base::e_tex_inter_type inter_type
	//	);

	l3_int32 load_tex(const std::string& file_path);

	l3_int32 load_from_image(const image& img);


	/* @brief 更新纹理 xoffset yoffset 填0  width height为pixels宽高 type format为pixels的数据格式 */
	l3_int32 update_tex(const l3_int32 xoffset, const l3_int32 yoffset,
		const l3_int32 width, const l3_int32 height,
		const e_pixel_format pix_format, const void * pixels)
	{
		texture_base::e_tex_format eformat = texture_win32gl::_get_format(pix_format);

		return _update_tex(xoffset, yoffset,
			width, height,
			e_tex_type_u8, eformat, pixels);
	}

public:

	static texture_base::e_tex_format _get_format(e_pixel_format format);

protected:

	/* @brief 创建颜色纹理
	* @param pixels: 图像像素指针
	* @param width: 图像宽
	* @param height: 图像高
	* @param type: 图像数据类弄 u8/f32
	* @param format: 图像像素格式  rgb/rgba/luminance
	* @param inter_type: 纹理内部像素格式与类型(rgba_u8 rgba_f16 rgba_f32)
	* @param filter: 纹理过滤
	* @param wrap: 边缘处理
	*/
	l3_int32 _create_tex(
		const l3_int32 width,
		const l3_int32 height,
		const texture_base::e_tex_type type,
		const texture_base::e_tex_format format,
		const void * pixels = NULL,
		const texture_base::e_tex_inter_type inter_type = e_tex_inter_type_rgba_u8,
		const texture_base::e_tex_filter filter = e_tex_filter_linear,
		const texture_base::e_tex_wrap wrap = e_tex_wrap_clamp,
		const texture_base::e_tex_compare comp_func = e_tex_compare_none
		);

	/* @brief 更新纹理 xoffset yoffset 填0  width height为pixels宽高 type format为pixels的数据格式 */
	l3_int32 _update_tex(const l3_int32 xoffset, const l3_int32 yoffset,
		const l3_int32 width, const l3_int32 height,
		const texture_base::e_tex_type type, const texture_base::e_tex_format format, const void * pixels);

	static GLenum _get_gl_format(const texture_base::e_tex_format eformat);
	static GLenum _get_gl_type(const texture_base::e_tex_type etype);

protected:

	GLuint tex_id_;
	l3_uint32 width_;
	l3_uint32 height_;
	e_tex_inter_type inter_type_;	

	e_tex_compare tex_cmp_;
};

}

#endif
