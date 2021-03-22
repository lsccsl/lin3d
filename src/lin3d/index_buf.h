/**
* @file index_buf.h
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
#ifndef __L3ENG_INDEX_BUF_H__
#define __L3ENG_INDEX_BUF_H__

#include <stdlib.h>
#include "device_buf.h"

namespace l3eng{

enum e_idx_type{
	E_ITYPE_POINTS,
	E_ITYPE_LINES,
	E_ITYPE_TRIANGLES,
};

class index_buf
{
public:

	index_buf():idx_count_(0),idx_type_(E_ITYPE_TRIANGLES){}

	virtual ~index_buf(){}

	//void set_buf(device_buf_base::ptr idx_ptr, l3_int32 idx_count = -1){

	//	this->idx_buf_ = idx_ptr;

	//	if(-1 != idx_count)
	//	{
	//		this->idx_count_ = idx_count;
	//		return;
	//	}

	//	this->idx_count_ = this->idx_buf_->get_buf_sz() / sizeof(l3_uint32);
	//}

	inline device_buf_base::ptr get_buf()const{ return idx_buf_; }

	inline void idx_count(l3_uint32 c){
		l3_uint32 real_count = this->recal_idx_count();
		if(c > real_count)
			this->idx_count_ = real_count;
		else
			this->idx_count_ = c;

	}
	inline const l3_uint32 idx_count()const{ return this->idx_count_; }
	const l3_uint32 recal_idx_count();

	inline void clear(){
		if(!this->idx_buf_.is_null())
			this->idx_buf_->clear();
		this->idx_count_ = 0;
	}

	l3_bool buf_is_null()const
	{ return this->idx_buf_.is_null(); }

	l3_int32 add_index_i32(l3_int32 t);
	l3_int32 add_3index_i32(l3_int32 i1, l3_int32 i2, l3_int32 i3);

	const l3_uint32 get_index(l3_uint32 i)const;

	inline const e_idx_type idx_type()const{ return this->idx_type_; }
	inline void idx_type(const e_idx_type itype){ this->idx_type_ = itype; }

	void deep_copy(const index_buf& ib);

private:

	device_buf_base::ptr idx_buf_;

	l3_uint32 idx_count_;

	e_idx_type idx_type_;
};

}

#endif
