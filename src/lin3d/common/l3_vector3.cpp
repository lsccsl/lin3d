/**
* @file vector3.cpp
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
#include "l3_vector3.h"

namespace l3eng{

const vector3 __vector3_x_(1.0f, 0.0f, 0.0f);
const vector3 __vector3_y_(0.0f, 1.0f, 0.0f);
const vector3 __vector3_z_(0.0f, 0.0f, 1.0f);
const vector3 __vector3_zero_(0.0f, 0.0f, 0.0f);

void vector3::get_up_left(vector3& up, vector3& left)const
{
	/* 当前所量假设为归一化 */
	up = __vector3_y_;

	if(l3_fabs(this->dot_product(up)) >= 1.0f)
	{
		/* 如果方向与y轴重合,up选为z轴 */
		up = __vector3_z_;
	}

	up.cross_product(*this, left);
	left.normalise();

	left.cross_product(*this, up);

	up.normalise();
}

void vector3::get_up_right(vector3& up, vector3& right)const
{
	/* 当前所量假设为归一化 */
	up = __vector3_y_;

	if(l3_fabs(this->dot_product(up)) >= 1.0f)
	{
		/* 如果方向与y轴重合,up选为z轴 */
		up = __vector3_z_;
	}

	this->cross_product(up, right);
	right.normalise();

	this->cross_product(right, up);

	up.normalise();
}

}



