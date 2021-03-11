/**
* @file quaternion.h
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
#ifndef __L3ENG_QUATERNION_H__
#define __L3ENG_QUATERNION_H__

#include "l3_type.h"
#include "l3_math.h"

namespace l3eng{

class matrix4;
class matrix3;
class vector3;

class quaternion
{
public:

	inline quaternion(l3_f32 w = 1.0f,
		l3_f32 x = 0.0f,
		l3_f32 y = 0.0f,
		l3_f32 z = 0.0f):w_(w),
			x_(x),
			y_(y),
			z_(z)
	{}
	
	inline ~quaternion(){}

	inline l3_f32 w()const{ return this->w_; }
	inline l3_f32 x()const{ return this->x_; }
	inline l3_f32 y()const{ return this->y_; }
	inline l3_f32 z()const{ return this->z_; }

	inline void w(l3_f32 f){ this->w_ = f; }
	inline void x(l3_f32 f){ this->x_ = f; }
	inline void y(l3_f32 f){ this->y_ = f; }
	inline void z(l3_f32 f){ this->z_ = f; }

	inline void set_xyzw(l3_f32 fx, l3_f32 fy, l3_f32 fz, l3_f32 fw)
	{
		this->x(fx);
		this->y(fy);
		this->z(fz);
		this->w(fw);
	}

	static void slerp(const quaternion& q1, const quaternion& q2, quaternion& qt, l3_f32 t, l3_f32 epsilon = 0.0001f);

	inline l3_f32 dotproduct(const quaternion& q)const
	{ return (this->x_ * q.x_) + (this->y_ * q.y_) + (this->z_ * q.z_) + (this->w_ * q.w_); }

	inline quaternion operator *(const l3_f32 scale)const
	{
		return quaternion(this->w_ * scale,
			this->x_ * scale,
			this->y_ * scale,
			this->z_ * scale);
	}

	inline quaternion operator +(const quaternion& q)const
	{
		return quaternion(this->w_ + q.w_,
			this->x_ + q.x_,
			this->y_ + q.y_,
			this->z_ + q.z_);
	}

	inline quaternion& quaternion::operator=(const quaternion& other)
	{
		this->x_ = other.x_;
		this->y_ = other.y_;
		this->z_ = other.z_;
		this->w_ = other.w_;
		return *this;
	}

	/* @brief 根据旋转角度与轴,生成一个四元数 */
	inline void from_angle_axis(const l3_f32 degree, const vector3& v)
	{
		float radian = degree * (L3_AR_CONVERT) * 0.5f;

		this->from_radian_axis(radian, v);
	}
	void from_radian_axis(const l3_f32 radian, const vector3& v);

	/* @brief 将某个向量旋转 (q) * (v) * (q^-1) */
	void rotate_vector(const vector3& v_in, vector3& v_out);

	/* @brief 将四元数据转换成变换矩阵 */
	void get_matrix(matrix4& m)const;

	/* @brief 旋转矩阵,转成四元数 */
	void from_matrix(const matrix3& m);

	/* @brief 根据转换后的坐标轴,得到位4元数(本质同from_matrix) */
	void from_axe(const vector3& x, const vector3& y, const vector3& z);

private:

	l3_f32 w_;
	l3_f32 x_;
	l3_f32 y_;
	l3_f32 z_;
};

}

#endif

