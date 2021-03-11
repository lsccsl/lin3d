/**
* @file matrix3.h
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
#ifndef __L3ENG_MATRIX3_H__
#define __L3ENG_MATRIX3_H__

#include "l3_type.h"
#include <assert.h>
#include <string.h>
#include "l3_vector3.h"

namespace l3eng{

class matrix3
{
public:

	matrix3(){ memset(this->m_, 0, sizeof(this->m_)); }

	matrix3(l3_f32 m00,
		l3_f32 m01,
		l3_f32 m02,
		l3_f32 m10,
		l3_f32 m11,
		l3_f32 m12,
		l3_f32 m20,
		l3_f32 m21,
		l3_f32 m22)
	{
		this->mv_[0][0] = m00;
		this->mv_[0][1] = m01;
		this->mv_[0][2] = m02;

		this->mv_[1][0] = m10;
		this->mv_[1][1] = m11;
		this->mv_[1][2] = m12;

		this->mv_[2][0] = m20;
		this->mv_[2][1] = m21;
		this->mv_[2][2] = m22;
	}

	l3_int32 gen_inverse(matrix3& out_matrix);

	inline l3_f32 * operator[](l3_uint32 irow)
	{
		assert(irow < 3);
		return this->mv_[irow];
	}

	inline const l3_f32 * const operator[](l3_uint32 irow)const
	{
		assert(irow < 3);
		return this->mv_[irow];
	}

	inline void mult_vector(const vector3& v, vector3& v_out)
	{
		v_out.x(v.x() * this->mv_[0][0] + v.y() * this->mv_[0][1] + v.z() * this->mv_[0][2]);
		v_out.y(v.x() * this->mv_[1][0] + v.y() * this->mv_[1][1] + v.z() * this->mv_[1][2]);
		v_out.z(v.x() * this->mv_[2][0] + v.y() * this->mv_[2][1] + v.z() * this->mv_[2][2]);
	}

private:

	union
	{
		l3_f32 m_[9];
		l3_f32 mv_[3][3];
	};

};

}

#endif
