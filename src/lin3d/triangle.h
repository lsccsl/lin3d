/**
* @file triangle.h
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
#ifndef __L3ENG_TRIANGLE_H__
#define __L3ENG_TRIANGLE_H__

#include <assert.h>
#include "l3_vector3.h"
#include "matrix4.h"

namespace l3eng{

class triangle
{
public:

	inline triangle(){}
	inline ~triangle(){}

	inline vector3& operator[](l3_uint32 i)
	{
		assert(i < sizeof(this->p_)/sizeof(this->p_[0]));
		return this->p_[i];
	}

	/* @brief 点是否在三角形"内部",假设该点与三角形共面 */
	l3_bool point_is_inside(const vector3& point);

	/* @brief p1 p2是否在a,b组成的边的同一侧 */
	static inline l3_bool is_on_sameSide(const vector3& p1, const vector3& p2,
		const vector3& a, const vector3& b)
	{
		/* 边的向量 */
		vector3 bminusa = b - a;

		/* 求出法线向量 */
		vector3 cp1;
		bminusa.cross_product(p1 - a, cp1);
		vector3 cp2;
		bminusa.cross_product(p2 - a, cp2);

		/* 法线方向相同,则说明在同一侧 */
		return (cp1.dot_product(cp2) >= 0.0f);
	}

	inline void set_point(const vertex_pos_t& p1,
		const vertex_pos_t& p2,
		const vertex_pos_t& p3)
	{
		this->p_[0].xyz(p1.x_, p1.y_, p1.z_);
		this->p_[1].xyz(p2.x_, p2.y_, p2.z_);
		this->p_[2].xyz(p3.x_, p3.y_, p3.z_);
	}

	inline void set_point(const vector3& p1,
		const vector3& p2,
		const vector3& p3)
	{
		this->p_[0].xyz(p1.x(), p1.y(), p1.z());
		this->p_[1].xyz(p2.x(), p2.y(), p2.z());
		this->p_[2].xyz(p3.x(), p3.y(), p3.z());
	}

	inline void tran_by_mtx(const matrix4& mtx)
	{
		mtx.trans_vect3(this->p_[0], this->p_[0]);
		mtx.trans_vect3(this->p_[1], this->p_[1]);
		mtx.trans_vect3(this->p_[2], this->p_[2]);
	}

private:

	vector3 p_[3];
};

}

#endif
