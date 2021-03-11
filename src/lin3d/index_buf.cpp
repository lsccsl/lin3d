/**
* @file ant_index_buf.cpp
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
#include "index_buf.h"

namespace l3eng{

l3_int32 index_buf::add_index_i32(l3_int32 t)
{
	if(this->idx_buf_.is_null())
		this->idx_buf_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->idx_buf_->get_buf_sz();
	this->idx_buf_->resize_buf_sz(old_sz + sizeof(l3_int32));

	l3_int32 * ibuf = (l3_int32 *)( ((l3_uint8 *)(this->idx_buf_->get_buf())) + old_sz );
	*ibuf = t;

	this->idx_count_ ++;

	return 0;
}

l3_int32 index_buf::add_3index_i32(l3_int32 i1, l3_int32 i2, l3_int32 i3)
{
	if(this->idx_buf_.is_null())
		this->idx_buf_ = device_buf_base::ptr(new device_common_buf);

	l3_int32 old_sz = this->idx_buf_->get_buf_sz();
	this->idx_buf_->resize_buf_sz(old_sz + 3 * sizeof(l3_int32));

	l3_int32 * ibuf = (l3_int32 *)( ((l3_uint8 *)(this->idx_buf_->get_buf())) + old_sz );
	ibuf[0] = i1;
	ibuf[1] = i2;
	ibuf[2] = i3;

	this->idx_count_ += 3;

	return 0;
}

const l3_uint32 index_buf::recal_idx_count()
{
	this->idx_count_ = this->idx_buf_->get_buf_sz() / sizeof(l3_uint32);
	return this->idx_count_;
}

const l3_uint32 index_buf::get_index(l3_uint32 i)const
{
	if(i > this->idx_count_)
		return -1;

	l3_uint32 * idx = (l3_uint32 *)(this->idx_buf_->get_buf());
	return idx[i];
}

void index_buf::deep_copy(const index_buf& ib)
{
	if(ib.idx_buf_.is_null())
		return;

	this->idx_buf_ = device_buf_base::ptr(new device_common_buf);

	this->idx_buf_->deep_copy(ib.idx_buf_);
	this->idx_type_ = ib.idx_type_;

	this->recal_idx_count();
}

}
