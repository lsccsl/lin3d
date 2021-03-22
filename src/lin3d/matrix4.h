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

	/* @brief 单位矩阵 */
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

	/* @brief 生成缩放矩阵 */
	static void gen_scale_matrix(matrix4& m, l3_f32 x_scale, l3_f32 y_scale, l3_f32 z_scale);
	/* @brief 从当前矩阵中获取x y z的缩放比例 */
	void get_matrix_scale(l3_f32& x_scale, l3_f32& y_scale, l3_f32& z_scale)const;

	/* @brief 绕z轴旋转,单位角度0-360 */
	static void gen_rotate_z_matrix_radian(l3_f32 radian, matrix4& m, l3_f32 epsilon = 0.00001f);
	inline static void gen_rotate_z_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon = 0.00001f)
	{
		l3_f32 radian = degree * (L3_AR_CONVERT);
		matrix4::gen_rotate_z_matrix_radian(radian, m, epsilon);
	}

	/* @brief 绕y轴旋转 */
	static void gen_rotate_y_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon = 0.00001f);

	/* @brief 绕x轴旋转 */
	static void gen_rotate_x_matrix(l3_f32 degree, matrix4& m, l3_f32 epsilon = 0.00001f);

	/* @brief 绕任意轴旋转 */
	static void gen_rotate_matrix(const l3_f32 degree,
		const vector3& axle,
		matrix4& m);

	/* @brief 矩阵转换成欧拉角 */
	void get_matrix_rotate_degree(l3_f32& x_degree, l3_f32& y_degree, l3_f32& z_degree);


	/* @brief 生成移动矩阵 */
	static void gen_move_matrix(matrix4& m, l3_f32 x, l3_f32 y, l3_f32 z);
	static void move_matrix(matrix4& m, l3_f32 x, l3_f32 y, l3_f32 z);

	/* @brief 生成透视投影矩阵
	* @param fovy:视角
	* @param aspect:纵横比
	* @param z_near:近平面
	* @param z_far:远平面
	*/
	static void gen_pers_proj_matrix(l3_f32 fovy, l3_f32 aspect, l3_f32 z_near, l3_f32 z_far, matrix4& out_mtx);

	/* @brief 生成正交投影矩阵
	*/
	static void gen_ortho_proj_matrix(l3_f32 view_width,
		l3_f32 view_height,
		l3_f32 near, l3_f32 far,
		matrix4& out_mtx);

	/* @brief 生成逆矩阵,利用伴随矩阵法, 0:成功, -1:当前矩阵是奇异矩阵 */
	l3_int32 gen_inverse_matrix(matrix4& out_mtx)const;
	/* @brief 生成逆矩阵,利用高斯消去法, 0:成功, -1:当前矩阵是奇异矩阵 */
	l3_int32 gen_inverse_matrix_by_ge(matrix4& out_mtx);

	/* @brief 矩阵乘法 */
	void mult(const matrix4& m, matrix4& out_m)const;
	/*
	* @brief 与一个3维点相乘,旋转平移(w假设为1)
	* @param 是否要消除齐次坐标,即结果的w
	*/
	void mult_v3(const vector3& v, vector3& v_out, l3_bool need_devide_w = false)const;
	/* @brief 与一个4维向量相乘 */
	void mult_v4(const vector4& v, vector4& v_out);

	/* @brief 拷贝 */
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

	/* @brief 生成转置矩阵 */
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

	/* @brief 将一个点转换, w 默认为 1 */
	void trans_vect3(const vector3& v_in, vector3& v_out, const l3_f32 w = 1.0f)const;
	/* @brief 转换一个向量,忽略矩阵中的平移分量 */
	void trans_rotate_vect3(const vector3& v_in, vector3& v_out)const;

	/* @brief 从矩阵中取出旋转矩阵 */
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

	/* @brief 根据坐标轴来生成变换矩阵 */
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

	/* @brief 根据某个平面,生成反射矩阵 */
	static void gen_reflect_matrix(const plane& p, matrix4& out_reflect_mtx);

	/* @brief 生成斜投影矩阵 */
	void gen_oblique_projection(const plane& clip_plane_in_camera_space, matrix4& out_oblique_project_mtx)const;

	/* @brief 将某个平面转换至矩阵变换所对应的空间(平面的法线,位置 trans_vect3/trans_rotate_vect3) */
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
