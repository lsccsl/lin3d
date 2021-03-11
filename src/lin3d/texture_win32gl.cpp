/**
* @file texture_win32gl.cpp
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
#include "texture_win32gl.h"
#include "image.h"

namespace l3eng{

texture_win32gl::texture_win32gl():tex_id_(-1),
	tex_cmp_(e_tex_compare_none),
	width_(1),
	height_(1)
{
}
texture_win32gl::~texture_win32gl()
{
	glDeleteTextures(1, &this->tex_id_);
}

l3_int32 texture_win32gl::tex_dev_bind(const l3_int32 tex_unit, const l3_bool bind_shadow)
{
	l3_int32 dev_tex_unit = GL_TEXTURE0_ARB + tex_unit;

	glActiveTexture(dev_tex_unit);
	glClientActiveTexture(dev_tex_unit);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, this->tex_id_);

	if(bind_shadow)
	{
		switch(this->tex_cmp_)
		{
		case e_tex_compare_none:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);
			break;

		case e_tex_compare_less:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LESS);
			break;

		case e_tex_compare_l_equal:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
			break;

		case e_tex_compare_equal:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_EQUAL);
			break;

		case e_tex_compare_greater:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GREATER);
			break;

		case e_tex_compare_no_equal:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_NOTEQUAL);
			break;

		case e_tex_compare_g_equal:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_GEQUAL);
			break;

		case e_tex_compare_always:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_ALWAYS);
			break;
		}
	}

	return 0;
}

l3_int32 texture_win32gl::_create_tex(
	const l3_int32 width,
	const l3_int32 height,
	const e_tex_type type,
	const e_tex_format format,
	const void * pixels,
	const e_tex_inter_type inter_type,
	const e_tex_filter filter,
	const e_tex_wrap wrap,
	const e_tex_compare comp_func
	)
{
	//todo:option 记录所有的参数,重新设置时比较,节省冗余opengl调用
	this->width_ = width;
	this->height_ = height;
	this->inter_type_ = inter_type;

	l3_int32 ret = 0;

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &(this->tex_id_));
	glBindTexture(GL_TEXTURE_2D, this->tex_id_);

	switch(filter)
	{
	case e_tex_filter_linear:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;

	case e_tex_filter_nearest:
	default:
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	}

	switch(wrap)
	{
	case e_tex_wrap_clamp:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;

	case e_tex_wrap_clamp_edge:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;

	case e_tex_wrap_repeate:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	}

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );//todo config by param

	GLenum gl_format = GL_RGBA;
	GLenum gl_type = GL_UNSIGNED_BYTE;
	GLenum gl_internalformat = GL_RGBA;

#if 1
	gl_format = texture_win32gl::_get_gl_format(format);
	if(-1 == gl_format)	{
		ret = -1;
		goto end_;
	}
#else
	//switch(format)
	//{
	//case e_tex_format_rgb:
	//	gl_format = GL_RGB;
	//	break;

	//case e_tex_format_rgba:
	//	gl_format = GL_RGBA;
	//	break;

	//case e_tex_format_luminance:
	//	gl_format = GL_LUMINANCE;
	//	break;

	//case e_tex_format_dep:
	//	gl_format = GL_DEPTH_COMPONENT;
	//	break;

	//default:
	//	{
	//		ret = -1;
	//		goto end_;
	//	}
	//}
#endif

#if 1
	gl_type = texture_win32gl::_get_gl_type(type);
	if(-1 == gl_type) {
		ret = -1;
		goto end_;
	}
#else
	//switch(type)
	//{
	//case e_tex_type_u8:
	//	gl_type = GL_UNSIGNED_BYTE;
	//	break;
	//case e_tex_type_f32:
	//default:
	//	gl_type = GL_FLOAT;
	//	break;
	//}
#endif

	switch(this->inter_type_)
	{
	case e_tex_inter_type_rgba_u8:
		gl_internalformat = GL_RGBA;
		break;

	case e_tex_inter_type_rgba_f16:
		gl_internalformat = GL_RGBA16F_ARB;
		break;

	case e_tex_inter_type_rgba_f32:
		gl_internalformat = GL_RGBA32F_ARB;
		break;

	case e_tex_inter_type_luminance_16:
		gl_internalformat = GL_LUMINANCE16F_ARB;
		break;

	case e_tex_inter_type_luminance_32:
		gl_internalformat = GL_LUMINANCE32F_ARB;
		break;

	case e_tex_inter_type_dep_f16:
		gl_internalformat = GL_DEPTH_COMPONENT16;
		break;

	case e_tex_inter_type_dep_f32:
		gl_internalformat = GL_DEPTH_COMPONENT32;
		break;

	default:
		{
			ret = -1;
			goto end_;
		}
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, gl_internalformat, width, height, 0, gl_format, gl_type, pixels);
	//switch(type)
	//{
	//case e_tex_clr_type_f32:
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//	break;			

	//case e_tex_clr_type_f16:
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	//	break;

	//case e_tex_clr_type_u8:
	//default:
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//	break;
	//}

	this->tex_cmp_ = comp_func;
	//GLint gl_func = -1;
	//switch(this->tex_cmp_)
	//{
	//case e_tex_compare_none:
	//	break;

	//case e_tex_compare_less:
	//	gl_func = GL_LESS;
	//	break;

	//case e_tex_compare_l_equal:
	//	gl_func = GL_LEQUAL;
	//	break;

	//case e_tex_compare_equal:
	//	gl_func = GL_EQUAL;
	//	break;

	//case e_tex_compare_greater:
	//	gl_func = GL_GREATER;
	//	break;

	//case e_tex_compare_no_equal:
	//	gl_func = GL_NOTEQUAL;
	//	break;

	//case e_tex_compare_g_equal:
	//	gl_func = GL_GEQUAL;
	//	break;

	//case e_tex_compare_always:
	//	gl_func = GL_ALWAYS;
	//	break;
	//}

	//if(gl_func > 0)
	//{
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);  
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, /*gl_func*/GL_LEQUAL);
	//}

end_:

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	return ret;
}

GLenum texture_win32gl::_get_gl_format(const texture_base::e_tex_format eformat)
{
	switch(eformat)
	{
	case e_tex_format_rgb:
		return GL_RGB;
		break;

	case e_tex_format_rgba:
		return GL_RGBA;
		break;

	case e_tex_format_luminance:
		return GL_LUMINANCE;
		break;

	case e_tex_format_dep:
		return GL_DEPTH_COMPONENT;
		break;

	default:
		return -1;
		break;
	}
}

GLenum texture_win32gl::_get_gl_type(const texture_base::e_tex_type etype)
{
	switch(etype)
	{
	case e_tex_type_u8:
		return GL_UNSIGNED_BYTE;
		break;

	case e_tex_type_f32:
		return GL_FLOAT;

	default:
		return -1;
		break;
	}
}

//l3_int32 texture_win32gl::create_dep_tex1(
//	const l3_int32 width,
//	const l3_int32 height,
//	const e_tex_inter_type inter_type
//	)
//{
//	this->width_ = width;
//	this->height_ = height;
//
//	glEnable(GL_TEXTURE_2D);
//	glGenTextures(1, &this->tex_id_);
//	glBindTexture(GL_TEXTURE_2D, this->tex_id_);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	switch(inter_type)
//	{
//	case e_tex_type_dep_f32:
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//		break;
//
//	case e_tex_type_dep_f16:
//	default:
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//		break;
//	}
//	glBindTexture(GL_TEXTURE_2D,0);
//	glDisable(GL_TEXTURE_2D);
//
//	return 0;
//}

texture_base::e_tex_format texture_win32gl::_get_format(e_pixel_format format)
{
	switch(format)
	{
	case E_PIXEL_LUMINANCE_8BIT:
		return e_tex_format_luminance;
		break;

	case E_PIXEL_RGB:
		return e_tex_format_rgb;
		break;

	case E_PIXEL_RGBA:
	default:
		return e_tex_format_rgba;
		break;
	}
}

l3_int32 texture_win32gl::load_tex(const std::string& file_path)
{
	image img;

	if(0 != img.load(file_path))
		return -1;

	const std::vector<l3_uint8>& data = img.data();
	if(data.empty())
		return -1;

	e_tex_format format = texture_win32gl::_get_format(img.pixel_format());

	return _create_tex(
		img.width(),
		img.height(),
		e_tex_type_u8,
		format,
		&data[0],
		e_tex_inter_type_rgba_u8
		);
}

l3_int32 texture_win32gl::load_from_image(const image& img)
{
	const std::vector<l3_uint8>& data = img.data();

	if(data.empty())
		return -1;

	e_tex_format format = texture_win32gl::_get_format(img.pixel_format());

	return _create_tex(
		img.width(),
		img.height(),
		e_tex_type_u8,
		format,
		&data[0],
		e_tex_inter_type_rgba_u8
		);
}

l3_int32 texture_win32gl::_update_tex(const l3_int32 xoffset, const l3_int32 yoffset,
	const l3_int32 width, const l3_int32 height,
	const texture_base::e_tex_type type, const texture_base::e_tex_format format, const void * pixels)
{
	if(!pixels)
		return -1;

	l3_int32 ret = -1;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->tex_id_);
	
	GLenum gl_format = texture_win32gl::_get_gl_format(format);
	if(-1 == gl_format)	{
		ret = -1;
		goto end_;
	}

	GLenum gl_type = texture_win32gl::_get_gl_type(type);
	if(-1 == gl_type)	{
		ret = -1;
		goto end_;
	}

	glTexSubImage2D (GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, gl_format, gl_type, pixels);

end_:
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	return ret;
}

void texture_win32gl::set_wrap(const texture_base::e_tex_wrap wrap)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->tex_id_);

	switch(wrap)
	{
	case e_tex_wrap_clamp:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		break;

	case e_tex_wrap_clamp_edge:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;

	case e_tex_wrap_repeate:
	default:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void texture_win32gl::set_filter(const texture_base::e_tex_filter filter)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, this->tex_id_);

	switch(filter)
	{
	case e_tex_filter_linear:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		break;

	case e_tex_filter_nearest:
	default:
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

}








