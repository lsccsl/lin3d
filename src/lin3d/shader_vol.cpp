/**
* @file shader_vol.cpp
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
#include "shader_vol.h"
#include "render_target_mgr.h"
#include "l3_engine.h"
#include "camera.h"
#include "sence_mgr.h"
#include "win_device.h"

namespace l3eng{

void shader_vol::init()
{
	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth"); //摄影机空间深度纹理 uniform sampler2D

	this->uni_tex_clr_loc_                = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr");
	this->uni_tex_light_depth_tex_loc_    = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_tex"); //光源空间深度 uniform sampler2D
	this->uni_tex_light_depth_shadow_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_shadow"); //同一个纹理,深度 uniform sampler2DShadow
	this->uni_light_world_spot_dir_loc_   = this->shdr_prg_->get_uniform_loc("uni_light_world_spot_dir"); //Spotlight方向 uniform vec3
	this->uni_light_cutoff_cos_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_cutoff_cos"); //聚光灯,光锥轴线与边缘夹角的cos值 uniform float
	this->uni_light_clr_loc_              = this->shdr_prg_->get_uniform_loc("uni_light_clr"); //灯光颜色
	this->uni_light_type_loc_             = this->shdr_prg_->get_uniform_loc("uni_light_type"); //光的类型 uniform int

	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov"); //视角一半的tan值 uniform float
	this->uni_aspect_loc_       = this->shdr_prg_->get_uniform_loc("uni_aspect"); //屏幕宽高比 uniform float
	this->uni_dep_range_loc_    = this->shdr_prg_->get_uniform_loc("uni_dep_range"); //最大深度 uniform vec2

	this->uni_view_inv_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_inv_mtx"); //观察逆矩阵 uniform mat4

	this->uni_light_dep_range_loc_      = this->shdr_prg_->get_uniform_loc("uni_light_dep_range"); //光源的depth range uniform vec2
	this->uni_light_view_mtx_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_view_mtx"); //光源观察 uniform mat4
	this->uni_light_proj_mtx_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_proj_mtx"); //光源投影 uniform mat4
	//this->uni_light_dep_pix_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_pix_offset"); //光源深度纹理分辨率 uniform vec2

	this->uni_eye_world_pos_loc_   = this->shdr_prg_->get_uniform_loc("uni_eye_world_pos");
	this->uni_light_world_pos_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_world_pos");


	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);


	/* 生成输出缓冲 */
	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

	render_target_base::ptr rtt_ptr;
	this->rtt_vol_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height(),
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

#endif
}

void shader_vol::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_vol_);
	dev->clear_fbo(this->rtt_vol_);

	this->shdr_prg_->active();
}

void shader_vol::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_vol_);

	this->shdr_prg_->inactive();
}

void shader_vol::set_light_info(light_mgr::light_info::ptr li)
{
	this->li_ = li;
}

void shader_vol::render_screen_quad(sence_mgr * sence)
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_view_dep_);
this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->tex_sence_clr_);

	render_target_base::ptr rtt;
	this->eng_->rtt_mgr()->get_render_target(this->rtt_light_depth_, rtt);
	if(!rtt.is_null())
	{
		if(!rtt->clr_tex_1().is_null())
			this->shdr_prg_->uni_bind_tex(this->uni_tex_light_depth_tex_loc_, rtt->clr_tex_1()->obj_id());
		if(!rtt->depth_tex().is_null())
			this->shdr_prg_->uni_bind_tex(this->uni_tex_light_depth_shadow_loc_, rtt->depth_tex()->obj_id());
	}

	camera * cam = sence->cam_cur();
	matrix4 cam_view_inv_mtx;
	cam->view_mtx().gen_inverse_matrix(cam_view_inv_mtx);

	this->shdr_prg_->uni_bind_mat4(this->uni_view_inv_mtx_loc_,
		cam_view_inv_mtx);

	this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_, cam->aspect());
	this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f));

	this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
		cam->z_near(),
		cam->z_far() - cam->z_near());

	this->shdr_prg_->uni_bind_float_vec3(uni_eye_world_pos_loc_,
		cam->x_pos(),
		cam->y_pos(),
		cam->z_pos());
	this->shdr_prg_->uni_bind_float_vec3(uni_light_world_pos_loc_,
		this->li_->l_->pos().x(),
		this->li_->l_->pos().y(),
		this->li_->l_->pos().z());

	this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
		this->li_->l_cam_->view_mtx());

	this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
		this->li_->l_cam_->proj_mtx());

	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
		this->li_->l_cam_->z_near(),
		this->li_->l_cam_->z_far() - this->li_->l_cam_->z_near());

	light& l = *(this->li_->l_);
	switch(l.light_type())
	{
	//case light::E_LIGHT_DIR:
	//	{
	//		vector3 light_dir;
	//		cam->view_mtx().trans_vect3(l.dir(), light_dir, 0);
	//		light_x = light_dir.x();
	//		light_y = light_dir.y();
	//		light_z = light_dir.z();
	//		light_w = 0.0f;
	//	}
	//	break;

	case light::E_LIGHT_SPOT:
		{
			this->shdr_prg_->uni_bind_float_vec3(this->uni_light_world_spot_dir_loc_,
				l.dir().x(),
				l.dir().y(),
				l.dir().z());

			this->shdr_prg_->uni_bind_float(this->uni_light_cutoff_cos_loc_,
				cos(l.cutoff() * L3_AR_CONVERT));
		}
		break;

	default:
		break;
	}
	this->shdr_prg_->uni_bind_int(this->uni_light_type_loc_,
		l.light_type());

	this->shdr_prg_->uni_bind_float_vec3(this->uni_light_clr_loc_,
		l.diffuse().r(),
		l.diffuse().g(),
		l.diffuse().b());

	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

}
