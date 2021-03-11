/**
* @file ant_vertex_buf.cpp
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
#include "vertex_buf.h"

namespace l3eng
{

l3_int32 vertex_buf::add_vertex_pos(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	if(this->buf_pos_.is_null())
		this->buf_pos_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->buf_pos_->get_buf_sz();
	this->buf_pos_->resize_buf_sz(old_sz + sizeof(vertex_pos_t));

	vertex_pos_t * buf = (vertex_pos_t *)( ((l3_uint8 *)(this->buf_pos_->get_buf())) + old_sz );
	buf->x_ = x;
	buf->y_ = y;
	buf->z_ = z;

	return 0;
}

l3_int32 vertex_buf::set_vertex_pos(const l3_uint32 idx, const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	if(this->buf_pos_.is_null())
		return -1;

	l3_int32 sz = (idx + 1) * sizeof(vertex_pos_t);
	if(this->buf_pos_->get_buf_sz() < sz)
		return -1;

	vertex_pos_t * v = (vertex_pos_t *)(this->buf_pos_->get_buf());
	if(!v)
		return -1;

	v[idx].x_ = x;
	v[idx].y_ = y;
	v[idx].z_ = z;

	return 0;
}

l3_int32 vertex_buf::get_vertex_pos(l3_uint32 i, vector3& pos)const
{
	if(this->buf_pos_.is_null())
		return -1;

	if(i * sizeof(vertex_pos_t) >= (l3_uint32)(this->buf_pos_->get_buf_sz()))
		return -1;

	const l3_f32 * vertex_pos = (l3_f32 *)(this->buf_pos_->get_buf());

	pos.x(vertex_pos[i * 3 + 0]);
	pos.y(vertex_pos[i * 3 + 1]);
	pos.z(vertex_pos[i * 3 + 2]);

	return 0;
}

l3_int32 vertex_buf::get_vertex_pos(l3_uint32 i, vertex_pos_t& pos)const
{
	if(this->buf_pos_.is_null())
		return -1;

	if(i * sizeof(vertex_pos_t) >= (l3_uint32)(this->buf_pos_->get_buf_sz()))
		return -1;

	const l3_f32 * vertex_pos = (l3_f32 *)(this->buf_pos_->get_buf());

	pos.x_ = vertex_pos[i * 3 + 0];
	pos.y_ = vertex_pos[i * 3 + 1];
	pos.z_ = vertex_pos[i * 3 + 2];

	return 0;
}


l3_int32 vertex_buf::add_vertex_clr(const l3_uint8 r, const l3_uint8 g, const l3_uint8 b, const l3_uint8 a)
{
	if(this->buf_clr_.is_null())
		this->buf_clr_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->buf_clr_->get_buf_sz();
	this->buf_clr_->resize_buf_sz(old_sz + sizeof(vertex_color_t));

	vertex_color_t * pclr = (vertex_color_t *)( ((l3_uint8 *)(this->buf_clr_->get_buf())) + old_sz );
	pclr->r_ = r;
	pclr->g_ = g;
	pclr->b_ = b;
	pclr->a_ = a;

	return 0;
}


l3_int32 vertex_buf::add_vertex_nml(const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	if(this->buf_nml_.is_null())
		this->buf_nml_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->buf_nml_->get_buf_sz();
	this->buf_nml_->resize_buf_sz(old_sz + sizeof(vertex_normal_t));

	vertex_normal_t * pnml = (vertex_normal_t *)( ((l3_uint8 *)(this->buf_nml_->get_buf())) + old_sz );
	pnml->x_ = x;
	pnml->y_ = y;
	pnml->z_ = z;

	return 0;
}

l3_int32 vertex_buf::set_vertex_nml(l3_uint32 idx, const l3_f32 x, const l3_f32 y, const l3_f32 z)
{
	if(this->buf_nml_.is_null())
		return -1;

	l3_int32 sz = (idx + 1) * sizeof(vertex_normal_t);
	if(this->buf_nml_->get_buf_sz() < sz)
		return -1;

	vertex_normal_t * pnml = (vertex_normal_t *)(this->buf_nml_->get_buf());
	if(!pnml)
		return -1;

	pnml[idx].x_ = x;
	pnml[idx].y_ = y;
	pnml[idx].z_ = z;

	return 0;
}

l3_int32 vertex_buf::get_vertex_nml(l3_uint32 i, vertex_normal_t& nml)const
{
	if(this->buf_nml_.is_null())
		return -1;

	vertex_normal_t * pnml = (vertex_normal_t *)(this->buf_nml_->get_buf());
	if(!pnml)
		return -1;

	nml.x_ = pnml[i].x_;
	nml.y_ = pnml[i].y_;
	nml.z_ = pnml[i].z_;

	return 0;
}

l3_int32 vertex_buf::get_vertex_nml(l3_uint32 i, vector3& nml)const
{
	if(this->buf_nml_.is_null())
		return -1;

	vertex_normal_t * pnml = (vertex_normal_t *)(this->buf_nml_->get_buf());
	if(!pnml)
		return -1;

	nml.x(pnml[i].x_);
	nml.y(pnml[i].y_);
	nml.z(pnml[i].z_);

	return 0;
}

/* @brief Ìí¼ÓÎÆÀíÓ³Éä */
l3_int32 vertex_buf::add_texcoord0(const l3_f32 u, const l3_f32 v)
{
	if(this->buf_texcoord0_.is_null())
		this->buf_texcoord0_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->buf_texcoord0_->get_buf_sz();
	this->buf_texcoord0_->resize_buf_sz(old_sz + sizeof(vertex_texcoord_t));

	vertex_texcoord_t * texcoord = (vertex_texcoord_t *)( ((l3_uint8 *)(this->buf_texcoord0_->get_buf())) + old_sz );
	texcoord->u_ = u;
	texcoord->v_ = v;

	return 0;
}


l3_int32 vertex_buf::add_texcoord1(const l3_f32 u, const l3_f32 v)
{
	if(this->buf_texcoord1_.is_null())
		this->buf_texcoord1_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->buf_texcoord1_->get_buf_sz();
	this->buf_texcoord0_->resize_buf_sz(old_sz + sizeof(vertex_texcoord_t));

	vertex_texcoord_t * texcoord = (vertex_texcoord_t *)( ((l3_uint8 *)(this->buf_texcoord1_->get_buf())) + old_sz );
	texcoord->u_ = u;
	texcoord->v_ = v;

	return 0;
}


//void vertex_buf::alloc_buf(const l3_int32 vertex_count, l3_uint64 bit)
//{
//	if(bit & e_buf_pos)
//	{
//		device_buf_base::ptr buf(new device_common_buf);
//		this->buf_pos_ = buf;
//		buf->resize_buf_sz(vertex_count * sizeof(vertex_pos_t));
//	}
//
//	if(bit & e_buf_clr)
//	{
//		device_buf_base::ptr buf(new device_common_buf);
//		this->buf_clr_ = buf;
//		buf->resize_buf_sz(vertex_count * sizeof(vertex_color_t));
//	}
//
//	if(bit & e_buf_nml)
//	{
//		device_buf_base::ptr buf(new device_common_buf);
//		this->buf_nml_ = buf;
//		buf->resize_buf_sz(vertex_count * sizeof(vertex_normal_t));
//	}
//
//	if(bit & e_buf_tc0)
//	{
//		device_buf_base::ptr buf(new device_common_buf);
//		this->buf_texcoord0_ = buf;
//		buf->resize_buf_sz(vertex_count * sizeof(vertex_texcoord_t));
//	}
//
//	if(bit & e_buf_tc1)
//	{
//		device_buf_base::ptr buf(new device_common_buf);
//		this->buf_texcoord1_ = buf;
//		buf->resize_buf_sz(vertex_count * sizeof(vertex_texcoord_t));
//	}
//}

void vertex_buf::deep_copy(const vertex_buf& vb)
{
	if(!vb.buf_pos_.is_null())
	{
		this->buf_pos_ = device_buf_base::ptr(new device_common_buf);
		this->buf_pos_->deep_copy(vb.buf_pos_);
	}

	if(!vb.buf_clr_.is_null())
	{
		this->buf_clr_ = device_buf_base::ptr(new device_common_buf);
		this->buf_clr_->deep_copy(vb.buf_clr_);
	}

	if(!vb.buf_texcoord0_.is_null())
	{
		this->buf_texcoord0_ = device_buf_base::ptr(new device_common_buf);
		this->buf_texcoord0_->deep_copy(vb.buf_texcoord0_);
	}

	if(!vb.buf_texcoord1_.is_null())
	{
		this->buf_texcoord1_ = device_buf_base::ptr(new device_common_buf);
		this->buf_texcoord1_->deep_copy(vb.buf_texcoord1_);
	}

	if(!vb.buf_nml_.is_null())
	{
		this->buf_nml_ = device_buf_base::ptr(new device_common_buf);
		this->buf_nml_->deep_copy(vb.buf_nml_);
	}
}

}





