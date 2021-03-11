/**
* @file shader_defer_render_gbuffer.cpp
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
#include "shader_defer_render_gbuffer.h"
#include "sence_mgr.h"
#include "l3_engine.h"
#include "render_target_mgr.h"
#include "win_device.h"


namespace l3eng{

void shader_defer_render_gbuffer::init()
{
	this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range");
	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
	this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx");
	this->uni_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_proj_mtx");

	this->uni_tex1_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex1");
	this->uni_tex2_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex2");
	
	this->uni_shininess_loc_ = this->shdr_prg_->get_uniform_loc("uni_shininess");
	this->uni_enable_reflect_loc_ = this->shdr_prg_->get_uniform_loc("uni_enable_reflect");

	win_device * dev = this->eng_->dev();
	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0
 	render_target_base::ptr rtt_ptr;
	this->rtt_gbuffer_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		(l3_int32)dev->width(), (l3_int32)dev->height()
		,color(1.1f, 1.1f, 1.1f), 1.0f
		);

	/*
	* tex1 : rgb + 是否反射
	* tex2 : normal + 是否高亮
	* tex3 : 32位深度
	*/
	rtt_ptr->add_tex_clr(
		//(l3_int32)dev->width(), (l3_int32)dev->height(),
		render_target_base::e_rtt_color_format_rgba_f16);
	rtt_ptr->add_tex_clr(
		//(l3_int32)dev->width(), (l3_int32)dev->height(),
		render_target_base::e_rtt_color_format_rgba_f16);
	rtt_ptr->add_tex_clr(
		//(l3_int32)dev->width(), (l3_int32)dev->height(),
		render_target_base::e_rtt_color_format_luminance_f32);

	rtt_ptr->set_tex_dep(
		//(l3_int32)dev->width(), (l3_int32)dev->height(),
		render_target_base::e_rtt_depth_format_32,
		texture_base::e_tex_compare_none,
		1.0f);
#endif
}

void shader_defer_render_gbuffer::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_gbuffer_);
	dev->clear_fbo(this->rtt_gbuffer_);

	this->shdr_prg_->active();
}

void shader_defer_render_gbuffer::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_gbuffer_);

	this->shdr_prg_->inactive();
}

void shader_defer_render_gbuffer::prepare(robj_base& robj)
{
	{
		this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
			robj.get_world_matrix());
	}

	{
		material& mtrl = robj.get_material();

		OBJ_ID tex1 = mtrl.get_tex_obj_id(0);
		OBJ_ID tex2 = mtrl.get_tex_obj_id(1);

		if(tex1 != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_tex1_loc_, tex1);
		if(tex2 != base_obj::INVALID_OBJ_ID)
			this->shdr_prg_->uni_bind_tex(this->uni_tex2_loc_, tex2);
	}

	{
		camera * cam = robj.sence()->cam_cur();

		this->shdr_prg_->uni_bind_mat4(this->uni_proj_mtx_loc_,
			cam->proj_mtx());

		this->shdr_prg_->uni_bind_mat4(this->uni_view_mtx_loc_,
			cam->view_mtx());

		this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
			cam->z_near(),
			cam->z_far() - cam->z_near());
	}

	this->shdr_prg_->uni_bind_float(this->uni_shininess_loc_,
		robj.get_material().shininess());
	this->shdr_prg_->uni_bind_int(this->uni_enable_reflect_loc_,
		robj.get_material().enable_reflect());
}

}
