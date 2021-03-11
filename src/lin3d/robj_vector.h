/**
* @file robj_vector.h
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
#ifndef __L3ENG_ROBJ_VECTOR_H__
#define __L3ENG_ROBJ_VECTOR_H__


#include "render_obj.h"
#include "l3_type.h"
#include "l3_vector3.h"
#include "robj_geometry.h"

namespace l3eng
{

class robj_tmp_geometry : public robj_geometry
{
public:
	robj_tmp_geometry(sence_mgr * sence):robj_geometry(sence)
	{
	}

	virtual const matrix4& get_world_matrix()const override
	{
		return world_mtx_;
	}

	virtual void set_world_mtx(const matrix4& mtx)override
	{
		this->world_mtx_.assign(mtx);
	}

private:

	matrix4 world_mtx_;
};

class robj_vector : public render_obj
{
public:

	typedef ref_ptr_thread<robj_vector> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_VECTOR)

public:

	robj_vector(sence_mgr * sence);

	virtual ~robj_vector();

public:

	void set_vector(const vector3& v_dir, l3_f32 draw_scale = 1.0f);

	void set_start(const vector3& v_start);

	void get_vector(vector3& v){
		v = this->v_dir_;
	}

	/* @brief 设置旋转轴与角度 */
	void rotate_by_axis_and_angle(const vector3& axis, const l3_f32 degree);

public:

	virtual void render()override;

private:

	/* @brief 旋转(禁止) */
	virtual void rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree) override {}

	/* @brief 平移(禁止) */
	virtual void move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z) override {}

	/* @brief 平移至x y z(禁止) */
	virtual void move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z) override {}

	/* @brief 缩放(禁止) */
	virtual void scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale) override {}

private:

	void _set_vertex();

private:

	vector3 v_start_;
	vector3 v_dir_;
	l3_f32 scale_;

	robj_tmp_geometry cube_;
};

}

#endif
