/**
* @file sence_render_forward.cpp
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
#include "sence_render_forward.h"
#include "sence_mgr.h"
#include "l3_engine.h"
#include "win_device.h"
#include "robj_vector.h"
#include "robj_cone.h"
#include "robj_sphere.h"
#include "l3_engine.h"
#include "render_target_mgr.h"
#include "texture_mgr.h"

namespace l3eng{

#define FORWARD_RTT_SIZE 1024

sence_render_forward::sence_render_forward(sence_mgr * sence):sence_(sence)
	,frender_mode_(EN_FORWARD_RENDER_MODE_NORMAL)
{
	assert(this->sence_);
}

void sence_render_forward::init()
{
}

void sence_render_forward::render_sence(const L3_RENDER_GROUP render_group)
{
	this->sence_->cam_cur()->check_and_cal_mtx();
	this->sence_->cam_cur()->check_and_cal_frustum();

	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	switch(this->frender_mode_)
	{
	case EN_FORWARD_RENDER_MODE_REFLECT:
		{
			this->sence_->eng()->dev()->enable_cull_front();

			// 设置反射变换矩阵
			matrix4 reflect_view_mtx;
			{
				matrix4 reflect_mtx;
				matrix4::gen_reflect_matrix(this->reflect_plane_, reflect_mtx);

				const matrix4& cam_view_mtx = this->sence_->cam_cur()->view_mtx();

				reflect_mtx.mult(cam_view_mtx, reflect_view_mtx);

				dev->set_view_matrix(reflect_view_mtx);
			}

			// 设置斜投影矩阵
			{
				const matrix4& mtx_proj = this->sence_->cam_cur()->proj_mtx();

				plane reflect_plant_viewspace;
				reflect_view_mtx.trans_plane(reflect_plant_viewspace, this->reflect_plane_);

				matrix4 mtx_oblique_proj;
				mtx_proj.gen_oblique_projection(reflect_plant_viewspace, mtx_oblique_proj);

				dev->set_proj_matrix(mtx_oblique_proj);
			}
		}
		break;

	case EN_FORWARD_RENDER_MODE_REFRACT:
		{
			this->sence_->eng()->dev()->enable_cull_back();

			dev->set_view_matrix(this->sence_->cam_cur()->view_mtx());

			// 设置斜投影矩阵
			{
				const matrix4& mtx_proj = this->sence_->cam_cur()->proj_mtx();
				const matrix4& mtx_view = this->sence_->cam_cur()->view_mtx();

				plane reflect_plant_viewspace;
				mtx_view.trans_plane(reflect_plant_viewspace, this->reflect_plane_, -1.0f);

				matrix4 mtx_oblique_proj;
				mtx_proj.gen_oblique_projection(reflect_plant_viewspace, mtx_oblique_proj);

				dev->set_proj_matrix(mtx_oblique_proj);

			}
		}
		break;

	case EN_FORWARD_RENDER_MODE_NORMAL:
	default:
		{
			this->sence_->eng()->dev()->enable_cull_back();

			dev->set_view_matrix(this->sence_->cam_cur()->view_mtx());
			dev->set_proj_matrix(this->sence_->cam_cur()->proj_mtx());
		}
		break;
	}

	for(std::set<OBJ_ID>::iterator it_set = this->sence_->set_rtt_.begin(); it_set != this->sence_->set_rtt_.end(); it_set ++)
	{
		this->sence_->eng()->dev()->enable_rtt(*it_set);
		this->sence_->eng()->dev()->clear_fbo(*it_set);
		this->sence_->eng()->dev()->disable_rtt(*it_set);
	}
	this->sence_->set_rtt_.clear();

	if(base_obj::INVALID_OBJ_ID != this->sence_->rtt_sence())
	{
		this->sence_->eng()->dev()->enable_rtt(this->sence_->rtt_sence());
		this->sence_->eng()->dev()->clear_fbo(this->sence_->rtt_sence());
	}

	std::list<robj_base *> lst_no_deptest;
	if(!this->sence_->map_obj().empty())
	{
		for(sence_mgr::MAP_ROBJ::const_iterator it = this->sence_->map_obj().begin(); it != this->sence_->map_obj().end(); it ++)
		{
			robj_base * robj = it->second.get();

			if(L3_RENDER_GROUP_ALL != render_group && render_group != robj->render_group())
				continue;

			if(!robj->get_material().enable_depth_test())
			{
				lst_no_deptest.push_back(robj);
				continue;
			}

			this->_render_obj(robj);
		}
	}

	if(!lst_no_deptest.empty())
	{
		for(std::list<robj_base *>::iterator it = lst_no_deptest.begin(); it != lst_no_deptest.end(); it ++)
		{
			this->_render_obj(*it);
		}
	}

	if(this->sence_->eng()->enable_debug())
	{
		if(EN_FORWARD_RENDER_MODE_NORMAL == this->frender_mode_)
		{
			robj_vector v(this->sence_);
			v.get_material().enable_depth_test(L3_FALSE);
			for(sence_mgr::MAP_ROBJ::const_iterator it = this->sence_->map_obj().begin(); it != this->sence_->map_obj().end(); it ++)
			{
				robj_base * robj = it->second.get();

				if(!robj->enable_debug())
					continue;

				{
					vector3 v_up;
					robj->dir_world_up(v_up);

					v.set_vector(v_up, 8.0f);
					v.set_start(robj->get_world_pos());

					v.render();
				}

				{
					vector3 v_toward;
					robj->dir_world_toward(v_toward);

					v.set_vector(v_toward, 8.0f);
					v.set_start(robj->get_world_pos());

					v.render();
				}

				{
					vector3 v_left;
					robj->dir_world_left(v_left);

					v.set_vector(v_left, 8.0f);
					v.set_start(robj->get_world_pos());

					v.render();
				}
			}
		}
	}

	if(base_obj::INVALID_OBJ_ID != this->sence_->rtt_sence())
		this->sence_->eng()->dev()->disable_rtt(this->sence_->rtt_sence());

	if(this->sence_->eng()->enable_debug())
		_debug();
}

void sence_render_forward::_render_obj(robj_base * robj)
{
	win_device * dev = this->sence_->eng()->dev();
	if(!dev)
		return;

	//shader
	shader::ptr shdr = robj->get_shader();
	if(!shdr.is_null())
	{
		dev->active_shdr(shdr);

		shdr->pre_frame(this->sence_);

		shdr->prepare(*robj);
	}

	if(robj->get_rtt() != base_obj::INVALID_OBJ_ID)
	{
		if(base_obj::INVALID_OBJ_ID != this->sence_->rtt_sence())
			this->sence_->eng()->dev()->disable_rtt(this->sence_->rtt_sence());

		this->sence_->set_rtt_.insert(robj->get_rtt());
		this->sence_->eng()->dev()->enable_rtt(robj->get_rtt());
		robj->render();
		this->sence_->eng()->dev()->disable_rtt(robj->get_rtt());

		if(base_obj::INVALID_OBJ_ID != this->sence_->rtt_sence())
			this->sence_->eng()->dev()->enable_rtt(this->sence_->rtt_sence());
	}
	else
	{
		robj->render();
	}

	//禁用shader
	if(!shdr.is_null())
	{
		shdr->post_frame(this->sence_);
		dev->set_active_shdr_null();
	}

	_render_obj_shader_pass(robj);
}

void sence_render_forward::_render_obj_shader_pass(robj_base * robj)
{
	win_device * dev = this->sence_->eng()->dev();
	if(!dev)
		return;

	std::list<shader::ptr>& lst_shdr = robj->get_shader_pass();
	if(lst_shdr.empty())
		return;

	for(std::list<shader::ptr>::iterator it = lst_shdr.begin(); it != lst_shdr.end(); it ++){
		//shader
		shader::ptr& shdr = *it;
		if(shdr.is_null())
			continue;

		{
			dev->active_shdr(shdr);
			shdr->pre_frame(this->sence_);
			shdr->prepare(*robj);
		}

		if(robj->get_rtt() != base_obj::INVALID_OBJ_ID)
		{
			if(base_obj::INVALID_OBJ_ID != this->sence_->rtt_sence())
				this->sence_->eng()->dev()->disable_rtt(this->sence_->rtt_sence());

			this->sence_->set_rtt_.insert(robj->get_rtt());
			this->sence_->eng()->dev()->enable_rtt(robj->get_rtt());
			robj->render();
			this->sence_->eng()->dev()->disable_rtt(robj->get_rtt());

			if(base_obj::INVALID_OBJ_ID != this->sence_->rtt_sence())
				this->sence_->eng()->dev()->enable_rtt(this->sence_->rtt_sence());
		}
		else
			robj->render();

		//禁用shader
		{
			shdr->post_frame(this->sence_);
			dev->set_active_shdr_null();
		}
	}
}

void sence_render_forward::_debug()
{
	const light_mgr::MAP_LIGHT& map_light = this->sence_->get_light_mgr()->get_map_light();
	for(light_mgr::MAP_LIGHT::const_iterator it = map_light.begin(); it != map_light.end(); it ++)
	{
		const light_mgr::light_info::ptr& li = it->second;
		if(!li->l_->enable_light_debug())
			continue;

		light& l = *li->l_;

		switch(l.light_type())
		{
		case light::E_LIGHT_DIR:
			{
				robj_cone cone(this->sence_);
				cone.get_material().enable_depth_test(L3_FALSE);
				cone.get_material().poly_mode(material::E_POLY_LINE);
				cone.init(5.0f, 30.0f, 6, vector3(0.0f, 0.0f, -1.0f), color(1.0f, 0.0f, 1.0f));

				cone.move_to_xyz(l.pos().x(), l.pos().y(), l.pos().z());

				l3_f32 x_degree;
				l3_f32 y_degree;
				l3_f32 z_degree;
				l.get_rotate(x_degree, y_degree, z_degree);

				cone.rotate(x_degree, y_degree, z_degree);

				cone.render();
			}
			break;

		case light::E_LIGHT_SPOT:
			{
				robj_cone cone(this->sence_);
				cone.get_material().enable_depth_test(L3_FALSE);
				cone.get_material().poly_mode(material::E_POLY_LINE);
				cone.init(5.0f, l.cutoff(), 6, vector3(0.0f, 0.0f, -1.0f), color(1.0f, 0.0f, 1.0f));

				cone.move_to_xyz(l.pos().x(), l.pos().y(), l.pos().z());

				l3_f32 x_degree;
				l3_f32 y_degree;
				l3_f32 z_degree;
				l.get_rotate(x_degree, y_degree, z_degree);

				cone.rotate(x_degree, y_degree, z_degree);

				cone.render();
			}
			break;

		case light::E_LIGHT_POINT:
			{
				robj_sphere sphere(this->sence_);
				sphere.get_material().enable_depth_test(L3_FALSE);
				sphere.get_material().poly_mode(material::E_POLY_LINE);

				sphere.init(2.0f, 8, color(1.0f, 0.0f, 1.0f));

				sphere.move_to_xyz(l.pos().x(), l.pos().y(), l.pos().z());
				sphere.render();
			}
			break;
		}
	}
}

}
