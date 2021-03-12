/**
* @file light_mgr.cpp
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
#include "light_mgr.h"
#include "l3_engine.h"
#include "base_obj_mgr.h"

namespace l3eng{

light::ptr light_mgr::create_light()
{
	light_info::ptr li(new light_info);

	light::ptr l = this->eng_->obj_mgr()->create_obj<light>();
	light_cam::ptr l_cam = this->eng_->obj_mgr()->create_obj<light_cam, light::ptr, sence_mgr *>(l, eng_->sence());

	li->l_ = l;
	li->l_cam_ = l_cam;

	this->map_light_[l->obj_id()] = li;

	return l;
}

light::ptr light_mgr::create_light_dir(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree,
	const color& clr_diff)
{
	light::ptr l = this->create_light();
	if (l.is_null())
		return l;
	light_mgr::light_info::ptr li;
	this->get_light_info(l->obj_id(), li);
	if (li.is_null())
		return l;

	l->set_rotate(x_degree, y_degree, z_degree);
	li->l_cam_->recal_light_mtx();

	l->diffuse() = clr_diff;

	return l;
}

l3_int32 light_mgr::get_light(OBJ_ID obj_id, light::ptr& l)
{
	MAP_LIGHT::iterator it = this->map_light_.find(obj_id);
	if(this->map_light_.end() == it)
		return -1;

	l = it->second->l_;
	return 0;
}

l3_int32 light_mgr::get_light_info(OBJ_ID obj_id, light_info::ptr& li)
{
	MAP_LIGHT::iterator it = this->map_light_.find(obj_id);
	if(this->map_light_.end() == it)
		return -1;

	li = it->second;
	return 0;
}

}
