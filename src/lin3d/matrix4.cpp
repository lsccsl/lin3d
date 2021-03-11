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

/* @brief �������ž��� */
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

/* @brief �ӵ�ǰ�����л�ȡx y z�����ű��� */
void matrix4::get_matrix_scale(l3_f32& x_scale, l3_f32& y_scale, l3_f32& z_scale)const
{
	// sin(x) cos(x)sin(x) cos(x)cos(x)����,�����еķ���ƽ������͵õ��� scale
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

/* @brief ��z����ת,��λ�Ƕ�0-360 */
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

/* @brief ��y����ת */
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

/* @brief ��x����ת */
void matrix4::gen_rotate_x_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon)
{
	//�������Ǻ��������Ƶ���(���Ǻ����)
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
	// ������ת����Ĺ���,�����Ǻ�������(����atan)
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

/* @brief �����ƶ����� */
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

/* @brief ����͸��ͶӰ����
* @param fovy:�ӽ�
* @param aspect:�ݺ��
* @param z_near:��ƽ��
* @param z_far:Զƽ��
*/
void matrix4::gen_pers_proj_matrix(l3_f32 fovy, l3_f32 aspect, l3_f32 z_near, l3_f32 z_far, matrix4& out_mtx)
{
	/*
	* �����ӽ����ڵĵ�(x, y, z) ͶӰ����׼�ӿռ����(x_p, y_p, z_p)
	* ��Ҫ���ӽ�����ĵ�,ӳ����׼�ӿռ���(-1, -1, 0) (1, 1, 1)
	* ��ͶӰ֮�� x_p y_pȡֵΪ(-1, 1) z_p��ȡֵΪ(0, 1)
	* ��������������,ͶӰ�γɵ� x_p y_p�϶���z�й�,���Ծ�������Ҫ����һ����οռ�����,�Ա�������˷���wȡֵΪz
	*
	* 2n/w, 0,    0, 0
	* 0,    2n/h, 0, 0   --- x yֻ�ǵ���������(��������������)
	* 0,    0,    p, q   --- ��������������Ϊ�˾����������������,�ڼ����ϲ���������,�����ܱ�֤�任���z_p��(0,1)���䵥������,����z�������ֵ�Ŵ�(f/z)��,��Ҳ�Ǻܶ�3d����鼮���ᵽ�ķŴ���
	* 0,    0,    1, 0   --- 1�����οռ�����,����(x, y, z)���������������(x_p * z, y_p * z, z_p * z, z),ȥ������������Ӻ�õ�(x_p, y_p, z_p, 1)
	*
	* ps: w,hΪͶӰ���Ϳ����
	*
	* p = f / (f - n)
	* q = - (f * n) / (f - n)
	*
	* ������d3d�����,opengl�ı�׼�ӿռ��� (-1, -1, -1) (1, 1, 1),�� z_p��ȡֵ��Χ��(-1, 1)
	* ����z�Ǹ�ֵ,��ΪopenglĬ����λ��(0, 0, 0)���ҿ���z������,��Ӧ������ϵ������
	*  x_p * (-z) = (2n/w) * x
	*  y_p * (-z) = (2n/h) * y
	*  z_p * (-z) = p_o * z + q_o   --- ����Ҫ��һ����ͨ z��Ȼ�Ǹ���,�����ʱ��,Ҫ���� z��(n,f) Ȼ��z_p��Ӧ(-1,1)
	*  w_p * (-z) = -z
	*
	* ����w_p �Ϳ�����ȥ x_p y_p���������,��z,
	*
	* ��� p_o = (f+n)/(f-n)  q_o=(-2fn)(f-n),  Ȼ����zʵ���Ǵ���(-f, -n)�������,�� p_oҪȡ -(f+n)/(f-n)
	* �� z=nʱ:z_p=-n,w=n   ��z=fʱ:z_p=f,w=f
	* ��Ӧ�ľ���
	*
	*  2n/w, 0,    0,   0
	*  0,    2n/h, 0,   0
	*  0,    0,    p_o, -1  --- ���ȡ-1,����Ϊ����������Ӵ�ʱ�� -z;
	*  0,    0,    q_o, 0   
	*/

	if(fovy > 179.0f)
		fovy = 179.0f;

	if(fovy < 1.0f)
		fovy = 1.0f;

	if(z_near >= z_far)
		z_far = z_near + 1.0f;

	/* 2n/h ʵ���Ͼ���ctan(fovy/2) */
	l3_f32 h = l3_reciprocal(tan(fovy * L3_AR_CONVERT * 0.5f));
	l3_f32 w = h / aspect;//aspect����Ϊ��
	l3_f32 p = (z_far + z_near) / (z_near - z_far);//��ĸ����Ϊ��
	l3_f32 q = (2 * z_far * z_near) / (z_near - z_far);//��ĸ����Ϊ��

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

/* @brief ��������ͶӰ����
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
	* �����еĶ���ת�����ӿռ�֮��,����������,
	* z �� n��f֮��,������-1,1
	*/

	/* ���������ż���,z��ȥnear,Ȼ���ڰ�far-near���� */
	out_mtx.m_[0] = 2.0f / view_width;
	out_mtx.m_[1] = 0;
	out_mtx.m_[2] = 0;
	out_mtx.m_[3] = 0;

	out_mtx.m_[4] = 0;
	out_mtx.m_[5] = 2.0f / view_height;
	out_mtx.m_[6] = 0;
	out_mtx.m_[7] = 0;

	/* opengl��-1,1,��,������0,1��? */
	out_mtx.m_[8] = 0;
	out_mtx.m_[9] = 0;
	out_mtx.m_[10] = 2.0f / (near - far);
	out_mtx.m_[11] = 0;

	out_mtx.m_[12] = 0;
	out_mtx.m_[13] = 0;
	out_mtx.m_[14] = (far + near) / (near - far);
	out_mtx.m_[15] = 1;
}

/* @brief ���������,���ð������ */
l3_int32 matrix4::gen_inverse_matrix(matrix4& out_mtx)const
{
	/* ��������ʽչ��,���:|A|, �Լ��������:A*, �����:(A*)/(|A|) */

	/* ��|A| 
	* �Ȱ���һ��չ��,Ȼ���ÿ������ʽ�ݹ��չ��,ֱ��2x2����ʽ 
	* �˴��޳ܵس�Ϯ��ogre�Ĵ���
	* ogre�Ĵ����߼���irrlicht������Ҫ����
	*/
	l3_f32 m00 = this->mv_[0][0], m01 = this->mv_[0][1], m02 = this->mv_[0][2], m03 = this->mv_[0][3];
	l3_f32 m10 = this->mv_[1][0], m11 = this->mv_[1][1], m12 = this->mv_[1][2], m13 = this->mv_[1][3];
	l3_f32 m20 = this->mv_[2][0], m21 = this->mv_[2][1], m22 = this->mv_[2][2], m23 = this->mv_[2][3];
	l3_f32 m30 = this->mv_[3][0], m31 = this->mv_[3][1], m32 = this->mv_[3][2], m33 = this->mv_[3][3];

	/* �ݹ�չ������ʽ ����2x2����ʽ */
	l3_f32 v0 = m20 * m31 - m21 * m30;
	l3_f32 v1 = m20 * m32 - m22 * m30;
	l3_f32 v2 = m20 * m33 - m23 * m30;
	l3_f32 v3 = m21 * m32 - m22 * m31;
	l3_f32 v4 = m21 * m33 - m23 * m31;
	l3_f32 v5 = m22 * m33 - m23 * m32;

	/* �ݹ�չ������ʽ,3x3 */
	l3_f32 t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
	l3_f32 t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
	l3_f32 t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
	l3_f32 t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

	/* �ж� invDet �Ƿ�Ϊ0,���Ϊ��,������������� */
	l3_f32 det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
	if(l3_is_zero(det))
		return -1;

	/* 4x4����ʽչ��,���1|A| */
	l3_f32 invDet = 1 / det;

	/* ��ʼ��������,����������ÿһ�� A(i,j),��Ӧa(i,j)�Ĵ�������ʽ,��һ��3x3������ʽ,Ҳ��ͨ������ʽչ����� */
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
/* @brief ���������,���ø�˹��ȥ�� */
l3_int32 matrix4::gen_inverse_matrix_by_ge(matrix4& out_mtx)
{
	/* ���þ�����ȱ任,�õ������ */

	return 0;
}

/* @brief ����˷�,this�� * m�� [x][y] x��ʾ��,y��ʾ�� */
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
/* @brief ��һ��3ά�����,��תƽ�� */
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

/* @brief ��һ��4ά������� */
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

/* @brief ��һ����ת�� */
void matrix4::trans_vect3(const vector3& v_in, vector3& v_out, const l3_f32 w)const
{
	l3_f32 in_x = v_in.x();
	l3_f32 in_y = v_in.y();
	l3_f32 in_z = v_in.z();

	v_out.x(in_x * this->mv_[0][0] + in_y * this->mv_[1][0] + in_z * this->mv_[2][0] + w * this->mv_[3][0]);
	v_out.y(in_x * this->mv_[0][1] + in_y * this->mv_[1][1] + in_z * this->mv_[2][1] + w * this->mv_[3][1]);
	v_out.z(in_x * this->mv_[0][2] + in_y * this->mv_[1][2] + in_z * this->mv_[2][2] + w * this->mv_[3][2]);
}
/* @brief ת��һ������,���Ծ����е�ƽ�Ʒ��� */
void matrix4::trans_rotate_vect3(const vector3& v_in, vector3& v_out)const
{
	l3_f32 in_x = v_in.x();
	l3_f32 in_y = v_in.y();
	l3_f32 in_z = v_in.z();

	v_out.x(in_x * this->mv_[0][0] + in_y * this->mv_[1][0] + in_z * this->mv_[2][0]);
	v_out.y(in_x * this->mv_[0][1] + in_y * this->mv_[1][1] + in_z * this->mv_[2][1]);
	v_out.z(in_x * this->mv_[0][2] + in_y * this->mv_[1][2] + in_z * this->mv_[2][2]);
}


/* @brief ����ĳ��ƽ��,���ɷ������ */
void matrix4::gen_reflect_matrix(const plane& p, matrix4& out_reflect_mtx)
{
	/*
	* �ο�: https://blog.csdn.net/aa20274270/article/details/52096905 �Լ�unityˮ����demo
	* ��ĳ����point,����ƽ��p�ľ���λ��point_ref,�õ�point��ƽ����p����,����ƽ��ķ����෴�ķ���ͬ���ľ���,����һ���������ɵõ���Ӧ�ķ��侵��λ��
	* ���ݾ���λ����point_ref��point������任��ϵ,����һ���任���󼴿�
	*
	* point���� (px, py, pz)  ƽ���һ������(nx, ny, nz)
	* point_ref (pr_x, pr_y, pr_z)
	* 
	* point��ƽ��p�ľ���  dis_p = px*nx + py*ny + pz*nz - d
	* ��Ӧ����v_r = {(dis_p * 2) * nx, (dis_p * 2) * ny, (dis_p * 2) * nz}
	* ���point_ref = point - v_r
	* ��:
	* pr_x = -(dis_p * 2) * nx + px
	* pr_y = -(dis_p * 2) * ny + py
	* pr_z = -(dis_p * 2) * nz + pz
	*
	* �ٰ�dis_p����
	* pr_x = -((px*nx + py*ny + pz*nz - d) * 2) * nx + px
	* pr_y = -((px*nx + py*ny + pz*nz - d) * 2) * ny + py
	* pr_z = -((px*nx + py*ny + pz*nz - d) * 2) * nz + pz
	* 
	* ����õ�����:
	* pr_x = -(2*nx*nx - 1)*px - 2*ny*nx*py       - 2*nz*nx*pz       + 2*d*nx
	* pr_y = -2*nx*ny*px       - (2*ny*ny - 1)*py - 2*nz*ny*pz       + 2*d*ny
	* pr_z = -2*nx*nz*px       - 2*ny*nz*py       - (2*nz*nz - 1)*pz + 2*d*nz
	*
	* (ע:���һ��+-����������ƽ�涨�巽ʽ�й�) ��np+d=0������Ϊ�� ��np=d������Ϊ��
	*/

	// �����pƽ�淨�߱����ǹ�һ����

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

/* @brief ����бͶӰ���� */
void matrix4::gen_oblique_projection(const plane& clip_plane_in_camera_space, matrix4& out_oblique_project_mtx)const
{
	/*
	�ο� 1 Kim.Pallister <GAME.Programming.Gems.5> 2.6 Ӧ���ھ������ڵ�б��׵
	�ο� 2 OpenGL Normal Vector Transformation http://www.songho.ca/opengl/gl_normaltransform.html

	unity standar assert waterԭ��ʵ��(����3.5֮ǰ�汾,���°汾��δ����ѱ���װ):
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
		-clip_plane_in_camera_space.dist() //ƽ��Ķ������� np=d���ø���  np+d=0������
		);

	matrix4 inverse_mtx;
	this->gen_inverse_matrix(inverse_mtx);

	l3_f32 x = v_clip.x();
	l3_f32 y = v_clip.y();

	l3_f32 sign_x = (0.0f == x) ? 0.0f : (L3_SIGN_OF_F32(x) ? -1.0f : 1.0f); //todo �������e����ʾ0
	l3_f32 sign_y = (0.0f == y) ? 0.0f : L3_SIGN_OF_F32(y) ? -1.0f : 1.0f;
	vector4 vtmp4(sign_x,
		sign_y,
		1.0f,
		1.0f);
	vector4 q;
	inverse_mtx.mult_v4(vtmp4, q);

	l3_f32 ftmp = (2.0f / (vector4::dot_product(v_clip,q)));
	vector4 c = v_clip * ftmp;
	// ������ = clip plane - ������
	out_oblique_project_mtx.assign(*this);
	out_oblique_project_mtx.m_[2] = c.x() - this->m_[3];
	out_oblique_project_mtx.m_[6] = c.y() - this->m_[7];
	out_oblique_project_mtx.m_[10] = c.z() - this->m_[11];
	out_oblique_project_mtx.m_[14] = c.w() - this->m_[15];
}

/* @brief ��ĳ��ƽ��ת��������任����Ӧ�Ŀռ�(ƽ��ķ���,λ�� trans_vect3/trans_rotate_vect3) */
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










