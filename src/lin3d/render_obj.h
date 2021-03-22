/**
* @file render_obj.h
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
#ifndef __L3ENG_RENDER_OBJ_H__
#define __L3ENG_RENDER_OBJ_H__

#include <list>

#include "robj_base.h"
#include "mesh.h"
#include "material.h"
#include "moveable.h"

namespace l3eng{

class render_obj : public robj_base
{
public:

	typedef ref_ptr_thread<render_obj> ptr; 

	OBJ_TYPE_DEFINE(robj_base, OBJ_RENDER)

public:

	render_obj(sence_mgr * sence);
	virtual ~render_obj();

	void clear_mesh()
	{
		this->msh_.clear();
	}

	virtual void pre_render()override;
	virtual void render()override;

	/* @brief ���� */
	virtual const vertex_buf& get_vertex_buf()const override
	{ return this->msh_.vb(); }

	/* @brief �������� */
	virtual const index_buf& get_index_buf()const override
	{ return this->msh_.ib(); }

	/* @brief �������� */
	virtual const mesh& get_mesh()const override
	{ return this->msh_; }

	virtual material& get_material()const override
	{ return this->mtrl_; }


	/* @brief ��ȡ����������Χ�� */
	virtual const aabbox& get_world_aabbox()const override
	{ return this->move_.get_world_aabbox(); }

	virtual const aabbox& get_local_aabbox()const override
	{ return this->move_.get_local_aabbox(); }

	virtual const sphere& get_world_sphere()const override
	{ return this->move_.get_world_sphere(); }
	virtual const vector3& get_world_pos()const override
	{ return this->move_.get_world_pos(); }

	/* @brief ��ȡ����任 */
	virtual const matrix4& get_world_matrix()const override
	{ return this->move_.get_world_matrix(); }

	/* @brief ��ת */
	virtual void rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree) override;

	/* @brief ƽ�� */
	virtual void move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z) override;

	/* @brief ƽ����x y z */
	virtual void move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z) override;

	/* @brief ���� */
	virtual void scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale) override;


	/* @brief ��ȡ����ĳ���,������ */
	virtual void dir_world_up(vector3& v) const override;

	/* @brief ��ȡ����ĳ���,ǰ���� */
	virtual void dir_world_toward(vector3& v) const override;

	/* @brief ��ȡ����ĳ���,������ */
	virtual void dir_world_left(vector3& v) const override;


	/* @brief ��Ⱦ��render target */
	virtual const OBJ_ID get_rtt()const{ return this->rtt_; }
	virtual void set_rtt(const OBJ_ID rtt_obj){ this->rtt_ = rtt_obj; }

	shader::ptr get_shader()override
	{ return this->shdr_; }
	void set_shader(shader::ptr& shrd_ptr)override
	{ this->shdr_ = shrd_ptr; }
	
	/* @brief �����Ⱦpass */
	virtual void add_shader_pass(shader::ptr& shrd_ptr)override{
		this->lst_shdr_.push_back(shrd_ptr);
	}
	virtual std::list<shader::ptr>& get_shader_pass()override{
		return this->lst_shdr_;
	}

protected:

	void _render();

protected:

	mutable moveable_entity move_;

	mutable mesh msh_;
	mutable material mtrl_;

	OBJ_ID rtt_;

	shader::ptr shdr_;

	std::list<shader::ptr> lst_shdr_;
};

}

#endif

