/**
* @file quaternion.cpp
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
#include "quaternion.h"
#include "matrix4.h"
#include "matrix3.h"
#include "l3_vector3.h"

namespace l3eng{

/* @brief 将四元数据转换成变换矩阵 */
void quaternion::get_matrix(matrix4& m_dest)const
{
	//可优化
	l3_f32 * m = m_dest.get_mutable_matrix_data();
	m[0] = 1.0f - 2.0f * this->y_ * this->y_ - 2.0f * this->z_ * this->z_;
	m[4] = 2.0f * this->x_ * this->y_ + 2.0f * this->z_ * this->w_;
	m[8] = 2.0f * this->x_ * this->z_ - 2.0f * this->y_ * this->w_;
	m[12] = 0.0f;

	m[1] = 2.0f* this->x_ * this->y_ - 2.0f * this->z_ * this->w_;
	m[5] = 1.0f - 2.0f * this->x_ * this->x_ - 2.0f * this->z_ * this->z_;
	m[9] = 2.0f * this->z_ * this->y_ + 2.0f * this->x_ * this->w_;
	m[13] = 0.0f;

	m[2] = 2.0f * this->x_ * this->z_ + 2.0f * this->y_ * this->w_;
	m[6] = 2.0f * this->z_ * this->y_ - 2.0f * this->x_ * this->w_;
	m[10] = 1.0f - 2.0f* this->x_ * this->x_ - 2.0f * this->y_ * this->y_;
	m[14] = 0.0f;

	m[3] = 0.f;
	m[7] = 0.f;
	m[11] = 0.f;
	m[15] = 1.f;
}

#if 0
void quaternion::from_matrix(const matrix3& m)
{//todo is wrong for x
	/*
	* 以下引自:3D.Math.Primer.for.Graphics.and.Game.Development
	* 10.6.4 矩阵转换成四元数
	* 附:
	* 四元数转成旋转矩阵:
	* 1-2*y^2-2*z^2     2*x*y            2*x*z-2*w*y
	* 2*x*y             1-2*x^2-2*z^2    2*y*z+2*w*z
	* 2*x*z+2*w*y       2*y*z-2*w*z      1-2*x^2-2*y^2
	* 注:代码中描述的矩阵是此矩阵的转置矩阵,见函数 quaternion::get_matrix
	*
	* 另:
	* x^2 + y^2 + z^2 + w^2 = 1 -- 式1
	* 可以得出
	* tr = 4w^2 - 1
	* m00 - m11 - m22 = 4*x^2 - 1  -- 式2
	* -m00+ m11 + m22 = 4*y^2 - 1  -- 式3
	* -m00- m11 + m22 = 4*z^2 - 1  -- 式4
	* m01  + m10 = 4*x*y 以此类推... -- 式x
	* 取式1..4 中任意一个求出 某个分量(如w),就可以利关于对角线对称位置元素,求出 x y z
	* 4个分量不能全由式1..4式求出,因为开方有正负,无法判断.
	* 但可以利用其求一个,并取其正值,因为四元数q=-q
	*/

	l3_f32 tr = m[0][0] + m[1][1] + m[2][2]+ 1.0f;

	if(tr > 0.0f)
	{
		/* 利用式1求出w
		* 利用 式x 求出x y z 
		*/
		ant_f32 root = ant_sqrt(tr);/* root=2*w */

		this->w_ = 0.5f * root;
		ant_f32 scale =  (1 / (4.0f * this->w_));

		this->x_ = (m[2][1] - m[1][2]) * scale;
		this->y_ = (m[0][2] - m[2][0]) * scale;
		this->z_ = (m[1][0] - m[0][1]) * scale;
	}
	else
	{
		/* 由哪个对角线元素最大,决定求哪个分量 */
		if(m[0][0] > m[1][1] && m[0][0] > m[2][2])
		{
			ant_f32 root = ant_sqrt(m[0][0] - m[1][1] - m[2][2] + 1.0f); /* 2*x */

			this->x_ = 0.5f * root;
			ant_f32 scale = (1 / (4.0f * this->x_));

			this->y_ = (m[1][0] + m[0][1]) * scale;
			this->z_ = (m[2][0] + m[0][2]) * scale;
			this->w_ = (m[2][1] - m[1][2]) * scale;;
		}
		else if(m[1][1] > m[2][2])
		{
			ant_f32 root = ant_sqrt(m[1][1] - m[0][0] - m[2][2] + 1.0f); /* 2*y */

			this->y_ = 0.5f * root;
			ant_f32 scale = (1 / (4.0f * this->y_));

			this->x_ = (m[1][0] + m[0][1]) * scale;
			this->z_ = (m[2][1] + m[1][2]) * scale;
			this->w_ = (m[0][2] - m[2][0]) * scale;
		}
		else
		{
			ant_f32 root = ant_sqrt(m[2][2] - m[2][2] - m[2][2] + 1.0f); /* 2*z */

			this->z_ = 0.5f * root;
			ant_f32 scale = (1 / (4.0f * this->z_));

			this->x_ = (m[2][0] + m[0][2]) * scale;
			this->y_ = (m[2][1] + m[1][2]) * scale;
			this->w_ = (m[1][0] - m[0][1]) * scale;
		}
	}
}

void quaternion::from_axe(const vector3& x, const vector3& y, const vector3& z)
{
	matrix3 m;

	m[0][0] = x.x();
	m[1][0] = x.y();
	m[2][0] = x.z();

	m[0][1] = y.x();
	m[1][1] = y.y();
	m[2][1] = y.z();

	m[0][2] = z.x();
	m[1][2] = z.y();
	m[2][2] = z.z();

	this->from_matrix(m);
}
#endif

void quaternion::slerp(const quaternion& q1, const quaternion& q2, quaternion& qt, l3_f32 t, l3_f32 epsilon)
{
	/* 点乘,得到夹解的余弦值(可以从二维向量推导,四维向量)
	*
	* 如何理解四维向量点乘:
	*
	* 设有二维单位向量A = [cos(a), sin(a)]; B = [cos(b), sin(b)]; 
	* 可以从二维向量很容易得到两个单位向量乘积为夹角c的cos(c) = cos(a)*cos(b) + sin(a)*sin(b) = cos(a-b) 三角公式
	* 同样,二维可以向三维推广
	* 可以假设坐标轴旋转,产分了分量,即A在三维坐标系当中
	* A = [cos(a), sin(a) * cos(r), sin(a) * sin(r)]
	* B = [cos(b), sin(b) * cos(r), sin(b) * sin(r)]
	* A 和 B的点乘:
	* cos(a) * cos(b) + sin(a) * sin(b) * cos(r) * cos(r) + sin(a) * sin(b) * sin(r) * sin(r);
	* 由于cos(r) * cos(r) + sin(r) * sin(r) = 1;
	* 结果仍然是cos(a-b)
	* 扩展到三维空间的情形,其实很容易理解
	* 
	* 同的道理,扩展的四维空间,在做点乘的时候,同样会产生类似cos(r) * cos(r) + sin(r) * sin(r)的项.
	* 从四元数的定义,可以认为多出了一个w轴,如同z轴一样,w分量认为是绕z轴旋转产生的,就可以依据z轴分量产生的,
	* 来推断四维空间的点乘意义,四维空间的w轴与 x y z都垂直
	* 
	* 从这点出发,n维空间的单位向量点乘,都可以认为n维向量的夹角余弦.
	* 估且从代数上这么理解
	*
	* 关于向量球面插值,从2维向量,扩展到3维向量,再扩展到4维向量,也可以类似的方法向理解
	*/

	l3_f32 angle_cos = q1.dotproduct(q2);
	quaternion q2_fix;

	l3_f32 scale;
	l3_f32 invscale;

	if(angle_cos < 0.0f)
	{
		angle_cos = -angle_cos;
		q2_fix.set_xyzw(-q2.x(), -q2.y(), -q2.z(), -q2.w());
	}
	else
	{
		q2_fix.set_xyzw(q2.x(), q2.y(), q2.z(), q2.w());
	}

	if(l3_fabs(angle_cos) < (1.0f - epsilon))
	{
		/* 4维球面插值 */
		l3_f32 angle = acosf(angle_cos);
		l3_f32 inv_sin = l3_reciprocal(sinf(angle));
		scale = sinf(angle * (1.0f - t)) * inv_sin;
		invscale = sinf(angle * t) * inv_sin;
	}
	else
	{
		/* 夹角太小了,线性插值 */
		scale = 1.0f - t;
		invscale = t;
		/* 应该还要归一化,由于夹角很小,可以忽略误差 */
	}

	qt = (q1 * scale) + (q2_fix * invscale);
}

///* @brief 根据旋转角度与轴,生成一个四元数 */
//void quaternion::from_angle_axis(ant_f32 degree, const vector3& v)
//{
//	float radian = degree * (ANT_AR_CONVERT) * 0.5f;
//
//	ant_f32 angle_cos = cos(radian);
//	ant_f32 angle_sin = sin(radian);
//
//	this->w_ = angle_cos;
//	this->x_ = angle_sin * v.x();
//	this->y_ = angle_sin * v.y();
//	this->z_ = angle_sin * v.z();
//}
void quaternion::from_radian_axis(const l3_f32 radian, const vector3& v)
{
	// from ogre
	// assert:  axis[] is unit length
	//
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	l3_f32 angle_cos = cos(radian);
	l3_f32 angle_sin = sin(radian);

	this->w_ = angle_cos;
	this->x_ = angle_sin * v.x();
	this->y_ = angle_sin * v.y();
	this->z_ = angle_sin * v.z();
}

/* @brief 将某个向量旋转 (q) * (v) * (q^-1)
* v可以认为是一个 w=0的四元数
*/
void quaternion::rotate_vector(const vector3& v, vector3& v_out)
{
	/* 
	* 以下代码来自ogre/irrlicht
	* ogre irrlicht 此处代码一样,连注释都一样啊...
	*/

	// nVidia SDK implementation 
	vector3 uv;
	vector3 uuv;
	vector3 qvec(this->x_, this->y_, this->z_);
	qvec.cross_product(v, uv);
	qvec.cross_product(uv, uuv);
	uv *= (2.0f * this->w_);
	uuv *= 2.0f;

	v_out = (v + uv + uuv);
}

}












