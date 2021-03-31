/**
* @file vector3.h
*
* @author lin shao chuan (email:czhama@163.com)
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
#ifndef __L3ENG_VECTOR2_H__
#define __L3ENG_VECTOR2_H__

#include "l3_type.h"
#include "l3_math.h"
#include <stdlib.h>
#include "l3_dllexp.h"

namespace l3eng {

class LIN3D_EX_PORT vector2
{
public:

	inline vector2(l3_f32 x, l3_f32 y){
		this->v2_.x_ = x;
		this->v2_.y_ = y;
	}
	inline vector2() {
		this->v2_.x_ = 0;
		this->v2_.y_ = 0;
	}

	const l3_f32* get_vector2_data()const {
		return this->v_array_;
	}
	l3_uint32 get_vector2_data_sz()const {
		return sizeof(v_array_);
	}
	static l3_uint32 vector2_data_sz()
	{
		return sizeof(((vector2*)NULL)->v_array_);
	}


private:

	union
	{
		struct vec2
		{
			l3_f32 x_;
			l3_f32 y_;
		};
		vec2 v2_;
		l3_f32 v_array_[2];
	};
};

}

#endif
