/**
* @file light_mgr.h
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
#ifndef __L3ENG_LIGHT_MGR_H__
#define __L3ENG_LIGHT_MGR_H__

#include <map>
#include "l3_type.h"
#include "light.h"
#include "light_cam.h"

namespace l3eng{

class l3_engine;

class light_mgr
{
public:

	struct light_info
	{
		typedef ref_ptr_thread<light_info> ptr;

		light::ptr l_;
		light_cam::ptr l_cam_;
	};

public:

	typedef std::map<OBJ_ID, light_info::ptr> MAP_LIGHT;

	light_mgr(l3_engine * eng):
		eng_(eng)
	{}
	~light_mgr(){}

	light::ptr create_light();

	l3_int32 get_light(OBJ_ID obj_id, light::ptr& l);

	l3_int32 get_light_info(OBJ_ID obj_id, light_info::ptr& li);

	inline const MAP_LIGHT& get_map_light(){ return this->map_light_; }

private:

	MAP_LIGHT map_light_;

	l3_engine * eng_;
};

}

#endif
