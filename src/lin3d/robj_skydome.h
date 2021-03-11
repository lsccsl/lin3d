/**
* @file robj_skydome.h
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
#ifndef __L3ENG_ROBJ_SKYDOME_H__
#define __L3ENG_ROBJ_SKYDOME_H__

#include "render_obj.h"
#include "mesh.h"

namespace l3eng{

class robj_skydome : public render_obj
{
public:

	typedef ref_ptr_thread<robj_skydome> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_SKYDOME)

public:

	robj_skydome(sence_mgr * sence):render_obj(sence){}

	virtual ~robj_skydome(){}

	void init(l3_f32 radius,
		l3_f32 tex_percent,
		l3_f32 sphere_percent,
		l3_int32 h_resolution,
		l3_int32 v_resolution);

public:

	virtual void render();

private:
};

}

#endif




