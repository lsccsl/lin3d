///**
//* @file shader_defer_render.cpp
//*
//* @author lin shao chuan (email:lsccsl@tom.com)
//*
//* @brief if it works, it was written by lin shao chuan, if not, i don't know who wrote it
//*
//* Permission to use, copy, modify, distribute and sell this software
//* and its documentation for any purpose is hereby granted without fee,
//* provided that the above copyright notice appear in all copies and
//* that both that copyright notice and this permission notice appear
//* in supporting documentation.  lin shao chuan makes no
//* representations about the suitability of this software for any
//* purpose.  It is provided "as is" without express or implied warranty.
//* see the GNU General Public License  for more detail.
//*/
//#include "shader_defer_render.h"
//#include "camera.h"
//#include "sence_mgr.h"
//#include "l3_engine.h"
//#include "render_target_mgr.h"
//#include "win_device.h"
//#include "device_buf.h"
//#include "robj_geometry.h"
//
//namespace l3eng{
//
//void shader_defer_render::init()
//{
//	this->_set_shader_loc();
//
//	win_device * dev = this->eng_->dev();
//	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();
//
//	this->rtt_gbuffer_ = rtt_mgr->create_fbo((l3_int32)dev->width(), (l3_int32)dev->height(),
//		2,
//		render_target_base::e_rtt_color_format_rgba_f16,
//		color(0.0f, 0.0f, 0.0f, 0.0f),
//		render_target_base::e_rtt_depth_format_32);
//
//	this->rtt_light_ = rtt_mgr->create_fbo((l3_int32)dev->width(), (l3_int32)dev->height(),
//		1,
//		render_target_base::e_rtt_color_format_rgba_f16,
//		color(0.0f, 0.0f, 0.0f, 0.0f),
//		render_target_base::e_rtt_depth_format_32);
//
//	this->geo_screen_quad_ = new robj_geometry(this->eng_->sence());
//
//	this->geo_screen_quad_->create_square(1, 1, L3_TRUE);
//
//	{
//		this->pos_buf_ = device_buf_base::ptr(new device_common_buf);
//
//		this->pos_buf_->resize_buf_sz(sizeof(l3_f32) * 2 * 4);
//
//		l3_f32 * buf = (l3_f32 *)(this->pos_buf_->get_buf());
//
//		buf[0] = -1.0f;
//		buf[1] = 1.0f;
//
//		buf[2] = -1.0f;
//		buf[3] = -1.0f;
//
//		buf[4] = 1.0f;
//		buf[5] = -1.0f;
//
//		buf[6] = 1.0f;
//		buf[7] = 1.0f;
//	}
//
//	{
//		this->texcoord_buf_ = device_buf_base::ptr(new device_common_buf);
//
//		this->texcoord_buf_->resize_buf_sz(sizeof(l3_f32) * 2 * 4);
//
//		l3_f32 * buf = (l3_f32 *)(this->texcoord_buf_->get_buf());
//
//		buf[0] = 0.0f;
//		buf[1] = 1.0f;
//
//		buf[2] = 0.0f;
//		buf[3] = 0.0f;
//
//		buf[4] = 1.0f;
//		buf[5] = 0.0f;
//
//		buf[6] = 1.0f;
//		buf[7] = 1.0f;
//	}
//
//	this->shdr_prg_->attr_bind(this->attr_screen_quad_pos_loc_, this->pos_buf_);
//	this->shdr_prg_->attr_bind(this->attr_screen_quad_texcoord_loc_, this->texcoord_buf_);
//}
//
//void shader_defer_render::pre_frame(sence_mgr * sence)
//{
//	switch(this->stage_)
//	{
//	case EN_DR_STAGE_DEFER_RENDER_GBUFFER:
//		this->_pre_frame_gbuffer(sence);
//		break;
//
//	case EN_DR_STAGE_DEFER_RENDER_LIGHTING:
//		this->_pre_frame_light(sence);
//		break;
//	}
//}
//
//void shader_defer_render::post_frame(sence_mgr * sence)
//{
//	switch(this->stage_)
//	{
//	case EN_DR_STAGE_DEFER_RENDER_GBUFFER:
//		this->_post_frame_gbuffer(sence);
//		break;
//
//	case EN_DR_STAGE_DEFER_RENDER_LIGHTING:
//		this->_post_frame_light(sence);
//		break;
//	}
//}
//
//void shader_defer_render::_pre_frame_gbuffer(sence_mgr * sence)
//{
//	win_device * dev = this->eng_->dev();
//
//	dev->enable_rtt(this->rtt_gbuffer_);
//	dev->clear_fbo(this->rtt_gbuffer_);
//
//	this->shdr_prg_->active();
//
//	{
//		camera * cam = sence->cam_current();
//
//		this->shdr_prg_->uni_bind_mat4(this->uni_view_mtx_loc_,
//			cam->view_mtx());
//
//		this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
//			cam->z_near(),
//			cam->z_far() - cam->z_near());
//
//		this->shdr_prg_->uni_bind_int(this->uni_shader_stage_loc_,
//			this->stage_);
//
//		this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_,
//			cam->aspect());
//
//		this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_,
//			tan(cam->fovy() * L3_AR_CONVERT * 0.5f));
//	}
//}
//void shader_defer_render::_post_frame_gbuffer(sence_mgr * sence)
//{
//	win_device * dev = this->eng_->dev();
//	dev->disable_rtt(this->rtt_gbuffer_);
//
//	this->shdr_prg_->inactive();
//}
//
//void shader_defer_render::_pre_frame_light(sence_mgr * sence)
//{
//	win_device * dev = this->eng_->dev();
//
//	dev->enable_rtt(this->rtt_light_);
//	dev->clear_fbo(this->rtt_light_);
//
//	camera * cam = sence->cam_current();
//
//	this->shdr_prg_->active();
//
//	this->shdr_prg_->uni_bind_int(this->uni_shader_stage_loc_,
//		this->stage_);
//
//	this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
//		cam->z_near(),
//		cam->z_far() - cam->z_near());
//
//	render_target_base::ptr rtt;
//	this->eng_->rtt_mgr()->get_render_target(this->rtt_gbuffer_, rtt);
//	if(!rtt.is_null())
//	{
//		if(!rtt->clr_tex_1().is_null())
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, rtt->clr_tex_1()->obj_id());
//		if(!rtt->clr_tex_2().is_null())
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_nml_loc_, rtt->clr_tex_2()->obj_id());
//		if(!rtt->depth_tex().is_null())
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, rtt->depth_tex()->obj_id());
//	}
//
//	this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_,
//		cam->aspect());
//
//	this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f)); //todo option here
//
//}
//void shader_defer_render::_post_frame_light(sence_mgr * sence)
//{
//	win_device * dev = this->eng_->dev();
//	dev->disable_rtt(this->rtt_light_);
//
//	this->shdr_prg_->inactive();
//}
//
//void shader_defer_render::prepare(robj_base& robj)
//{
//	switch(this->stage_)
//	{
//	case EN_DR_STAGE_DEFER_RENDER_GBUFFER:
//		this->_prepare_gbuffer(robj);
//		break;
//
//	case EN_DR_STAGE_DEFER_RENDER_LIGHTING:
//		break;
//	}
//}
//
//void shader_defer_render::_prepare_gbuffer(robj_base& robj)
//{
//	{
//		this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
//			robj.get_world_matrix());
//	}
//
//	{
//		material& mtrl = robj.get_material();
//
//		OBJ_ID tex1 = mtrl.get_tex_obj_id(0);
//		OBJ_ID tex2 = mtrl.get_tex_obj_id(1);
//
//		if(tex1 != base_obj::INVALID_OBJ_ID)
//			this->shdr_prg_->uni_bind_tex(this->uni_tex1_loc_, tex1);
//		if(tex2 != base_obj::INVALID_OBJ_ID)
//			this->shdr_prg_->uni_bind_tex(this->uni_tex2_loc_, tex2);
//	}
//}
//
//void shader_defer_render::_set_shader_loc()
//{
//	// EN_DR_STAGE_DEFER_RENDER_GBUFFER
//	this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range");
//	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
//	this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
//	this->uni_shader_stage_loc_ = this->shdr_prg_->get_uniform_loc("uni_shader_stage");
//	this->uni_tex1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex1");
//	this->uni_tex2_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex2");
//
//
//	// EN_DR_STAGE_DEFER_RENDER_LIGHTING
//	this->uni_tex_nml_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_nml");
//	this->uni_tex_clr_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr");
//	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");
//
//	this->uni_light_type_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_type");
//	this->uni_light_view_pos_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_pos");
//	this->uni_light_clr_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_clr");
//	this->uni_light_attenuation_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_attenuation");
//	this->uni_light_max_len_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_max_len");
//	this->uni_light_view_spot_dir_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_spot_dir");
//	this->uni_light_cutoff_cos_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_cutoff_cos");
//	this->uni_light_exponent_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_exponent");
//
//	this->uni_aspect_loc_ = this->shdr_prg_->get_uniform_loc("uni_aspect");
//	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov");
//	this->attr_screen_quad_pos_loc_ = this->shdr_prg_->get_attrib_loc("attr_screen_quad_pos");
//	this->attr_screen_quad_texcoord_loc_ = this->shdr_prg_->get_attrib_loc("attr_screen_quad_texcoord");
//}
//
//void shader_defer_render::set_light(const light& l, sence_mgr * sence)
//{
//	vector3 light_pos;
//
//	camera * cam = sence->cam_current();
//	cam->view_mtx().trans_vect3(l.pos(), light_pos, 1);	
//
//	l3_f32 light_x = light_pos.x();
//	l3_f32 light_y = light_pos.y();
//	l3_f32 light_z = light_pos.z();
//	l3_f32 light_w = 1.0f;
//
//	switch(l.type())
//	{
//	case light::E_LIGHT_DIR:
//		{
//			vector3 light_dir;
//			cam->view_mtx().trans_vect3(l.dir(), light_dir, 0);
//			light_x = light_dir.x();
//			light_y = light_dir.y();
//			light_z = light_dir.z();
//			light_w = 0.0f;
//		}
//		break;
//
//	case light::E_LIGHT_SPOT:
//		{
//			vector3 spot_dir;
//			cam->view_mtx().trans_vect3(l.dir(), spot_dir, 0);
//
//			this->shdr_prg_->uni_bind_float_vec3(this->uni_light_view_spot_dir_loc_,
//				spot_dir.x(),
//				spot_dir.y(),
//				spot_dir.z());
//
//			this->shdr_prg_->uni_bind_float(this->uni_light_cutoff_cos_loc_,
//				cos(l.cutoff() * L3_AR_CONVERT));
//
//			this->shdr_prg_->uni_bind_float(this->uni_light_exponent_loc_,
//				l.exponent());
//		}
//		break;
//
//	default:
//		break;
//	}
//
//	this->shdr_prg_->uni_bind_int(this->uni_light_type_loc_,
//		l.type());
//
//	this->shdr_prg_->uni_bind_float_vec4(this->uni_light_view_pos_loc_,
//		light_x,
//		light_y,
//		light_z,
//		light_w);
//
//	this->shdr_prg_->uni_bind_float_vec3(this->uni_light_clr_loc_,
//		l.diffuse().r(),
//		l.diffuse().g(),
//		l.diffuse().b());
//
//	this->shdr_prg_->uni_bind_float(this->uni_light_attenuation_loc_,
//		l.attenuation_begin());
//
//	this->shdr_prg_->uni_bind_float(this->uni_light_max_len_loc_,
//		l.light_max_len());
//}
//
//void shader_defer_render::render_screen_quad()
//{
//	this->geo_screen_quad_->render();
//}
//
//}
