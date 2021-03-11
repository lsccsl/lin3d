/**
* @file robj_base.h
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
#ifndef __L3ENG_ROBJ_BASE_H__
#define __L3ENG_ROBJ_BASE_H__

#include <list>
#include "base_obj.h"
#include "l3_ref_ptr.h"
#include "shader.h"
#include "l3_enum_define.h"

namespace l3eng{

class sence_mgr;
class vertex_buf;
class index_buf;
class mesh;
class matrix4;
class aabbox;
class material;
class aabbox;
class sphere;
class vector3;

class robj_base : public base_obj
{
public:

	typedef ref_ptr_thread<robj_base> ptr;

	OBJ_TYPE_DEFINE(base_obj, OBJ_ROBJ_BASE)

public:

	robj_base(sence_mgr * sence):
		enable_debug_(L3_FALSE),
		sence_(sence),
		parent_obj_(base_obj::INVALID_OBJ_ID),
		defer_render_(L3_FALSE),
		defer_light_(L3_TRUE),
		render_group_(L3_RENDER_GROUP_DEFAULT)
	{}

	virtual ~robj_base(){}


	virtual void update(){}
	virtual void pre_render(){}
	virtual void render(){}
	virtual void post_render(){}

	virtual const vertex_buf& get_vertex_buf()const = 0;
	virtual const index_buf& get_index_buf()const = 0;
	virtual const mesh& get_mesh()const = 0;

	/* @brief 获取世界变换矩阵 */
	virtual const matrix4& get_world_matrix()const = 0;

	/* @brief 获取坐标轴对齐包围盒 */
	virtual const aabbox& get_world_aabbox()const = 0;
	virtual const aabbox& get_local_aabbox()const = 0;
	virtual const sphere& get_world_sphere()const = 0;
	virtual const vector3& get_world_pos()const = 0;

	/* @brief 位置旋转缩放 */
	virtual void rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree){}
	virtual void move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z){}
	virtual void move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z){}
	virtual void scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale){}
	virtual void set_world_mtx(const matrix4& mtx){}


	/* @brief 获取物体的朝向,上向量 */
	virtual void dir_world_up(vector3& v)const{}

	/* @brief 获取物体的朝向,前向量 */
	virtual void dir_world_toward(vector3& v)const{}

	/* @brief 获取物体的朝向,左向量 */
	virtual void dir_world_left(vector3& v)const{}


	virtual material& get_material()const = 0;

	/* @brief render target */
	virtual const OBJ_ID get_rtt()const{ return base_obj::INVALID_OBJ_ID; }
	virtual void set_rtt(const OBJ_ID rtt_obj){}

	/* @brief get shader */
	virtual shader::ptr get_shader()
	{
		static shader::ptr __shader_;
		return __shader_;
	}
	virtual void set_shader(shader::ptr& shrd_ptr){}

	/* @brief 添加渲染pass */
	virtual void add_shader_pass(shader::ptr& shrd_ptr){}
	virtual std::list<shader::ptr>& get_shader_pass(){
		static std::list<shader::ptr> __lst_empty_shdr_;
		return __lst_empty_shdr_;
	}

	virtual sence_mgr * sence(){ return this->sence_; }

	/* @brife 是否延迟渲染 */
	const l3_bool defer_render()const{ return this->defer_render_; }
	void defer_render(l3_bool b){ this->defer_render_ = b; }

	/* @brief defer light */
	const l3_bool defer_light()const{ return this->defer_light_; }
	void defer_light(l3_bool b){ this->defer_light_ = b; }

	L3_RENDER_GROUP render_group(){ return render_group_; }
	void render_group(const L3_RENDER_GROUP r_group){ render_group_ = r_group; }

public:

	l3_bool enable_debug()
	{
		return this->enable_debug_;
	}

	void enable_debug(l3_bool b)
	{
		this->enable_debug_ = b;
	}

protected:

	virtual void _ntf_rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree){}
	virtual void _ntf_move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z){}
	virtual void _ntf_scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale){}

protected:

	l3_bool enable_debug_;

	sence_mgr * sence_;

	OBJ_ID parent_obj_;

	l3_bool defer_render_;
	l3_bool defer_light_;

	L3_RENDER_GROUP render_group_;
};


}

#endif
