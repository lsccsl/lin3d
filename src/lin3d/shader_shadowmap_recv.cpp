/**
* @file shader_shadowmap_recv.cpp
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
#include "shader_shadowmap_recv.h"
#include "win_device.h"
#include "render_target_mgr.h"
#include "robj_geometry.h"
#include "camera.h"
#include "sence_mgr.h"
#include "robj_base.h"

namespace l3eng{

void shader_shadowmap_recv::init()
{
	this->uni_tex_light_loc_        = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light"); //法线 uniform sampler2D
	this->uni_tex_clr_loc_        = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr"); //颜色 uniform sampler2D
	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth"); //摄影机空间深度纹理 uniform sampler2D

	this->uni_tex_light_depth_tex_loc_    = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_tex"); //光源空间深度 uniform sampler2D
	this->uni_tex_light_depth_shadow_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_shadow"); //同一个纹理,深度 uniform sampler2DShadow

	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov"); //视角一半的tan值 uniform float
	this->uni_aspect_loc_       = this->shdr_prg_->get_uniform_loc("uni_aspect"); //屏幕宽高比 uniform float
	this->uni_dep_range_loc_    = this->shdr_prg_->get_uniform_loc("uni_dep_range"); //最大深度 uniform vec2

	this->uni_view_inv_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_inv_mtx"); //观察逆矩阵 uniform mat4

	this->uni_light_dep_range_loc_      = this->shdr_prg_->get_uniform_loc("uni_light_dep_range"); //光源的depth range uniform vec2
	this->uni_light_view_mtx_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_view_mtx"); //光源观察 uniform mat4
	this->uni_light_proj_mtx_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_proj_mtx"); //光源投影 uniform mat4
	this->uni_light_dep_pix_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_pix_offset"); //光源深度纹理分辨率 uniform vec2
	this->uni_loop_loc_                 = this->shdr_prg_->get_uniform_loc("uni_loop");


#if 1
	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);
#else
//	this->attr_screen_quad_pos_loc_      = this->shdr_prg_->get_attrib_loc("attr_screen_quad_pos"); //attribute vec2
//	this->attr_screen_quad_texcoord_loc_ = this->shdr_prg_->get_attrib_loc("attr_screen_quad_texcoord"); //attribute vec2
//
//
//	this->geo_screen_quad_ = new robj_geometry(this->eng_->sence());
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
//	//生成render target,输出光照与阴影
//	win_device * dev = this->eng_->dev();
//	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();
//
//#if 0
//
//	render_target_base::ptr rtt_ptr;
//	this->rtt_light_shadow_ = rtt_mgr->create_fbo_empty(rtt_ptr,
//		(l3_int32)dev->width(), (l3_int32)dev->height(),
//		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
//		);
//
//	rtt_ptr->add_tex_clr(
//		render_target_base::e_rtt_color_format_rgba_f16);
//
//#endif
#endif
}

void shader_shadowmap_recv::pre_frame(sence_mgr * sence)
{
	//l3_int32 err_count = win_device::print_error(__FILE__, __LINE__);
	//if(err_count != 0)
	//{
	//	int a = 0;
	//	int b = a;
	//}

	this->shdr_prg_->active();

	//l3_int32 err_count1 = win_device::print_error(__FILE__, __LINE__);
	//if(err_count1 != 0)
	//{
	//	int a = 0;
	//	int b = a;
	//}

	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_light_shadow_);
	dev->clear_fbo(this->rtt_light_shadow_);
}

void shader_shadowmap_recv::post_frame(sence_mgr * sence)
{
	this->shdr_prg_->inactive();

	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_light_shadow_);
}

void shader_shadowmap_recv::prepare(robj_base& robj)
{}

void shader_shadowmap_recv::set_light_info(light_mgr::light_info::ptr li)
{
	this->l_cam_ = li->l_cam_;

	//switch(li->l_->light_type())
	//{
	//case light::E_LIGHT_DIR:
	//	this->sample_loop_ = 1;
	//	break;

	//case light::E_LIGHT_SPOT:
	//case light::E_LIGHT_POINT:
	//	this->sample_loop_ = 7;
	//	break;
	//}
}

void shader_shadowmap_recv::render_screen_quad(sence_mgr * sence)
{
#if 0
	//this->shdr_prg_->attr_bind(this->attr_screen_quad_pos_loc_, this->pos_buf_);
	//this->shdr_prg_->attr_bind(this->attr_screen_quad_texcoord_loc_, this->texcoord_buf_);
#endif

	camera * cam = sence->cam_cur();
	matrix4 cam_view_inv_mtx;
	cam->view_mtx().gen_inverse_matrix(cam_view_inv_mtx);

	this->shdr_prg_->uni_bind_mat4(this->uni_view_inv_mtx_loc_,
		cam_view_inv_mtx);

	render_target_base::ptr rtt;
	this->eng_->rtt_mgr()->get_render_target(this->rtt_light_depth_, rtt);
	if(!rtt.is_null())
	{
		if(!rtt->clr_tex_1().is_null())
			this->shdr_prg_->uni_bind_tex(this->uni_tex_light_depth_tex_loc_, rtt->clr_tex_1()->obj_id());
		if(!rtt->depth_tex().is_null())
			this->shdr_prg_->uni_bind_tex(this->uni_tex_light_depth_shadow_loc_, rtt->depth_tex()->obj_id());
	}

	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->sence_dep_tex_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->sence_clr_tex_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_loc_, this->sence_light_tex_);


	this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_, cam->aspect());
	this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f));

	this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
		cam->z_near(),
		cam->z_far() - cam->z_near());

	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_pix_offset_loc_,
		this->offset_x_, this->offset_y_);

	this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
		this->l_cam_->view_mtx());

	this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
		this->l_cam_->proj_mtx());

	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
		this->l_cam_->z_near(),
		this->l_cam_->z_far() - this->l_cam_->z_near());

	this->shdr_prg_->uni_bind_int(this->uni_loop_loc_,
		this->sample_loop_ ? this->sample_loop_ : 1);

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

}
