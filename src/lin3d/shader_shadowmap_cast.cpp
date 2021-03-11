/**
* @file shader_shadowmap_cast.cpp
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
#include "shader_shadowmap_cast.h"
#include "sence_mgr.h"
#include "win_device.h"
#include "render_target_mgr.h"

namespace l3eng{

void shader_shadowmap_cast::init()
{
	_set_shader_loc();

	render_target_mgr * rtt_mgr = this->eng_->rtt_mgr();

#if 0

 	render_target_base::ptr rtt_ptr;
	this->rtt_light_depth_ = rtt_mgr->create_fbo_empty(rtt_ptr,
		1000, 1000,
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f
		);

	rtt_ptr->add_tex_clr(
		render_target_base::e_rtt_color_format_rgba_f16);

	rtt_ptr->set_tex_dep(
		render_target_base::e_rtt_depth_format_16,
		texture_base::e_tex_compare_less,
		1.0f);

#endif

	this->offset_x_ = 1.0f / 1000.0f;
	this->offset_y_ = 1.0f / 1000.0f;
}

void shader_shadowmap_cast::pre_frame(sence_mgr * sence)
{
	this->shdr_prg_->active();
}

void shader_shadowmap_cast::post_frame(sence_mgr * sence)
{
	this->shdr_prg_->inactive();
}

void shader_shadowmap_cast::prepare(robj_base& robj)
{
	//this->l_cam_ = robj.sence()->current_light()->l_cam_;
	assert(this->l_cam_.get());

	this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
		robj.get_world_matrix());

	this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
		this->l_cam_->view_mtx());

	this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
		this->l_cam_->proj_mtx());

	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
		this->l_cam_->z_near(),
		this->l_cam_->z_far() - this->l_cam_->z_near());
}

void shader_shadowmap_cast::set_light_info(light_mgr::light_info::ptr li)
{
	this->l_cam_ = li->l_cam_;
	if(li->l_->light_type() == light::E_LIGHT_DIR)
		this->l_cam_->recal_light_mtx();

	win_device * dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_light_depth_);
	dev->clear_fbo(this->rtt_light_depth_);
}

void shader_shadowmap_cast::unset_light_info()
{
	assert(this->l_cam_.get());

	win_device * dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_light_depth_);
}

void shader_shadowmap_cast::_set_shader_loc()
{
	this->uni_light_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_range");
	this->uni_light_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_mtx");
	this->uni_light_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_proj_mtx");

	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
}

}
