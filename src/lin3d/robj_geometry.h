/**
* @file robj_geometry.h
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
#ifndef __L3ENG_ROBJ_GEOMETRY_H__
#define __L3ENG_ROBJ_GEOMETRY_H__

#include "render_obj.h"
#include "l3_vector3.h"
#include "l3_type.h"

namespace l3eng{

class robj_geometry : public render_obj
{
public:

	typedef ref_ptr_thread<robj_geometry> ptr; 

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_GEOMETRY)

public:

	robj_geometry(sence_mgr * sence);

	virtual ~robj_geometry();

public:

	void create_cube(l3_f32 d1, l3_f32 d2, l3_f32 d3,
		l3_bool user_clr = L3_FALSE, l3_uint8 clr_r = 255, l3_uint8 clr_g = 255, l3_uint8 clr_b = 255);

	void create_square(l3_f32 width, l3_f32 height,
		l3_bool user_clr = 0, l3_uint8 clr_r = 255, l3_uint8 clr_g = 255, l3_uint8 clr_b = 255,
		l3_uint8 alpha = 0);

public:
	void add_triangle(const vector3& p1,
		const vector3& p2,
		const vector3& p3);

	void clear_mesh();
};

}

#endif
