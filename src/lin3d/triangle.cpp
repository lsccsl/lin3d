/**
* @file triangle.cpp
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
#include "triangle.h"
#include "plane.h"
#include "l3_log.h"

#define L3_UINT_RADIUS (1.0f)

namespace l3eng{

/* @brief 点是否在三角形"内部",假设该点与三角形共面 */
l3_bool triangle::point_is_inside(const vector3& point)
{
	return (triangle::is_on_sameSide(point, this->p_[0], this->p_[1], this->p_[2]) &&
 		triangle::is_on_sameSide(point, this->p_[1], this->p_[0], this->p_[2]) &&
 		triangle::is_on_sameSide(point, this->p_[2], this->p_[0], this->p_[1]));
}

}
