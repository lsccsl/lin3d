/**
* @file robj_sphere.h
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
#ifndef __L3ENG_ROBJ_SPHERE_H__
#define __L3ENG_ROBJ_SPHERE_H__

#include "render_obj.h"
#include "mesh.h"

namespace l3eng
{

class robj_sphere : public render_obj
{
public:

	typedef ref_ptr_thread<robj_sphere> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_SPHERE)

public:

	robj_sphere(sence_mgr * sence);

	virtual ~robj_sphere();

	void init(const l3_f32 radius, const l3_uint32 resolution, const color& clr);

public:

	virtual sphere& get_world_sphere()const override{
		sphere& s = (sphere&)this->render_obj::get_world_sphere();
		s.radius() = this->radius_;
		return s;
	}

private:

	void _add_sphere_vertex(const l3_f32 nml_x, const l3_f32 nml_y, const l3_f32 nml_z,
		const l3_f32 x, const l3_f32 y, const l3_f32 z,
		const l3_f32 u, const l3_f32 v,
		const l3_uint8 r, const l3_uint8 g, const l3_uint8 b, const l3_uint8 a);

private:

	l3_f32 radius_;
	l3_uint32 resolution_;
};

}

#endif
