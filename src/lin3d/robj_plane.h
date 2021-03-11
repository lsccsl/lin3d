/**
* @file robj_plane.h
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
#ifndef __L3ENG_ROBJ_PLANE_H__
#define __L3ENG_ROBJ_PLANE_H__


#include <string>

#include "render_obj.h"
#include "mesh.h"
#include "plane.h"

namespace l3eng
{

class vector3;
class matrix4;

class robj_plane : public render_obj
{
public:

	typedef ref_ptr_thread<robj_plane> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_PLANE)

public:

	robj_plane(sence_mgr * sence);

	virtual ~robj_plane();

	void set_plane_info(const l3_f32 width, const l3_f32 length, const vector3& normal, const vector3& inter_point, l3_bool user_clr = L3_FALSE, const vertex_color_t& clr = vertex_color_t());

	void set_nml(const vector3& normal);

	void set_nml_point(const vector3& normal, const vector3& inter_point);

	const plane& pln()
	{
		return this->pln_;
	}

private:

	void set_from_nml_point_inter(const vector3& normal, const vector3& inter_point);
	
	void set_plane_inter(const matrix4& mtx_tran);

private:

	/* @brief 旋转(禁止) */
	virtual void rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree) override {}

	/* @brief 平移(禁止) */
	virtual void move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z) override {}

	/* @brief 平移至x y z(禁止) */
	virtual void move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z) override {}

	/* @brief 缩放(禁止) */
	virtual void scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale) override {}


	/* @brief 获取物体的朝向,上向量 */
	virtual void dir_world_up(vector3& v) const override;

	/* @brief 获取物体的朝向,前向量 */
	virtual void dir_world_toward(vector3& v) const override;

	/* @brief 获取物体的朝向,左向量 */
	virtual void dir_world_left(vector3& v) const override;

	/* @brief 返回过原点的垂点在平面上的垂足 */
	virtual const vector3& get_world_pos()const override
	{
		return v_pedal_;
	}

private:

	plane pln_;
	l3_f32 pln_wid_;
	l3_f32 pln_len_;
	vertex_color_t pln_clr_;

	/* @brief 原点到平面的垂足 */
	vector3 v_pedal_;

	l3_bool user_clr_;
};

}

#endif
