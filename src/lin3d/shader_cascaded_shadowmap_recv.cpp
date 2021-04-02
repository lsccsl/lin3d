/**
* @file shader_cascaded_shadowmap_recv.cpp
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
#include "shader_cascaded_shadowmap_recv.h"
#include "sence_mgr.h"
#include "win_device.h"
#include "render_target_mgr.h"
#include "texture_mgr.h"

namespace l3eng {

void shader_cascaded_shadowmap_recv::init()
{
	assert(this->eng_);

	render_target_mgr* rtt_mgr = this->eng_->rtt_mgr();
	assert(rtt_mgr);
	texture_mgr* tex_mgr = this->eng_->tex_mgr();
	assert(tex_mgr);
	win_device* dev = this->eng_->dev();
	assert(dev);

	render_target_base::ptr rtt;
	rtt_mgr->create_fbo_empty(rtt,
		(l3_int32)dev->width(), (l3_int32)dev->height(),
		color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);
	if (rtt.is_null())
		return;
	this->rtt_light_shadow_ = rtt->obj_id();

	texture_base::ptr tex_clr;
	tex_mgr->create_tex_null(tex_clr, (l3_int32)dev->width(), (l3_int32)dev->height(),
		texture_base::e_tex_inter_type_rgba_f16);
	rtt->add_tex_clr(tex_clr);
	this->tex_light_shadow_ = tex_clr->obj_id();


	texture_base::ptr tex_depth;
	tex_mgr->create_dep_tex(tex_depth, (l3_int32)dev->width(), (l3_int32)dev->height(), L3_FALSE, texture_base::e_tex_compare_less);
	rtt->set_tex_dep(tex_depth);

	{
		this->uni_tex_light_loc_      = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light");
		this->uni_tex_view_depth_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex_view_depth");

		this->uni_view_inv_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_inv_mtx"); //观察逆矩阵
		this->uni_tan_half_fov_loc_ = this->shdr_prg_->get_uniform_loc("uni_tan_half_fov"); //视角一半的tan值
		this->uni_aspect_loc_ = this->shdr_prg_->get_uniform_loc("uni_aspect"); //屏幕宽高比
		this->uni_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep_range"); //最大深度

		this->uni_tex_light_depth_tex_loc_  = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_tex");
		this->uni_tex_light_depth_shadow_loc_  = this->shdr_prg_->get_uniform_tex_loc("uni_tex_light_depth_shadow");
		this->uni_far_thredhold_loc_ = this->shdr_prg_->get_uniform_loc("uni_far_thredhold");

		this->uni_light_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_range"); //光源的depth range
		this->uni_light_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_mtx"); //光源观察
		this->uni_light_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_proj_mtx"); //光源投影
		this->uni_light_dep_pix_offset_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_pix_offset"); //光源深度纹理分辨率
		this->uni_loop_loc_ = this->shdr_prg_->get_uniform_loc("uni_loop");
		this->uni_seg_count_loc_ = this->shdr_prg_->get_uniform_loc("uni_seg_count");
	}

	this->screen_quad_.init_screen_full(this->shdr_prg_, this->eng_);
}

void shader_cascaded_shadowmap_recv::pre_frame(sence_mgr* sence)
{
	this->shdr_prg_->active();

	win_device* dev = this->eng_->dev();

	dev->enable_rtt(this->rtt_light_shadow_);
	dev->clear_fbo(this->rtt_light_shadow_);
}

void shader_cascaded_shadowmap_recv::post_frame(sence_mgr* sence)
{
	this->shdr_prg_->inactive();

	win_device* dev = this->eng_->dev();
	dev->disable_rtt(this->rtt_light_shadow_);
}

void shader_cascaded_shadowmap_recv::prepare(robj_base& robj)
{

}

void shader_cascaded_shadowmap_recv::get_csm_cast_info(const shader_cascaded_shadowmap_cast& shdr_cast)
{
	this->offset_x_ = shdr_cast.offset_x();
	this->offset_y_ = shdr_cast.offset_y();

	for (l3_int32 i = 0; i < this->ourdoor_light_recv_.v_cam_seg_.size(); i++)
		shdr_cast.get_csm_cam_seg(i, this->ourdoor_light_recv_.v_cam_seg_[i]);
}

void shader_cascaded_shadowmap_recv::render_screen_quad(sence_mgr* sence)
{
	camera* cam = sence->cam_cur();
	matrix4 cam_view_inv_mtx;
	cam->view_mtx().gen_inverse_matrix(cam_view_inv_mtx);

	//绑定摄影机逆变换矩阵
	this->shdr_prg_->uni_bind_mat4(this->uni_view_inv_mtx_loc_,
		cam_view_inv_mtx);
	this->shdr_prg_->uni_bind_float(this->uni_aspect_loc_, cam->aspect());
	this->shdr_prg_->uni_bind_float(this->uni_tan_half_fov_loc_, tan(cam->fovy() * L3_AR_CONVERT * 0.5f));

	this->shdr_prg_->uni_bind_float_ve2(this->uni_dep_range_loc_,
		cam->z_near(),
		cam->z_far() - cam->z_near());

	//绑定场景深度纹理
	this->shdr_prg_->uni_bind_tex(this->uni_tex_view_depth_loc_, this->tex_sence_dep_);
	this->shdr_prg_->uni_bind_tex(this->uni_tex_light_loc_, this->tex_sence_light_);

	//csm shadow solution
	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_pix_offset_loc_,
		this->offset_x_, this->offset_y_);

	//绑定光源深度纹理
	{
		std::vector<OBJ_ID> v_tex_obj;
		for (l3_int32 i = 0; i < this->ourdoor_light_recv_.v_cam_seg_.size(); i++)
			v_tex_obj.push_back(this->ourdoor_light_recv_.v_cam_seg_[i].tex_csm_depth_clr_);
		//v_tex_obj.push_back(ourdoor_light_recv_.v_cam_seg_[0].tex_csm_depth_clr_);
		//v_tex_obj.push_back(ourdoor_light_recv_.v_cam_seg_[1].tex_csm_depth_clr_);
		this->shdr_prg_->uni_bind_tex_array(this->uni_tex_light_depth_tex_loc_,
			v_tex_obj);
	}

#if 1
	{
		std::vector<OBJ_ID> v_shadowtex_obj;
		for (l3_int32 i = 0; i < this->ourdoor_light_recv_.v_cam_seg_.size(); i++)
			v_shadowtex_obj.push_back(this->ourdoor_light_recv_.v_cam_seg_[i].tex_csm_depth_);
		//v_shadowtex_obj.push_back(ourdoor_light_recv_.v_cam_seg_[0].tex_csm_depth_);
		//v_shadowtex_obj.push_back(ourdoor_light_recv_.v_cam_seg_[1].tex_csm_depth_);
		this->shdr_prg_->uni_bind_tex_array(this->uni_tex_light_depth_shadow_loc_,
			v_shadowtex_obj);		
	}
#endif

	{
		std::vector<vector2> v_vec2;
		for(l3_int32 i = 0; i < this->ourdoor_light_recv_.v_cam_seg_.size(); i ++)
			v_vec2.push_back(vector2(this->ourdoor_light_recv_.v_cam_seg_[i].z_near_,
				this->ourdoor_light_recv_.v_cam_seg_[i].z_far_ - this->ourdoor_light_recv_.v_cam_seg_[i].z_near_));
		//v_vec2.push_back(vector2(ourdoor_light_recv_.v_cam_seg_[0].z_near_,
		//	ourdoor_light_recv_.v_cam_seg_[0].z_far_ - ourdoor_light_recv_.v_cam_seg_[0].z_near_));
		//v_vec2.push_back(vector2(ourdoor_light_recv_.v_cam_seg_[1].z_near_,
		//	ourdoor_light_recv_.v_cam_seg_[1].z_far_ - ourdoor_light_recv_.v_cam_seg_[1].z_near_));
		this->shdr_prg_->uni_bind_float_vec2_array(this->uni_light_dep_range_loc_, v_vec2);
	}

	{
		std::vector<matrix4> v_mtx_view;
		for(l3_int32 i = 0; i < this->ourdoor_light_recv_.v_cam_seg_.size(); i ++)
			v_mtx_view.push_back(this->ourdoor_light_recv_.v_cam_seg_[i].mtx_view_);
		//v_mtx_view.push_back(ourdoor_light_recv_.v_cam_seg_[0].mtx_view_);
		//v_mtx_view.push_back(ourdoor_light_recv_.v_cam_seg_[1].mtx_view_);
		this->shdr_prg_->uni_bind_mat4_array(this->uni_light_view_mtx_loc_, v_mtx_view);
	}

	{
		std::vector<matrix4> v_mtx_proj;
		for(l3_int32 i = 0; i < this->ourdoor_light_recv_.v_cam_seg_.size(); i ++)
			v_mtx_proj.push_back(this->ourdoor_light_recv_.v_cam_seg_[i].mtx_proj_);
		//v_mtx_proj.push_back(ourdoor_light_recv_.v_cam_seg_[0].mtx_proj_);
		//v_mtx_proj.push_back(ourdoor_light_recv_.v_cam_seg_[1].mtx_proj_);
		this->shdr_prg_->uni_bind_mat4_array(this->uni_light_proj_mtx_loc_, v_mtx_proj);
	}

	this->shdr_prg_->uni_bind_int(this->uni_loop_loc_,
		this->sample_loop_ ? this->sample_loop_ : 1);

	this->shdr_prg_->uni_bind_int(this->uni_seg_count_loc_,
		this->ourdoor_light_recv_.v_cam_seg_.size());
	/*
	l3_int32 uni_far_thredhold_loc_;
	*/
	this->screen_quad_.render_screen_quad(this->shdr_prg_);
}

}
