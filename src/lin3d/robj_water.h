/**
* @file robj_water.h
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
#ifndef __L3ENG_ROBJ_WATER_H__
#define __L3ENG_ROBJ_WATER_H__


#include "render_obj.h"

namespace l3eng
{

class robj_water : public render_obj
{
public:

	typedef ref_ptr_thread<robj_water> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_WATER)

public:

	robj_water(sence_mgr * sence);

	virtual ~robj_water();

	void init(const l3_f32 width, const l3_f32 length, const color& clr);
};

}

#endif
