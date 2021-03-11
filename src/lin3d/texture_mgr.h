/**
* @file texture_mgr.h
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
#ifndef __L3ENG_TEXTURE_MGR_H__
#define __L3ENG_TEXTURE_MGR_H__

#include <map>
#include <list>
#include "texture_base.h"
#include "image.h"

namespace l3eng{

class l3_engine;

class tex_abstract
{
public:

	l3_int32 width_;
	l3_int32 height_;
	texture_base::e_tex_inter_type inter_type_;

	bool operator < (const tex_abstract& r)const
	{
		if(this->width_ < r.width_)
			return true;
		else if(this->width_ > r.width_)
			return false;

		if(this->height_ < r.height_)
			return true;
		else if(this->height_ > r.height_)
			return false;

		if(this->inter_type_ < r.inter_type_)
			return true;
		return false;
	}
};

class texture_mgr
{
public:

	texture_mgr(l3_engine * eng):eng_(eng){}

	OBJ_ID create_tex_null(texture_base::ptr& tex,
		const l3_int32 width,
		const l3_int32 height,
		const texture_base::e_tex_inter_type inter_type,
		const texture_base::e_tex_filter filter = texture_base::e_tex_filter_linear,
		const texture_base::e_tex_wrap wrap = texture_base::e_tex_wrap_clamp_edge)
	{
		return this->create_tex(tex,
			width,
			height,
			texture_base::e_tex_type_f32,
			texture_base::e_tex_format_rgba,
			NULL,
			inter_type,
			filter,
			wrap);
	}

	OBJ_ID create_tex(texture_base::ptr& tex,
		const l3_int32 width,
		const l3_int32 height,
		const texture_base::e_tex_type type,
		const texture_base::e_tex_format format,
		const void * pixels = NULL,
		const texture_base::e_tex_inter_type inter_type = texture_base::e_tex_inter_type_rgba_u8,
		const texture_base::e_tex_filter filter = texture_base::e_tex_filter_linear,
		const texture_base::e_tex_wrap wrap = texture_base::e_tex_wrap_clamp,
		const texture_base::e_tex_compare comp_func = texture_base::e_tex_compare_none
		);

	OBJ_ID create_dep_tex(texture_base::ptr& tex,
		const l3_int32 width,
		const l3_int32 height,
		const l3_bool b32 = L3_FALSE,
		const texture_base::e_tex_compare dep_cmp = texture_base::e_tex_compare_none
		);

	OBJ_ID load_tex(const std::string& file_path, texture_base::ptr& tex);

	OBJ_ID load_from_image(const image& img, texture_base::ptr& tex);

	OBJ_ID find_tex(OBJ_ID obj_id, texture_base::ptr& tex);

	l3_int32 update_tex(OBJ_ID obj_id,
		const l3_int32 xoffset, const l3_int32 yoffset,
		const l3_int32 width, const l3_int32 height,
		const e_pixel_format pix_format, const void * pixels);

private:

	l3_engine * eng_;

	typedef std::map<OBJ_ID, texture_base::ptr> MAP_TEXTRUE;
	MAP_TEXTRUE map_tex_;

	typedef std::map<std::string/* file path */, OBJ_ID> MAP_PATH_TEX_OBJ;
	MAP_PATH_TEX_OBJ map_path_;


	typedef std::map<tex_abstract, std::list<OBJ_ID> > MAP_TEXTRUE_ABS;
	MAP_TEXTRUE_ABS map_tex_abs_;

	typedef std::map<tex_abstract, std::list<OBJ_ID> > MAP_TEXTRUE_ABS;
	MAP_TEXTRUE_ABS map_dep_tex_abs_;
};

}

#endif
