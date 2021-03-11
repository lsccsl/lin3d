/**
* @file ray.cpp
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
#include "ray.h"
#include "aabb.h"
#include "index_buf.h"
#include "vertex_buf.h"
#include "mesh.h"
#include "triangle.h"

namespace l3eng{

/* @brief 是否与aabb盒碰撞,以及碰撞的时间(根据t即可求出碰撞点) */
l3_bool ray::intersects_aabbox(const aabbox& bbox, l3_f32& min_t)const
{
	if(aabbox::AABB_NULL == bbox.extent())
		return L3_FALSE;

	if(aabbox::AABB_INFINITE == bbox.extent())
	{
		min_t = 0.0f;
		return L3_TRUE;
	}

	min_t = FLT_MAX;
	l3_bool hit = L3_FALSE;

	const vector3& bbox_min = bbox.get_min();
	const vector3& bbox_max = bbox.get_max();

	/* 如果起点在包围盒之内 */
	if(this->v_start_.greater_than(bbox_min) && this->v_start_.less_than(bbox_max))
		return L3_TRUE;

	/* check x */
	if(this->v_start_.x() <= bbox_min.x() && this->v_dir_.x() > 0)
	{
		l3_f32 t = (bbox_min.x() - this->v_start_.x()) / this->v_dir_.x();

		if(t >= 0)
		{
			vector3 hitpoint = this->v_start_ + this->v_dir_ * t;

			if((hitpoint.y() >= bbox_min.y() && hitpoint.y() <= bbox_max.y()) &&
				(hitpoint.z() >= bbox_min.z() && hitpoint.z() <= bbox_max.z()))
			{
				hit = L3_TRUE;
				min_t = t;
			}
		}
	}
	if(this->v_start_.x() >= bbox_max.x() && this->v_dir_.x() < 0)
	{
		l3_f32 t = (bbox_max.x() - this->v_start_.x()) / this->v_dir_.x();

		if(t >= 0)
		{
			vector3 hitpoint = this->v_start_ + this->v_dir_ * t;
			if((hitpoint.y() >= bbox_min.y() && hitpoint.y() <= bbox_max.y()) &&
				(hitpoint.z() >= bbox_min.z() && hitpoint.z() <= bbox_max.z()))
			{
				hit = L3_TRUE;

				if(min_t > t)
					min_t = t;
			}
		}
	}

	/* check y */
	if(this->v_start_.y() <= bbox_min.y() && this->v_dir_.y() > 0)
	{
		l3_f32 t = (bbox_min.y() - this->v_start_.y()) / this->v_dir_.y();

		if(t >= 0)
		{
			vector3 hitpoint = this->v_start_ + this->v_dir_ * t;
			if((hitpoint.x() >= bbox_min.x() && hitpoint.x() <= bbox_max.x()) && 
				(hitpoint.z() >= bbox_min.z() && hitpoint.z() <= bbox_max.z()))
			{
				hit = L3_TRUE;

				if(min_t > t)
					min_t = t;
			}
		}
	}
	if(this->v_start_.y() >= bbox_max.y() && this->v_dir_.y() < 0)
	{
		l3_f32 t = (bbox_max.y() - this->v_start_.y()) / this->v_dir_.y();

		if(t >= 0)
		{
			vector3 hitpoint = this->v_start_ + this->v_dir_ * t;
			if((hitpoint.x() >= bbox_min.x() && hitpoint.x() <= bbox_max.x()) && 
				(hitpoint.z() >= bbox_min.z() && hitpoint.z() <= bbox_max.z()))
			{
				hit = L3_TRUE;

				if(min_t > t)
					min_t = t;
			}
		}
	}

	/* check z */
	if(this->v_start_.z() <= bbox_min.z() && this->v_dir_.z() > 0)
	{
		l3_f32 t = (bbox_min.z() - this->v_start_.z()) / this->v_dir_.z();

		if(t >= 0)
		{
			vector3 hitpoint = this->v_start_ + this->v_dir_ * t;
			if((hitpoint.x() >= bbox_min.x() && hitpoint.x() <= bbox_max.x()) &&
				(hitpoint.y() >= bbox_min.y() && hitpoint.y() <= bbox_max.y()))
			{
				hit = L3_TRUE;

				if(min_t > t)
					min_t = t;
			}
		}
	}
	if(this->v_start_.z() >= bbox_max.z() && this->v_dir_.z() < 0)
	{
		l3_f32 t = (bbox_max.z() - this->v_start_.z()) / this->v_dir_.z();

		if(t >= 0)
		{
			vector3 hitpoint = this->v_start_ + this->v_dir_ * t;
			if((hitpoint.x() >= bbox_min.x() && hitpoint.x() <= bbox_max.x()) &&
				(hitpoint.y() >= bbox_min.y() && hitpoint.y() <= bbox_max.y()))
			{
				hit = L3_TRUE;

				if(min_t > t)
					min_t = t;
			}
		}
	}

	return hit;
}

l3_bool ray::intersects_triangle(triangle& tr,
	l3_f32& t)const
{
	/* 射线与三角形相交检测,from dx sample */
	l3_f32 u;
	l3_f32 v;

	vector3 e1 = tr[1] - tr[0];
	vector3 e2 = tr[2] - tr[0];

	vector3 P;
	this->v_dir_.cross_product(e2, P);

	l3_f32 det = e1.dot_product(P);

	vector3 T;

	if(det > 0)
	{
		T = this->v_start_ - tr[0];
	}
	else
	{
		T = tr[0] - this->v_start_;
		det = -det;
	}

	if(l3_is_zero(det))
		return L3_FALSE;

	u = T.dot_product(P);
	if(u < 0.0f || u > det)
		return L3_FALSE;

	vector3 Q;
	T.cross_product(e1, Q);

	v = this->v_dir_.dot_product(Q);

	if(v < 0.0f || (u + v) > det)
		return L3_FALSE;

	t = e2.dot_product(Q);

	l3_f32 inv_det = 1.0f / det;

	t *= inv_det;

	return L3_TRUE;
}

l3_bool ray::intersects_mesh(const matrix4& mtx_world, const mesh& msh,
	l3_f32& min_t, triangle& tr_hit)const
{
	index_buf& ib = msh.ib();
	vertex_buf& vb = msh.vb();

	if(E_ITYPE_TRIANGLES != ib.idx_type())
		return L3_FALSE;

	const vertex_pos_t * pos = (vertex_pos_t *)(vb.buf_pos()->get_buf());
	if(NULL == pos)
		return L3_FALSE;
	vertex_idx_t * idx_buf = (vertex_idx_t *)(ib.get_buf()->get_buf());
	if(NULL == idx_buf)
		return L3_FALSE;

	min_t = FLT_MAX;
	l3_bool hit = L3_FALSE;

	matrix4 inv_m;
	mtx_world.gen_inverse_matrix(inv_m);

	ray r_local;
	inv_m.trans_rotate_vect3(this->v_dir_, r_local.v_dir());
	inv_m.trans_vect3(this->v_start_, r_local.v_start());

	l3_uint32 idx_count = ib.idx_count();

	l3_f32 t;
	triangle tr;
	for(l3_uint32 i = 0; i < idx_count; i += 3)
	{
		l3_uint32 idx0 = idx_buf[i + 0].idx_;
		l3_uint32 idx1 = idx_buf[i + 1].idx_;
		l3_uint32 idx2 = idx_buf[i + 2].idx_;

		tr[0].xyz(pos[idx0].x_, pos[idx0].y_, pos[idx0].z_);
		tr[1].xyz(pos[idx1].x_, pos[idx1].y_, pos[idx1].z_);
		tr[2].xyz(pos[idx2].x_, pos[idx2].y_, pos[idx2].z_);

		if(!r_local.intersects_triangle(tr, t))
			continue;

		hit = L3_TRUE;

		if(min_t > t)
		{
			min_t = t;
			tr_hit = tr;
		}
	}

	return hit;
}

}


