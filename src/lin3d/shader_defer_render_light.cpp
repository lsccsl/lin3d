/**
* @file shader_defer_render_light.cpp
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
#include "shader_defer_render_light.h"
#include "win_device.h"
#include "render_target_mgr.h"
#include "robj_geometry.h"
#include "camera.h"
#include "sence_mgr.h"
#include "robj_base.h"

namespace l3eng{

void shader_defer_render_light::init()
{
	this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov");
	this->uni_dep_range_loc_    = this->shdr_prg_->get_uniform_loc("uni_dep_range"); //最大深度 uniform vec2
	this->uni_aspect_loc_       = this->shdr_prg_->get_uniform_loc("uni_aspect"); //屏幕宽高比 uniform float

	this->uni_tex_nml_loc_        = this->shdr_prg_->get_uniform_tex_loc("uni_tex_nml"); //法线 uniform sampler2D
	this->uni_tex_clr_loc_        = this->shdr_prg_->get_uniform_tex_loc("uni_tex_clr"); //颜色 uniform sampler2D
	this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth"); //摄影机空间深度纹理 uniform sampler2D

	this->uni_light_type_loc_          = this->shdr_prg_->get_uniform_loc("uni_light_type"); //光的类型 uniform int
	this->uni_light_view_spot_dir_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_spot_dir"); //Spotlight方向 uniform vec3
	this->uni_light_view_pos_loc_      = this->shdr_prg_->get_uniform_loc("uni_light_view_pos"); //灯光位置(w=0平行光方向 w=1点光源位置) uniform vec4
	this->uni_light_clr_loc_           = this->shdr_prg_->get_uniform_loc("uni_light_clr"); // uniform vec3
	this->uni_light_attenuation_loc_   = this->shdr_prg_->get_uniform_loc("uni_light_attenuation"); //距离大于此值开始衰减 uniform float
	this->uni_light_max_len_loc_       = this->shdr_prg_->get_uniform_loc("uni_light_max_len"); //距离大于此值光线衰减为0 uniform float
	this->uni_light_cutoff_cos_loc_    = this->shdr_prg_->get_uniform_loc("uni_light_cutoff_cos"); //聚光灯,光锥轴线与边缘夹角的cos值 uniform float
	this->uni_light_exponent_loc_      = this->shdr_prg_->get_uniform_loc("uni_light_exponent"); //聚光灯由中心向边缘衰减 uniform float


#if 1
	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);
#else
//	this->attr_screen_quad_pos_loc_      = this->shdr_prg_->get_attrib_loc("attr_screen_quad_pos"); //attribute vec2
//	this->attr_screen_quad_texcoord_loc_ = this->shdr_prg_->get_attrib_loc("attr_screen_quad_texcoord"); //attribute vec2
//
//
//	win_device * dev = this->eng_->dev();
//	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();
//
////#if 0
////
////
//// 	render_target_base::ptr rtt_ptr;
////	this->rtt_light_ = rtt_mgr->create_fbo_empty(rtt_ptr,
////		(l3_int32)dev->width(), (l3_int32)dev->height(),
////		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
////		);
////
////	rtt_ptr->add_tex_clr(
////		render_target_base::e_rtt_color_format_rgba_f16);
////
////#endif
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
#endif
}

void shader_defer_render_light::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_light_);
	dev->clear_fbo(this->rtt_light_);

	this->shdr_prg_->active();
}

void shader_defer_render_light::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_light_);

	this->shdr_prg_->inactive();
}

void shader_defer_render_light::set_light(const light_mgr::light_info::ptr& li, sence_mgr * sence)
{
	this->li_ = li;
	light& l = *this->li_->l_;

	vector3 light_pos;

	camera * cam = sence->cam_cur();
	cam->view_mtx().trans_vect3(l.pos(), light_pos, 1);	

	l3_f32 light_x = light_pos.x();
	l3_f32 light_y = light_pos.y();
	l3_f32 light_z = light_pos.z();
	l3_f32 light_w = 1.0f;

	switch(l.light_type())
	{
	case light::E_LIGHT_DIR:
		{
			vector3 light_dir;
			cam->view_mtx().trans_vect3(l.dir(), light_dir, 0);
			light_x = light_dir.x();
			light_y = light_dir.y();
			light_z = light_dir.z();
			light_w = 0.0f;
		}
		break;

	case light::E_LIGHT_SPOT:
		{
			vector3 spot_dir;
			cam->view_mtx().trans_vect3(l.dir(), spot_dir, 0);

			this->shdr_prg_->uni_bind_float_vec3(this->uni_light_view_spot_dir_loc_,
				spot_dir.x(),
				spot_dir.y(),
				spot_dir.z());

			this->shdr_prg_->uni_bind_float(this->uni_light_cutoff_cos_loc_,
				cos(l.cutoff() * L3_AR_CONVERT));

			this->shdr_prg_->uni_bind_float(this->uni_light_exponent_loc_,
				l.exponent());
		}
		break;

	default:
		break;
	}

	this->shdr_prg_->uni_bind_int(this->uni_light_type_loc_,
		l.light_type());

	this->shdr_prg_->uni_bind_float_vec4(this->uni_light_view_pos_loc_,
		light_x,
		light_y,
		light_z,
		light_w);

	this->shdr_prg_->uni_bind_float_vec3(this->uni_light_clr_loc_,
		l.diffuse().r(),
		l.diffuse().g(),
		l.diffuse().b());

	this->shdr_prg_->uni_bind_float(this->uni_light_attenuation_loc_,
		l.attenuation_begin());

	this->shdr_prg_->uni_bind_float(this->uni_light_max_len_loc_,
		l.light_max_len());
}

void shader_defer_render_light::un_set_light()
{
}

void shader_defer_render_light::render_screen_quad(sence_mgr * sence)
{
#if 0
	//this->shdr_prg_->attr_bind(this->attr_screen_quad_pos_loc_, this->pos_buf_);
	//this->shdr_prg_->attr_bind(this->attr_screen_quad_texcoord_loc_, this->texcoord_buf_);
#endif

	camera * cam = sence->cam_cur();

	{
		this->shdr_prg_->uni_bind_tex(this->uni_tex_clr_loc_, this->tex_sence_view_clr_);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_nml_loc_, this->tex_sence_view_nml_);
		this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_sence_view_dep_);
	}

	this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
		cam->z_near(),
		cam->z_far() - cam->z_near());

	this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_,
		cam->aspect());

	this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f)); //todo option here

#if 1
	this->screen_quad_.render_screen_quad(this->shdr_prg_);
#else
	//this->geo_screen_quad_->render();
#endif
}

}

