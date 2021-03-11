///**
//* @file shader_shadowmap.cpp
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
//#include "shader_shadowmap.h"
//#include "win_device.h"
//#include "robj_base.h"
//#include "camera.h"
//#include "sence_mgr.h"
//#include "robj_geometry.h"
//#include "render_target_mgr.h"
//
//namespace l3eng{
//
//void shader_shadowmap::init()
//{
//	this->_set_shader_loc();
//	this->_init_screen_quad();
//}
//
//void shader_shadowmap::_init_screen_quad()
//{
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
//
//	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();
//
//	this->rtt_light_depth_ = rtt_mgr->create_fbo(1000, 1000,
//		1,
//		render_target_base::e_rtt_color_format_rgba_f16,
//		color(0.0f, 0.0f, 0.0f, 0.0f),
//		render_target_base::e_rtt_depth_format_16,
//		texture_base::e_tex_compare_less);
//
//	this->offset_x_ = 1.0f / 1000.0f;
//	this->offset_y_ = 1.0f / 1000.0f;
//}
//
//void shader_shadowmap::pre_frame(sence_mgr * sence)
//{
//	this->shdr_prg_->active();
//	this->shdr_prg_->uni_bind_int(this->uni_shader_stage_loc_,
//		this->stage_);
//
//	switch(this->stage_)
//	{
//	case EN_SHADOWMAP_STAGE_CAST:
//		{
//			win_device * dev = this->eng_->dev();
//
//			dev->enable_rtt(this->rtt_light_depth_);
//			dev->clear_fbo(this->rtt_light_depth_);
//
//			this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
//				this->l_cam_->view_mtx());
//
//			this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
//				this->l_cam_->proj_mtx());
//
//			this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
//				this->l_cam_->z_near(),
//				this->l_cam_->z_far() - this->l_cam_->z_near());
//		}
//		break;
//
//	case EN_SHADOWMAP_STAGE_RECV:
//		{
//			camera * cam = sence->cam_main();
//			matrix4 cam_view_inv_mtx;
//			cam->view_mtx().gen_inverse_matrix(cam_view_inv_mtx);
//
//			this->shdr_prg_->uni_bind_mat4(this->uni_view_inv_mtx_loc_,
//				cam_view_inv_mtx);
//
//			render_target_base::ptr rtt;
//			this->eng_->rtt_mgr()->get_render_target(this->rtt_light_depth_, rtt);
//			if(!rtt.is_null())
//			{
//				if(!rtt->clr_tex_1().is_null())
//					this->shdr_prg_->uni_bind_tex(this->uni_tex_light_depth_tex_loc_, rtt->clr_tex_1()->obj_id());
//				if(!rtt->depth_tex().is_null())
//					this->shdr_prg_->uni_bind_tex(this->uni_tex_light_depth_shadow_loc_, rtt->depth_tex()->obj_id());
//			}
//
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->sence_dep_tex_);
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_nml_loc_, this->sence_nml_tex_);
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->sence_clr_tex_);
//			this->shdr_prg_->uni_bind_tex(this->uni_tex_light_loc_, this->sence_light_tex_);
//			
//
//			this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_, cam->aspect());
//			this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f));
//
//			this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
//				cam->z_near(),
//				cam->z_far() - cam->z_near());
//
//			this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_pix_offset_loc_,
//				this->offset_x_, this->offset_y_);
//
//			this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
//				this->l_cam_->view_mtx());
//
//			this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
//				this->l_cam_->proj_mtx());
//
//			this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
//				this->l_cam_->z_near(),
//				this->l_cam_->z_far() - this->l_cam_->z_near());
//		}
//		break;
//	}
//}
//
//void shader_shadowmap::post_frame(sence_mgr * sence)
//{
//	win_device * dev = this->eng_->dev();
//
//	switch(this->stage_)
//	{
//	case EN_SHADOWMAP_STAGE_CAST:
//		{
//			dev->disable_rtt(this->rtt_light_depth_);
//		}
//		break;
//
//	case EN_SHADOWMAP_STAGE_RECV:
//		break;
//	}
//
//	this->shdr_prg_->inactive();
//}
//
//void shader_shadowmap::prepare(robj_base& robj)
//{
//	this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
//		robj.get_world_matrix());
//}
//
//void shader_shadowmap::_set_shader_loc()
//{
//	this->uni_shader_stage_loc_ = this->shdr_prg_->get_uniform_loc("uni_shader_stage");
//	this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range");
//	//this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
//	this->uni_view_inv_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_inv_mtx");
//	//this->uni_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");
//	this->uni_aspect_loc_ = this->shdr_prg_->get_uniform_loc("uni_aspect");
//	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov");
//	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
//
//	this->uni_tex_light_depth_tex_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_tex");
//	this->uni_tex_light_depth_shadow_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_shadow");
//	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");
//	this->uni_tex_nml_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_nml");
//	this->uni_tex_clr_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr");
//	this->uni_tex_light_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light");
//
//
//	this->attr_screen_quad_pos_loc_ = this->shdr_prg_->get_attrib_loc("attr_screen_quad_pos");
//	this->attr_screen_quad_texcoord_loc_ = this->shdr_prg_->get_attrib_loc("attr_screen_quad_texcoord");
//
//	this->uni_light_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_range");
//	this->uni_light_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_mtx");
//	this->uni_light_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_proj_mtx");
//	this->uni_light_dep_pix_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_pix_offset");	
//}
//
//void shader_shadowmap::set_light_info(const light_mgr::light_info& li)
//{
//	this->l_cam_ = li.l_cam_;
//	//this->rtt_dep_ = li.l_cam_->rtt_depth();
//
//	//{
//	//	this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
//	//		li.l_cam_->view_mtx());
//
//	//	this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
//	//		li.l_cam_->proj_mtx());
//
//	//	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
//	//		li.l_cam_->z_near(),
//	//		li.l_cam_->z_far() - li.l_cam_->z_near());
//
//	//	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_pix_offset_loc_,
//	//		li.l_cam_->offset_x(), li.l_cam_->offset_y());
//	//}
//}
//
//void shader_shadowmap::render_screen_quad()
//{
//	this->geo_screen_quad_->render();
//}
//
//}
