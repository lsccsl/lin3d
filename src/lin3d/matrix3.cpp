/**
* @file matrix3.cpp
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
#include "matrix3.h"
#include "l3_math.h"

namespace l3eng{

l3_int32 matrix3::gen_inverse(matrix3& out_matrix)
{
	/* 将行列式展开成 2x2,得到伴随矩阵,继续无耻地抄袭ogre代码 */
	out_matrix.mv_[0][0] = this->mv_[1][1] * this->mv_[2][2] -
		this->mv_[1][2] * this->mv_[2][1];
	out_matrix.mv_[0][1] = this->mv_[0][2] * this->mv_[2][1] -
		this->mv_[0][1] * this->mv_[2][2];
	out_matrix.mv_[0][2] = this->mv_[0][1] * this->mv_[1][2] -
		this->mv_[0][2]* this->mv_[1][1];
	out_matrix.mv_[1][0] = this->mv_[1][2] * this->mv_[2][0] -
		this->mv_[1][0] * this->mv_[2][2];
	out_matrix.mv_[1][1] = this->mv_[0][0] * this->mv_[2][2] -
		this->mv_[0][2] * this->mv_[2][0];
	out_matrix.mv_[1][2] = this->mv_[0][2] * this->mv_[1][0] -
		this->mv_[0][0] * this->mv_[1][2];
	out_matrix.mv_[2][0] = this->mv_[1][0] * this->mv_[2][1] -
		this->mv_[1][1] * this->mv_[2][0];
	out_matrix.mv_[2][1] = this->mv_[0][1] * this->mv_[2][0] -
		this->mv_[0][0] * this->mv_[2][1];
	out_matrix.mv_[2][2] = this->mv_[0][0] * this->mv_[1][1] -
		this->mv_[0][1] * this->mv_[1][0];

	/* 求|A| */
	l3_f32 det = this->mv_[0][0] * out_matrix.mv_[0][0] +
		this->mv_[0][1] * out_matrix.mv_[1][0]+
		this->mv_[0][2] * out_matrix.mv_[2][0];

	/* 是否为奇异矩阵 */
	if(l3_is_zero(det))
		return -1;

	/* (A*)/|A| 得到逆矩阵 */
	l3_f32 inv_det = (1.0f / det);
	for (size_t iRow = 0; iRow < 3; iRow++)
	{
		for (size_t iCol = 0; iCol < 3; iCol++)
			out_matrix.mv_[iRow][iCol] *= inv_det;
	}

	return 0;
}

}
