/**
* @file ant_plane.h
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
#ifndef __L3ENG_PLANE_H__
#define __L3ENG_PLANE_H__

#include "l3_vector3.h"
#include "aabb.h"

namespace l3eng{

class plane
{
public:

	enum E_SIDE{
		/* @brief 错误 */
		ERR_SIDE = 0,

		/* @brief 前面 */
		FRONT_SIDE,
		/* @brief 后面 */
		BACK_SIDE,
		/* @brief 跨过平面 */
		BOTH_SIDE,
		/* @brief 在平面上 */
		ON_PLANE,
	};

public:

	inline plane():dist_(0){}
	inline plane(const vector3& v, const l3_f32 d):dist_(d),normal_(v){}
	plane(const vector3& p1, const vector3& p2, const vector3& p3);
	inline plane(const vector3& normal, const vector3& point):normal_(normal)
	{
		this->normal_.normalise();
		this->dist_ = point.dot_product(this->normal_);
	}

	/* @brief 设置法线 */
	inline void set_normal(l3_f32 x, l3_f32 y, l3_f32 z){
		this->normal_.x(x);
		this->normal_.y(y);
		this->normal_.z(z);
	}

	/* @brief 设置平面的dist */
	inline void dist(l3_f32 d){ this->dist_ = d; }
	inline const l3_f32 dist()const{ return this->dist_; }

	/* @brief 法线 */
	const vector3& normal()const{ return this->normal_; }


	/* @brief 归一化法线,并返回法线向量的"长度",以及长度的倒数 */
	inline l3_f32 normalise_normal(l3_f32 * prleng = NULL){
		return this->normal_.normalise(prleng);
	}

	/* @brief 与另外一个平面的相交线, 0:表示相交, -1:表示两平面平行 */
	l3_int32 intersection_with_plane(const plane& p, vector3& point, vector3& direction)const;

	/* @brief 求平面与直接的交点 */
	l3_int32 intersection_with_line(const vector3& line_point,
		const vector3& line_dir,
		vector3& point_out)const;

	/* @brief 求三个平面的交点 */
	l3_int32 intersection_with_2planes(const plane& p1, const plane& p2, vector3& out_point)const;

	/* @brief 求点在平面的前面还是后面 */
	const E_SIDE point_on_plane_side(const vector3& point)const;

	/* @brief 位平面的前面还是后面 */
	const E_SIDE aabb_on_plane_side(const aabbox& box)const;

	/* @brief 向量与平面是否相对 */
	l3_bool is_front_facing(const vector3& v);

	/* @brief 获取点到平面的距离 */
	l3_f32 get_distance(const vector3& point);

private:

	/* @brief 平面的法线
	* 平面方程 x * normal.x() + y * normal.y() + z * normal.z() = dist_
	* dist_ * normal_ 得到的就是垂足
	*/
	vector3 normal_;
	/* @brief 平面到原点的距离 */
	l3_f32 dist_;
};

}

#endif
