/**
* @file base_obj_mgr.cpp
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
#include "base_obj_mgr.h"
#include "camera.h"

namespace l3eng{

class sence_mgr;

static void test_1()
{
	base_obj_mgr m;
	ref_ptr_thread<camera> ptr = m.create_obj<camera, sence_mgr *>(NULL);

	ref_ptr_thread<base_obj> ptr1;
	m.get_obj(ptr->obj_id(), ptr1);
	m.obj_del(1);
}

}
