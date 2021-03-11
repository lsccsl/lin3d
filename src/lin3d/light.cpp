/**
* @file light.cpp
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
#include "light.h"

namespace l3eng{

void light::set_pos(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	this->mov_.x_pos(x);
	this->mov_.y_pos(y);
	this->mov_.z_pos(z);
	this->mov_.recal_mtx();

	//this->pos_.xyz(x, y, z);
}

void light::set_rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree)
{
	this->mov_.x_degree(x_degree);
	this->mov_.y_degree(y_degree);
	this->mov_.z_degree(z_degree);
	this->mov_.recal_mtx();

	const matrix4& mtx_world = this->mov_.mtx_world();

	vector3 axis_z(0.0, 0.0f, -1.0f);
	mtx_world.trans_rotate_vect3(axis_z, this->dir_);
}

void light::get_rotate(l3_f32& x_degree, l3_f32& y_degree, l3_f32& z_degree)
{
	x_degree = this->mov_.x_degree();
	y_degree = this->mov_.y_degree();
	z_degree = this->mov_.z_degree();
}

}





