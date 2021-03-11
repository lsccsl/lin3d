/**
* @file robj_vector.cpp
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
#include "robj_vector.h"
#include "quaternion.h"

namespace l3eng
{

robj_vector::robj_vector(sence_mgr * sence):render_obj(sence)
	,v_start_(0.0f, 0.0f, 0.0f)
	,cube_(sence)
{
	this->mtrl_.force_light_off(L3_TRUE);
	this->msh_.ib().idx_type(E_ITYPE_LINES);

	vertex_buf& vb = this->msh_.vb();
	index_buf&  ib = this->msh_.ib();
	{
		vb.add_vertex_pos(0.0f, 0.0f, 0.0f);
		vb.add_vertex_pos(1.0f, 1.0f, 1.0f);
	}

	{
		vb.add_vertex_clr(255, 0, 0, 255);
		vb.add_vertex_clr(0, 255, 0, 255);
	}

	{
		ib.add_index_i32(0);
		ib.add_index_i32(1);
	}

	{
		this->cube_.create_cube(1.0f, 1.0f, 0.5f
			//,L3_TRUE, 255, 0, 255
			);
		//this->cube_.get_material().poly_mode(material::E_POLY_LINE);
		this->cube_.get_material().force_light_off(L3_TRUE);
		this->cube_.get_material().enable_depth_test(L3_FALSE);
	}
}

robj_vector::~robj_vector()
{
}

void robj_vector::set_vector(const vector3& v_dir, l3_f32 draw_scale)
{
	this->v_dir_   = v_dir;
	this->scale_   = draw_scale;

	this->v_dir_.normalise();

	this->_set_vertex();
}

void robj_vector::set_start(const vector3& v_start)
{
	this->v_start_ = v_start;

	this->_set_vertex();
}

void robj_vector::rotate_by_axis_and_angle(const vector3& axis, const l3_f32 degree)
{
	quaternion q;
	q.from_angle_axis(degree, axis);

	vector3 v_new;
	q.rotate_vector(this->v_dir_, v_new);

	this->v_dir_ = v_new;

	this->_set_vertex();
}

void robj_vector::_set_vertex()
{
	vertex_buf& vb = this->msh_.vb();

	vb.set_vertex_pos(0, this->v_start_.x(), this->v_start_.y(), this->v_start_.z());

	vector3 v_end = this->v_start_ + this->v_dir_ * this->scale_;
	vb.set_vertex_pos(1, v_end.x(), v_end.y(), v_end.z());


	{
		vector3 up;
		vector3 right;
		this->v_dir_.get_up_right(up, right);

		matrix4 mtx_rotate;
		mtx_rotate.gen_rotate_mtx_from_axis(right, up, this->v_dir_);

		matrix4 mtx;
		mtx_rotate.gen_inverse_matrix(mtx);

		matrix4::move_matrix(mtx,
			this->v_start_.x() + this->v_dir_.x() * this->scale_,
			this->v_start_.y() + this->v_dir_.y() * this->scale_,
			this->v_start_.z() + this->v_dir_.z() * this->scale_);

		this->cube_.set_world_mtx(mtx);
	}
}

void robj_vector::render()
{
	this->cube_.render();
	render_obj::render();
}

}
