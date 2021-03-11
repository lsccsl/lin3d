/**
* @file win32gl_texturemgr.cpp
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
#include "texture_mgr.h"
#include "l3_engine.h"
#include "texture_win32gl.h"
#include "base_obj_mgr.h"
#include "config.h"
#include "l3_log.h"

namespace l3eng{


OBJ_ID texture_mgr::create_tex(texture_base::ptr& tex,
	const l3_int32 width,
	const l3_int32 height,
	const texture_base::e_tex_type type,
	const texture_base::e_tex_format format,
	const void * pixels,
	const texture_base::e_tex_inter_type inter_type,
	const texture_base::e_tex_filter filter,
	const texture_base::e_tex_wrap wrap,
	const texture_base::e_tex_compare comp_func
	)
{
	texture_win32gl::ptr win_tex = this->eng_->obj_mgr()->create_obj<texture_win32gl>();

	win_tex->create_tex(width,
		height,
		type,
		format,
		pixels,
		inter_type,
		filter,
		wrap,
		comp_func);

	tex = win_tex;
	this->map_tex_[tex->obj_id()] = tex;


	tex_abstract tex_abs;
	tex_abs.width_ = win_tex->width();
	tex_abs.height_ = win_tex->height();
	tex_abs.inter_type_ = win_tex->inter_type();

	this->map_tex_abs_[tex_abs].push_back(tex->obj_id());


	return tex->obj_id();
}

OBJ_ID texture_mgr::create_dep_tex(texture_base::ptr& tex, const l3_int32 width,
	const l3_int32 height,
	const l3_bool b32,
	const texture_base::e_tex_compare dep_cmp
	)
{
	texture_win32gl::ptr win_tex = this->eng_->obj_mgr()->create_obj<texture_win32gl>();

	win_tex->create_dep_tex(width,
		height,
		b32,
		dep_cmp);

	tex = win_tex;
	this->map_tex_[tex->obj_id()] = tex;


	tex_abstract tex_abs;
	tex_abs.width_ = win_tex->width();
	tex_abs.height_ = win_tex->height();
	tex_abs.inter_type_ = win_tex->inter_type();

	this->map_dep_tex_abs_[tex_abs].push_back(tex->obj_id());


	return tex->obj_id();
}

OBJ_ID texture_mgr::load_tex(const std::string& file_path, texture_base::ptr& tex)
{
	std::string file_path_full;
	l3_int32 ret = this->eng_->cfg()->find_file_full_path(file_path, file_path_full);
	if(ret != 0)
	{
		L3_LOG_ENG_DEBUG(("fail load tex %s", file_path.c_str()));
		return base_obj::INVALID_OBJ_ID;
	}

	MAP_PATH_TEX_OBJ::iterator it_path = this->map_path_.find(file_path_full);
	if(it_path != this->map_path_.end())
	{
		OBJ_ID tex_obj_id = it_path->second;
		MAP_TEXTRUE::iterator it_tex = this->map_tex_.find(tex_obj_id);
		if(this->map_tex_.end() == it_tex)
			return base_obj::INVALID_OBJ_ID;

		tex = it_tex->second;

		return tex_obj_id;
	}
	else
	{
		texture_win32gl::ptr win_tex = this->eng_->obj_mgr()->create_obj<texture_win32gl>();

		win_tex->load_tex(file_path_full);

		tex = win_tex;
		this->map_tex_[tex->obj_id()] = tex;

		this->map_path_[file_path_full] = tex->obj_id();

		return tex->obj_id();
	}
}

OBJ_ID texture_mgr::load_from_image(const image& img, texture_base::ptr& tex)
{
	texture_win32gl::ptr win_tex = this->eng_->obj_mgr()->create_obj<texture_win32gl>();

	win_tex->load_from_image(img);

	tex = win_tex;
	this->map_tex_[tex->obj_id()] = tex;

	return tex->obj_id();
}

OBJ_ID texture_mgr::find_tex(OBJ_ID obj_id, texture_base::ptr& tex)
{
	MAP_TEXTRUE::iterator it = this->map_tex_.find(obj_id);
	if(it == this->map_tex_.end())
		return base_obj::INVALID_OBJ_ID;

	tex = it->second;

	return tex->obj_id();
}

l3_int32 texture_mgr::update_tex(OBJ_ID obj_id,
	const l3_int32 xoffset, const l3_int32 yoffset,
	const l3_int32 width, const l3_int32 height,
	const e_pixel_format pix_format, const void * pixels)
{
	texture_base::ptr tex;
	find_tex(obj_id, tex);
	if(tex.is_null())
		return -1;

	texture_win32gl::ptr win_tex;
	win_tex.assign_static_cast(tex);

	return win_tex->update_tex(xoffset, yoffset,
		width, height,
		pix_format, pixels);
}

}
