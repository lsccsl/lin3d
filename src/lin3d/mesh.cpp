/**
* @file ant_mesh.h
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
#include "mesh.h"
#include "l3_log.h"

namespace l3eng{

void mesh::cal_normal()
{
	if(this->vb_.buf_pos().is_null())
		return;

	if(this->ib_.get_buf().is_null())
		return;

	l3_uint32 vertex_total_count = this->vb_.get_vertex_count();
	l3_uint32 total_index_count = this->ib_.recal_idx_count();

	//vertex_pos_t * pos = (vertex_pos_t *)(this->vb_.buf_pos()->get_buf());
	//vertex_idx_t * index = (vertex_idx_t *)(this->ib_.get_buf()->get_buf());

	//vertex_normal_t * ptr_nml = NULL;

	//device_buf_base::ptr buf(new device_common_buf);
	//{
	//	buf->resize_buf_sz(sizeof(vertex_normal_t) * vertex_total_count);

	//	ptr_nml = (vertex_normal_t *)(buf->get_buf());
	//}

	for(l3_uint32 j = 0; j < vertex_total_count; j ++)
		this->vb_.add_vertex_nml(0.0f, 0.0f, 0.0f);

	vector3 vertex_1;
	vector3 vertex_2;
	vector3 vertex_3;

	vector3 nml_1;
	vector3 nml_2;
	vector3 nml_3;

	for(l3_uint32 i = 0; i < total_index_count; i += 3)
	{
		l3_uint32 idx1 = this->ib_.get_index(i);
		l3_uint32 idx2 = this->ib_.get_index(i + 1);
		l3_uint32 idx3 = this->ib_.get_index(i + 2);

		if(idx1 >= vertex_total_count)
		{
			L3_LOG_ENG_TRACE(("over array bound %d", idx1));
			continue;
		}
		if(idx2 >= vertex_total_count)
		{
			L3_LOG_ENG_TRACE(("over array bound %d", idx2));
			continue;
		}
		if(idx3 >= vertex_total_count)
		{
			L3_LOG_ENG_TRACE(("over array bound %d", idx3));
			continue;
		}

		this->vb_.get_vertex_pos(idx1, vertex_1);
		this->vb_.get_vertex_pos(idx2, vertex_2);
		this->vb_.get_vertex_pos(idx3, vertex_3);

		vector3 v1(vertex_2.x() - vertex_1.x(),
			vertex_2.y() - vertex_1.y(),
			vertex_2.z() - vertex_1.z());
		v1.normalise();

		vector3 v2(vertex_3.x() - vertex_1.x(),
			vertex_3.y() - vertex_1.y(),
			vertex_3.z() - vertex_1.z());
		v2.normalise();

		vector3 normal_plane;
		v1.cross_product(v2, normal_plane);

		normal_plane.normalise();

		/* 顶点的法线应该是相邻面法线向量组合 */
		this->vb_.get_vertex_nml(idx1, nml_1);
		this->vb_.get_vertex_nml(idx2, nml_2);
		this->vb_.get_vertex_nml(idx3, nml_3);

		nml_1.x(nml_1.x() + normal_plane.x());
		nml_1.y(nml_1.y() + normal_plane.y());
		nml_1.z(nml_1.z() + normal_plane.z());

		nml_2.x(nml_2.x() + normal_plane.x());
		nml_2.y(nml_2.y() + normal_plane.y());
		nml_2.z(nml_2.z() + normal_plane.z());

		nml_3.x(nml_3.x() + normal_plane.x());
		nml_3.y(nml_3.y() + normal_plane.y());
		nml_3.z(nml_3.z() + normal_plane.z());

		this->vb_.set_vertex_nml(idx1, nml_1.x(), nml_1.y(), nml_1.z());
		this->vb_.set_vertex_nml(idx2, nml_2.x(), nml_2.y(), nml_2.z());
		this->vb_.set_vertex_nml(idx3, nml_3.x(), nml_3.y(), nml_3.z());

		//ptr_nml[idx1].x_ += normal_plane.x();
		//ptr_nml[idx1].y_ += normal_plane.y();
		//ptr_nml[idx1].z_ += normal_plane.z();

		//ptr_nml[idx2].x_ += normal_plane.x();
		//ptr_nml[idx2].y_ += normal_plane.y();
		//ptr_nml[idx2].z_ += normal_plane.z();

		//ptr_nml[idx3].x_ += normal_plane.x();
		//ptr_nml[idx3].y_ += normal_plane.y();
		//ptr_nml[idx3].z_ += normal_plane.z();
	}

	vector3 nml;
	for(l3_uint32 j = 0; j < vertex_total_count; j ++)
	{
		this->vb_.get_vertex_nml(j, nml);

		nml.normalise();

		this->vb_.set_vertex_nml(j, nml.x(), nml.y(), nml.z());

		//vector3 v(ptr_nml[j].x_, ptr_nml[j].y_, ptr_nml[j].z_);

		//v.normalise();

		//ptr_nml[j].x_ = v.x();
		//ptr_nml[j].y_ = v.y();
		//ptr_nml[j].z_ = v.z();
	}

	//this->vb_.buf_normal(buf);
}

void mesh::deep_copy(const mesh& msh)
{
	this->vb_.deep_copy(msh.vb_);
	this->ib_.deep_copy(msh.ib_);
}

void test_mesh()
{
	mesh::ptr p(new mesh);
}

}








