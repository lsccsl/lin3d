/**
* @file vertex_buf.h
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
#ifndef __L3ENG_VERTEX_BUF_H__
#define __L3ENG_VERTEX_BUF_H__

#include <stdlib.h>
#include <map>
#include <vector>
#include "l3_type.h"
#include "device_buf.h"
#include "l3_vector3.h"

namespace l3eng{
	
enum{
	e_buf_pos = 0x01,
	e_buf_clr = 0x02,
	e_buf_tc0 = 0x04,
	e_buf_tc1 = 0x08,
	e_buf_nml = 0x10,
};

/* @brief */
class vertex_buf
{
public:

	void clear()
	{
		clear_buf_pos();
		clear_buf_clr();
		clear_buf_nml();
		clear_texcoord0_pos();
		clear_texcoord1_pos();
	}

	/* @brief 顶点位置 */
	const device_buf_base::ptr& buf_pos() const
	{ return this->buf_pos_; }
	inline l3_int32 add_vertex_pos(const vertex_pos_t& pos)
	{ return add_vertex_pos(pos.x_, pos.y_, pos.z_); }
	l3_int32 add_vertex_pos(const l3_f32 x, const l3_f32 y, const l3_f32 z);

	inline l3_int32 set_vertex_pos(const l3_uint32 idx, const vertex_pos_t& pos)
	{ return set_vertex_pos(idx, pos.x_, pos.y_, pos.z_); }
	l3_int32 set_vertex_pos(const l3_uint32 idx, const l3_f32 x, const l3_f32 y, const l3_f32 z);

	l3_int32 get_vertex_pos(l3_uint32 i, vector3& pos)const;
	l3_int32 get_vertex_pos(l3_uint32 i, vertex_pos_t& pos)const;
	inline l3_uint32 get_vertex_count()const
	{
		if(this->buf_pos_.is_null())
			return 0;
		
		return this->buf_pos_->get_buf_sz()/sizeof(vertex_pos_t);
	}
	inline void clear_buf_pos()
	{
		if(!this->buf_pos_.is_null())
			this->buf_pos_->clear();
	}


	/* @brief 顶点颜色 */
	const device_buf_base::ptr& buf_clr() const
	{ return this->buf_clr_; }
	l3_int32 add_vertex_clr(const vertex_color_t& clr)
	{ return add_vertex_clr(clr.r_, clr.g_, clr.b_, clr.a_); }
	l3_int32 add_vertex_clr(const l3_uint8 r, const l3_uint8 g, const l3_uint8 b, const l3_uint8 a);
	l3_int32 get_vertex_clr(l3_uint32 i, vertex_color_t& clr)const;
	inline l3_uint32 get_clr_count()const
	{
		if(this->buf_clr_.is_null())
			return 0;
		
		return this->buf_clr_->get_buf_sz()/sizeof(vertex_color_t);
	}
	inline void clear_buf_clr()
	{
		if(!this->buf_clr_.is_null())
			this->buf_clr_->clear();
	}


	/* @brief 法线 */
	const device_buf_base::ptr& buf_nml() const
	{ return this->buf_nml_; }
	l3_int32 add_vertex_nml(const vertex_normal_t& nml)
	{ return add_vertex_nml(nml.x_, nml.y_, nml.z_); }
	l3_int32 add_vertex_nml(const l3_f32 x, const l3_f32 y, const l3_f32 z);

	l3_int32 set_vertex_nml(l3_uint32 i, const l3_f32 x, const l3_f32 y, const l3_f32 z);

	l3_int32 get_vertex_nml(l3_uint32 i, vertex_normal_t& nml)const;
	l3_int32 get_vertex_nml(l3_uint32 i, vector3& nml)const;
	inline l3_uint32 get_nml_count()const
	{
		if(this->buf_nml_.is_null())
			return 0;

		return this->buf_nml_->get_buf_sz()/sizeof(vertex_normal_t);
	}
	inline void clear_buf_nml()
	{
		if(!this->buf_nml_.is_null())
			this->buf_nml_->clear();
	}


	/* @brief 添加纹理映射0 */
	const device_buf_base::ptr& buf_texcoord0() const
	{ return this->buf_texcoord0_; }
	l3_int32 add_texcoord0(const vertex_texcoord_t& tc)
	{ return add_texcoord0(tc.u_, tc.v_); }
	l3_int32 add_texcoord0(const l3_f32 u, const l3_f32 v);
	inline l3_uint32 get_texcoord0_count()const
	{
		if(this->buf_texcoord0_.is_null())
			return 0;
		
		return this->buf_texcoord0_->get_buf_sz()/sizeof(vertex_color_t);
	}
	inline void clear_texcoord0_pos()
	{
		if(!this->buf_texcoord0_.is_null())
			this->buf_texcoord0_->clear();
	}

	/* @brief 添加纹理映射1 */
	const device_buf_base::ptr& buf_texcoord1() const
	{ return this->buf_texcoord1_; }
	l3_int32 add_texcoord1(const vertex_texcoord_t& tc)
	{ return add_texcoord1(tc.u_, tc.v_); }
	l3_int32 add_texcoord1(const l3_f32 u, const l3_f32 v);
	inline l3_uint32 get_texcoord1_count()const
	{
		if(this->buf_texcoord1_.is_null())
			return 0;
		
		return this->buf_texcoord1_->get_buf_sz()/sizeof(vertex_color_t);
	}
	inline void clear_texcoord1_pos()
	{
		if(!this->buf_texcoord1_.is_null())
			this->buf_texcoord1_->clear();
	}


	//void alloc_buf(const l3_int32 vertex_count, l3_uint64 bit);

	void deep_copy(const vertex_buf& vb);

private:

	device_buf_base::ptr buf_pos_;
	device_buf_base::ptr buf_clr_;
	device_buf_base::ptr buf_nml_;
	device_buf_base::ptr buf_texcoord0_;
	mutable device_buf_base::ptr buf_texcoord1_;
};

}

#endif
