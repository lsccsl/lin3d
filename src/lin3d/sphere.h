/**
* @file sphere.h
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
#ifndef __L3ENG_SPHERE_H__
#define __L3ENG_SPHERE_H__

#include "l3_vector3.h"

namespace l3eng{

class sphere
{
public:

	sphere(){}
	~sphere(){}

	vector3& center(){ return this->center_; }

	l3_f32& radius(){ return this->radius_; }

protected:

	vector3 center_;
	l3_f32 radius_;
};

}

#endif
