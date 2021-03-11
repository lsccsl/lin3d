/**
* @file l3_vector4.h
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
#ifndef __L3ENG_VECTOR4_H__
#define __L3ENG_VECTOR4_H__

#include "l3_dllexp.h"
#include "l3_type.h"

namespace l3eng{

class LIN3D_EX_PORT vector4
{
public:

	inline vector4(l3_f32 x, l3_f32 y, l3_f32 z, l3_f32 w):x_(x),y_(y),z_(z),w_(w){}
	inline vector4():x_(0),y_(0),z_(0),w_(0){}

	inline const l3_f32 x()const{ return this->x_; }
	inline const l3_f32 y()const{ return this->y_; }
	inline const l3_f32 z()const{ return this->z_; }
	inline const l3_f32 w()const{ return this->w_; }

	inline void x(l3_f32 f){ this->x_ = f; }
	inline void y(l3_f32 f){ this->y_ = f; }
	inline void z(l3_f32 f){ this->z_ = f; }
	inline void w(l3_f32 f){ this->w_ = f; }

	static inline l3_f32 dot_product(const vector4& v1, const vector4& v2)
	{
		return (
			v1.x_ * v2.x_ +
			v1.y_ * v2.y_ +
			v1.z_ * v2.z_ +
			v1.w_ * v2.w_
		);
	}

	inline vector4 operator *(l3_f32 scale)const
	{
		vector4 vret;

		vret.x_ = this->x_ * scale;
		vret.y_ = this->y_ * scale;
		vret.z_ = this->z_ * scale;
		vret.w_ = this->w_ * scale;

		return vret;
	}

	inline vector4& operator=(const vector4& v)
	{
		this->x_ = v.x_;
		this->y_ = v.y_;
		this->z_ = v.z_;
		this->w_ = v.w_;

		return *this;
	}

private:

	l3_f32 x_;
	l3_f32 y_;
	l3_f32 z_;
	l3_f32 w_;
};

}

#endif
