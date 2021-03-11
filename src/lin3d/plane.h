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
		/* @brief ���� */
		ERR_SIDE = 0,

		/* @brief ǰ�� */
		FRONT_SIDE,
		/* @brief ���� */
		BACK_SIDE,
		/* @brief ���ƽ�� */
		BOTH_SIDE,
		/* @brief ��ƽ���� */
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

	/* @brief ���÷��� */
	inline void set_normal(l3_f32 x, l3_f32 y, l3_f32 z){
		this->normal_.x(x);
		this->normal_.y(y);
		this->normal_.z(z);
	}

	/* @brief ����ƽ���dist */
	inline void dist(l3_f32 d){ this->dist_ = d; }
	inline const l3_f32 dist()const{ return this->dist_; }

	/* @brief ���� */
	const vector3& normal()const{ return this->normal_; }


	/* @brief ��һ������,�����ط���������"����",�Լ����ȵĵ��� */
	inline l3_f32 normalise_normal(l3_f32 * prleng = NULL){
		return this->normal_.normalise(prleng);
	}

	/* @brief ������һ��ƽ����ཻ��, 0:��ʾ�ཻ, -1:��ʾ��ƽ��ƽ�� */
	l3_int32 intersection_with_plane(const plane& p, vector3& point, vector3& direction)const;

	/* @brief ��ƽ����ֱ�ӵĽ��� */
	l3_int32 intersection_with_line(const vector3& line_point,
		const vector3& line_dir,
		vector3& point_out)const;

	/* @brief ������ƽ��Ľ��� */
	l3_int32 intersection_with_2planes(const plane& p1, const plane& p2, vector3& out_point)const;

	/* @brief �����ƽ���ǰ�滹�Ǻ��� */
	const E_SIDE point_on_plane_side(const vector3& point)const;

	/* @brief λƽ���ǰ�滹�Ǻ��� */
	const E_SIDE aabb_on_plane_side(const aabbox& box)const;

	/* @brief ������ƽ���Ƿ���� */
	l3_bool is_front_facing(const vector3& v);

	/* @brief ��ȡ�㵽ƽ��ľ��� */
	l3_f32 get_distance(const vector3& point);

private:

	/* @brief ƽ��ķ���
	* ƽ�淽�� x * normal.x() + y * normal.y() + z * normal.z() = dist_
	* dist_ * normal_ �õ��ľ��Ǵ���
	*/
	vector3 normal_;
	/* @brief ƽ�浽ԭ��ľ��� */
	l3_f32 dist_;
};

}

#endif
