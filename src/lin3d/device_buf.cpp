/**
* @file device_buf.cpp
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
#include "device_buf.h"

namespace l3eng
{

/* @brief Éî¿½±´»º³åÇø */
void device_buf_base::deep_copy(const device_buf_base::ptr& buf_src)
{
	this->resize_buf_sz(buf_src->get_buf_sz());

	memcpy(this->get_buf(), buf_src->get_buf(), this->get_buf_sz());
}

device_common_buf::~device_common_buf()
{}

void * device_common_buf::get_buf()
{
#ifdef VECTOR_BUF
	return &this->buf_[0];
#else
	return this->pbuf_;
#endif
}

l3_int32 device_common_buf::get_buf_sz()
{
#ifdef VECTOR_BUF
	return (l3_int32)this->buf_.size();
#else
	return this->buf_sz_;
#endif
}

void device_common_buf::resize_buf_sz(l3_uint32 sz)
{
#ifdef VECTOR_BUF
	if(sz <= this->buf_.size())
		return;
	l3_int32 old_sz = (l3_int32)this->buf_.size();
	this->buf_.resize(sz);
	memset(&this->buf_[old_sz], 0xcd, sz - old_sz);
#else
	if(sz <= this->buf_sz_)
		return;
	l3_int32 old_sz = this->buf_sz_;
	if(sz <= this->buf_cap_)
	{
		this->buf_sz_ = sz;
	}
	else
	{
		this->buf_cap_ = sz * 2;
		l3_uint8 * ptmp = (l3_uint8 *)malloc(this->buf_cap_);
		memcpy(ptmp, this->pbuf_, this->buf_sz_);
		this->pbuf_ = ptmp;

		this->buf_sz_ = sz;
	}
	memset(&this->pbuf_[old_sz], 0xcd, sz - old_sz);
#endif
}

l3_int32 device_common_buf::get_buf_cap()
{
#ifdef VECTOR_BUF
	return (l3_int32)this->buf_.capacity();
#else
	return this->buf_cap_;
#endif
}



}
