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
#ifndef __L3ENG_VECTOR3_H__
#define __L3ENG_VECTOR3_H__

#include "l3_type.h"
#include "l3_math.h"
#include <stdlib.h>
#include "l3_dllexp.h"

namespace l3eng{

class LIN3D_EX_PORT vector3
{
public:

	inline vector3(l3_f32 x, l3_f32 y, l3_f32 z):x_(x),y_(y),z_(z){}
	inline vector3():x_(0),y_(0),z_(0){}

	inline vector3(const vector3& v):x_(v.x_),y_(v.y_),z_(v.z_){}

	inline const l3_f32 x()const{ return this->x_; }
	inline const l3_f32 y()const{ return this->y_; }
	inline const l3_f32 z()const{ return this->z_; }

	inline void x(l3_f32 f){ this->x_ = f; }
	inline void y(l3_f32 f){ this->y_ = f; }
	inline void z(l3_f32 f){ this->z_ = f; }

	inline void xyz(l3_f32 x, l3_f32 y, l3_f32 z)
	{
		this->x_ = x;
		this->y_ = y;
		this->z_ = z;
	}

	/* @brief 归一化向量,返回"长度" */
	inline l3_f32 normalise(l3_f32 * prleng = NULL, l3_f32 epsilon = 0.000000001f)
	{
		l3_f32 leng = l3_sqrt(this->x_ * this->x_ +
			this->y_ * this->y_ +
			this->z_ * this->z_);

		if(leng > epsilon)
		{
			l3_f32 rleng = l3_reciprocal(leng);
			if(prleng)
				*prleng = rleng;

			this->x_ = this->x_ * rleng;
			this->y_ = this->y_ * rleng;
			this->z_ = this->z_ * rleng;
		}
		else
		{
			if(prleng)
				*prleng = l3_reciprocal(epsilon);
		}

		return leng;
	}

	inline l3_f32 get_length_square()const
	{
		l3_f32 leng = (this->x_ * this->x_ +
			this->y_ * this->y_ +
			this->z_ * this->z_);

		return leng;
	}

	inline l3_f32 get_length()const
	{
		l3_f32 leng = (this->x_ * this->x_ +
			this->y_ * this->y_ +
			this->z_ * this->z_);

		return l3_sqrt(leng);
	}

	/*@brief 得到一个与this,以及 v都垂直的向量v_out,(右手法则,垂直向量v_out的方向为, this绕向 v,大拇指的方向)
	每个分量都可以认为是另外两个分量夹角sin值,并且垂直对另个两个分量 如果x垂直于yz
	*/
	inline void cross_product(const vector3& v, vector3& v_out)const
	{
		v_out.x_ = this->y_ * v.z_ - this->z_ * v.y_;
		v_out.y_ = this->z_ * v.x_ - this->x_ * v.z_;
		v_out.z_ = this->x_ * v.y_ - this->y_ * v.x_;
	}

	inline l3_f32 dot_product(const vector3& v)const
	{
		return (this->x_ * v.x_ + this->y_ * v.y_ + this->z_ * v.z_);
	}

	inline l3_f32 abs_dot_product(const vector3& v)const
	{
		return (l3_fabs(this->x_ * v.x_) + l3_fabs(this->y_ * v.y_) + l3_fabs(this->z_ * v.z_));
	}

	inline void assign(const vector3& v)
	{
		this->x_ = v.x_;
		this->y_ = v.y_;
		this->z_ = v.z_;
	}

	inline l3_bool xyz_lager(const vector3& v)const
	{
		if(this->x_ > v.x_ && this->y_ > v.y_ && this->z_ > v.z_)
			return L3_TRUE;

		return L3_FALSE;
	}

	/* @brief 向量减法 */
	inline void sub(const vector3& v, vector3& v_out)const
	{
		v_out.x_ = this->x_ - v.x_;
		v_out.y_ = this->y_ - v.y_;
		v_out.z_ = this->z_ - v.z_;
	}
	inline void sub(const vector3& v)
	{
		this->x_ -= v.x_;
		this->y_ -= v.y_;
		this->z_ -= v.z_;
	}

	inline vector3 operator -(const vector3& v)const
    {
        vector3 vret;
        this->sub(v, vret);
        return vret;
    }

    inline vector3 operator +(const vector3& v)const
    {
        vector3 vret;

		vret.x_ = this->x_ + v.x_;
		vret.y_ = this->y_ + v.y_;
		vret.z_ = this->z_ + v.z_;

        return vret;
    }

	inline vector3& operator +=(const vector3& v)
	{
		this->x_ = this->x_ + v.x_;
		this->y_ = this->y_ + v.y_;
		this->z_ = this->z_ + v.z_;

        return *this;
	}

	inline void mult(l3_f32 scale)
	{
		this->x_ *= scale;
		this->y_ *= scale;
		this->z_ *= scale;
	}
    
    inline vector3 operator *(l3_f32 scale)const
    {
        vector3 vret;

		vret.x_ = this->x_ * scale;
		vret.y_ = this->y_ * scale;
		vret.z_ = this->z_ * scale;

        return vret;
    }

    inline vector3& operator *=(l3_f32 scale)
    {
        this->mult(scale);
        return *this;
    }

	inline vector3& operator=(const vector3& v)
	{
		this->x_ = v.x_;
		this->y_ = v.y_;
		this->z_ = v.z_;

		return *this;
	}

	inline l3_bool operator==(const vector3& v)const
	{
		return (this->x_ == v.x_ && this->y_ == v.y_ && this->z_ == v.z_);
	}

	inline l3_bool operator!=(const vector3& v)const
	{
		return (this->x_ != v.x_ || this->y_ != v.y_ || this->z_ != v.z_);
	}

	void get_up_left(vector3& up, vector3& left)const;
	void get_up_right(vector3& up, vector3& right)const;

	/* @brief 当前向量的每个值都大于/小于被比较的向量v */
	inline l3_bool greater_than(const vector3& v)const
	{ return (this->x_ > v.x_ && this->y_ > v.y_ && this->z_ > v.z_); }
	inline l3_bool less_than(const vector3& v)const
	{ return (this->x_ < v.x_ && this->y_ < v.y_ && this->z_ < v.z_); }

private:

	l3_f32 x_;
	l3_f32 y_;
	l3_f32 z_;
};

extern const vector3 __vector3_x_;
extern const vector3 __vector3_y_;
extern const vector3 __vector3_z_;
extern const vector3 __vector3_zero_;

}

#endif
