/**
* @file matrix4.h
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
#ifndef __L3ENG_MATRIX4_H__
#define __L3ENG_MATRIX4_H__

#include <assert.h>
#include <string>
#include "l3_type.h"
#include "l3_vector3.h"

namespace l3eng{

class plane;
class vector4;

class matrix4
{
public:

	/* @brief ��λ���� */
	static matrix4 _identity_;

public:

	matrix4(l3_int32 b_identity = 1);
	matrix4(l3_f32 m0,
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
		l3_f32 m15);

	~matrix4(){}

	l3_uint32 get_mtx_memsz() {
		return sizeof(this->m_);
	}

	/* @brief �������ž��� */
	static void gen_scale_matrix(matrix4& m, l3_f32 x_scale, l3_f32 y_scale, l3_f32 z_scale);
	/* @brief �ӵ�ǰ�����л�ȡx y z�����ű��� */
	void get_matrix_scale(l3_f32& x_scale, l3_f32& y_scale, l3_f32& z_scale)const;

	/* @brief ��z����ת,��λ�Ƕ�0-360 */
	static void gen_rotate_z_matrix_radian(l3_f32 radian, matrix4& m, l3_f32 epsilon = 0.00001f);
	inline static void gen_rotate_z_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon = 0.00001f)
	{
		l3_f32 radian = degree * (L3_AR_CONVERT);
		matrix4::gen_rotate_z_matrix_radian(radian, m, epsilon);
	}

	/* @brief ��y����ת */
	static void gen_rotate_y_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon = 0.00001f);

	/* @brief ��x����ת */
	static void gen_rotate_x_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon = 0.00001f);

	/* @brief ����������ת */
	static void gen_rotate_matrix(const l3_f32 degree,
		const vector3& axle,
		matrix4& m);

	/* @brief ����ת����ŷ���� */
	void get_matrix_rotate_degree(l3_f32& x_degree, l3_f32& y_degree, l3_f32& z_degree);


	/* @brief �����ƶ����� */
	static void gen_move_matrix(matrix4& m, l3_f32 x, l3_f32 y, l3_f32 z);
	static void move_matrix(matrix4& m, l3_f32 x, l3_f32 y, l3_f32 z);

	/* @brief ����͸��ͶӰ����
	* @param fovy:�ӽ�
	* @param aspect:�ݺ��
	* @param z_near:��ƽ��
	* @param z_far:Զƽ��
	*/
	static void gen_pers_proj_matrix(l3_f32 fovy, l3_f32 aspect, l3_f32 z_near, l3_f32 z_far, matrix4& out_mtx);

	/* @brief ��������ͶӰ����
	*/
	static void gen_ortho_proj_matrix(l3_f32 view_width,
		l3_f32 view_height,
		l3_f32 near, l3_f32 far,
		matrix4& out_mtx);

	/* @brief ���������,���ð������, 0:�ɹ�, -1:��ǰ������������� */
	l3_int32 gen_inverse_matrix(matrix4& out_mtx)const;
	/* @brief ���������,���ø�˹��ȥ��, 0:�ɹ�, -1:��ǰ������������� */
	l3_int32 gen_inverse_matrix_by_ge(matrix4& out_mtx);

	/* @brief ����˷� */
	void mult(const matrix4& m, matrix4& out_m)const;
	/*
	* @brief ��һ��3ά�����,��תƽ��(w����Ϊ1)
	* @param �Ƿ�Ҫ�����������,�������w
	*/
	void mult_v3(const vector3& v, vector3& v_out, l3_bool need_devide_w = false)const;
	/* @brief ��һ��4ά������� */
	void mult_v4(const vector4& v, vector4& v_out);

	/* @brief ���� */
	void assign(const matrix4& m);
	void assign(l3_f32 m0,
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
		l3_f32 m15);

	/* @brief ����ת�þ��� */
	void transpose(matrix4& m)const
	{
		m.mv_[0][0] = this->mv_[0][0];
		m.mv_[0][1] = this->mv_[1][0];
		m.mv_[0][2] = this->mv_[2][0];
		m.mv_[0][3] = this->mv_[3][0];

		m.mv_[1][0] = this->mv_[0][1];
		m.mv_[1][1] = this->mv_[1][1];
		m.mv_[1][2] = this->mv_[2][1];
		m.mv_[1][3] = this->mv_[3][1];

		m.mv_[2][0] = this->mv_[0][2];
		m.mv_[2][1] = this->mv_[1][2];
		m.mv_[2][2] = this->mv_[2][2];
		m.mv_[2][3] = this->mv_[3][2];

		m.mv_[3][0] = this->mv_[0][3];
		m.mv_[3][1] = this->mv_[1][3];
		m.mv_[3][2] = this->mv_[2][3];
		m.mv_[3][3] = this->mv_[3][3];
	}

	inline l3_f32 * operator[](l3_uint32 irow)
	{
		assert(irow < 4);
		return this->mv_[irow];
	}

	inline const l3_f32 * const operator[](l3_uint32 irow)const
	{
		assert(irow < 4);
		return this->mv_[irow];
	}

	inline const l3_f32 * get_matrix_data()const
	{ return this->m_; }

	inline l3_f32 * get_mutable_matrix_data()
	{ return this->m_; }

	/* @brief ��һ����ת��, w Ĭ��Ϊ 1 */
	void trans_vect3(const vector3& v_in, vector3& v_out, const l3_f32 w = 1.0f)const;
	/* @brief ת��һ������,���Ծ����е�ƽ�Ʒ��� */
	void trans_rotate_vect3(const vector3& v_in, vector3& v_out)const;

	/* @brief �Ӿ�����ȡ����ת���� */
	void get_rotate(matrix4& out_rotate_mtx)const
	{		
		out_rotate_mtx.mv_[0][0] = this->mv_[0][0];
		out_rotate_mtx.mv_[0][1] = this->mv_[0][1];
		out_rotate_mtx.mv_[0][2] = this->mv_[0][2];

		out_rotate_mtx.mv_[1][0] = this->mv_[1][0];
		out_rotate_mtx.mv_[1][1] = this->mv_[1][1];
		out_rotate_mtx.mv_[1][2] = this->mv_[1][2];

		out_rotate_mtx.mv_[2][0] = this->mv_[2][0];
		out_rotate_mtx.mv_[2][1] = this->mv_[2][1];
		out_rotate_mtx.mv_[2][2] = this->mv_[2][2];
	}

	/* @brief ���������������ɱ任���� */
	inline void gen_rotate_mtx_from_axis(const vector3& x, const vector3& y, const vector3& z)
	{
		this->mv_[0][0] = x.x();
		this->mv_[1][0] = x.y();
		this->mv_[2][0] = x.z();

		this->mv_[0][1] = y.x();
		this->mv_[1][1] = y.y();
		this->mv_[2][1] = y.z();

		this->mv_[0][2] = z.x();
		this->mv_[1][2] = z.y();
		this->mv_[2][2] = z.z();
	}

	inline matrix4& operator=(const matrix4& r)
	{
		memcpy(this->m_, r.m_, sizeof(this->m_));

		return *this;
	}

	/* @brief ����ĳ��ƽ��,���ɷ������ */
	static void gen_reflect_matrix(const plane& p, matrix4& out_reflect_mtx);

	/* @brief ����бͶӰ���� */
	void gen_oblique_projection(const plane& clip_plane_in_camera_space, matrix4& out_oblique_project_mtx)const;

	/* @brief ��ĳ��ƽ��ת��������任����Ӧ�Ŀռ�(ƽ��ķ���,λ�� trans_vect3/trans_rotate_vect3) */
	void trans_plane(plane& out_plane, const plane& in_plane, const l3_f32 side = 1.0f)const;

public:

	void dump(std::string& info)const;

private:

	union
	{
		l3_f32 m_[16];
		l3_f32 mv_[4][4];
	};
};

}

#endif
