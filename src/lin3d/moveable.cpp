/**
* @file moveable.h
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
#include "moveable.h"
#include "l3_log.h"

namespace l3eng{

l3_bool moveable_obj::_recal_mtx()const
{
	if(this->e_cal_mode_ & e_cal_mode_view)
	{
		if(this->dirty_view_)
		{
			this->dirty_view_ = L3_FALSE;

			matrix4 x_rotate_mtx;
			matrix4::gen_rotate_x_matrix(-this->x_degree_, x_rotate_mtx);
			matrix4 y_rotate_mtx;
			matrix4::gen_rotate_y_matrix(-this->y_degree_, y_rotate_mtx);
			matrix4 z_rotate_mtx;
			matrix4::gen_rotate_y_matrix(-this->z_degree_, z_rotate_mtx);

			matrix4 rotate_mtx1;
			matrix4 rotate_mtx2;
			y_rotate_mtx.mult(x_rotate_mtx, rotate_mtx1);
			rotate_mtx1.mult(z_rotate_mtx, rotate_mtx2);

			matrix4 move_mtx;
			matrix4::gen_move_matrix(move_mtx, -this->v_pos_.x(), -this->v_pos_.y(), -this->v_pos_.z());

			move_mtx.mult(rotate_mtx2, this->mtx_view_);

			this->bchange_ = L3_TRUE;
		}
	}

	if(this->e_cal_mode_ & e_cal_mode_world)
	{
		if(this->dirty_world_)
		{
			this->dirty_world_ = L3_FALSE;

			matrix4 x_rotate_mtx;
			matrix4::gen_rotate_x_matrix(this->x_degree_, x_rotate_mtx);
			matrix4 y_rotate_mtx;
			matrix4::gen_rotate_y_matrix(this->y_degree_, y_rotate_mtx);
			matrix4 z_rotate_mtxm;
			matrix4::gen_rotate_z_matrix(this->z_degree_, z_rotate_mtxm);

			matrix4 rotate_mtx1;
			matrix4 rotate_mtx2;

			x_rotate_mtx.mult(y_rotate_mtx, rotate_mtx1);
			rotate_mtx1.mult(z_rotate_mtxm, rotate_mtx2);

			matrix4 m_scale;
			matrix4::gen_scale_matrix(m_scale,
				this->x_scale_, this->y_scale_, this->z_scale_);

			m_scale.mult(rotate_mtx2, this->mtx_world_);

			matrix4::move_matrix(this->mtx_world_,
				this->v_pos_.x(), this->v_pos_.y(), this->v_pos_.z());

			this->bchange_ = L3_TRUE;
		}
	}

	return this->bchange_;
}

void moveable_obj::rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree)
{
	if(x_degree == this->x_degree_ &&
		y_degree == this->y_degree_ &&
		z_degree == this->z_degree_)
	{ return; }

	this->x_degree_ = x_degree;
	this->y_degree_ = y_degree;
	this->z_degree_ = z_degree;

	this->set_dirty();
}

void moveable_obj::move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	if(x == 0.0f &&
		y == 0.0f &&
		z == 0.0f)
	{ return; }

	this->v_pos_.x(this->v_pos_.x() + x);
	this->v_pos_.y(this->v_pos_.y() + y);
	this->v_pos_.z(this->v_pos_.z() + z);

	this->set_dirty();
}

void moveable_obj::move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	if(x == this->v_pos_.x() &&
		y == this->v_pos_.y() &&
		z == this->v_pos_.z())
	{ return; }

	this->v_pos_.x(x);
	this->v_pos_.y(y);
	this->v_pos_.z(z);

	this->set_dirty();
}

void moveable_obj::scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale)
{
	if(x_scale == this->x_scale_ &&
		y_scale == this->y_scale_ &&
		z_scale == this->z_scale_)
	{ return; }

	this->x_scale_ = x_scale;
	this->y_scale_ = y_scale;
	this->z_scale_ = z_scale;

	this->set_dirty();
}

void moveable_obj::dir_world_up(vector3& v) const
{
	const matrix4& mtx = this->mtx_world();

	v.x(mtx[1][0]);
	v.y(mtx[1][1]);
	v.z(mtx[1][2]);
}

void moveable_obj::dir_world_toward(vector3& v) const
{
	const matrix4& mtx = this->mtx_world();

	v.x(mtx[2][0]);
	v.y(mtx[2][1]);
	v.z(mtx[2][2]);
}

void moveable_obj::dir_world_left(vector3& v) const
{
	const matrix4& mtx = this->mtx_world();

	v.x(mtx[0][0]);
	v.y(mtx[0][1]);
	v.z(mtx[0][2]);
}

}





