/**
* @file robj_skydome.h
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
#include "robj_skydome.h"
#include "sence_mgr.h"
#include "l3_engine.h"

namespace l3eng{

void robj_skydome::render()
{
	l3_f32 cam_x = this->sence_->cam_cur()->x_pos();
	l3_f32 cam_y = this->sence_->cam_cur()->y_pos();
	l3_f32 cam_z = this->sence_->cam_cur()->z_pos();

	this->move_to_xyz(cam_x, cam_y, cam_z);

	this->_render();
}

void robj_skydome::init(l3_f32 radius,
	l3_f32 tex_percent,
	l3_f32 sphere_percent,
	l3_int32 h_resolution,
	l3_int32 v_resolution)
{
	//this->mtrl_.poly_mode(material::E_POLY_LINE);

	if(tex_percent > 1.0f)
		tex_percent = 1.0f;
	if(tex_percent < 0.0f)
		tex_percent = 0.0f;

	if(sphere_percent > 1.0f)
		sphere_percent = 1.0f;
	if(sphere_percent < 0.0f)
		sphere_percent = 0.0f;

	l3_int32 vertex_count = (h_resolution + 1) * (v_resolution + 1);

	vertex_buf& vb = this->msh_.vb();
	index_buf& ib = this->msh_.ib();

	l3_f32 h_step  = (2.0f * L3_PI) / h_resolution;
	l3_f32 azimuth = 0;

	l3_f32 elevation_step = (sphere_percent * L3_PI) / v_resolution;
	l3_f32 tv_step        = tex_percent / v_resolution;

	for(l3_int32 i = 0; i <= h_resolution; i ++)
	{
		l3_f32 elevation = L3_PI / 2.0f;

		if(i == h_resolution)
			azimuth = 0.0f;

		l3_f32 cos_h = cosf(azimuth);
		l3_f32 sin_h = sinf(azimuth);
		l3_f32 tu    = (l3_f32)i / (l3_f32)h_resolution;

		if(i == h_resolution)
			tu = 1.0;

		for(l3_int32 j = 0; j <= v_resolution; j ++)
		{
			l3_f32 tv = j * tv_step;
			l3_f32 cos_r = radius * cosf(elevation);

			vb.add_texcoord0(tu, tv);

			vb.add_vertex_pos(cos_r  * sin_h, radius * sinf(elevation), cos_r  * cos_h);

			vb.add_vertex_clr(255, 255, 255, 255);

			vb.add_vertex_nml(0.0f, 0.0f, 0.0f);

			elevation -= elevation_step;
		}

		azimuth += h_step;
	}

	l3_int32 idx_count = (3 * (2 * v_resolution - 1) * h_resolution);


	for(l3_int32 i = 0; i < h_resolution; i ++)
	{
		ib.add_3index_i32(
			v_resolution + 2 + (v_resolution + 1) * i,
			1 + (v_resolution + 1) * i,
			0 + (v_resolution + 1) * i
			);

		for(l3_int32 j = 1; j < v_resolution; j ++)
		{
			ib.add_3index_i32(
				v_resolution + 2 + (v_resolution + 1) * i + j,
				1 + (v_resolution + 1) * i + j,
				0 + (v_resolution + 1) * i + j
				);

			ib.add_3index_i32(
				v_resolution + 1 + (v_resolution + 1) * i + j,
				v_resolution + 2 + (v_resolution + 1) * i + j,
				0 + (v_resolution + 1) * i + j
				);
		}
	}

	//this->msh_.ib().set_buf(idx_buf);
}

}


