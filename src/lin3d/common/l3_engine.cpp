/**
* @file l3_engine.cpp
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
#include "l3_engine.h"
#include "win_device.h"
#include "base_obj_mgr.h"
#include "robj_geometry.h"
#include "sence_mgr.h"
#include "camera.h"
#include "render_target_mgr.h"
#include "texture_mgr.h"
#include "image.h"
#include "config.h"
#include "shader_program_mgr.h"
#include "shader_defer_render.h"
#include "shader_defer_render_gbuffer.h"
#include "shader_defer_render_light.h"
#include "shader_shadowmap_cast.h"
#include "light_mgr.h"
#include "robj_sphere.h"
#include "robj_cone.h"
#include "robj_skeleton.h"
#include "robj_skydome.h"
#include "shader_program_inter.h"
#include "ray.h"
#include "triangle.h"
#include "plane.h"
#include "robj_plane.h"
#include "robj_vector.h"

namespace l3eng{

l3_int32 default_listener::keyboard_event(const keyboard_data_t& e)
{
	if(!e.pressed_down_)
		return 0;

	camera * cam = this->eng_->sence_->cam_main();
	l3_f32 fovy = cam->fovy();

	l3_bool b_set_title = L3_FALSE;

	switch(e.key_)
	{
	case L3_KEY_DOWN:
		{
			cam->move_toward(-0.5f);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_UP:
		{
			cam->move_toward(0.5f);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_RIGHT:
		{
			cam->move_sideward(0.5f);
			b_set_title = L3_TRUE;
			//this->y_degree_ -= 1;
			//cam->set_rotate(this->x_degree_, this->y_degree_);
		}
		break;
	case L3_KEY_LEFT:
		{
			cam->move_sideward(-0.5f);
			b_set_title = L3_TRUE;
			//this->y_degree_ += 1;
			//cam->set_rotate(this->x_degree_, this->y_degree_);
		}
		break;

	case L3_KEY_W:
		{

			this->x_degree_ = cam->x_degree();
			this->y_degree_ = cam->y_degree();

			this->x_degree_ += 1;
			cam->set_rotate(this->x_degree_, this->y_degree_);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_S:
		{
			this->x_degree_ = cam->x_degree();
			this->y_degree_ = cam->y_degree();

			this->x_degree_ -= 1;
			cam->set_rotate(this->x_degree_, this->y_degree_);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_A:
		{
			this->x_degree_ = cam->x_degree();
			this->y_degree_ = cam->y_degree();

			this->y_degree_ += 1;
			cam->set_rotate(this->x_degree_, this->y_degree_);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_D:
		{
			this->x_degree_ = cam->x_degree();
			this->y_degree_ = cam->y_degree();

			this->y_degree_ -= 1;
			cam->set_rotate(this->x_degree_, this->y_degree_);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_Q:
		{
			cam->move_vertical(0.5f);
			//l3_f32 fovy = cam->fovy();
			//l3_f32 aspect = cam->aspect();
			//l3_f32 z_near = cam->z_near();
			//l3_f32 z_far = cam->z_far();

			//fovy += 1.0f;
			//if(fovy > 179.0f)
			//	fovy = 179.0f;

			//cam->set_pers_proj(fovy, aspect, z_near, z_far);
			b_set_title = L3_TRUE;
		}
		break;
	case L3_KEY_E:
		{
			cam->move_vertical(-0.5f);
			//l3_f32 fovy = cam->fovy();
			//l3_f32 aspect = cam->aspect();
			//l3_f32 z_near = cam->z_near();
			//l3_f32 z_far = cam->z_far();

			//fovy -= 1.0f;
			//if(fovy < 1.0f)
			//	fovy = 1.0f;

			//cam->set_pers_proj(fovy, aspect, z_near, z_far);
			b_set_title = L3_TRUE;
		}
		break;

	case L3_KEY_SPACE:
		{
			cam->reset();
			this->x_degree_ = 0;
			this->y_degree_ = 0;
			b_set_title = L3_TRUE;
		}
		break;
	}

	if(b_set_title)
	{
		char actemp[256] = {0};
		_snprintf_s(actemp, sizeof(actemp) - 1, "fps:%f degree:[%f %f] pos:[%f %f %f] fovy:%f",
			this->eng_->fps_.fps_,
			cam->x_degree(), cam->y_degree(),
			cam->x_pos(), cam->y_pos(), cam->z_pos(),
			cam->fovy());
		this->eng_->dev_->set_title(actemp);
	}

	return 0;
}

l3_int32 default_listener::mouse_event(const mouse_data_t& e)
{
	switch(e.mouse_event_)
	{
	case L3_MOUSE_MOVE:
		{
#if 0
			if((this->mouse_x_ - e.mouse_.x_) < 0)
				this->y_degree_ += 1;
			if((this->mouse_x_ - e.mouse_.x_) > 0)
				this->y_degree_ -= 1;

			if((this->mouse_y_ - e.mouse_.y_) < 0)
				this->x_degree_ += 1;
			if((this->mouse_y_ - e.mouse_.y_) > 0)
				this->x_degree_ -= 1;

			this->mouse_x_ = e.mouse_.x_;
			this->mouse_y_ = e.mouse_.y_;

			this->eng_->cam_default()->set_rotate(this->x_degree_, this->y_degree_);
#endif
		}
		break;
	}

	return 0;
}

l3_int32 default_listener::dev_event(const l3_event_t& e)
{
	switch(e.event_type_)
	{
	case L3_DEV_EVENT_KEYBOARD:
		return this->keyboard_event(e.keyboard_);
		break;

	case L3_DEV_EVENT_MOUSE:
		return this->mouse_event(e.mouse_);
		break;
	}
	return 0;
}

l3_engine::l3_engine(): obj_mgr_(new base_obj_mgr)
	, cfg_(new config)
	, enable_debug_(L3_FALSE)
	, t_start_(-1)
	, t_elapse_(0)
	, t_frame_start_(-1)
	, t_frame_end_(-1)
	//, t_frame_elapse_(-1)

{
	this->dev_ = new win_device(this);
	this->def_lsn_ = new default_listener(this);
	this->frm_lsn_ = this->def_lsn_;
	this->rtt_mgr_ = new render_target_mgr(this);
	this->tex_mgr_ = new texture_mgr(this);
	this->shdr_mgr_ = new shader_program_mgr(this);
}

l3_engine::~l3_engine()
{}

l3_int32 l3_engine::init_eng(frame_listener * fl, l3_bool test_mode)
{
	image::_init();
	this->sence_ = new sence_mgr(this);
	this->sence_->test_render_mode(test_mode);

	if(fl)
		this->frm_lsn_ = fl;

	this->dev_->init();
	this->shdr_mgr_->init(test_mode);
	this->sence_->init();

	this->t_start_ = this->dev_->get_millsecond();

	return 0;
}

//void l3_engine::set_t_start()
//{
//	assert(this->dev_);
//	this->t_start_ = this->dev_->get_millsecond();
//}
//void l3_engine::set_t_frame_start()
//{
//	assert(this->dev_);
//	this->t_frame_start_ = this->dev_->get_millsecond();
//}
//void l3_engine::set_t_frame_end()
//{
//	assert(this->dev_);
//	this->t_frame_end_ = this->dev_->get_millsecond();
//}

void l3_engine::run()
{
	l3_int32 ret = 0;
	this->t_start_ = this->dev_->get_millsecond();
	while(1)
	{
		this->t_frame_start_ = this->dev_->get_millsecond();
		this->t_elapse_ = this->t_frame_start_ - this->t_start_;
		ret = _run_frame();
		this->t_frame_end_ = this->dev_->get_millsecond();

		//this->t_frame_elapse_ = t_frame_end_ - t_frame_start_;

		if(-1 == ret)
			break;
	}
}

l3_int32 l3_engine::_run_frame()
{
	win_device::print_error(__FILE__, __LINE__);

	l3_int32 ret = _run_frame_msg();
	_run_frame_eng();
	return ret;
}

l3_int32 l3_engine::_run_frame_eng()
{
	if(this->dev_->have_event())
	{
		const l3_event_t& evt = this->dev_->win_event();
		frm_lsn_->dev_event(evt);
		this->dev_->set_no_event();
	}

	this->dev_->begin_render();

	this->frm_lsn_->pre_render();
	this->sence_->pre_render();

	this->sence_->render();

	this->sence_->post_render();
	this->frm_lsn_->post_render();

	this->dev_->end_render();

	//fps count
	{
		this->fps_.frame_count_ ++;
		if(this->fps_.fps_start_ < 0){
			this->fps_.fps_start_ = this->dev_->get_millsecond();
		}
		else{
			l3_uint32 ms_elapsed = (l3_uint32)(this->t_frame_start_ - this->fps_.fps_start_);
			if(ms_elapsed > 3000)
			{
				this->fps_.fps_ = (this->fps_.frame_count_ / (ms_elapsed / 1000.0f));
				this->fps_.frame_count_ = 0;
				this->fps_.fps_start_ = this->dev_->get_millsecond();

				char actemp[256] = {0};
				camera * cam = this->sence_->cam_main(); 
				l3_f32 fovy = cam->fovy();

				_snprintf_s(actemp, sizeof(actemp) - 1, "fps:%f degree: %f %f pos %f %f %f fovy:%f",
					this->fps_.fps_,
					cam->x_degree(), cam->y_degree(),
					cam->x_pos(), cam->y_pos(), cam->z_pos(),
					fovy);
				this->dev_->set_title(actemp);
			}
		}
	}

	return 0;
}

l3_int32 l3_engine::_render_sence(const L3_RENDER_GROUP rgroup, OBJ_ID rtt_obj_id)
{
	this->sence_->rtt_sence(rtt_obj_id);
	this->sence_->render(rgroup);
	this->sence_->rtt_sence(base_obj::INVALID_OBJ_ID);
	return 0;
}

l3_int32 l3_engine::_run_frame_msg()
{
	l3_int32 ret = this->dev_->msg_loop();
	if(ret < 0)
		return -1;

	return 0;
}

/* @brief 从场景中删除 */
l3_int32 l3_engine::obj_del_from_sence(OBJ_ID obj_id)
{
	this->sence_->robj_del(obj_id);

	return 0;
}

/* @brief 删除对象 */
l3_int32 l3_engine::obj_del(OBJ_ID obj_id)
{
	this->sence_->robj_del(obj_id);
	this->obj_mgr_->obj_del(obj_id);

	return 0;
}

void l3_engine::camera_move_toward(l3_f32 f)
{
	this->sence_->cam_main()->move_toward(f);
}

void l3_engine::camera_move_sideward(l3_f32 f)
{
	this->sence_->cam_main()->move_sideward(f);
}

void l3_engine::camera_set(vector3& angle, vector3& pos)
{
	this->sence_->cam_main()->x_degree(angle.x());
	this->sence_->cam_main()->y_degree(angle.y());
	//this->pd_->cam_default_->z_degree(z_degree);

	this->sence_->cam_main()->x_pos(pos.x());
	this->sence_->cam_main()->y_pos(pos.y());
	this->sence_->cam_main()->z_pos(pos.z());
}

void l3_engine::camera_get(vector3& angle, vector3& pos)
{
	camera * cam = this->sence_->cam_main();
	angle.x(cam->x_degree());
	angle.y(cam->y_degree());
	//angle.z(cam->z_degree());

	pos.x(cam->x_pos());
	pos.y(cam->y_pos());
	pos.z(cam->z_pos());
}

void l3_engine::camera_set_pers(l3_f32 fovy, l3_f32 z_near, l3_f32 z_far)
{
	camera * cam = this->sence_->cam_main();
	l3_f32 aspect = cam->aspect();
	cam->set_pers_proj(fovy, aspect, z_near, z_far);
}

void l3_engine::camera_set_ortho(l3_f32 view_width, l3_f32 view_height, l3_f32 z_near, l3_f32 z_far)
{
	camera * cam = this->sence_->cam_main();
	cam->set_ortho_proj(view_width, view_height, z_near, z_far);
}

void l3_engine::cam_get_screen_ray(const l3_int32 x, const l3_int32 y,
	l3_f32& r_start_x, l3_f32& r_start_y, l3_f32& r_start_z,
	l3_f32& r_dir_x, l3_f32& r_dir_y, l3_f32& r_dir_z)
{
	ray r;

	l3_f32 x_normalize = ((l3_f32)x) / this->dev_->width();
	l3_f32 y_normalize = ((l3_f32)y) / this->dev_->height();
	camera * cam = this->sence_->cam_main();
	cam->get_ray_from_screen(x_normalize, y_normalize, r);

	r_start_x = r.v_start().x();
	r_start_y = r.v_start().y();
	r_start_z = r.v_start().z();

	r_dir_x = r.v_dir().x();
	r_dir_y = r.v_dir().y();
	r_dir_z = r.v_dir().z();
}

OBJ_ID l3_engine::light_create()
{
	light::ptr l = this->sence_->get_light_mgr()->create_light();
	return l->obj_id();
}

OBJ_ID l3_engine::light_get_rtt(OBJ_ID light_obj)
{
	//shader::ptr shdr = this->sence_->shdr_shadow_cast();

	//shader_shadowmap_cast::ptr shdr_cast(shdr);

	//return shdr_cast->rtt_light_depth();

	return base_obj::INVALID_OBJ_ID;
}

l3_int32 l3_engine::light_enable(OBJ_ID obj, const l3_bool enable)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->enable(enable);
	return 0;
}

l3_int32 l3_engine::light_enable_vol(OBJ_ID obj, const l3_bool enable)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->enable_vol(enable);
	return 0;
}

l3_int32 l3_engine::light_enable_shadow(OBJ_ID obj, const l3_bool enable)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->enable_shadow(enable);
	return 0;
}

l3_int32 l3_engine::light_type(OBJ_ID obj, const l3_int32 type)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_->light_type((light::enum_light_type)type);
	li->l_cam_->recal_light_mtx();
	return 0;
}

l3_int32 l3_engine::light_clr_diffuse(OBJ_ID obj, const color& clr)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->diffuse() = clr;
	return 0;
}

l3_int32 l3_engine::light_clr_specular(OBJ_ID obj, const color& clr)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->specular() = clr;
	return 0;
}

l3_int32 l3_engine::light_clr_ambient(OBJ_ID obj, const color& clr)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->ambient() = clr;
	return 0;
}

l3_int32 l3_engine::light_set_pos(OBJ_ID obj, const vector3& vpos)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_->set_pos(vpos.x(), vpos.y(), vpos.z());
	li->l_cam_->recal_light_mtx();
	return 0;
}

l3_int32 l3_engine::light_set_rotate(OBJ_ID obj, l3_f32 x_degree, l3_f32 y_degree, l3_f32 z_degree)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_->set_rotate(x_degree, y_degree, z_degree);
	li->l_cam_->recal_light_mtx();
	return 0;
}

l3_int32 l3_engine::light_spot_exponent(OBJ_ID obj, const l3_f32 exponent)
{
	light::ptr l;
	this->sence_->get_light_mgr()->get_light(obj, l);
	if(l.is_null())
		return -1;

	l->exponent(exponent);
	return 0;
}

l3_int32 l3_engine::light_spot_cutoff(OBJ_ID obj, const l3_f32 cutoff)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_->cutoff(cutoff);
	li->l_cam_->recal_light_mtx();
	return 0;
}

l3_int32 l3_engine::light_spot_cutoff_inner_outer(OBJ_ID obj, const l3_f32 cutoff_inner, const l3_f32 cutoff_outer)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_->cutoff_inner(cutoff_inner);
	li->l_->cutoff_outer(cutoff_outer);
	li->l_cam_->recal_light_mtx();
	return 0;
}

l3_int32 l3_engine::light_max_len(OBJ_ID obj, l3_f32 max_len)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_->light_max_len(max_len);
	li->l_cam_->recal_light_mtx();

	return 0;
}

l3_int32 l3_engine::light_dir_virtual_cam(OBJ_ID obj, l3_f32 f)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_cam_->virtual_cam(f);
	li->l_cam_->recal_light_mtx();

	return 0;
}

l3_int32 l3_engine::light_dir_virtual_dir(OBJ_ID obj, l3_f32 f)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_cam_->virtual_dir(f);
	li->l_cam_->recal_light_mtx();

	return 0;
}

l3_int32 l3_engine::light_dir_virtual_width(OBJ_ID obj, l3_f32 f)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_cam_->virtual_width(f);
	li->l_cam_->recal_light_mtx();

	return 0;
}

l3_int32 l3_engine::light_dir_virtual_height(OBJ_ID obj, l3_f32 f)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return -1;

	li->l_cam_->virtual_height(f);
	li->l_cam_->recal_light_mtx();

	return 0;
}

void l3_engine::light_enable_light_debug(OBJ_ID obj, l3_bool e)
{
	light_mgr::light_info::ptr li;
	this->sence_->get_light_mgr()->get_light_info(obj, li);
	if(li.is_null())
		return;

	li->l_->enable_light_debug(e);
}

l3_int32 l3_engine::material_poly_mode(OBJ_ID obj, l3_int32 mode)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj, robj))
		return -1;

	robj->get_material().poly_mode((material::e_poly_mode)mode);

	return 0;
}

l3_int32 l3_engine::material_shininess(OBJ_ID obj, l3_f32 s)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj, robj))
		return -1;

	robj->get_material().shininess(s);

	return 0;
}

l3_int32 l3_engine::material_enable_reflect(OBJ_ID obj, l3_bool b)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj, robj))
		return -1;

	robj->get_material().enable_reflect(b);

	return 0;
}

l3_int32 l3_engine::material_enable_depth_test(OBJ_ID obj, l3_bool b)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj, robj))
		return -1;

	robj->get_material().enable_depth_test(b);

	return 0;
}

l3_int32 l3_engine::robj_get_pos(OBJ_ID obj_id, vector3& pos)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	pos = robj->get_world_pos();

	return 0;
}

l3_int32 l3_engine::robj_move_to_xyz(OBJ_ID obj_id, l3_f32 x, l3_f32 y, l3_f32 z)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->move_to_xyz(x, y, z);

	return 0;
}

l3_int32 l3_engine::robj_move_xyz(OBJ_ID obj_id, l3_f32 x, l3_f32 y, l3_f32 z)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->move_xyz(x, y, z);

	return 0;
}

/* @brief 旋传一个物体 */
l3_int32 l3_engine::robj_rotate(OBJ_ID obj_id, l3_f32 x_degree, l3_f32 y_degree, l3_f32 z_degree)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->rotate(x_degree, y_degree, z_degree);

	return 0;
}

/* @brief 缩放一个特体 */
l3_int32 l3_engine::robj_scale(OBJ_ID obj_id, l3_f32 x_scale, l3_f32 y_scale, l3_f32 z_scale)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->scale(x_scale, y_scale, z_scale);

	return 0;
}

l3_int32 l3_engine::dir_world_up(OBJ_ID obj_id, vector3& v)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->dir_world_up(v);

	return 0;
}

l3_int32 l3_engine::dir_world_toward(OBJ_ID obj_id, vector3& v)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->dir_world_toward(v);

	return 0;
}

l3_int32 l3_engine::dir_world_left(OBJ_ID obj_id, vector3& v)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->dir_world_left(v);

	return 0;
}

/* @brief 设置某个物体的render_group */
l3_int32 l3_engine::robj_set_render_group(OBJ_ID obj_id, const L3_RENDER_GROUP rgoup)
{
	this->sence_->robj_set_render_group(obj_id, rgoup);
	return 0;
}

/*
* @brief 设置纹理
* @param obj_id: render obj id
* @param tex_obj: 纹理对象id(由load_tex创建)
*/
l3_int32 l3_engine::robj_set_tex(OBJ_ID obj_id, OBJ_ID tex_obj, l3_uint32 lay)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	material& mtrl = robj->get_material();
	mtrl.set_tex_obj_id(tex_obj, lay);

	return 0;
}

/* @brief set render target	*/
l3_int32 l3_engine::robj_set_rtt(OBJ_ID obj_id, OBJ_ID rtt_obj)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->set_rtt(rtt_obj);

	return 0;
}

l3_int32 l3_engine::robj_set_sys_shader(OBJ_ID obj_id, const char * name)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	shader::ptr shrd;
	this->shdr_mgr_->get_shader_by_name(name, shrd);
	if(shrd.is_null())
		return -1;

	robj->set_shader(shrd);

	return 0;
}

l3_int32 l3_engine::robj_add_sys_shader_pass(OBJ_ID obj_id, const char * name)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	shader::ptr shrd;
	this->shdr_mgr_->get_shader_by_name(name, shrd);
	if(shrd.is_null())
		return -1;

	robj->add_shader_pass(shrd);

	return 0;
}

OBJ_ID l3_engine::robj_geometry_create_cube(l3_f32 d1, l3_f32 d2, l3_f32 d3,
	l3_bool user_clr, l3_uint8 clr_r, l3_uint8 clr_g, l3_uint8 clr_b)
{
	robj_geometry::ptr obj = this->obj_mgr_->create_obj<robj_geometry>(this->sence_);

	obj->create_cube(d1, d2, d3,
		user_clr, clr_r, clr_g, clr_b);

	this->sence_->robj_add(obj);

	return obj->obj_id();
}

OBJ_ID l3_engine::robj_geometry_create_square(l3_f32 width, l3_f32 height,
	l3_bool user_clr, l3_uint8 clr_r, l3_uint8 clr_g, l3_uint8 clr_b,
	l3_uint8 alpha)
{
	robj_geometry::ptr obj = this->obj_mgr_->create_obj<robj_geometry>(this->sence_);

	obj->create_square(width, height,
		user_clr, clr_r, clr_g, clr_b,
		alpha);

	this->sence_->robj_add(obj);

	return obj->obj_id();
}

OBJ_ID l3_engine::robj_geometry_create_triangle(l3_f32 p1x, l3_f32 p1y, l3_f32 p1z,
	l3_f32 p2x, l3_f32 p2y, l3_f32 p2z,
	l3_f32 p3x, l3_f32 p3y, l3_f32 p3z)
{
	robj_geometry::ptr triangle = this->obj_mgr_->create_obj<robj_geometry, sence_mgr *>(this->sence_);

	triangle->get_material().enable_depth_test(L3_FALSE);

	vector3 p1(p1x, p1y, p1z);
	vector3 p2(p2x, p2y, p2z);
	vector3 p3(p3x, p3y, p3z);

	triangle->add_triangle(p1, p2, p3);
	this->sence_->robj_add(triangle);

	return triangle->obj_id();
}

/* @brief 平面 */
OBJ_ID l3_engine::robj_plane_create(const l3_f32 width, const l3_f32 length,
	const vector3& normal, const vector3& inter_point,
	l3_bool user_clr, const vertex_color_t& clr)
{
	robj_plane::ptr p = this->obj_mgr_->create_obj<robj_plane, sence_mgr *>(this->sence_);

	p->set_plane_info(width, length, normal, inter_point, user_clr, clr);

	this->sence_->robj_add(p);

	return p->obj_id();
}

/* @brief 设置平面 */
l3_int32 l3_engine::robj_plane_set_nml_inter_point(OBJ_ID obj_id, const vector3& normal, const vector3& inter_point)
{
	robj_plane::ptr pln_obj;
	if(0 != this->obj_mgr_->get_obj(obj_id, pln_obj))
		return -1;

	if(pln_obj.is_null())
		return -1;

	pln_obj->set_nml_point(normal, inter_point);

	return 0;
}

l3_int32 l3_engine::robj_plane_get_plane_info(OBJ_ID obj_id,
	l3_f32& nml_x, l3_f32& nml_y, l3_f32& nml_z, l3_f32& d)
{
	robj_plane::ptr pln_obj;
	if(0 != this->obj_mgr_->get_obj(obj_id, pln_obj))
		return -1;

	if(pln_obj.is_null())
		return -1;

	const plane& p = pln_obj->pln();

	nml_x = p.normal().x();
	nml_y = p.normal().y();
	nml_z = p.normal().z();

	d = p.dist();

	return 0;
}

OBJ_ID l3_engine::robj_vector_create(const vector3& v_start, const vector3& v_dir, l3_f32 draw_scale)
{
	robj_vector::ptr obj = this->obj_mgr_->create_obj<robj_vector>(this->sence_);

	obj->set_vector(v_dir, draw_scale);
	obj->set_start(v_start);

	this->sence_->robj_add(obj);
	obj->defer_render(0);

	return obj->obj_id();
}

l3_int32 l3_engine::robj_vector_set_dir(OBJ_ID obj_id, const vector3& v_dir, l3_f32 draw_scale)
{
	robj_vector::ptr obj;
	if(0 != this->obj_mgr_->get_obj(obj_id, obj))
		return -1;

	if(obj.is_null())
		return -1;

	obj->set_vector(v_dir, draw_scale);

	return 0;
}

OBJ_ID l3_engine::robj_sphere_create(l3_f32 radius, l3_uint32 resolution, const color& clr)
{
	robj_sphere::ptr obj = this->obj_mgr_->create_obj<robj_sphere>(this->sence_);

	obj->init(radius, resolution, clr);

	this->sence_->robj_add(obj);
	return obj->obj_id();
}

OBJ_ID l3_engine::robj_cone_create(const l3_f32 cone_height, const l3_f32 cone_angle_degree, const l3_uint32 resolution, 
	const vector3& v_dir,
	const color& clr)
{
	robj_cone::ptr obj = this->obj_mgr_->create_obj<robj_cone>(this->sence_);

	obj->init(cone_height, cone_angle_degree, resolution, 
		v_dir,
		clr);

	this->sence_->robj_add(obj);
	return obj->obj_id();
}

OBJ_ID l3_engine::robj_skydome_create(l3_f32 radius,
	l3_f32 tex_percent,
	l3_f32 sphere_percent,
	l3_int32 h_resolution,
	l3_int32 v_resolution)
{
	robj_skydome::ptr obj = this->obj_mgr_->create_obj<robj_skydome>(this->sence_);

	obj->init(radius,
		tex_percent,
		sphere_percent,
		h_resolution,
		v_resolution);

	this->sence_->robj_add(obj);
	return obj->obj_id();
}

OBJ_ID l3_engine::robj_skeleton_create(const char * file_path)
{
	std::string file_path_full;
	l3_int32 ret = this->cfg_->find_file_full_path(file_path, file_path_full);
	if(ret != 0)
		return base_obj::INVALID_OBJ_ID;

	robj_skeleton::ptr skeleton = this->obj_mgr_->create_obj<robj_skeleton, sence_mgr *>(this->sence_);

	skeleton->load_mesh(file_path_full);

	this->sence_->robj_add(skeleton);

	return skeleton->obj_id();
}

l3_int32 l3_engine::robj_skeleton_animation(OBJ_ID obj_id, const char * ani_name, const l3_int32 frame, const l3_f32 t)
{
	assert(ani_name);

	robj_skeleton::ptr obj;

	if(0 != this->obj_mgr_->get_obj(obj_id, obj))
		return -1;

	obj->animation(ani_name, frame, t);
	return 0;
}

void l3_engine::dev_window_size(l3_f32 width, l3_f32 height)
{
	camera& cam = *this->sence_->cam_main();

	cam.set_pers_proj(cam.fovy(), width/height, cam.z_near(), cam.z_far());
}

l3_f32 l3_engine::dev_window_width()
{
	return this->dev_->width();
}

l3_f32 l3_engine::dev_window_height()
{
	return this->dev_->height();
}

void l3_engine::robj_enable_debug(OBJ_ID obj, l3_bool b)
{
	robj_base::ptr pobj;
	if(0 != this->obj_mgr_->get_obj(obj, pobj))
		return;

	if(pobj.is_null())
		return;

	pobj->enable_debug(b);
}

OBJ_ID l3_engine::rtt_fbo_create(const char * file_path,
	const l3_int32 width,
	const l3_int32 height,
	const l3_int32 clr_attach_count,
	const color& clear_clr,
	const e_rtt_clr_type clr_type,
	const e_rtt_depth_type dep_type)
{
	assert(file_path);
	render_target_base::e_rtt_color_format rtt_clr_format = render_target_base::e_rtt_color_format_rgba_f16;
	switch(clr_type)
	{
	case e_rtt_clr_rgba_f16:
		rtt_clr_format = render_target_base::e_rtt_color_format_rgba_f16;
		break;

	case e_rtt_clr_rgba_f32:
		rtt_clr_format = render_target_base::e_rtt_color_format_rgba_f32;
		break;
	
	case e_rtt_clr_rgba_u8:
	default:
		rtt_clr_format = render_target_base::e_rtt_color_format_rgba;
		break;
	}

	render_target_base::e_rtt_depth_format rtt_dep_format = render_target_base::e_rtt_depth_format_16;
	switch(dep_type)
	{
	case e_rtt_depth_f16:
		rtt_dep_format = render_target_base::e_rtt_depth_format_16;
		break;

	case e_rtt_depth_f32:
	default:
		rtt_dep_format = render_target_base::e_rtt_depth_format_32;
		break;
	}

#if 0

	//return this->rtt_mgr_->create_fbo(width,
	//	height,
	//	clr_attach_count,
	//	rtt_clr_format,
	//	clear_clr,
	//	rtt_dep_format);

#else

 	render_target_base::ptr rtt_ptr;
	this->rtt_mgr_->create_fbo_empty(rtt_ptr,
		width, height,
		clear_clr, 1.0f
		);

	rtt_ptr->add_tex_clr(
		rtt_clr_format);

	rtt_ptr->set_tex_dep(
		rtt_dep_format,
		texture_base::e_tex_compare_less,
		1.0f);

	return rtt_ptr->obj_id();

#endif
}

/* @brief 获取tex */
OBJ_ID l3_engine::rtt_get_tex_clr(OBJ_ID rtt,
	l3_uint32 idx)
{
	render_target_base::ptr rtt_ptr;
	this->rtt_mgr_->get_render_target(rtt, rtt_ptr);
	if(rtt_ptr.is_null())
		return base_obj::INVALID_OBJ_ID;

	if(rtt_ptr->clr_tex_n(idx).is_null())
		return base_obj::INVALID_OBJ_ID;

	return rtt_ptr->clr_tex_n(idx)->obj_id();
}

/* @brief 获取dep */
OBJ_ID l3_engine::rtt_get_tex_dep(OBJ_ID rtt)
{
	render_target_base::ptr rtt_ptr;
	this->rtt_mgr_->get_render_target(rtt, rtt_ptr);
	if(rtt_ptr.is_null())
		return base_obj::INVALID_OBJ_ID;

	if(rtt_ptr->depth_tex().is_null())
		return base_obj::INVALID_OBJ_ID;

	return rtt_ptr->depth_tex()->obj_id();
}

/* @brief show rrt tex */
void l3_engine::rtt_show_tex(OBJ_ID rtt, const l3_uint32 tex_idx,
	const l3_f32 x_start, const l3_f32 y_start,
	const l3_f32 x_len, const l3_f32 y_len,
	const l3_f32 dep)
{
	this->sence_->render_show_rtt_tex(rtt, tex_idx,
		x_start, y_start,
		x_len, y_len,
		dep);
}

/* @brief 加载纹理 */
OBJ_ID l3_engine::tex_load(const char * file_path)
{
	assert(file_path);
	texture_base::ptr tex;
	OBJ_ID tex_obj = this->tex_mgr_->load_tex(file_path, tex);

	return tex_obj;
}

OBJ_ID l3_engine::tex_load_img(OBJ_ID img_obj_id)
{
	image::ptr img_obj;
	if(0 != this->obj_mgr_->get_obj(img_obj_id, img_obj))
		return base_obj::INVALID_OBJ_ID;

	if(img_obj.is_null())
		return base_obj::INVALID_OBJ_ID;

	texture_base::ptr tex;
	OBJ_ID tex_obj_id = this->tex_mgr_->load_from_image(*img_obj, tex);

	return tex_obj_id;
}

OBJ_ID l3_engine::tex_load_empty(const l3_int32 width, const l3_int32 height)
{
	texture_base::ptr tex;
	OBJ_ID tex_obj_id = this->tex_mgr_->create_tex_null(tex, width, height, texture_base::e_tex_inter_type_rgba_u8);

	return tex_obj_id;
}

l3_int32 l3_engine::tex_update(OBJ_ID tex_obj_id,
	const l3_int32 xoffset, const l3_int32 yoffset,
	const l3_int32 width, const l3_int32 height,
	const e_pixel_format pix_format, const void * pixels)
{
	return this->tex_mgr_->update_tex(tex_obj_id, xoffset, yoffset,
		width, height,
		pix_format, pixels);
}

l3_int32 l3_engine::tex_wrap_repeate(OBJ_ID tex)
{
	texture_base::ptr tex_ptr;
	this->tex_mgr_->find_tex(tex, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	tex_ptr->set_wrap(texture_base::e_tex_wrap_repeate);

	return 0;
}
l3_int32 l3_engine::tex_wrap_clamp(OBJ_ID tex)
{
	texture_base::ptr tex_ptr;
	this->tex_mgr_->find_tex(tex, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	tex_ptr->set_wrap(texture_base::e_tex_wrap_clamp);

	return 0;
}
l3_int32 l3_engine::tex_wrap_clamp_edge(OBJ_ID tex)
{
	texture_base::ptr tex_ptr;
	this->tex_mgr_->find_tex(tex, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	tex_ptr->set_wrap(texture_base::e_tex_wrap_clamp_edge);

	return 0;
}

l3_int32 l3_engine::tex_filter_nearest(OBJ_ID tex)
{
	texture_base::ptr tex_ptr;
	this->tex_mgr_->find_tex(tex, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	tex_ptr->set_filter(texture_base::e_tex_filter_nearest);

	return 0;
}

l3_int32 l3_engine::tex_filter_linear(OBJ_ID tex)
{
	texture_base::ptr tex_ptr;
	this->tex_mgr_->find_tex(tex, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	tex_ptr->set_filter(texture_base::e_tex_filter_linear);

	return 0;
}

OBJ_ID l3_engine::image_create(const char * path)
{
	image::ptr img_obj = this->obj_mgr_->create_obj<image>(path);
	if(img_obj.is_null())
		return base_obj::INVALID_OBJ_ID;

	return img_obj->obj_id();
}

l3_int32 l3_engine::image_save(OBJ_ID img_obj_id, const char * file_name)
{
	image::ptr img_obj;
	if(0 != this->obj_mgr_->get_obj(img_obj_id, img_obj))
		return -1;

	if(img_obj.is_null())
		return -1;

	return img_obj->save(file_name);
}

l3_int32 l3_engine::image_load_raw_memory(OBJ_ID img_obj_id,
	const l3_uint8 * data, const l3_int32 data_sz,
	const l3_uint32 width, const l3_uint32 height, const e_pixel_format pix_format)
{
	image::ptr img_obj;
	if(0 != this->obj_mgr_->get_obj(img_obj_id, img_obj))
		return -1;

	if(img_obj.is_null())
		return -1;

	std::vector<l3_uint8> vdata;
	vdata.insert(vdata.end(), data, data + data_sz);

	img_obj->load_raw_memory(vdata, width, height, pix_format);

	return 0;
}

OBJ_ID l3_engine::image_load_raw_memory(const char * path,
	const l3_uint8 * data, const l3_int32 data_sz,
	const l3_uint32 width, const l3_uint32 height, const e_pixel_format pix_format)
{
	image::ptr img_obj = this->obj_mgr_->create_obj<image>(path);
	if(img_obj.is_null())
		return base_obj::INVALID_OBJ_ID;

	std::vector<l3_uint8> vdata;
	vdata.insert(vdata.end(), data, data + data_sz);

	img_obj->load_raw_memory(vdata,
		width, height, pix_format);

	return img_obj->obj_id();
}

/* @brief 从一个图片里创建一个法线图 */
OBJ_ID l3_engine::image_load_normal(const char * virtual_img_path, const char * img_path, const l3_f32 scale)
{
	std::string file_path_full;
	l3_int32 ret = this->cfg_->find_file_full_path(img_path, file_path_full);
	if(ret != 0)
		return base_obj::INVALID_OBJ_ID;

	image::ptr img_obj = this->obj_mgr_->create_obj<image>(virtual_img_path);
	if(img_obj.is_null())
		return base_obj::INVALID_OBJ_ID;

	image tmp_img;
	tmp_img.load(file_path_full);

	tmp_img.gen_normal_img(*img_obj, scale);

	return img_obj->obj_id();
}

/* @brief 加载一个图片 */
OBJ_ID l3_engine::image_load(const char * virtual_img_path, const char * img_path)
{
	std::string file_path_full;
	l3_int32 ret = this->cfg_->find_file_full_path(img_path, file_path_full);
	if(ret != 0)
		return base_obj::INVALID_OBJ_ID;

	image::ptr img_obj = this->obj_mgr_->create_obj<image>(virtual_img_path);
	if(img_obj.is_null())
		return base_obj::INVALID_OBJ_ID;

	img_obj->load(file_path_full);

	return img_obj->obj_id();
}

l3_int32 l3_engine::shader_defer_render(OBJ_ID obj_id, l3_bool b)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->defer_render(b);

	return 0;
}

l3_int32 l3_engine::shader_defer_light(OBJ_ID obj_id, l3_bool b)
{
	robj_base::ptr robj;
	if(0 != this->obj_mgr_->get_obj(obj_id, robj))
		return -1;

	robj->defer_light(b);

	return 0;
}

OBJ_ID l3_engine::shader_defer_gbuffer()
{
	//shader::ptr shdr = this->sence_->shdr_dr_gbuffer();

	//shader_defer_render_gbuffer::ptr shdr_gbuffer(shdr);

	//return shdr_gbuffer->rtt_gbuffer();
	return base_obj::INVALID_OBJ_ID;
}

OBJ_ID l3_engine::shader_defer_rtt_light()
{
	//shader::ptr shdr = this->sence_->shdr_dr_light();

	//shader_defer_render_light::ptr shdr_light(shdr);

	//return shdr_light->rtt_light();
	return base_obj::INVALID_OBJ_ID;
}

OBJ_ID l3_engine::shader_defer_rtt_light_shadow()
{
	//shader::ptr shdr = this->sence_->shdr_shadow_recv();

	//shader_shadowmap_recv::ptr shdr_light_shadow(shdr);

	//return shdr_light_shadow->rtt_light_shadow();
	return base_obj::INVALID_OBJ_ID;
}

OBJ_ID l3_engine::shader_get(const char * name)
{
	return this->shdr_mgr_->get_shader_handle(name);
}

void l3_engine::shader_set_param_f32(OBJ_ID shader_id, const l3_int32 param_type, const l3_f32 param_val)
{
	shader::ptr shrd;
	this->shdr_mgr_->get_shader(shader_id, shrd);
	if(shrd.is_null())
		return;

	shrd->set_param_f32(param_type, param_val);
}

void l3_engine::shader_set_param_vec3(OBJ_ID shader_id, const l3_int32 param_type, const vector3& param_val)
{
	shader::ptr shrd;
	this->shdr_mgr_->get_shader(shader_id, shrd);
	if(shrd.is_null())
		return;

	shrd->set_param_vec3(param_type, param_val);
}

void l3_engine::shader_set_param_bool(OBJ_ID shader_id, const l3_int32 param_type, const l3_bool param_val)
{
	shader::ptr shrd;
	this->shdr_mgr_->get_shader(shader_id, shrd);
	if(shrd.is_null())
		return;

	shrd->set_param_bool(param_type, param_val);
}

void l3_engine::shader_set_param(OBJ_ID shader_id, const l3_int32 param_type, const void * param, const l3_uint32 param_sz)
{
	shader::ptr shrd;
	this->shdr_mgr_->get_shader(shader_id, shrd);
	if(shrd.is_null())
		return;

	shrd->set_param(param_type, param, param_sz);
}

void l3_engine::shader_set_param_obj(OBJ_ID shader_id, const l3_int32 param_type, OBJ_ID param_obj)
{
	shader::ptr shrd;
	this->shdr_mgr_->get_shader(shader_id, shrd);
	if(shrd.is_null())
		return;

	shrd->set_param_obj(param_type, param_obj);
}

l3_bool l3_engine::ray_pickup(const l3_f32 start_x, const l3_f32 start_y, const l3_f32 start_z,
	const l3_f32 dir_x, const l3_f32 dir_y, const l3_f32 dir_z,
	OBJ_ID& obj_id,
	l3_f32& t,
	l3_f32& tr_p1x, l3_f32& tr_p1y, l3_f32& tr_p1z,
	l3_f32& tr_p2x, l3_f32& tr_p2y, l3_f32& tr_p2z,
	l3_f32& tr_p3x, l3_f32& tr_p3y, l3_f32& tr_p3z)
{
	obj_id = base_obj::INVALID_OBJ_ID;

	ray r;
	r.v_start().xyz(start_x, start_y, start_z);
	r.v_dir().xyz(dir_x, dir_y, dir_z);

	robj_base::ptr obj;
	triangle tr;
	this->sence_->ray_pickup(r, obj, tr, t);

	if(obj.is_null())
		return L3_FALSE;

	obj_id = obj->obj_id();

	tr_p1x = tr[0].x();
	tr_p1y = tr[0].y();
	tr_p1z = tr[0].z();

	tr_p2x = tr[1].x();
	tr_p2y = tr[1].y();
	tr_p2z = tr[1].z();

	tr_p3x = tr[2].x();
	tr_p3y = tr[2].y();
	tr_p3z = tr[2].z();

	return L3_TRUE;
}

void l3_engine::set_title(const char * name)
{
	assert(this->dev_);
	this->dev_->set_title(name);
}

void l3_engine::add_sys_folder(const char * folder_name)
{
	assert(this->cfg_);
	this->cfg_->add_sys_folder(folder_name);
}

void l3_engine::enable_defer_render(l3_bool e)
{
	assert(this->sence_);
	this->sence_->enable_defer_render(e);
}

void l3_engine::enable_atmospheric(l3_bool e)
{
	assert(this->sence_);
	this->sence_->enable_atmospheric(e);
}

void l3_engine::enable_hdr(l3_bool e)
{
	assert(this->sence_);
	this->sence_->enable_hdr(e);
}

void l3_engine::enable_ssao(l3_bool e)
{
	assert(this->sence_);
	this->sence_->enable_ssao(e);
}

void l3_engine::enable_post_vol_rb(l3_bool e)
{
	assert(this->sence_);
	this->sence_->enable_post_vol_rb(e);
}


// for debug
void l3_engine::set_reflect_mode(const l3_f32 nx, const l3_f32 ny, const l3_f32 nz, const l3_f32 d)
{
	this->sence_->sence_frender()->reflect_mode(plane(vector3(nx, ny, nz), d));
}

void l3_engine::set_refract_mode(const l3_f32 nx, const l3_f32 ny, const l3_f32 nz, const l3_f32 d)
{
	this->sence_->sence_frender()->refract_mode(plane(vector3(nx, ny, nz), d));
}

void l3_engine::set_normal_mode()
{
	this->sence_->sence_frender()->normal_mode();
}


}



