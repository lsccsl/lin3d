/**
* @file aabb.h
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
#ifndef __L3ENG_AABB_H__
#define __L3ENG_AABB_H__

#include "l3_vector3.h"
#include "matrix4.h"

namespace l3eng{

class aabbox
{
public:

	enum AABB_EXTENT
	{
		AABB_NULL,
		AABB_FINITE,
		AABB_INFINITE,
	};

	/*
           1-----2
          /|    /|
         / |   / |
        5-----4  |
        |  0--|--3
        | /   | /
        |/    |/
        6-----7
	*/
	enum AABB_CORNER
	{
		AABB_FAR_LEFT_BOTTOM = 0,
		AABB_FAR_LEFT_TOP = 1,
		AABB_FAR_RIGHT_TOP = 2,
		AABB_FAR_RIGHT_BOTTOM = 3,

		AABB_NEAR_RIGHT_TOP = 4,
		AABB_NEAR_LEFT_TOP = 5,
		AABB_NEAR_LEFT_BOTTOM = 6,
		AABB_NEAR_RIGHT_BOTTOMN = 7,
		AABB_MAX,
	};

public:

	inline aabbox():extent_(AABB_NULL){}

	inline aabbox(const vector3& v1, const vector3 v2):extent_(AABB_NULL){
		this->merge(v1);
		this->merge(v2);
	}

	inline aabbox(l3_f32 bound_min_x, l3_f32 bound_min_y, l3_f32 bound_min_z,
		l3_f32 bound_max_x, l3_f32 bound_max_y, l3_f32 bound_max_z):extent_(AABB_FINITE)
	{
		this->min_.x(bound_min_x);
		this->min_.y(bound_min_y);
		this->min_.z(bound_min_z);

		this->max_.x(bound_max_x);
		this->max_.y(bound_max_y);
		this->max_.z(bound_max_z);
	}

	inline void merge(const l3_f32 x,
		const l3_f32 y,
		const l3_f32 z)
	{
		switch(this->extent_)
		{
		case AABB_NULL:
			{
				this->min_.x(x);
				this->min_.y(y);
				this->min_.z(z);

				this->max_.x(x);
				this->max_.y(y);
				this->max_.z(z);

				this->need_rel_cal_corner_ = 1;
				this->extent_ = AABB_FINITE;
			}
			break;

		case AABB_FINITE:
			{
				if(x > this->max_.x())
					this->max_.x(x);
				if(y > this->max_.y())
					this->max_.y(y);
				if(z > this->max_.z())
					this->max_.z(z);

				if(x < this->min_.x())
					this->min_.x(x);
				if(y < this->min_.y())
					this->min_.y(y);
				if(z < this->min_.z())
					this->min_.z(z);

				this->need_rel_cal_corner_ = 1;
			}
			break;

		case AABB_INFINITE:
			break;
		}
	}

	inline void merge(const vector3& v)
	{
		switch(this->extent_)
		{
		case AABB_NULL:
			{
				this->min_ = v;
				this->max_ = v;

				this->need_rel_cal_corner_ = 1;
				this->extent_ = AABB_FINITE;
			}
			break;

		case AABB_FINITE:
			{
				if(v.x() > this->max_.x())
					this->max_.x(v.x());
				if(v.y() > this->max_.y())
					this->max_.y(v.y());
				if(v.z() > this->max_.z())
					this->max_.z(v.z());

				if(v.x() < this->min_.x())
					this->min_.x(v.x());
				if(v.y() < this->min_.y())
					this->min_.y(v.y());
				if(v.z() < this->min_.z())
					this->min_.z(v.z());

				this->need_rel_cal_corner_ = 1;
			}
			break;

		case AABB_INFINITE:
			break;
		}
	}

	inline const vector3 * get_corners(){

		if(!this->need_rel_cal_corner_)
			return this->corner_;

		this->corner_[AABB_FAR_LEFT_BOTTOM].assign(this->min_);
		this->corner_[AABB_FAR_LEFT_TOP].xyz(this->min_.x(), this->max_.y(), this->min_.z());
		this->corner_[AABB_FAR_RIGHT_TOP].xyz(this->max_.x(), this->max_.y(), this->min_.z());
		this->corner_[AABB_FAR_RIGHT_BOTTOM].xyz(this->max_.x(), this->min_.y(), this->min_.z());

		this->corner_[AABB_NEAR_RIGHT_TOP].assign(this->max_);
		this->corner_[AABB_NEAR_LEFT_TOP].xyz(this->min_.x(), this->max_.y(), this->max_.z());
		this->corner_[AABB_NEAR_LEFT_BOTTOM].xyz(this->min_.x(), this->min_.y(), this->max_.z());
		this->corner_[AABB_NEAR_RIGHT_BOTTOMN].xyz(this->max_.x(), this->min_.y(), this->max_.z());

		this->need_rel_cal_corner_ = 0;
	}

	/* @brief 获取aab盒的中心 */
	inline l3_int32 get_center(vector3& vc)const
	{
		switch(this->extent_)
		{
		case AABB_NULL:
		case AABB_INFINITE:
			return -1;
		}

        vc.x((this->max_.x() + this->min_.x()) * 0.5f);
		vc.y((this->max_.y() + this->min_.y()) * 0.5f);
		vc.z((this->max_.z() + this->min_.z()) * 0.5f);

		return 0;
	}

	/* @brief 获取aab盒三个轴的尺寸的一半 */
	inline l3_int32 get_half(vector3& vh)const
	{
		switch(this->extent_)
		{
		case AABB_NULL:
			vh.x(0);
			vh.y(0);
			vh.z(0);
			break;

		case AABB_INFINITE:
			return -1;
			break;
		}

        vh.x((this->max_.x() - this->min_.x()) * 0.5f);
		vh.y((this->max_.y() - this->min_.y()) * 0.5f);
		vh.z((this->max_.z() - this->min_.z()) * 0.5f);

		return 0;
	}

	inline l3_int32 get_size(vector3& v_sz)const
	{
		switch(this->extent_)
		{
		case AABB_NULL:
			v_sz.x(0);
			v_sz.y(0);
			v_sz.z(0);
			break;

		case AABB_INFINITE:
			return -1;
			break;
		}

        v_sz.x(this->max_.x() - this->min_.x());
		v_sz.y(this->max_.y() - this->min_.y());
		v_sz.z(this->max_.z() - this->min_.z());

		return 0;
	}

	/* @brief 旋转平旋包围盒,得到新的包围盒 */
	void transform(const matrix4& m, aabbox& box)const;

	inline const AABB_EXTENT extent()const{ return this->extent_; }
	inline void extent(AABB_EXTENT e){ this->extent_ = e; }

	/* @brief 设置min max xyz */
	inline void min_x(l3_f32 f){ this->min_.x(f); }
	inline void min_y(l3_f32 f){ this->min_.y(f); }
	inline void min_z(l3_f32 f){ this->min_.z(f); }
	inline void max_x(l3_f32 f){ this->max_.x(f); }
	inline void max_y(l3_f32 f){ this->max_.y(f); }
	inline void max_z(l3_f32 f){ this->max_.z(f); }

	inline l3_f32 min_x(){ return this->min_.x(); }
	inline l3_f32 min_y(){ return this->min_.y(); }
	inline l3_f32 min_z(){ return this->min_.z(); }
	inline l3_f32 max_x(){ return this->max_.x(); }
	inline l3_f32 max_y(){ return this->max_.y(); }
	inline l3_f32 max_z(){ return this->max_.z(); }


	inline const vector3& get_min()const{ return this->min_; }
	inline const vector3& get_max()const{ return this->max_; }

	aabbox& operator = (const aabbox& bbox)
	{
		this->min_ = bbox.min_;
		this->max_ = bbox.max_;
		this->extent_ = bbox.extent_;

		this->need_rel_cal_corner_ = L3_TRUE;

		return *this;
	}

	inline l3_bool intersects(const aabbox& b2) const
	{
		if((AABB_NULL == this->extent_) || (AABB_NULL == b2.extent_))
			return L3_FALSE;

		if((AABB_INFINITE == this->extent_) || (AABB_INFINITE == b2.extent_))
			return L3_FALSE;//应该要返回true

		if(this->max_.x() < b2.min_.x())
			return L3_FALSE;
		if(this->max_.y() < b2.min_.y())
			return L3_FALSE;
		if(this->max_.z() < b2.min_.z())
			return L3_FALSE;

		if(this->min_.x() > b2.max_.x())
			return L3_FALSE;
		if(this->min_.y() > b2.max_.y())
			return L3_FALSE;
		if(this->min_.z() > b2.max_.z())
			return L3_FALSE;

		return L3_TRUE;
	}

private:

	vector3 min_;
	vector3 max_;

	vector3 corner_[AABB_MAX];
	l3_bool need_rel_cal_corner_;

	AABB_EXTENT extent_;
};

}

#endif
