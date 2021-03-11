/**
* @file robj_sphere.cpp
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
#include "robj_sphere.h"
#include "l3_log.h"

namespace l3eng{

robj_sphere::robj_sphere(sence_mgr * sence):render_obj(sence),
	radius_(1.0f),
	resolution_(10)
{}

robj_sphere::~robj_sphere()
{}

void robj_sphere::_add_sphere_vertex(const l3_f32 nml_x, const l3_f32 nml_y, const l3_f32 nml_z,
	const l3_f32 x, const l3_f32 y, const l3_f32 z,
	const l3_f32 u, const l3_f32 v,
	const l3_uint8 r, const l3_uint8 g, const l3_uint8 b, const l3_uint8 a)
{
	vertex_buf& vb = this->msh_.vb();

	vb.add_vertex_nml(nml_x, nml_y, nml_z);
	vb.add_vertex_pos(x,  y,  z);
	vb.add_texcoord0(u, v);
	vb.add_vertex_clr(r, g, b, a);

	this->move_.merge_local(x, y, z);
}

void robj_sphere::init(const l3_f32 radius, const l3_uint32 resolution, const color& clr_in)
{
	this->radius_ = radius;
	this->resolution_ = resolution;
	if(this->resolution_ < 2)
		this->resolution_ = 2;

	l3_f32 u_step = 0.5f / ((l3_f32)resolution);
	l3_f32 v_step = 1.0f / ((l3_f32)resolution);

	/* 一圈 2* resolution个点,总共resolution-1个圈,加两个极点 */
	l3_uint32 total_vertex_count = (2 * this->resolution_ + 1) * (this->resolution_ - 1) + 2;
	/* 三角形索引个数 */
	l3_uint32 total_idx_count = (this->resolution_ * 4 + 2 * (2 * this->resolution_) * (this->resolution_ - 2)) * 3;

	//vertex_buf& vb = this->msh_.vb();
	//index_buf& ib = this->msh_.ib();

	//vertex_pos_t * vertex = NULL;
	//{
	//	device_buf_base::ptr pos_buf(new device_common_buf);
	//	vb.buf_pos(pos_buf);
	//	pos_buf->resize_buf_sz(total_vertex_count * sizeof(vertex_pos_t));

	//	vertex = (vertex_pos_t *)(pos_buf->get_buf());
	//}
	//vertex_idx_t * idx = NULL;
	//{
	//	device_buf_base::ptr idx_buf(new device_common_buf);
	//	idx_buf->resize_buf_sz(total_idx_count * sizeof(vertex_idx_t));
	//	ib.set_buf(idx_buf);

	//	idx = (vertex_idx_t *)(idx_buf->get_buf());
	//}
	//vertex_color_t * clr = NULL;
	//{
	//	device_buf_base::ptr clr_buf(new device_common_buf);
	//	vb.buf_color(clr_buf);
	//	clr_buf->resize_buf_sz(total_vertex_count * sizeof(vertex_color_t));

	//	clr = (vertex_color_t *)(clr_buf->get_buf());
	//}
	//vertex_normal_t * nml = NULL;
	//{
	//	device_buf_base::ptr normal_buf(new device_common_buf);
	//	vb.buf_normal(normal_buf);
	//	normal_buf->resize_buf_sz(total_vertex_count * sizeof(vertex_normal_t));

	//	nml = (vertex_normal_t *)(normal_buf->get_buf());
	//}
	//vertex_texcoord_t * texcoord = NULL;
	//{
	//	device_buf_base::ptr buf(new device_common_buf);
	//	vb.buf_texcoord0(buf);
	//	buf->resize_buf_sz(total_vertex_count * sizeof(vertex_texcoord_t));

	//	texcoord = (vertex_texcoord_t *)(buf->get_buf());
	//}

	{

		l3_f32 sweep_step = (L3_PI) / (l3_f32)this->resolution_;
		l3_f32 longitude = 0.0f;/* 经度 */
		l3_f32 latitude = L3_PI/2.0f - sweep_step;/* 纬度 */

		this->_add_sphere_vertex(0.0f, 1.0f, 0.0f,
			0.0f, radius, 0.0f,
			0.5f, 0.0f,
			(l3_uint8)(clr_in.r() * 255), (l3_uint8)(clr_in.g() * 255), (l3_uint8)(clr_in.b() * 255), (l3_uint8)(clr_in.a() * 255));
		//nml[0].x_ = 0.0f;
		//nml[0].y_ = 1.0f;
		//nml[0].z_ = 0.0f;

		//vertex[0].x_ = 0.0f;
		//vertex[0].y_ = radius;
		//vertex[0].z_ = 0.0f;

		//texcoord[0].u_ = 0.5f;
		//texcoord[0].v_ = 0.0f;

		//this->merge_local(vertex[0].x_,
		//	vertex[0].y_,
		//	vertex[0].z_);

		//clr[0].r_ = clr_in.r() * 255;
		//clr[0].g_ = clr_in.g() * 255;
		//clr[0].b_ = clr_in.b() * 255;
		//clr[0].a_ = clr_in.a() * 255;

		l3_uint32 vertex_count = 1;
		for(l3_uint32 i = 0; i < this->resolution_ - 1; i ++)
		{
			l3_f32 sin_latiude = sin(latitude);// * this->radius_;
			l3_f32 cos_latiude = cos(latitude);// * this->radius_;

			l3_f32 v_step_all = (i + 1) * v_step;

			longitude = 0.0f;
			for(l3_uint32 j = 0; j < (this->resolution_ * 2 + 1); j ++)
			{
				l3_f32 nml_x = cos_latiude * sin(longitude);
				if(l3_is_zero(nml_x))
					nml_x = 0.0f;
				l3_f32 nml_y = sin_latiude;
				if(l3_is_zero(nml_y))
					nml_y = 0.0f;
				l3_f32 nml_z = cos_latiude * cos(longitude);	
				if(l3_is_zero(nml_z))
					nml_z = 0.0f;

				l3_f32 pos_x = nml_x * this->radius_;
				l3_f32 pos_y = nml_y * this->radius_;
				l3_f32 pos_z = nml_z * this->radius_;

				l3_f32 u = j * u_step;
				l3_f32 v = v_step_all;

				this->_add_sphere_vertex(nml_x, nml_y, nml_z,
					pos_x, pos_y, pos_z,
					u, v,
					(l3_uint8)(clr_in.r() * 255), (l3_uint8)(clr_in.g() * 255), (l3_uint8)(clr_in.b() * 255), (l3_uint8)(clr_in.a() * 255));

				//vertex[vertex_count].x_ = cos_latiude * sin(longitude);
				//if(ant_is_zero(vertex[vertex_count].x_))
				//	vertex[vertex_count].x_ = 0.0f;
				//vertex[vertex_count].y_ = sin_latiude;
				//if(ant_is_zero(vertex[vertex_count].y_))
				//	vertex[vertex_count].y_ = 0.0f;
				//vertex[vertex_count].z_ = cos_latiude * cos(longitude);	
				//if(ant_is_zero(vertex[vertex_count].z_))
				//	vertex[vertex_count].z_ = 0.0f;

				//nml[vertex_count].x_ = vertex[vertex_count].x_;
				//nml[vertex_count].y_ = vertex[vertex_count].y_;
				//nml[vertex_count].z_ = vertex[vertex_count].z_;

				//vertex[vertex_count].x_ *= this->radius_;
				//vertex[vertex_count].y_ *= this->radius_;
				//vertex[vertex_count].z_ *= this->radius_;

				//texcoord[vertex_count].u_ = j * u_step;
				//texcoord[vertex_count].v_ = v_step_all;

				//this->merge_local(vertex[vertex_count].x_,
				//	vertex[vertex_count].y_,
				//	vertex[vertex_count].z_);

				//clr[vertex_count].r_ = clr_in.r() * 255;
				//clr[vertex_count].g_ = clr_in.g() * 255;//0;//j * 255/(this->resolution_ * 2);
				//clr[vertex_count].b_ = clr_in.b() * 255;//0;//i * 255/(this->resolution_);
				//clr[vertex_count].a_ = clr_in.a() * 255;

				longitude += sweep_step;

				vertex_count ++;
			}

			latitude -= sweep_step;
		}

		this->_add_sphere_vertex(0.0f, -1.0f, 0.0f,
			0.0f, -radius, 0.0f,
			0.5f, 1.0f,
			(l3_uint8)(clr_in.r() * 255), (l3_uint8)(clr_in.g() * 255), (l3_uint8)(clr_in.b() * 255), (l3_uint8)(clr_in.a() * 255));

		//nml[total_vertex_count - 1].x_ = 0.0f;
		//nml[total_vertex_count - 1].y_ = -1.0f;
		//nml[total_vertex_count - 1].z_ = 0.0f;

		//vertex[total_vertex_count - 1].x_ = 0.0f;
		//vertex[total_vertex_count - 1].y_ = -radius;
		//vertex[total_vertex_count - 1].z_ = 0.0f;

		//texcoord[total_vertex_count - 1].u_ = 0.5f;
		//texcoord[total_vertex_count - 1].v_ = 1.0f;

		//this->merge_local(vertex[total_vertex_count - 1].x_,
		//	vertex[total_vertex_count - 1].y_,
		//	vertex[total_vertex_count - 1].z_);

		//clr[total_vertex_count - 1].r_ = clr_in.r() * 255;
		//clr[total_vertex_count - 1].g_ = clr_in.g() * 255;
		//clr[total_vertex_count - 1].b_ = clr_in.b() * 255;
		//clr[total_vertex_count - 1].a_ = clr_in.a() * 255;
	}

	// set vertex index
	index_buf& ib = this->msh_.ib();
//L3_LOG_DEBUG(1, ("%x total_vertex_count:%d total_idx_count:%d", ib.get_buf().get(), total_vertex_count, total_idx_count));
	{
		l3_uint32 idx_count = 0;
		{
			/* "北极" */
			for(l3_uint32 i = 1; i <= (this->resolution_ * 2); i ++)
			{
				ib.add_3index_i32(0, i, i + 1);
				//if(i == 1)
				//	L3_LOG_DEBUG(1, ("%x", ib.get_buf().get()));

				//idx[idx_count + 0].idx_ = 0;
				//idx[idx_count + 1].idx_ = i;
				//idx[idx_count + 2].idx_ = i + 1;

				//idx_count += 3;
			}
		}

		l3_uint32 north_end = 1;

		//if(0)
		{
			for(l3_uint32 i = 0; i < (this->resolution_ - 2) ; i ++)
			{
				l3_uint32 idx_line0 = north_end + (i * (this->resolution_ * 2 + 1));
				l3_uint32 idx_line1 = north_end + ((i + 1) * (this->resolution_ * 2 + 1));
				for(l3_uint32 j = 0; j < (this->resolution_ * 2); j ++)
				{
					ib.add_3index_i32(idx_line0 + j, idx_line1 + j, idx_line0 + j + 1);
					//idx[idx_count + 0].idx_ = idx_line0 + j;
					//idx[idx_count + 1].idx_ = idx_line1 + j;
					//idx[idx_count + 2].idx_ = idx_line0 + j + 1;

					//idx_count += 3;

					ib.add_3index_i32(idx_line1 + j, idx_line1 + j + 1, idx_line0 + j + 1);
					//idx[idx_count + 0].idx_ = idx_line1 + j;
					//idx[idx_count + 1].idx_ = idx_line1 + j + 1;
					//idx[idx_count + 2].idx_ = idx_line0 + j + 1;

					//idx_count += 3;
				}
			}
		}

		l3_uint32 south_begin = 1 + (2 * this->resolution_ + 1) * (this->resolution_ - 2);

		{
			/* "南极" */
			for(l3_uint32 i = 0; i < (this->resolution_ * 2); i ++)
			{
				ib.add_3index_i32(south_begin + i, total_vertex_count - 1, south_begin + i + 1);
				//idx[idx_count + 0].idx_ = south_begin + i;
				//idx[idx_count + 1].idx_ = total_vertex_count - 1;
				//idx[idx_count + 2].idx_ = south_begin + i + 1;

				//idx_count += 3;
			}
		}
	}
//L3_LOG_DEBUG(1, ("%x", ib.get_buf().get()));
}

}

