/**
* @file robj_plane.cpp
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

#include "robj_plane.h"

namespace l3eng
{

robj_plane::robj_plane(sence_mgr * sence):render_obj(sence)
	,user_clr_(L3_FALSE)
{}

robj_plane::~robj_plane()
{}

/* @brief @param mtx_tran:变换矩阵 */
void robj_plane::set_plane_inter(const matrix4& mtx_tran)
{
	//this->mtrl_.force_light_off(L3_TRUE);

	vertex_buf& vb = this->msh_.vb();
	index_buf& ib = this->msh_.ib();

	vertex_color_t clr;
	vertex_color_t clr1;
	vertex_color_t clr2;
	vertex_color_t clr3;

	if(this->user_clr_)
	{		
		clr1 = clr2 = clr3 = clr = this->pln_clr_;
	}
	else
	{
		clr.r_ = 255;
		clr.g_ = 0;
		clr.b_ = 0;
		clr.a_ = 255;

		clr1.r_ = 0;
		clr1.g_ = 255;
		clr1.b_ = 0;
		clr1.a_ = 255;

		clr2.r_ = 0;
		clr2.g_ = 0;
		clr2.b_ = 255;
		clr2.a_ = 255;

		clr3.r_ = 255;
		clr3.g_ = 255;
		clr3.b_ = 255;
		clr3.a_ = 255;
	}

	vector3 v_pos[4];

	l3_f32 hw = this->pln_wid_ * 0.5f;
	l3_f32 hh = this->pln_len_ * 0.5f;

	v_pos[0].x(hw);
	v_pos[0].y(hh);
	v_pos[0].z(0.0f);

	v_pos[1].x(-hw);
	v_pos[1].y(hh);
	v_pos[1].z(0.0f);

	v_pos[2].x(-hw);
	v_pos[2].y(-hh);
	v_pos[2].z(0.0f);

	v_pos[3].x(hw);
	v_pos[3].y(-hh);
	v_pos[3].z(0.0f);

	mtx_tran.trans_vect3(v_pos[0], v_pos[0]);
	mtx_tran.trans_vect3(v_pos[1], v_pos[1]);
	mtx_tran.trans_vect3(v_pos[2], v_pos[2]);
	mtx_tran.trans_vect3(v_pos[3], v_pos[3]);

	//顶点位置
	if(4 == vb.get_vertex_count())
	{
		for(l3_int32 i = 0; i < sizeof(v_pos)/sizeof(v_pos[0]); i ++)
			vb.set_vertex_pos(i, v_pos[i].x(),  v_pos[i].y(),  v_pos[i].z());
	}
	else
	{
		vb.clear_buf_pos();
		for(l3_int32 i = 0; i < sizeof(v_pos)/sizeof(v_pos[0]); i ++)
			vb.add_vertex_pos(v_pos[i].x(),  v_pos[i].y(),  v_pos[i].z());
	}

	//法线
	vertex_normal_t nml;
	nml.x_ = this->pln_.normal().x();
	nml.y_ = this->pln_.normal().y();
	nml.z_ = this->pln_.normal().z();
	if(4 == vb.get_nml_count())
	{
		vb.set_vertex_nml(0, nml.x_, nml.y_, nml.z_);
		vb.set_vertex_nml(1, nml.x_, nml.y_, nml.z_);
		vb.set_vertex_nml(2, nml.x_, nml.y_, nml.z_);
		vb.set_vertex_nml(3, nml.x_, nml.y_, nml.z_);
	}
	else
	{
		vb.clear_buf_nml();

		vb.add_vertex_nml(nml);
		vb.add_vertex_nml(nml);
		vb.add_vertex_nml(nml);
		vb.add_vertex_nml(nml);
	}


	//纹理映射
	if(0 == vb.get_texcoord0_count())
	{
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(1.0f, 1.0f);
	}

	//顶点索引
	if(0 == ib.idx_count())
	{
		ib.add_3index_i32(0, 1, 2);
		ib.add_3index_i32(3, 0, 2);
	}

	//颜色
	if(0 == vb.get_clr_count())
	{
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
	}
}

void robj_plane::set_plane_info(const l3_f32 width, const l3_f32 length, const vector3& normal, const vector3& inter_point,
	l3_bool user_clr, const vertex_color_t& clr)
{
	vector3 n_normalise = normal;
	n_normalise.normalise();


	this->pln_wid_ = width;
	this->pln_len_ = length;

	this->user_clr_ = user_clr;
	if(user_clr)
		this->pln_clr_ = clr;

	set_from_nml_point_inter(normal, inter_point);
}

void robj_plane::set_from_nml_point_inter(const vector3& normal, const vector3& inter_point)
{
	vector3 n_normalise = normal;
	n_normalise.normalise();

	this->pln_ = plane(n_normalise, inter_point);

	/* 垂足 */
	this->v_pedal_ = this->pln_.normal() * this->pln_.dist();

	vector3 righthand_up;
	vector3 righthand_right;
	n_normalise.get_up_right(righthand_up, righthand_right);

	matrix4 mtx_rotate;
	mtx_rotate.gen_rotate_mtx_from_axis(righthand_right, righthand_up, n_normalise);

	matrix4 mtx;
	mtx_rotate.gen_inverse_matrix(mtx);

	matrix4::move_matrix(mtx,
		this->v_pedal_.x(),
		this->v_pedal_.y(),
		this->v_pedal_.z());

	set_plane_inter(mtx);
}

void robj_plane::set_nml(const vector3& normal)
{
	vector3 n_normalise = normal;
	n_normalise.normalise();

	set_from_nml_point_inter(normal, this->v_pedal_);
}

void robj_plane::set_nml_point(const vector3& normal, const vector3& inter_point)
{
	vector3 n_normalise = normal;
	n_normalise.normalise();

	set_from_nml_point_inter(normal, inter_point);
}

/* @brief 获取物体的朝向,上向量 */
void robj_plane::dir_world_up(vector3& v) const
{
	v = this->pln_.normal();
}

/* @brief 获取物体的朝向,前向量 */
void robj_plane::dir_world_toward(vector3& v) const
{
	vector3 left;
	this->pln_.normal().get_up_left(v, left);
}

/* @brief 获取物体的朝向,左向量 */
void robj_plane::dir_world_left(vector3& v) const
{
	vector3 up;
	this->pln_.normal().get_up_left(up, v);
}

}
