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

/* @brief ����Ԫ����ת���ɱ任���� */
void quaternion::get_matrix(matrix4& m_dest)const
{
	//���Ż�
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
	* ��������:3D.Math.Primer.for.Graphics.and.Game.Development
	* 10.6.4 ����ת������Ԫ��
	* ��:
	* ��Ԫ��ת����ת����:
	* 1-2*y^2-2*z^2     2*x*y            2*x*z-2*w*y
	* 2*x*y             1-2*x^2-2*z^2    2*y*z+2*w*z
	* 2*x*z+2*w*y       2*y*z-2*w*z      1-2*x^2-2*y^2
	* ע:�����������ľ����Ǵ˾����ת�þ���,������ quaternion::get_matrix
	*
	* ��:
	* x^2 + y^2 + z^2 + w^2 = 1 -- ʽ1
	* ���Եó�
	* tr = 4w^2 - 1
	* m00 - m11 - m22 = 4*x^2 - 1  -- ʽ2
	* -m00+ m11 + m22 = 4*y^2 - 1  -- ʽ3
	* -m00- m11 + m22 = 4*z^2 - 1  -- ʽ4
	* m01  + m10 = 4*x*y �Դ�����... -- ʽx
	* ȡʽ1..4 ������һ����� ĳ������(��w),�Ϳ��������ڶԽ��߶Գ�λ��Ԫ��,��� x y z
	* 4����������ȫ��ʽ1..4ʽ���,��Ϊ����������,�޷��ж�.
	* ��������������һ��,��ȡ����ֵ,��Ϊ��Ԫ��q=-q
	*/

	l3_f32 tr = m[0][0] + m[1][1] + m[2][2]+ 1.0f;

	if(tr > 0.0f)
	{
		/* ����ʽ1���w
		* ���� ʽx ���x y z 
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
		/* ���ĸ��Խ���Ԫ�����,�������ĸ����� */
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
	/* ���,�õ��н������ֵ(���ԴӶ�ά�����Ƶ�,��ά����)
	*
	* ��������ά�������:
	*
	* ���ж�ά��λ����A = [cos(a), sin(a)]; B = [cos(b), sin(b)]; 
	* ���ԴӶ�ά���������׵õ�������λ�����˻�Ϊ�н�c��cos(c) = cos(a)*cos(b) + sin(a)*sin(b) = cos(a-b) ���ǹ�ʽ
	* ͬ��,��ά��������ά�ƹ�
	* ���Լ�����������ת,�����˷���,��A����ά����ϵ����
	* A = [cos(a), sin(a) * cos(r), sin(a) * sin(r)]
	* B = [cos(b), sin(b) * cos(r), sin(b) * sin(r)]
	* A �� B�ĵ��:
	* cos(a) * cos(b) + sin(a) * sin(b) * cos(r) * cos(r) + sin(a) * sin(b) * sin(r) * sin(r);
	* ����cos(r) * cos(r) + sin(r) * sin(r) = 1;
	* �����Ȼ��cos(a-b)
	* ��չ����ά�ռ������,��ʵ���������
	* 
	* ͬ�ĵ���,��չ����ά�ռ�,������˵�ʱ��,ͬ�����������cos(r) * cos(r) + sin(r) * sin(r)����.
	* ����Ԫ���Ķ���,������Ϊ�����һ��w��,��ͬz��һ��,w������Ϊ����z����ת������,�Ϳ�������z�����������,
	* ���ƶ���ά�ռ�ĵ������,��ά�ռ��w���� x y z����ֱ
	* 
	* ��������,nά�ռ�ĵ�λ�������,��������Ϊnά�����ļн�����.
	* ���ҴӴ�������ô���
	*
	* �������������ֵ,��2ά����,��չ��3ά����,����չ��4ά����,Ҳ�������Ƶķ��������
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
		/* 4ά�����ֵ */
		l3_f32 angle = acosf(angle_cos);
		l3_f32 inv_sin = l3_reciprocal(sinf(angle));
		scale = sinf(angle * (1.0f - t)) * inv_sin;
		invscale = sinf(angle * t) * inv_sin;
	}
	else
	{
		/* �н�̫С��,���Բ�ֵ */
		scale = 1.0f - t;
		invscale = t;
		/* Ӧ�û�Ҫ��һ��,���ڼнǺ�С,���Ժ������ */
	}

	qt = (q1 * scale) + (q2_fix * invscale);
}

///* @brief ������ת�Ƕ�����,����һ����Ԫ�� */
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

/* @brief ��ĳ��������ת (q) * (v) * (q^-1)
* v������Ϊ��һ�� w=0����Ԫ��
*/
void quaternion::rotate_vector(const vector3& v, vector3& v_out)
{
	/* 
	* ���´�������ogre/irrlicht
	* ogre irrlicht �˴�����һ��,��ע�Ͷ�һ����...
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












