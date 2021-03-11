/**
* @file aabb.cpp
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
#include "aabb.h"

namespace l3eng{

/* @brief 旋转平旋包围盒,得到新的包围盒 */
void aabbox::transform(const matrix4& m, aabbox& box)const
{
	vector3 p_tmp;

	vector3 point(this->min_);
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);

	point.z(this->max_.z());
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);

	point.y(this->max_.y());
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);

	point.z(this->min_.z());
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);


	point.assign(this->max_);
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);

	point.z(this->min_.z());
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);

	point.y(this->min_.z());
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);

	point.z(this->max_.z());
	m.mult_v3(point, p_tmp);
	box.merge(p_tmp);
}

}














