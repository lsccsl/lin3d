/**
* @file ray.h
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
#ifndef __L3ENG_RAY_H__
#define __L3ENG_RAY_H__

#include "l3_vector3.h"

namespace l3eng{

class aabbox;
class matrix4;
class mesh;
class triangle;

class ray
{
public:

	ray(){}
	~ray(){}

	inline vector3& v_start(){ return this->v_start_; }
	inline void v_start(const vector3& v){ this->v_start_ = v; }

	inline vector3& v_dir(){ return this->v_dir_; }
	inline void v_dir(const vector3& v){ this->v_dir_ = v; }

	/* @brief 是否与aabb盒碰撞,以及碰撞的时间(根据t即可求出碰撞点) */
	l3_bool intersects_aabbox(const aabbox& bbox, l3_f32& t)const;
	
	l3_bool intersects_triangle(triangle& tr,
		l3_f32& t)const;

	l3_bool intersects_mesh(const matrix4& mtx_world, const mesh& msh,
		l3_f32& t, triangle& tr)const;

private:

	vector3 v_start_;
	vector3 v_dir_;
};

}

#endif
