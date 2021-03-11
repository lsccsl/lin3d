/**
* @file robj_cone.cpp
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
#include "robj_cone.h"
#include "quaternion.h"

namespace l3eng{

robj_cone::robj_cone(sence_mgr * sence):render_obj(sence)
{}

robj_cone::~robj_cone()
{}

void robj_cone::_add_cone_vertex(const l3_f32 x, const l3_f32 y, const l3_f32 z,
	const l3_uint8 r, const l3_uint8 g, const l3_uint8 b, const l3_uint8 a)
{
	vertex_buf& vb = this->msh_.vb();

	vb.add_vertex_pos(x,  y,  z);
	vb.add_vertex_clr(r, g, b, a);

	this->move_.merge_local(x, y, z);
}

void robj_cone::init(const l3_f32 cone_height, const l3_f32 cone_angle_degree, const l3_uint32 resolution, 
	const vector3& v_dir,
	const color& clr)
{
	this->cone_height_ = cone_height;
	this->cone_angle_ = cone_angle_degree;

	l3_uint8 r = (l3_uint8)(clr.r() * 255);
	l3_uint8 g = (l3_uint8)(clr.g() * 255);
	l3_uint8 b = (l3_uint8)(clr.b() * 255);
	l3_uint8 a = (l3_uint8)(clr.a() * 255);

	//顶点一圈
	for(l3_uint32 i = 0; i < resolution; i ++)
	{
		this->_add_cone_vertex(0.0f, 0.0f, 0.0f,
			r, g, b, a);
	}

	l3_f32 tan_angle = tan(cone_angle_degree * L3_AR_CONVERT / 2.0f);
	l3_f32 base_radius = cone_height * tan_angle;

	vector3 v_dir_n = v_dir;

	v_dir_n.normalise();

	vector3 up;
	vector3 left;
	v_dir_n.get_up_left(up, left);

	//求出边
	vector3 v_side = v_dir_n * cone_height + left * base_radius;


	//底盘一圈
	l3_f32 angle_rad_step = (2.0f * L3_PI) / resolution;
	for(l3_uint32 i = 0; i < resolution; i ++)
	{
		l3_f32 angle_rad = angle_rad_step * i;

		//绕着 v_dir_n 旋转
		quaternion q;
		q.from_radian_axis(angle_rad * 0.5f, v_dir_n);

		vector3 v_new;
		q.rotate_vector(v_side, v_new);

		//l3_f32 x = base_radius * cos(angle_rad);
		//l3_f32 y = -cone_height;
		//l3_f32 z = base_radius * sin(angle_rad);
		l3_f32 x = v_new.x();
		l3_f32 y = v_new.y();
		l3_f32 z = v_new.z();		

		this->_add_cone_vertex(x, y, z,
			r, g, b, a);
	}

	//重复底盘一圈
	for(l3_uint32 i = 0; i < resolution; i ++)
	{
		l3_f32 angle_rad = angle_rad_step * i;

		//绕着 v_dir_n 旋转
		quaternion q;
		q.from_radian_axis(angle_rad * 0.5f, v_dir_n);

		vector3 v_new;
		q.rotate_vector(v_side, v_new);

		l3_f32 x = v_new.x();
		l3_f32 y = v_new.y();
		l3_f32 z = v_new.z();

		this->_add_cone_vertex(x, y, z,
			r, g, b, a);
	}

	//底盘中心
	vector3 base_center = v_dir_n * cone_height;
	this->_add_cone_vertex(base_center.x(), base_center.y(), base_center.z(),
		r, g, b, a);


	//顶点索引
	index_buf& ib = this->msh_.ib();

	//侧面
	for(l3_uint32 i = 0; i < (resolution - 1); i ++)
		ib.add_3index_i32(i, resolution + i + 1, resolution + i);
	ib.add_3index_i32(resolution - 1, resolution, resolution + resolution - 1);

	//底盘
	l3_int32 idx_start = 2 * resolution;
	l3_int32 idx_end = 3 * resolution + 1 - 1;
	for(l3_uint32 i = 0; i < (resolution - 1); i ++)
		ib.add_3index_i32(idx_start + i, idx_start + i + 1, idx_end);
	ib.add_3index_i32(idx_start + resolution - 1, idx_start, idx_end);

	this->msh_.cal_normal();
}

}
