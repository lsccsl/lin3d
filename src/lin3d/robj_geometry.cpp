/**
* @file robj_geometry.cpp
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
#include "robj_geometry.h"

namespace l3eng{

robj_geometry::robj_geometry(sence_mgr * sence) : render_obj(sence)
{}

robj_geometry::~robj_geometry()
{}

void robj_geometry::create_cube(l3_f32 d1, l3_f32 d2, l3_f32 d3,
	l3_bool user_clr,
	l3_uint8 clr_r, l3_uint8 clr_g, l3_uint8 clr_b)
{
	vertex_buf& vb = this->msh_.vb();
	index_buf& ib = this->msh_.ib();

	l3_f32 x = d1 / 2;
	l3_f32 y = d2 / 2;
	l3_f32 z = d3 / 2;

	vertex_color_t clr;
	vertex_color_t clr1;
	vertex_color_t clr2;
	vertex_color_t clr3;
	if(user_clr)
	{
		clr.r_ = clr_r;
		clr.g_ = clr_g;
		clr.b_ = clr_b;
		clr.a_ = 255;

		clr1 = clr2 = clr3 = clr;
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

	/*  */
	{
		/* 1:正面
			1 ----- 0
			|       |
			|       |
			2 ----- 3
		*/
		vb.add_vertex_pos( x,  y,  z);
		vb.add_vertex_pos(-x,  y,  z);
		vb.add_vertex_pos(-x, -y,  z);
		vb.add_vertex_pos( x, -y,  z);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);

		ib.add_3index_i32(0, 1, 2);
		ib.add_3index_i32(3, 0, 2);


		/* 2:右侧
			 7
		  4 /|
		   | | 6
		   |/
		   5
		*/
		vb.add_vertex_pos( x,  y,  z);
		vb.add_vertex_pos( x, -y,  z);
		vb.add_vertex_pos( x, -y, -z);
		vb.add_vertex_pos( x,  y, -z);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_vertex_nml(1.0f, 0.0f, 0.0f);
		vb.add_vertex_nml(1.0f, 0.0f, 0.0f);
		vb.add_vertex_nml(1.0f, 0.0f, 0.0f);
		vb.add_vertex_nml(1.0f, 0.0f, 0.0f);

		ib.add_3index_i32(4, 5, 6);
		ib.add_3index_i32(7, 4, 6);


		/* 3:背面	  
		11 ----- 8
		 |       |
		 |       |
		10 ----- 9
		*/
		vb.add_vertex_pos( x,  y, -z);
		vb.add_vertex_pos( x, -y, -z);
		vb.add_vertex_pos(-x, -y, -z);
		vb.add_vertex_pos(-x,  y, -z);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, -1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, -1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, -1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, -1.0f);

		ib.add_3index_i32(8, 9, 10);
		ib.add_3index_i32(11, 8, 10);


		/* 4:左侧
	            
               13
			12 /|
			  | |
			  |/ 14
			 15
		*/
		vb.add_vertex_pos(-x,  y,  z);
		vb.add_vertex_pos(-x,  y, -z);
		vb.add_vertex_pos(-x, -y, -z);
		vb.add_vertex_pos(-x, -y,  z);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_vertex_nml(-1.0f, 0.0f, 0.0f);
		vb.add_vertex_nml(-1.0f, 0.0f, 0.0f);
		vb.add_vertex_nml(-1.0f, 0.0f, 0.0f);
		vb.add_vertex_nml(-1.0f, 0.0f, 0.0f);

		ib.add_3index_i32(12, 13, 14);
		ib.add_3index_i32(15, 12, 14);


		/* 5:顶部
			18 ----- 17
			/       /
		  19 ----- 16
		*/
		vb.add_vertex_pos( x,  y,  z);
		vb.add_vertex_pos( x,  y, -z);
		vb.add_vertex_pos(-x,  y, -z);
		vb.add_vertex_pos(-x,  y,  z);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_vertex_nml(0.0f, 1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, 1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, 1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, 1.0f, 0.0f);

		ib.add_3index_i32(16, 17, 18);
		ib.add_3index_i32(19, 16, 18);


		/* 6:底部
			22 ---- 23
			/      /
		  21 ---- 20            
		*/
		vb.add_vertex_pos( x, -y,  z);
		vb.add_vertex_pos(-x, -y,  z);
		vb.add_vertex_pos(-x, -y, -z);
		vb.add_vertex_pos( x, -y, -z);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, -1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, -1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, -1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, -1.0f, 0.0f);

		ib.add_3index_i32(20, 21, 22);
		ib.add_3index_i32(23, 20, 22);
	}
}

void robj_geometry::create_square(l3_f32 width, l3_f32 height,
	l3_bool user_clr, l3_uint8 clr_r, l3_uint8 clr_g, l3_uint8 clr_b,
	l3_uint8 alpha)
{
	vertex_buf& vb = this->msh_.vb();
	index_buf& ib = this->msh_.ib();

	l3_f32 x = width / 2;
	l3_f32 y = height / 2;

	vertex_color_t clr;
	vertex_color_t clr1;
	vertex_color_t clr2;
	vertex_color_t clr3;
	if(user_clr)
	{
		clr.r_ = clr_r;
		clr.g_ = clr_g;
		clr.b_ = clr_b;
		clr.a_ = 255;

		clr1 = clr2 = clr3 = clr;
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

	/*  */
	{
		/* 1:正面
			1 ----- 0
			|       |
			|       |
			2 ----- 3
		*/
		vb.add_vertex_pos( x,  y,  0);
		vb.add_vertex_pos(-x,  y,  0);
		vb.add_vertex_pos(-x, -y,  0);
		vb.add_vertex_pos( x, -y,  0);
		vb.add_vertex_clr(clr);
		vb.add_vertex_clr(clr1);
		vb.add_vertex_clr(clr2);
		vb.add_vertex_clr(clr3);
		vb.add_texcoord0(1.0f, 1.0f);
		vb.add_texcoord0(0.0f, 1.0f);
		vb.add_texcoord0(0.0f, 0.0f);
		vb.add_texcoord0(1.0f, 0.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);
		vb.add_vertex_nml(0.0f, 0.0f, 1.0f);

		ib.add_3index_i32(0, 1, 2);
		ib.add_3index_i32(3, 0, 2);

		this->move_.merge_local( x,  y,  0);
		this->move_.merge_local(-x,  y,  0);
		this->move_.merge_local(-x, -y,  0);
		this->move_.merge_local( x, -y,  0);
	}
}

void robj_geometry::add_triangle(const vector3& p1,
	const vector3& p2,
	const vector3& p3)
{
	l3_int32 old_count = this->msh_.vb().get_vertex_count();

	this->msh_.vb().add_vertex_pos(p1.x(), p1.y(), p1.z());
	this->msh_.vb().add_vertex_pos(p2.x(), p2.y(), p2.z());
	this->msh_.vb().add_vertex_pos(p3.x(), p3.y(), p3.z());

	vertex_color_t clr;

	clr.r_ = 255;
	clr.g_ = 0;
	clr.b_ = 0;
	clr.a_ = 0;

	this->msh_.vb().add_vertex_clr(clr);
	this->msh_.vb().add_vertex_clr(clr);
	this->msh_.vb().add_vertex_clr(clr);

	this->msh_.ib().add_index_i32(old_count);
	this->msh_.ib().add_index_i32(old_count + 1);
	this->msh_.ib().add_index_i32(old_count + 2);

	//this->bi_.local_box_.merge(p1);
	//this->bi_.local_box_.merge(p2);
	//this->bi_.local_box_.merge(p3);
}

void robj_geometry::clear_mesh()
{
}

}
