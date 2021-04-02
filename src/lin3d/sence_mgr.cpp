/**
* @file sence_mgr.cpp
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
#include "sence_mgr.h"
#include "l3_engine.h"
#include "base_obj_mgr.h"
#include "win_device.h"
#include "win32gl_glee.h"
#include "render_target_mgr.h"
#include "shader_program_mgr.h"
#include "shader_program_inter.h"
#include "robj_cone.h"
#include "l3_log.h"
#include "ray.h"
#include "triangle.h"

namespace l3eng{

sence_mgr::sence_mgr(l3_engine * eng):eng_(eng)
	, enable_defer_render_(L3_TRUE)
	, rtt_sence_(base_obj::INVALID_OBJ_ID)
	, test_render_mode_(L3_FALSE)	  
{
	this->cam_main_ = eng->obj_mgr()->create_obj<camera, sence_mgr *>(this);

	this->cam_cur_ = this->cam_main_;

	this->light_mgr_ = new light_mgr(eng);
}

sence_mgr::~sence_mgr()
{}

void sence_mgr::init()
{
	shader_program_mgr * shdr_mgr = this->eng_->shdr_mgr();

	// show texture
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_show_tex, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_show_ = ptr_shdr;
		assert(this->shdr_show_->is_my_type(ptr_shdr->obj_type()));
	}

	this->sence_frender_ = sence_render_forward::ptr(new sence_render_forward(this));

	if(this->test_render_mode_)
	{
#ifdef HAS_TEST_MODE
		this->sence_test_render_ = sence_render_test::ptr(new sence_render_test(this));
		this->sence_test_render_->init();
#endif
	}
	else
	{
		this->sence_render_ = sence_render_defer::ptr(new sence_render_defer(this));
		this->sence_render_->init();
	}
}

void sence_mgr::robj_add(robj_base::ptr robj)
{
	assert(robj->obj_id() != robj_base::INVALID_OBJ_ID);

	this->map_obj_[robj->obj_id()] = robj;

	this->map_render_group_[robj->render_group()].map_group_obj_[robj->obj_id()] = robj;
}

void sence_mgr::robj_del(OBJ_ID oid)
{
	MAP_ROBJ::iterator it = this->map_obj_.find(oid);
	if(this->map_obj_.end() != it)
		this->map_render_group_[it->second->render_group()].map_group_obj_.erase(it->second->obj_id());

	this->map_obj_.erase(oid);
}

void sence_mgr::robj_set_render_group(OBJ_ID oid, const L3_RENDER_GROUP rgroup)
{
	MAP_ROBJ::iterator it = this->map_obj_.find(oid);
	if(this->map_obj_.end() == it)
		return;
	robj_base::ptr& robj = it->second;
	if(robj.is_null())
		return;

	L3_RENDER_GROUP rgroup_old = robj->render_group();

	this->map_render_group_[rgroup_old].map_group_obj_.erase(oid);
	this->map_render_group_[rgroup].map_group_obj_[oid] = robj;

	robj->render_group(rgroup);
}

const sence_mgr::MAP_ROBJ& sence_mgr::render_group_obj(const L3_RENDER_GROUP group)const
{
	static MAP_ROBJ __empty_map_obj_;

	MAP_RENDER_GROUP::const_iterator it = this->map_render_group_.find(group);
	if(this->map_render_group_.end() == it)
		return __empty_map_obj_;

	return it->second.map_group_obj_;
}

void sence_mgr::pre_render()
{
	for(MAP_ROBJ::iterator it = this->map_obj_.begin(); it != this->map_obj_.end(); it ++)
	{
		robj_base * robj = it->second.get();
		robj->pre_render();
	}
}

void sence_mgr::post_render()
{
	for(MAP_ROBJ::iterator it = this->map_obj_.begin(); it != this->map_obj_.end(); it ++)
	{
		robj_base * robj = it->second.get();
		robj->post_render();
	}
}

void sence_mgr::render(const L3_RENDER_GROUP render_group)
{
	this->cam_main_->check_and_cal_mtx();
	this->cam_main_->check_and_cal_frustum();

	this->cam_cur_ = this->cam_main_;

	if(test_render_mode_)
	{
#ifdef HAS_TEST_MODE
		this->sence_test_render_->render_sence(render_group);
		return;
#endif
	}

	if(this->enable_defer_render_)
		this->sence_render_->render_sence(render_group);
	else
		this->sence_frender_->render_sence(render_group);
}

void sence_mgr::render_show_tex(OBJ_ID rtt_tex,
	const l3_f32 x_start, const l3_f32 y_start,
	const l3_f32 x_len, const l3_f32 y_len,
	const l3_f32 dep)
{
	win_device * dev = this->eng_->dev();

	dev->active_shdr(this->shdr_show_);
	this->shdr_show_->pre_frame(this);

	this->shdr_show_->render_debug_rtt(rtt_tex,
		x_start, y_start,
		x_len, y_len,
		dep);

	this->shdr_show_->post_frame(this);
	dev->set_active_shdr_null();
}

void sence_mgr::_render_show_rtt_tex(OBJ_ID rtt, const l3_uint32 tex_idx,
	const l3_f32 x_start, const l3_f32 y_start,
	const l3_f32 x_len, const l3_f32 y_len,
	const l3_f32 dep)
{
	render_target_base::ptr rtt_ptr;
	this->eng_->rtt_mgr()->get_render_target(rtt, rtt_ptr);
	if(rtt_ptr.is_null())
		return;

	texture_base::ptr tex_ptr = rtt_ptr->clr_tex_n(tex_idx);
	if(tex_ptr.is_null())
		return;

	this->render_show_tex(tex_ptr->obj_id(),
		x_start, y_start,
		x_len, y_len,
		dep);
}

/* @brief 射线拾取 */
l3_bool sence_mgr::ray_pickup(const ray& r, robj_base::ptr& obj, triangle& tr_hit, l3_f32& t)const
{
	l3_f32 min_t = FLT_MAX;

	for(MAP_ROBJ::const_iterator it = this->map_obj_.begin(); it != this->map_obj_.end(); it ++) {
		const aabbox& bbox = it->second->get_world_aabbox();

		/* 射线是否与bbox相交,如果是,进一步判断交点(也可能没有交点略过) */
		l3_f32 t = 0.0f;
		if(!r.intersects_aabbox(bbox, t))
			continue;

		if(t >= min_t)
			continue;

		triangle tr;
		if(!r.intersects_mesh(it->second->get_world_matrix(), it->second->get_mesh(), t, tr))
			continue;

		if(t >= min_t)
			continue;

		min_t = t;
		obj = it->second;
		tr_hit = tr;
	}

	if(obj.is_null())
		return L3_FALSE;

	t = min_t;

	{
		const matrix4& m = obj->get_world_matrix();

		vector3 vtmp;

		m.trans_vect3(tr_hit[0], vtmp);
		tr_hit[0] = vtmp;

		m.trans_vect3(tr_hit[1], vtmp);
		tr_hit[1] = vtmp;

		m.trans_vect3(tr_hit[2], vtmp);
		tr_hit[2] = vtmp;
	}
	return L3_TRUE;
}

void sence_mgr::set_sun_light(OBJ_ID light_obj)
{
	light_mgr::light_info::ptr li;
	this->light_mgr_->get_light_info(light_obj, li);
	if (li.is_null())
		return;
	li->l_->enable(L3_FALSE);

	if (this->test_render_mode_)
	{
#ifdef HAS_TEST_MODE
		if (!this->sence_test_render_.is_null())
			this->sence_test_render_->set_sun_light(light_obj);
#endif
	}
	else
	{
		if (!this->sence_render_.is_null())
			this->sence_render_->set_sun_light(light_obj);
	}
}

}
