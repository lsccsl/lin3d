/**
* @file render_obj.cpp
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
#include "render_obj.h"
#include "sence_mgr.h"
#include "win_device.h"

namespace l3eng{


render_obj::render_obj(sence_mgr * sence):robj_base(sence),
	rtt_(base_obj::INVALID_OBJ_ID)
{}
render_obj::~render_obj()
{}

void render_obj::rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree)
{
	this->move_.rotate(x_degree, y_degree, z_degree);

	this->_ntf_rotate(x_degree, y_degree, z_degree);
}

void render_obj::move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	this->move_.move_xyz(x, y, z);

	const vector3& v_pos = this->move_.get_world_pos();

	this->_ntf_move_to_xyz(v_pos.x(),
		v_pos.y(),
		v_pos.z());
}

void render_obj::move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	this->move_.move_to_xyz(x, y, z);

	this->_ntf_move_to_xyz(x, y, z);
}

void render_obj::scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale)
{
	this->move_.scale(x_scale, y_scale, z_scale);

	this->_ntf_scale(x_scale, y_scale, z_scale);
}

void render_obj::dir_world_up(vector3& v)const
{
	//工作断点：测试此函数
	//方便获取上方向向量,用途:反射面的法线获取
	this->move_.dir_world_up(v);
}

void render_obj::dir_world_toward(vector3& v)const
{
	this->move_.dir_world_toward(v);
}

void render_obj::dir_world_left(vector3& v)const
{
	this->move_.dir_world_left(v);
}

void render_obj::pre_render()
{
	this->move_.update_all();
}

void render_obj::render()
{
	this->_render();
}

void render_obj::_render()
{
	win_device * dev = this->sence_->eng()->dev();

	dev->render(this);
}

}

