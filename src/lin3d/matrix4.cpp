/**
* @file matrix4.cpp
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
#include "matrix4.h"
#include <string.h>
#include <math.h>
#include <sstream>
#include "l3_math.h"
#include "quaternion.h"
#include "plane.h"
#include "l3_vector4.h"

namespace l3eng{

matrix4 matrix4::_identity_(1);

matrix4::matrix4(l3_int32 b_identity)
{
	if(!b_identity)
	{
		memset(this->m_, 0, sizeof(this->m_));
		return;
	}

	this->m_[0] = 1;
	this->m_[1] = 0;
	this->m_[2] = 0;
	this->m_[3] = 0;

	this->m_[4] = 0;
	this->m_[5] = 1;
	this->m_[6] = 0;
	this->m_[7] = 0;

	this->m_[8] = 0;
	this->m_[9] = 0;
	this->m_[10] = 1;
	this->m_[11] = 0;

	this->m_[12] = 0;
	this->m_[13] = 0;
	this->m_[14] = 0;
	this->m_[15] = 1;
}
matrix4::matrix4(l3_f32 m0,
	l3_f32 m1,
	l3_f32 m2,
	l3_f32 m3,
	l3_f32 m4,
	l3_f32 m5,
	l3_f32 m6,
	l3_f32 m7,
	l3_f32 m8,
	l3_f32 m9,
	l3_f32 m10,
	l3_f32 m11,
	l3_f32 m12,
	l3_f32 m13,
	l3_f32 m14,
	l3_f32 m15)
{
	this->m_[0] = m0;
	this->m_[1] = m1;
	this->m_[2] = m2;
	this->m_[3] = m3;

	this->m_[4] = m4;
	this->m_[5] = m5;
	this->m_[6] = m6;
	this->m_[7] = m7;

	this->m_[8] = m8;
	this->m_[9] = m9;
	this->m_[10] = m10;
	this->m_[11] = m11;

	this->m_[12] = m12;
	this->m_[13] = m13;
	this->m_[14] = m14;
	this->m_[15] = m15;
}

/* @brief 生成缩放矩阵 */
void matrix4::gen_scale_matrix(matrix4& m, l3_f32 x_scale, l3_f32 y_scale, l3_f32 z_scale)
{
	m.m_[0] = x_scale;
	m.m_[1] = 0;
	m.m_[2] = 0;
	m.m_[3] = 0;

	m.m_[4] = 0;
	m.m_[5] = y_scale;
	m.m_[6] = 0;
	m.m_[7] = 0;

	m.m_[8] = 0;
	m.m_[9] = 0;
	m.m_[10] = z_scale;
	m.m_[11] = 0;

	m.m_[12] = 0;
	m.m_[13] = 0;
	m.m_[14] = 0;
	m.m_[15] = 1;
}

/* @brief 从当前矩阵中获取x y z的缩放比例 */
void matrix4::get_matrix_scale(l3_f32& x_scale, l3_f32& y_scale, l3_f32& z_scale)const
{
	// sin(x) cos(x)sin(x) cos(x)cos(x)类似,将所有的分类平方求根就得到了 scale
	// this code from irrlicht
#if 0
	if(l3_is_zero(this->m_[1]) && l3_is_zero(this->m_[2]) &&
		l3_is_zero(this->m_[4]) && l3_is_zero(this->m_[6]) &&
		l3_is_zero(this->m_[8]) && l3_is_zero(this->m_[9]))
	{
		x_scale = this->m_[0];
		y_scale = this->m_[5];
		z_scale = this->m_[10];
		return;
	}
#endif

	x_scale = l3_sqrt(this->m_[0] * this->m_[0] + this->m_[1] * this->m_[1] + this->m_[2] * this->m_[2]);
	y_scale = l3_sqrt(this->m_[4] * this->m_[4] + this->m_[5] * this->m_[5] + this->m_[6] * this->m_[6]);
	z_scale = l3_sqrt(this->m_[8] * this->m_[8] + this->m_[9] * this->m_[9] + this->m_[10] * this->m_[10]);
}

/* @brief 绕z轴旋转,单位角度0-360 */
void matrix4::gen_rotate_z_matrix_radian(l3_f32 radian, matrix4& m, l3_f32 epsilon)
{
	m.m_[0] = cos(radian);
	if(l3_is_zero(m.m_[0], epsilon))
		m.m_[0] = 0.0f;
	m.m_[1] = sin(radian);
	if(l3_is_zero(m.m_[1], epsilon))
		m.m_[1] = 0.0f;
	m.m_[2] = 0;
	m.m_[3] = 0;

	m.m_[4] = -sin(radian);
	if(l3_is_zero(m.m_[4], epsilon))
		m.m_[4] = 0.0f;
	m.m_[5] = cos(radian);
	if(l3_is_zero(m.m_[5], epsilon))
		m.m_[5] = 0.0f;
	m.m_[6] = 0;
	m.m_[7] = 0;

	m.m_[8] = 0;
	m.m_[9] = 0;
	m.m_[10] = 1;
	m.m_[11] = 0;

	m.m_[12] = 0;
	m.m_[13] = 0;
	m.m_[14] = 0;
	m.m_[15] = 1;
}

/* @brief 绕y轴旋转 */
void matrix4::gen_rotate_y_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon)
{
	float radian = degree * (L3_AR_CONVERT);

	m.m_[0] = cos(radian);
	if(l3_is_zero(m.m_[0], epsilon))
		m.m_[0] = 0.0f;
	m.m_[1] = 0;
	m.m_[2] = -sin(radian);
	if(l3_is_zero(m.m_[2], epsilon))
		m.m_[2] = 0.0f;
	m.m_[3] = 0;

	m.m_[4] = 0;
	m.m_[5] = 1;
	m.m_[6] = 0;
	m.m_[7] = 0;

	m.m_[8] = sin(radian);
	if(l3_is_zero(m.m_[8], epsilon))
		m.m_[8] = 0.0f;
	m.m_[9] = 0;
	m.m_[10] = cos(radian);
	if(l3_is_zero(m.m_[10], epsilon))
		m.m_[10] = 0.0f;
	m.m_[11] = 0;

	m.m_[12] = 0;
	m.m_[13] = 0;
	m.m_[14] = 0;
	m.m_[15] = 1;
}

/* @brief 绕x轴旋转 */
void matrix4::gen_rotate_x_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon)
{
	//根据三角函数即可推导出(两角和与差)
	float radian = degree * (L3_AR_CONVERT);

	m.m_[0] = 1;
	m.m_[1] = 0;
	m.m_[2] = 0;
	m.m_[3] = 0;

	m.m_[4] = 0;
	m.m_[5] = cos(radian);
	if(l3_is_zero(m.m_[5], epsilon))
		m.m_[5] = 0.0f;
	m.m_[6] = sin(radian);
	if(l3_is_zero(m.m_[6], epsilon))
		m.m_[6] = 0.0f;

	m.m_[7] = 0;

	m.m_[8] = 0;
	m.m_[9] = -sin(radian);
	if(l3_is_zero(m.m_[9], epsilon))
		m.m_[9] = 0.0f;
	m.m_[10] = cos(radian);
	if(l3_is_zero(m.m_[10], epsilon))
		m.m_[10] = 0.0f;
	m.m_[11] = 0;

	m.m_[12] = 0;
	m.m_[13] = 0;
	m.m_[14] = 0;
	m.m_[15] = 1;
}

void matrix4::get_matrix_rotate_degree(l3_f32& x_degree, l3_f32& y_degree, l3_f32& z_degree)
{
	// https://blog.csdn.net/darlingqiang/article/details/80829666
	// 根据旋转矩阵的规律,求反三角函数即可(反切atan)
	// this code from irrlicht

	x_degree = 0.0f;
	y_degree = 0.0f;
	z_degree = 0.0f;

#if 0
	l3_f32 x_scale = 1.0f;
	l3_f32 y_scale = 1.0f;
	l3_f32 z_scale = 1.0f;
	this->get_matrix_scale(x_scale, y_scale, z_scale);

	// we need to check for negative scale on to axes, which would bring up wrong results
	if (y_scale<0 && z_scale<0)
	{
		y_scale = -y_scale;
		z_scale = -z_scale;
	}
	else if (x_scale<0 && z_scale<0)
	{
		x_scale = -x_scale;
		z_scale = -z_scale;
	}
	else if (x_scale<0 && y_scale<0)
	{
		x_scale = -x_scale;
		y_scale = -y_scale;
	}
	const core::vector3d<f64> invScale(core::reciprocal(scale.X),core::reciprocal(scale.Y),core::reciprocal(scale.Z));

	f64 Y = -asin(core::clamp(mat[2]*invScale.X, -1.0, 1.0));
	const f64 C = cos(Y);
	Y *= RADTODEG64;

	f64 rotx, roty, X, Z;

	if (!core::iszero(C))
	{
		const f64 invC = core::reciprocal(C);
		rotx = mat[10] * invC * invScale.Z;
		roty = mat[6] * invC * invScale.Y;
		X = atan2( roty, rotx ) * RADTODEG64;
		rotx = mat[0] * invC * invScale.X;
		roty = mat[1] * invC * invScale.X;
		Z = atan2( roty, rotx ) * RADTODEG64;
	}
	else
	{
		X = 0.0;
		rotx = mat[5] * invScale.Y;
		roty = -mat[4] * invScale.Y;
		Z = atan2( roty, rotx ) * RADTODEG64;
	}

	// fix values that get below zero
	if (X < 0.0) X += 360.0;
	if (Y < 0.0) Y += 360.0;
	if (Z < 0.0) Z += 360.0;

	return vector3d<T>((T)X,(T)Y,(T)Z);
#endif
}

void matrix4::gen_rotate_matrix(const l3_f32 degree,
	const vector3& axle,
	matrix4& m)
{
	quaternion q;
	q.from_angle_axis(degree, axle);

	q.get_matrix(m);
}

/* @brief 生成移动矩阵 */
void matrix4::gen_move_matrix(matrix4& m, l3_f32 x, l3_f32 y, l3_f32 z)
{
	m.m_[0] = 1;
	m.m_[1] = 0;
	m.m_[2] = 0;
	m.m_[3] = 0;

	m.m_[4] = 0;
	m.m_[5] = 1;
	m.m_[6] = 0;
	m.m_[7] = 0;

	m.m_[8] = 0;
	m.m_[9] = 0;
	m.m_[10] = 1;
	m.m_[11] = 0;

	m.m_[12] = x;
	m.m_[13] = y;
	m.m_[14] = z;
	m.m_[15] = 1;
}
void matrix4::move_matrix(matrix4& m, l3_f32 x, l3_f32 y, l3_f32 z)
{
	m.m_[12] = x;
	m.m_[13] = y;
	m.m_[14] = z;
}

/* @brief 生成透视投影矩阵
* @param fovy:视角
* @param aspect:纵横比
* @param z_near:近平面
* @param z_far:远平面
*/
void matrix4::gen_pers_proj_matrix(l3_f32 fovy, l3_f32 aspect, l3_f32 z_near, l3_f32 z_far, matrix4& out_mtx)
{
	/*
	* 假设视截体内的点(x, y, z) 投影到标准视空间里的(x_p, y_p, z_p)
	* 需要把视截体里的点,映身到标准视空间里(-1, -1, 0) (1, 1, 1)
	* 即投影之后 x_p y_p取值为(-1, 1) z_p的取值为(0, 1)
	* 根据相似三角形,投影形成的 x_p y_p肯定与z有关,所以矩阵中需要包含一个齐次空间因子,以便做矩阵乘法后w取值为z
	*
	* 2n/w, 0,    0, 0
	* 0,    2n/h, 0, 0   --- x y只是单纯的缩放(根据相似三角形)
	* 0,    0,    p, q   --- 这两个参数纯粹为了矩阵运算是造出来的,在几何上不存在意义,它们能保证变换后的z_p在(0,1)区间单调递增,并且z会与比例值放大(f/z)倍,这也是很多3d编程书籍中提到的放大倍数
	* 0,    0,    1, 0   --- 1添加齐次空间因子,最终(x, y, z)经过矩阵运算会变成(x_p * z, y_p * z, z_p * z, z),去除齐次坐标因子后得到(x_p, y_p, z_p, 1)
	*
	* ps: w,h为投影矩型宽与高
	*
	* p = f / (f - n)
	* q = - (f * n) / (f - n)
	*
	* 以上是d3d的情况,opengl的标准视空间是 (-1, -1, -1) (1, 1, 1),即 z_p的取值范围是(-1, 1)
	* 并且z是负值,因为opengl默认是位于(0, 0, 0)并且看向z负方向,相应的缩放系数会变成
	*  x_p * (-z) = (2n/w) * x
	*  y_p * (-z) = (2n/h) * y
	*  z_p * (-z) = p_o * z + q_o   --- 这里要做一个变通 z虽然是负的,但解的时候,要假设 z在(n,f) 然后z_p对应(-1,1)
	*  w_p * (-z) = -z
	*
	* 利用w_p 就可以消去 x_p y_p的齐次因子,即z,
	*
	* 解得 p_o = (f+n)/(f-n)  q_o=(-2fn)(f-n),  然后考虑z实际是处于(-f, -n)这个区间,则 p_o要取 -(f+n)/(f-n)
	* 当 z=n时:z_p=-n,w=n   当z=f时:z_p=f,w=f
	* 对应的矩阵
	*
	*  2n/w, 0,    0,   0
	*  0,    2n/h, 0,   0
	*  0,    0,    p_o, -1  --- 这个取-1,是因为齐次坐标因子此时是 -z;
	*  0,    0,    q_o, 0   
	*/

	if(fovy > 179.0f)
		fovy = 179.0f;

	if(fovy < 1.0f)
		fovy = 1.0f;

	if(z_near >= z_far)
		z_far = z_near + 1.0f;

	/* 2n/h 实际上就是ctan(fovy/2) */
	l3_f32 h = l3_reciprocal(tan(fovy * L3_AR_CONVERT * 0.5f));
	l3_f32 w = h / aspect;//aspect不可为零
	l3_f32 p = (z_far + z_near) / (z_near - z_far);//分母不可为零
	l3_f32 q = (2 * z_far * z_near) / (z_near - z_far);//分母不可为零

	out_mtx.m_[0] = w;
	out_mtx.m_[1] = 0;
	out_mtx.m_[2] = 0;
	out_mtx.m_[3] = 0;

	out_mtx.m_[4] = 0;
	out_mtx.m_[5] = h;
	out_mtx.m_[6] = 0;
	out_mtx.m_[7] = 0;

	/*  */
	out_mtx.m_[8] = 0;
	out_mtx.m_[9] = 0;
	out_mtx.m_[10] = p;
	out_mtx.m_[11] = -1;

	out_mtx.m_[12] = 0;
	out_mtx.m_[13] = 0;
	out_mtx.m_[14] = q;
	out_mtx.m_[15] = 0;
}

/* @brief 生成正交投影矩阵
*/
void matrix4::gen_ortho_proj_matrix(l3_f32 view_width,
	l3_f32 view_height,
	l3_f32 near, l3_f32 far,
	matrix4& out_mtx)
{
	/*
	* 2/w    0           0   0
	*   0  2/h           0   0
	*   0    0      2/(n-f)  0
	*   0    0  (f+n)/(n-f)  1
	*
	* 将所有的顶点转换到视空间之后,按比例缩放,
	* z 在 n与f之间,缩放至-1,1
	*/

	/* 按比例缩放即可,z减去near,然后在按far-near缩放 */
	out_mtx.m_[0] = 2.0f / view_width;
	out_mtx.m_[1] = 0;
	out_mtx.m_[2] = 0;
	out_mtx.m_[3] = 0;

	out_mtx.m_[4] = 0;
	out_mtx.m_[5] = 2.0f / view_height;
	out_mtx.m_[6] = 0;
	out_mtx.m_[7] = 0;

	/* opengl是-1,1,擦,不能是0,1吗? */
	out_mtx.m_[8] = 0;
	out_mtx.m_[9] = 0;
	out_mtx.m_[10] = 2.0f / (near - far);
	out_mtx.m_[11] = 0;

	out_mtx.m_[12] = 0;
	out_mtx.m_[13] = 0;
	out_mtx.m_[14] = (far + near) / (near - far);
	out_mtx.m_[15] = 1;
}

/* @brief 生成逆矩阵,利用伴随矩阵法 */
l3_int32 matrix4::gen_inverse_matrix(matrix4& out_mtx)const
{
	/* 利用行列式展开,求出:|A|, 以及伴随矩阵:A*, 逆矩阵即:(A*)/(|A|) */

	/* 求|A| 
	* 先按第一行展开,然后对每个余子式递归地展到,直到2x2行列式 
	* 此处无耻地抄袭了ogre的代码
	* ogre的代码逻辑比irrlicht表述得要清晰
	*/
	l3_f32 m00 = this->mv_[0][0], m01 = this->mv_[0][1], m02 = this->mv_[0][2], m03 = this->mv_[0][3];
	l3_f32 m10 = this->mv_[1][0], m11 = this->mv_[1][1], m12 = this->mv_[1][2], m13 = this->mv_[1][3];
	l3_f32 m20 = this->mv_[2][0], m21 = this->mv_[2][1], m22 = this->mv_[2][2], m23 = this->mv_[2][3];
	l3_f32 m30 = this->mv_[3][0], m31 = this->mv_[3][1], m32 = this->mv_[3][2], m33 = this->mv_[3][3];

	/* 递归展开行列式 最后的2x2行列式 */
	l3_f32 v0 = m20 * m31 - m21 * m30;
	l3_f32 v1 = m20 * m32 - m22 * m30;
	l3_f32 v2 = m20 * m33 - m23 * m30;
	l3_f32 v3 = m21 * m32 - m22 * m31;
	l3_f32 v4 = m21 * m33 - m23 * m31;
	l3_f32 v5 = m22 * m33 - m23 * m32;

	/* 递归展开行列式,3x3 */
	l3_f32 t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
	l3_f32 t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
	l3_f32 t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
	l3_f32 t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

	/* 判断 invDet 是否为0,如果为零,矩阵是奇异矩阵 */
	l3_f32 det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
	if(l3_is_zero(det))
		return -1;

	/* 4x4行列式展开,求出1|A| */
	l3_f32 invDet = 1 / det;

	/* 开始求伴随矩阵,伴随矩阵里的每一项 A(i,j),对应a(i,j)的代数余子式,即一个3x3的行列式,也是通过行列式展开求解 */
	l3_f32 d00 = t00 * invDet;
	l3_f32 d10 = t10 * invDet;
	l3_f32 d20 = t20 * invDet;
	l3_f32 d30 = t30 * invDet;

	l3_f32 d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	l3_f32 d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	l3_f32 d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	l3_f32 d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	l3_f32 d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	l3_f32 d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	l3_f32 d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	l3_f32 d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	l3_f32 d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	l3_f32 d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	l3_f32 d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	l3_f32 d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

#if 1
	static l3_f32 e = 0.00000001f;
	out_mtx.assign(
		l3_fabs(d00) > e ? d00 : 0.0f,
		l3_fabs(d01) > e ? d01 : 0.0f,
		l3_fabs(d02) > e ? d02 : 0.0f,
		l3_fabs(d03) > e ? d03 : 0.0f,

		l3_fabs(d10) > e ? d10 : 0.0f,
		l3_fabs(d11) > e ? d11 : 0.0f,
		l3_fabs(d12) > e ? d12 : 0.0f,
		l3_fabs(d13) > e ? d13 : 0.0f,

		l3_fabs(d20) > e ? d20 : 0.0f,
		l3_fabs(d21) > e ? d21 : 0.0f,
		l3_fabs(d22) > e ? d22 : 0.0f,
		l3_fabs(d23) > e ? d23 : 0.0f,

		l3_fabs(d30) > e ? d30 : 0.0f,
		l3_fabs(d31) > e ? d31 : 0.0f,
		l3_fabs(d32) > e ? d32 : 0.0f,
		l3_fabs(d33) > e ? d33 : 0.0f
		);
#else
	out_mtx.assign(d00, d01, d02, d03,
		d10, d11, d12, d13,
		d20, d21, d22, d23,
		d30, d31, d32, d33);
#endif
	return 0;
}
/* @brief 生成逆矩阵,利用高斯消去法 */
l3_int32 matrix4::gen_inverse_matrix_by_ge(matrix4& out_mtx)
{
	/* 利用矩阵初等变换,得到逆矩阵 */

	return 0;
}

/* @brief 矩阵乘法,this横 * m竖 [x][y] x表示行,y表示列 */
void matrix4::mult(const matrix4& m, matrix4& out_m)const
{
	for(l3_int32 col = 0; col < 4; col ++)
	{
		for(l3_int32 row = 0; row < 4; row ++)
		{
			l3_f32 * ftmp = &(out_m.m_[col * 4 + row]);
			*ftmp = 0;

			for(l3_int32 k = 0; k < 4; k ++)
				*ftmp += this->m_[col * 4 + k] * m.m_[k * 4 + row];
		}
	}
}
/* @brief 与一个3维点相乘,旋转平移 */
void matrix4::mult_v3(const vector3& v, vector3& v_out, l3_bool need_devide_w)const
{
	v_out.x(v.x() * this->mv_[0][0] + v.y() * this->mv_[1][0] + v.z() * this->mv_[2][0] + this->mv_[3][0]);
	v_out.y(v.x() * this->mv_[0][1] + v.y() * this->mv_[1][1] + v.z() * this->mv_[2][1] + this->mv_[3][1]);
	v_out.z(v.x() * this->mv_[0][2] + v.y() * this->mv_[1][2] + v.z() * this->mv_[2][2] + this->mv_[3][2]);

	if(!need_devide_w)
		return;

	l3_f32 w = v.x() * this->mv_[0][3] + v.y() * this->mv_[1][3] + v.z() * this->mv_[2][3] + this->mv_[3][3];
	if(w)
		w = 1.0f / w;

	v_out.mult(w);
}

/* @brief 与一个4维向量相乘 */
void matrix4::mult_v4(const vector4& v, vector4& v_out)
{
	v_out.x(v.x() * this->mv_[0][0] + v.y() * this->mv_[1][0] + v.z() * this->mv_[2][0] + this->mv_[3][0]);
	v_out.y(v.x() * this->mv_[0][1] + v.y() * this->mv_[1][1] + v.z() * this->mv_[2][1] + this->mv_[3][1]);
	v_out.z(v.x() * this->mv_[0][2] + v.y() * this->mv_[1][2] + v.z() * this->mv_[2][2] + this->mv_[3][2]);
	v_out.w(v.x() * this->mv_[0][3] + v.y() * this->mv_[1][3] + v.z() * this->mv_[2][3] + this->mv_[3][3]);
}

/* @brief  */
void matrix4::assign(const matrix4& m)
{
	memcpy(this->m_, m.m_, sizeof(this->m_));
}
void matrix4::assign(l3_f32 m0,
	l3_f32 m1,
	l3_f32 m2,
	l3_f32 m3,
	l3_f32 m4,
	l3_f32 m5,
	l3_f32 m6,
	l3_f32 m7,
	l3_f32 m8,
	l3_f32 m9,
	l3_f32 m10,
	l3_f32 m11,
	l3_f32 m12,
	l3_f32 m13,
	l3_f32 m14,
	l3_f32 m15)
{
	this->m_[0] = m0;
	this->m_[1] = m1;
	this->m_[2] = m2;
	this->m_[3] = m3;

	this->m_[4] = m4;
	this->m_[5] = m5;
	this->m_[6] = m6;
	this->m_[7] = m7;

	this->m_[8] = m8;
	this->m_[9] = m9;
	this->m_[10] = m10;
	this->m_[11] = m11;

	this->m_[12] = m12;
	this->m_[13] = m13;
	this->m_[14] = m14;
	this->m_[15] = m15;
}

/* @brief 将一个点转换 */
void matrix4::trans_vect3(const vector3& v_in, vector3& v_out, const l3_f32 w)const
{
	l3_f32 in_x = v_in.x();
	l3_f32 in_y = v_in.y();
	l3_f32 in_z = v_in.z();

	v_out.x(in_x * this->mv_[0][0] + in_y * this->mv_[1][0] + in_z * this->mv_[2][0] + w * this->mv_[3][0]);
	v_out.y(in_x * this->mv_[0][1] + in_y * this->mv_[1][1] + in_z * this->mv_[2][1] + w * this->mv_[3][1]);
	v_out.z(in_x * this->mv_[0][2] + in_y * this->mv_[1][2] + in_z * this->mv_[2][2] + w * this->mv_[3][2]);
}
/* @brief 转换一个向量,忽略矩阵中的平移分量 */
void matrix4::trans_rotate_vect3(const vector3& v_in, vector3& v_out)const
{
	l3_f32 in_x = v_in.x();
	l3_f32 in_y = v_in.y();
	l3_f32 in_z = v_in.z();

	v_out.x(in_x * this->mv_[0][0] + in_y * this->mv_[1][0] + in_z * this->mv_[2][0]);
	v_out.y(in_x * this->mv_[0][1] + in_y * this->mv_[1][1] + in_z * this->mv_[2][1]);
	v_out.z(in_x * this->mv_[0][2] + in_y * this->mv_[1][2] + in_z * this->mv_[2][2]);
}


/* @brief 根据某个平面,生成反射矩阵 */
void matrix4::gen_reflect_matrix(const plane& p, matrix4& out_reflect_mtx)
{
	/*
	* 参考: https://blog.csdn.net/aa20274270/article/details/52096905 以及unity水反射demo
	* 求某个点point,关于平面p的镜像位置point_ref,得到point在平面上p垂足,延着平面的法线相反的方向同样的距离,即加一个向量即可得到对应的反射镜向位置
	* 根据镜像位置与point_ref与point的坐标变换关系,构造一个变换矩阵即可
	*
	* point坐标 (px, py, pz)  平面归一化法线(nx, ny, nz)
	* point_ref (pr_x, pr_y, pr_z)
	* 
	* point到平面p的距离  dis_p = px*nx + py*ny + pz*nz - d
	* 对应向量v_r = {(dis_p * 2) * nx, (dis_p * 2) * ny, (dis_p * 2) * nz}
	* 则点point_ref = point - v_r
	* 即:
	* pr_x = -(dis_p * 2) * nx + px
	* pr_y = -(dis_p * 2) * ny + py
	* pr_z = -(dis_p * 2) * nz + pz
	*
	* 再把dis_p代入
	* pr_x = -((px*nx + py*ny + pz*nz - d) * 2) * nx + px
	* pr_y = -((px*nx + py*ny + pz*nz - d) * 2) * ny + py
	* pr_z = -((px*nx + py*ny + pz*nz - d) * 2) * nz + pz
	* 
	* 整理得到矩阵:
	* pr_x = -(2*nx*nx - 1)*px - 2*ny*nx*py       - 2*nz*nx*pz       + 2*d*nx
	* pr_y = -2*nx*ny*px       - (2*ny*ny - 1)*py - 2*nz*ny*pz       + 2*d*ny
	* pr_z = -2*nx*nz*px       - 2*ny*nz*py       - (2*nz*nz - 1)*pz + 2*d*nz
	*
	* (注:最后一列+-符号问题与平面定义方式有关) 按np+d=0定义则为负 按np=d定义则为正
	*/

	// 传入的p平面法线必须是归一化的

	l3_f32 nx = p.normal().x();
	l3_f32 ny = p.normal().y();
	l3_f32 nz = p.normal().z();
	l3_f32 d  = p.dist();

#if 0
	//out_reflect_mtx.mv_[0][0] = -(2.0f*nx*nx - 1.0f);
	//out_reflect_mtx.mv_[0][1] = -2.0f*ny*nx;
	//out_reflect_mtx.mv_[0][2] = -2.0f*nz*nx;
	//out_reflect_mtx.mv_[0][3] = 2.0f*d*nx;

	//out_reflect_mtx.mv_[1][0] = -2.0f*nx*ny;
	//out_reflect_mtx.mv_[1][1] = - (2.0f*ny*ny - 1);
	//out_reflect_mtx.mv_[1][2] = - 2.0f*nz*ny;
	//out_reflect_mtx.mv_[1][3] = 2.0f*d*ny;

	//out_reflect_mtx.mv_[2][0] = -2.0f*nx*nz;
	//out_reflect_mtx.mv_[2][1] = - 2.0f*ny*nz;
	//out_reflect_mtx.mv_[2][2] = - (2.0f*nz*nz - 1);
	//out_reflect_mtx.mv_[2][3] = 2.0f*d*nz;

	//out_reflect_mtx.mv_[3][0] = 0.0f;
	//out_reflect_mtx.mv_[3][1] = 0.0f;
	//out_reflect_mtx.mv_[3][2] = 0.0f;
	//out_reflect_mtx.mv_[3][3] = 1.0f;
#else
	out_reflect_mtx.mv_[0][0] = -(2.0f*nx*nx - 1.0f);
	out_reflect_mtx.mv_[1][0] = -2.0f*ny*nx;
	out_reflect_mtx.mv_[2][0] = -2.0f*nz*nx;
	out_reflect_mtx.mv_[3][0] = 2.0f*d*nx;

	out_reflect_mtx.mv_[0][1] = -2.0f*nx*ny;
	out_reflect_mtx.mv_[1][1] = - (2.0f*ny*ny - 1);
	out_reflect_mtx.mv_[2][1] = - 2.0f*nz*ny;
	out_reflect_mtx.mv_[3][1] = 2.0f*d*ny;

	out_reflect_mtx.mv_[0][2] = -2.0f*nx*nz;
	out_reflect_mtx.mv_[1][2] = - 2.0f*ny*nz;
	out_reflect_mtx.mv_[2][2] = - (2.0f*nz*nz - 1);
	out_reflect_mtx.mv_[3][2] = 2.0f*d*nz;

	out_reflect_mtx.mv_[0][3] = 0.0f;
	out_reflect_mtx.mv_[1][3] = 0.0f;
	out_reflect_mtx.mv_[2][3] = 0.0f;
	out_reflect_mtx.mv_[3][3] = 1.0f;
#endif

	/*
	reflectionMat.m00 = (1F - 2F * plane[0] * plane[0]);
	reflectionMat.m01 = (- 2F * plane[0] * plane[1]);
	reflectionMat.m02 = (- 2F * plane[0] * plane[2]);
	reflectionMat.m03 = (- 2F * plane[3] * plane[0]);

	reflectionMat.m10 = (- 2F * plane[1] * plane[0]);
	reflectionMat.m11 = (1F - 2F * plane[1] * plane[1]);
	reflectionMat.m12 = (- 2F * plane[1] * plane[2]);
	reflectionMat.m13 = (- 2F * plane[3] * plane[1]);

	reflectionMat.m20 = (- 2F * plane[2] * plane[0]);
	reflectionMat.m21 = (- 2F * plane[2] * plane[1]);
	reflectionMat.m22 = (1F - 2F * plane[2] * plane[2]);
	reflectionMat.m23 = (- 2F * plane[3] * plane[2]);

	reflectionMat.m30 = 0F;
	reflectionMat.m31 = 0F;
	reflectionMat.m32 = 0F;
	reflectionMat.m33 = 1F;
	*/
}

/* @brief 生成斜投影矩阵 */
void matrix4::gen_oblique_projection(const plane& clip_plane_in_camera_space, matrix4& out_oblique_project_mtx)const
{
	/*
	参考 1 Kim.Pallister <GAME.Programming.Gems.5> 2.6 应用于镜面和入口的斜视椎
	参考 2 OpenGL Normal Vector Transformation http://www.songho.ca/opengl/gl_normaltransform.html

	unity standar assert water原版实现(早于3.5之前版本,最新版本这段代码已被封装):
	Vector4 q = projection.inverse * new Vector4(
	sgn(clipPlane.x),
	sgn(clipPlane.y),
	1.0f,
	1.0f
	);
	Vector4 c = clipPlane * (2.0F / (Vector4.Dot (clipPlane, q)));
	// third row = clip plane - fourth row
	projection[2] = c.x - projection[3];
	projection[6] = c.y - projection[7];
	projection[10] = c.z - projection[11];
	projection[14] = c.w - projection[15];
	*/

	vector4 v_clip(clip_plane_in_camera_space.normal().x(),
		clip_plane_in_camera_space.normal().y(),
		clip_plane_in_camera_space.normal().z(),
		-clip_plane_in_camera_space.dist() //平面的定义引起 np=d则用负号  np+d=0用正号
		);

	matrix4 inverse_mtx;
	this->gen_inverse_matrix(inverse_mtx);

	l3_f32 x = v_clip.x();
	l3_f32 y = v_clip.y();

	l3_f32 sign_x = (0.0f == x) ? 0.0f : (L3_SIGN_OF_F32(x) ? -1.0f : 1.0f); //todo 引入参数e来表示0
	l3_f32 sign_y = (0.0f == y) ? 0.0f : L3_SIGN_OF_F32(y) ? -1.0f : 1.0f;
	vector4 vtmp4(sign_x,
		sign_y,
		1.0f,
		1.0f);
	vector4 q;
	inverse_mtx.mult_v4(vtmp4, q);

	l3_f32 ftmp = (2.0f / (vector4::dot_product(v_clip,q)));
	vector4 c = v_clip * ftmp;
	// 第三排 = clip plane - 第四排
	out_oblique_project_mtx.assign(*this);
	out_oblique_project_mtx.m_[2] = c.x() - this->m_[3];
	out_oblique_project_mtx.m_[6] = c.y() - this->m_[7];
	out_oblique_project_mtx.m_[10] = c.z() - this->m_[11];
	out_oblique_project_mtx.m_[14] = c.w() - this->m_[15];
}

/* @brief 将某个平面转换至矩阵变换所对应的空间(平面的法线,位置 trans_vect3/trans_rotate_vect3) */
void matrix4::trans_plane(plane& out_plane, const plane& in_plane, const l3_f32 side)const
{
	out_plane = in_plane;

	vector3 v_nml;
	this->trans_rotate_vect3(in_plane.normal(), v_nml);

	vector3 v_point;
	this->trans_vect3(in_plane.normal() * in_plane.dist(), v_point);

	out_plane = plane(v_nml * side, v_point);
}

void matrix4::dump(std::string& info)const
{
	std::ostringstream oss;

	for(l3_int32 i = 0; i < sizeof(this->m_)/sizeof(this->m_[0]); i ++)
	{
		if(0 == (i % 4))
			oss << "\r\n";
		oss << this->m_[i] << " ";
	}

	info = oss.str();
}

}










