/**
* @file robj_cone.h
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
#ifndef __L3ENG_ROBJ_CONE_H__
#define __L3ENG_ROBJ_CONE_H__

#include "render_obj.h"
#include "l3_color.h"
#include "l3_vector3.h"

namespace l3eng{

class robj_cone : public render_obj
{
public:

	typedef ref_ptr_thread<robj_cone> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_CONE)

public:

	robj_cone(sence_mgr * sence);

	virtual ~robj_cone();

	void init(const l3_f32 cone_height, const l3_f32 cone_angle_degree, const l3_uint32 resolution = 6, 
		const vector3& v_dir = vector3(0.0f, -1.0f, 0.0f),
		const color& clr = color());

private:

	void _add_cone_vertex(const l3_f32 x, const l3_f32 y, const l3_f32 z,
		const l3_uint8 r, const l3_uint8 g, const l3_uint8 b, const l3_uint8 a);

private:

	/* 圆锥的高度 */
	l3_f32 cone_height_;
	/* 圆锥的顶角 */
	l3_f32 cone_angle_;
};

}

#endif
