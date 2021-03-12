#include "l3_compile_config.h"

#ifdef HAS_TEST_MODE

/**
* @file sence_render_test.cpp
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
#include "sence_render_test.h"
#include "sence_mgr.h"
#include "l3_engine.h"
#include "render_target_mgr.h"
#include "win_device.h"
#include "texture_mgr.h"
#include "shader_program_mgr.h"
#include "l3_log.h"
#include "base_obj_mgr.h"
#include "robj_cone.h"

namespace l3eng{

sence_render_test::sence_render_test(sence_mgr * sence):sence_(sence)
	, enable_atmospheric_(L3_TRUE)
	, enable_hdr_(L3_TRUE)
	, enable_ssr_(L3_FALSE)
	, enable_ssao_(L3_TRUE)
{}

void sence_render_test::init()
{
	this->_init_rtt();
	this->_init_shader();
}

void sence_render_test::_tex_dev_sz_pool_init()
{
	/*
	win_device * dev = this->sence_->eng()->dev();
	texture_mgr * tex_mgr = this->sence_->eng()->tex_mgr();

	for(l3_int32 i = 0; i < 3; i ++)
	{
		texture_base::ptr tex_tmp;
		tex_mgr->create_tex_null(tex_tmp, (l3_int32)dev->width(), (l3_int32)dev->height(),		
			texture_base::e_tex_inter_type_rgba_f16);

		this->set_free_tex_dev_sz_.insert(tex_tmp);
	}
	*/
}

void sence_render_test::_tex_dev_sz_pool_get(texture_base::ptr& tex)
{
	if(!tex.is_null())
		L3_LOG_ENG_WARN(("not release %x", tex.get()));

	win_device * dev = this->sence_->eng()->dev();
	texture_mgr * tex_mgr = this->sence_->eng()->tex_mgr();

	if(this->set_free_tex_dev_sz_.empty())
	{
		tex_mgr->create_tex_null(tex, (l3_int32)dev->width(), (l3_int32)dev->height(),		
			texture_base::e_tex_inter_type_rgba_f16);

		this->set_tex_dev_sz_.insert(tex);
		if(this->set_tex_dev_sz_.size() >= 10){
			L3_LOG_ENG_WARN(("too many tex %d", this->set_tex_dev_sz_.size()));
		}

		return;
	}

	std::set<texture_base::ptr>::iterator it = this->set_free_tex_dev_sz_.begin();
	tex = *(it);

	this->set_free_tex_dev_sz_.erase(it);
}

void sence_render_test::_tex_dev_sz_pool_release(texture_base::ptr& tex)
{
	if(tex.is_null())
		return;

	this->set_free_tex_dev_sz_.insert(tex);
	tex.set_null();
}

void sence_render_test::_init_rtt()
{
	win_device * dev = this->sence_->eng()->dev();
	render_target_mgr * rtt_mgr = this->sence_->eng()->rtt_mgr();
	texture_mgr * tex_mgr = this->sence_->eng()->tex_mgr();

	{
		rtt_mgr->create_fbo_empty(this->rtt_dev_size_,
			(l3_int32)dev->width(), (l3_int32)dev->height(),
			color(.0f, .0f, .0f), 1.0f);
	}

	// gbuffer
	{
		rtt_mgr->create_fbo_empty(this->rtt_gbuffer_,
			(l3_int32)dev->width(), (l3_int32)dev->height(),
			color(1.0f, 1.0f, 1.0f), 1.0f);

		tex_mgr->create_tex_null(this->tex_gbuffer_diff_, (l3_int32)dev->width(), (l3_int32)dev->height(),		
			texture_base::e_tex_inter_type_rgba_f16);
		tex_mgr->create_tex_null(this->tex_gbuffer_normal_, (l3_int32)dev->width(), (l3_int32)dev->height(),		
			texture_base::e_tex_inter_type_rgba_f16);
		tex_mgr->create_tex_null(this->tex_gbuffer_dep_line_, (l3_int32)dev->width(), (l3_int32)dev->height(),		
			texture_base::e_tex_inter_type_rgba_f32);

		this->rtt_gbuffer_->add_tex_clr(this->tex_gbuffer_diff_);
		this->rtt_gbuffer_->add_tex_clr(this->tex_gbuffer_normal_);
		this->rtt_gbuffer_->add_tex_clr(this->tex_gbuffer_dep_line_);

		tex_mgr->create_dep_tex(this->dep_tex_gbuffer_proj_, (l3_int32)dev->width(), (l3_int32)dev->height(), L3_TRUE);
		this->rtt_gbuffer_->set_tex_dep(this->dep_tex_gbuffer_proj_);
	}

	// shadow cast
	{
		rtt_mgr->create_fbo_empty(this->rtt_shadow_cast_,
			1000, 1000,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);

		tex_mgr->create_tex_null(this->tex_shadow_cast_, 1000, 1000,
			texture_base::e_tex_inter_type_rgba_f16);
		this->rtt_shadow_cast_->add_tex_clr(this->tex_shadow_cast_);

		tex_mgr->create_dep_tex(this->dep_tex_shadow_cast_, 1000, 1000, L3_FALSE, texture_base::e_tex_compare_less);
		this->rtt_shadow_cast_->set_tex_dep(this->dep_tex_shadow_cast_);
	}
}

void sence_render_test::_init_shader()
{
	shader_program_mgr * shdr_mgr = this->sence_->eng()->shdr_mgr();

	// defer rendering gbuffer
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_defer_render_gbuffer, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_dr_gbuffer_ = ptr_shdr;
		assert(this->shdr_dr_gbuffer_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_dr_gbuffer_->rtt_gbuffer(this->rtt_gbuffer_->obj_id());
	}

	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_defer_render_light, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_dr_light_ = ptr_shdr;
		assert(this->shdr_dr_light_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_dr_light_->rtt_light(this->rtt_dev_size_->obj_id());
		this->shdr_dr_light_->tex_sence_view_dep(this->tex_gbuffer_dep_line_->obj_id());
		this->shdr_dr_light_->tex_sence_view_nml(this->tex_gbuffer_normal_->obj_id());
		this->shdr_dr_light_->tex_sence_view_clr(this->tex_gbuffer_diff_->obj_id());
	}

	// shadowmap cast
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_shadowmap_cast, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_shadow_cast_ = ptr_shdr;
		assert(this->shdr_shadow_cast_->is_my_type(ptr_shdr->obj_type()));
		
		this->shdr_shadow_cast_->rtt_light_depth(this->rtt_shadow_cast_->obj_id());
	}

	// shadowmap recv
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_shadowmap_recv, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_shadow_recv_ = ptr_shdr;
		assert(this->shdr_shadow_recv_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_shadow_recv_->set_sence_dep_tex(this->tex_gbuffer_dep_line_->obj_id());

		this->shdr_shadow_recv_->set_offset_x(this->shdr_shadow_cast_->offset_x());
		this->shdr_shadow_recv_->set_offset_y(this->shdr_shadow_cast_->offset_y());
		this->shdr_shadow_recv_->set_rtt_light_depth(this->rtt_shadow_cast_->obj_id());

		this->shdr_shadow_recv_->rtt_light_shadow(this->rtt_dev_size_->obj_id());
	}

	// volume light
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_volume_light, ptr_shdr);
		assert(!ptr_shdr.is_null());
		this->shdr_vol_ = ptr_shdr;
		assert(this->shdr_vol_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_vol_->rtt_light_depth(this->rtt_shadow_cast_->obj_id());
		this->shdr_vol_->tex_view_dep(this->tex_gbuffer_dep_line_->obj_id());
		this->shdr_vol_->rtt_vol(this->rtt_dev_size_->obj_id());
	}

	// defer rendering light mix
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_defer_render_light_mix, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_dr_light_mix_ = ptr_shdr;
		assert(this->shdr_dr_light_mix_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_dr_light_mix_->rtt_light_mix1(this->rtt_dev_size_->obj_id());
		this->shdr_dr_light_mix_->rtt_light_mix2(this->rtt_dev_size_->obj_id());
	}

	// defer rendering light vol mix
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_defer_render_light_vol_mix, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_dr_light_vol_mix_ = ptr_shdr;
		assert(this->shdr_dr_light_vol_mix_->is_my_type(ptr_shdr->obj_type()));		

		this->shdr_dr_light_vol_mix_->rtt_light_vol_mix1(this->rtt_dev_size_->obj_id());
		this->shdr_dr_light_vol_mix_->rtt_light_vol_mix2(this->rtt_dev_size_->obj_id());
	}

	// defer rendering final
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_defer_render_final, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_dr_final_ = ptr_shdr;
		assert(this->shdr_dr_final_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_dr_final_->rtt_final_output(this->rtt_dev_size_->obj_id());
	}

	// defer rendering atmospheric
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_defer_render_atmospheric, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_dr_atmospheric_ = ptr_shdr;
		assert(this->shdr_dr_atmospheric_->is_my_type(ptr_shdr->obj_type()));

		this->shdr_dr_atmospheric_->tex_view_depth(this->tex_gbuffer_dep_line_->obj_id());
		this->shdr_dr_atmospheric_->rtt_out_put(this->rtt_dev_size_->obj_id());
	}

	// hdr
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_hdr, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_hdr_ = ptr_shdr;
		assert(this->shdr_hdr_->is_my_type(ptr_shdr->obj_type()));
	}

	// ssao
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_ssao, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_ssao_ = ptr_shdr;

		this->shdr_ssao_->tex_view_depth(this->tex_gbuffer_dep_line_->obj_id());
		this->shdr_ssao_->tex_view_nml(this->tex_gbuffer_normal_->obj_id());

		if(!this->rtt_gbuffer_.is_null())
		{
			this->shdr_ssao_->pix_offset_x(this->rtt_gbuffer_->pix_offset_x());
			this->shdr_ssao_->pix_offset_y(this->rtt_gbuffer_->pix_offset_y());
		}
		this->shdr_ssao_->rtt_ssao(this->rtt_dev_size_->obj_id());
	}

	// ssr 屏幕空间反射
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_ssr, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_ssr_ = ptr_shdr;
		assert(this->shdr_ssr_->is_my_type(ptr_shdr->obj_type()));

		//this->shdr_ssr_->tex_final(this->tex_sence_final_);
		this->shdr_ssr_->tex_nml(this->tex_gbuffer_normal_->obj_id());
		this->shdr_ssr_->tex_clr(this->tex_gbuffer_diff_->obj_id());
		this->shdr_ssr_->tex_dep(this->tex_gbuffer_dep_line_->obj_id());

		this->shdr_ssr_->rtt_ssr(this->rtt_dev_size_->obj_id());
	}

	// 反射
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_reflect_forward, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_reflect_ = ptr_shdr;
		assert(this->shdr_reflect_->is_my_type(ptr_shdr->obj_type()));
	}

	// volume light radial blur
	{
		shader::ptr ptr_shdr;
		shdr_mgr->get_shader_by_name(shader_program_inter::_l3eng_inter_volume_light_radial_blur, ptr_shdr);
		assert(!ptr_shdr.is_null());

		this->shdr_vol_rb_ = ptr_shdr;
		assert(this->shdr_vol_rb_->is_my_type(ptr_shdr->obj_type()));
	}	
}

void sence_render_test::render_sence(const L3_RENDER_GROUP render_group)
{
	this->_render_reflect_tex();
	this->_render_refract_tex();
	this->_render_sence();

	//输出各阶段处理结果纹理
	if(this->sence_->eng()->enable_debug())
		this->_debug();

	this->_release_all_tex();
	this->_release_reflect_tex();
	this->_release_refract_tex();
}

void sence_render_test::_render_sence()
{
#if 1
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	dev->set_proj_matrix(this->sence_->cam_cur()->proj_mtx());
	dev->set_view_matrix(this->sence_->cam_cur()->view_mtx());
	dev->enable_cull_back();

	//得到g-buffer
	this->_render_gbuffer();

	//屏幕空间遮挡
	if(this->enable_ssao_)
		this->_render_ssao();

	//光照/阴影/体积光
	this->_render_light();

	//大气散射
	if(this->enable_atmospheric_)
		this->_render_atmospheric();

	//合并所有处理结果
	this->_tex_dev_sz_pool_get(this->ref_tex_final_);
	this->_render_final(this->ref_tex_final_);

	//forward渲染
	this->_render_forward(this->ref_tex_final_);
	this->_render_reflect_obj(this->ref_tex_final_);

	texture_base::ptr tex_input = this->ref_tex_final_;

	//屏幕空间局部反射
	if(this->enable_ssr_)
	{
		this->shdr_ssr_->set_reflect_far();
		this->_render_ssr(this->ref_tex_final_);

		//this->shdr_ssr_->set_reflect_near();
		//this->_render_ssr(this->ref_tex_final_);

		tex_input = this->ref_tex_ssr_;
	}
#endif

	OBJ_ID tex_final = tex_input->obj_id();

	//高亮与hdr合并
	//将体积光融合进最终位图

	this->shdr_vol_rb_->set_test_mode(1);
	this->shdr_vol_rb_->vol_rb_tex_src(tex_input->obj_id());
	dev->active_shdr(this->shdr_vol_rb_);
	this->shdr_vol_rb_->pre_frame(this->sence_);
	this->shdr_vol_rb_->render_screen_quad(this->sence_);
	this->shdr_vol_rb_->post_frame(this->sence_);
	dev->set_active_shdr_null();

	//this->sence_->render_show_tex(this->shdr_vol_rb_->tex_output_final(),
	//	0.0f, 0.0f,
	//	2.0f, 2.0f, 0.9f);

	tex_final = this->shdr_vol_rb_->tex_output_final();

	//hdr处理
	if(this->enable_hdr_)
	{
		this->_render_hdr(tex_final);
	}
	else
	{
		this->sence_->render_show_tex(tex_final,
			0.0f, 0.0f,
			2.0f, 2.0f, 0.9f);
	}
}

void sence_render_test::_release_all_tex()
{
	{
		if(!this->ref_tex_light_vol_mix_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_light_vol_mix_);
		if(!this->ref_tex_light_mix_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_light_mix_);
		if(!this->ref_tex_atmospheric_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_atmospheric_);
		if(!this->ref_tex_ssao_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_ssao_);		
		//if(!this->ref_tex_obj_far_.is_null())
		//	this->_tex_dev_sz_pool_release(this->ref_tex_obj_far_);
	}

	{
		if(!this->ref_tex_final_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_final_);
		if(!this->ref_tex_ssr_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_ssr_);
	}
}

void sence_render_test::_release_reflect_tex()
{
	if(this->map_tex_reflect_.empty())
		return;

	for(MAP_TEX_REFLECT::iterator it = this->map_tex_reflect_.begin(); it != this->map_tex_reflect_.end(); it ++)
		this->_tex_dev_sz_pool_release(it->second);

	this->map_tex_reflect_.clear();
}
void sence_render_test::_release_refract_tex()
{
	if(this->map_tex_refract_.empty())
		return;

	for(MAP_TEX_REFLECT::iterator it = this->map_tex_refract_.begin(); it != this->map_tex_refract_.end(); it ++)
		this->_tex_dev_sz_pool_release(it->second);

	this->map_tex_refract_.clear();
}

void sence_render_test::_render_reflect_obj(texture_base::ptr& tex_out_put)
{
	if(tex_out_put.is_null())
		return;

	const sence_mgr::MAP_ROBJ& render_obj = this->sence_->render_group_obj(L3_RENDER_GROUP_WATER);
	if(render_obj.empty())
		return;

	{
		static l3_f32 __wave_offset_[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		static l3_f32 __wave_speed_[4]  = {9.0f, 4.5f, -8.0f, -3.5f};

		l3_uint64 t1 = this->sence_->eng()->t_elapse();
		l3_f32 t11 = (t1 / 1000.0f) * 0.001f;
		__wave_offset_[0] = __wave_speed_[0] * t11;
		__wave_offset_[1] = __wave_speed_[1] * t11;
		__wave_offset_[2] = __wave_speed_[2] * t11;
		__wave_offset_[3] = __wave_speed_[3] * t11;
		this->shdr_reflect_->set_param(EN_SHADER_PARAM_RELECT_wave_offset, __wave_offset_, sizeof(__wave_offset_));
	}

	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->rtt_dev_size_->add_tex_clr(tex_out_put);
	this->rtt_dev_size_->set_tex_dep(this->dep_tex_gbuffer_proj_);

	dev->enable_rtt(this->rtt_dev_size_->obj_id());

	dev->active_shdr(this->shdr_reflect_);
	this->shdr_reflect_->pre_frame(this->sence_);
	for(sence_mgr::MAP_ROBJ::const_iterator it = render_obj.begin(); it != render_obj.end(); it ++)
	{
		robj_base * robj = it->second.get();
		if(!robj)
			continue;

		this->shdr_reflect_->prepare(*robj);
		robj->render();
	}

	this->shdr_reflect_->post_frame(this->sence_);
	dev->set_active_shdr_null();
	dev->disable_rtt(this->rtt_dev_size_->obj_id());
}

void sence_render_test::_render_reflect_tex()
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	const sence_mgr::MAP_ROBJ& map_water_obj =  this->sence_->render_group_obj(L3_RENDER_GROUP_WATER);
	if(map_water_obj.empty())
		return;

	camera::ptr cam_tmp = this->sence_->eng()->obj_mgr()->create_obj<camera, sence_mgr *>(this->sence_);

	const matrix4& org_mtx_view = this->sence_->cam_cur()->view_mtx();
	const matrix4& org_mtx_proj = this->sence_->cam_cur()->proj_mtx();

	*cam_tmp = *(this->sence_->cam_cur());
	dev->push_cull_info();
	dev->enable_cull_front();

	for(sence_mgr::MAP_ROBJ::const_iterator it = map_water_obj.begin(); it != map_water_obj.end(); it ++)
	{
		const robj_base::ptr& obj_reflect = it->second;
		if(obj_reflect.is_null())
			continue;

		this->sence_->set_cam_cur(cam_tmp);

		vector3 v_up;
		obj_reflect->dir_world_up(v_up);
		plane reflect_pln(v_up, obj_reflect->get_world_pos());

		// 设置反射变换矩阵
		matrix4 reflect_view_mtx;
		{
			matrix4 reflect_mtx;
			matrix4::gen_reflect_matrix(reflect_pln, reflect_mtx);

			reflect_mtx.mult(org_mtx_view, reflect_view_mtx);

			cam_tmp->view_mtx(reflect_view_mtx);
			dev->set_view_matrix(reflect_view_mtx);
		}

		// 设置斜投影矩阵
		{
			plane reflect_plant_viewspace;
			reflect_view_mtx.trans_plane(reflect_plant_viewspace, reflect_pln);

			matrix4 mtx_oblique_proj;
			org_mtx_proj.gen_oblique_projection(reflect_plant_viewspace, mtx_oblique_proj);

			cam_tmp->proj_mtx(mtx_oblique_proj);
			dev->set_proj_matrix(mtx_oblique_proj);
		}


		texture_base::ptr reflect_tex;
		this->_tex_dev_sz_pool_get(reflect_tex);

		{
			this->_render_gbuffer();

			//屏幕空间遮挡
			if(this->enable_ssao_)
				this->_render_ssao();

			//光照/阴影/体积光
			this->_render_light();

			//大气散射
			if(this->enable_atmospheric_)
				this->_render_atmospheric();

			//合并所有处理结果
			this->_render_final(reflect_tex);

			//forward渲染
			this->_render_forward(reflect_tex);
		}

		obj_reflect->get_material().set_tex_obj_id(reflect_tex->obj_id());
		this->map_tex_reflect_[obj_reflect->obj_id()] = reflect_tex;

		this->_release_all_tex();
	}

	dev->pop_cull_info();
	this->sence_->reset_cam_cur();
}

void sence_render_test::_render_refract_tex()
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	const sence_mgr::MAP_ROBJ& map_water_obj =  this->sence_->render_group_obj(L3_RENDER_GROUP_WATER);
	if(map_water_obj.empty())
		return;

	camera::ptr cam_tmp = this->sence_->eng()->obj_mgr()->create_obj<camera, sence_mgr *>(this->sence_);

	const matrix4& org_mtx_view = this->sence_->cam_cur()->view_mtx();
	const matrix4& org_mtx_proj = this->sence_->cam_cur()->proj_mtx();

	*cam_tmp = *(this->sence_->cam_cur());
	dev->push_cull_info();
	dev->enable_cull_back();

	for(sence_mgr::MAP_ROBJ::const_iterator it = map_water_obj.begin(); it != map_water_obj.end(); it ++)
	{
		const robj_base::ptr& obj_reflect = it->second;
		if(obj_reflect.is_null())
			continue;

		this->sence_->set_cam_cur(cam_tmp);

		vector3 v_up;
		obj_reflect->dir_world_up(v_up);
		plane reflect_pln(v_up, obj_reflect->get_world_pos());

		dev->set_view_matrix(org_mtx_view);

		// 设置斜投影矩阵
		{
			plane reflect_plant_viewspace;
			org_mtx_view.trans_plane(reflect_plant_viewspace, reflect_pln, -1.0f);

			matrix4 mtx_oblique_proj;
			org_mtx_proj.gen_oblique_projection(reflect_plant_viewspace, mtx_oblique_proj);

			cam_tmp->proj_mtx(mtx_oblique_proj);
			dev->set_proj_matrix(mtx_oblique_proj);
		}

		texture_base::ptr refract_tex;
		this->_tex_dev_sz_pool_get(refract_tex);

		{
			this->_render_gbuffer();

			//屏幕空间遮挡
			if(this->enable_ssao_)
				this->_render_ssao();

			//光照/阴影/体积光
			this->_render_light();

			//大气散射
			if(this->enable_atmospheric_)
				this->_render_atmospheric();

			//合并所有处理结果
			this->_render_final(refract_tex);

			//forward渲染
			this->_render_forward(refract_tex);
		}

		obj_reflect->get_material().set_tex_obj_id(refract_tex->obj_id(), 1);
		this->map_tex_refract_[obj_reflect->obj_id()] = refract_tex;

		this->_release_all_tex();
	}

	dev->pop_cull_info();
	this->sence_->reset_cam_cur();
}

void sence_render_test::_render_gbuffer()
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	dev->active_shdr(this->shdr_dr_gbuffer_);
	this->shdr_dr_gbuffer_->pre_frame(this->sence_);

	const sence_mgr::MAP_ROBJ& render_obj = this->sence_->render_group_obj(L3_RENDER_GROUP_DEFAULT);
	for(sence_mgr::MAP_ROBJ::const_iterator it = render_obj.begin(); it != render_obj.end(); it ++)
	{
		robj_base * robj = it->second.get();
		l3_bool need_process = robj->defer_render() && robj->defer_light();
		if(!need_process)
			continue;

		this->shdr_dr_gbuffer_->prepare(*robj);
		robj->render();
	}

	this->shdr_dr_gbuffer_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_ssao()
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->_tex_dev_sz_pool_get(this->ref_tex_ssao_);
	this->rtt_dev_size_->add_tex_clr(this->ref_tex_ssao_);

	//处理光照与阴影
	dev->active_shdr(this->shdr_ssao_);
	this->shdr_ssao_->pre_frame(this->sence_);

	this->shdr_ssao_->render_screen_quad(this->sence_);

	this->shdr_ssao_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_light()
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	const light_mgr::MAP_LIGHT& map_light = this->sence_->get_light_mgr()->get_map_light();

	//清掉前一帧留下的数据(ref_tex_light_vol_mix_1_/ref_tex_light_vol_mix_2_)
	{
		this->rtt_dev_size_->del_all_tex_clr();
		this->rtt_dev_size_->unset_tex_dep();

		this->_tex_dev_sz_pool_get(this->ref_tex_light_vol_mix_1_);
		this->_tex_dev_sz_pool_get(this->ref_tex_light_vol_mix_2_);

		this->shdr_dr_light_vol_mix_->tex_light_vol_mix1(this->ref_tex_light_vol_mix_1_->obj_id());
		this->shdr_dr_light_vol_mix_->tex_light_vol_mix2(this->ref_tex_light_vol_mix_2_->obj_id());

		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_vol_mix_1_);
		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_vol_mix_2_);

		dev->enable_rtt(this->rtt_dev_size_->obj_id());
		dev->clear_fbo(this->rtt_dev_size_->obj_id());
		dev->disable_rtt(this->rtt_dev_size_->obj_id());
	}

	//清掉前一帧留下的数据(ref_tex_light_mix_1_/ref_tex_light_mix_2_)
	{
		this->rtt_dev_size_->del_all_tex_clr();
		this->rtt_dev_size_->unset_tex_dep();

		this->_tex_dev_sz_pool_get(this->ref_tex_light_mix_1_);
		this->_tex_dev_sz_pool_get(this->ref_tex_light_mix_2_);

		this->shdr_dr_light_mix_->tex_light_mix1(this->ref_tex_light_mix_1_->obj_id());
		this->shdr_dr_light_mix_->tex_light_mix2(this->ref_tex_light_mix_2_->obj_id());

		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_mix_1_);
		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_mix_2_);

		dev->enable_rtt(this->rtt_dev_size_->obj_id());
		dev->clear_fbo(this->rtt_dev_size_->obj_id());
		dev->disable_rtt(this->rtt_dev_size_->obj_id());
	}

	l3_int32 idx = 0;
	for(light_mgr::MAP_LIGHT::const_iterator it = map_light.begin(); it != map_light.end(); it ++)
	{
		if(it->second.is_null())
			continue;
		const light_mgr::light_info::ptr& li = it->second;

		if(li->l_cam_.is_null())
			continue;

		//处理每一盏灯的光照与阴影
		this->_render_light_one(li);

		if(this->sence_->eng()->enable_debug() && li->l_->enable_light_debug())
			this->_debug_light(idx);

		idx ++;

		if(!this->ref_tex_light_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_light_);
		if(!this->ref_tex_light_shadow_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_light_shadow_);
		if(!this->ref_tex_light_vol_.is_null())
			this->_tex_dev_sz_pool_release(this->ref_tex_light_vol_);
	}

	{
		if(!this->ref_tex_light_vol_mix_1_.is_null())
		{
			if(this->shdr_dr_light_vol_mix_->idx_rtt() == 0)
			{
				this->ref_tex_light_vol_mix_ = this->ref_tex_light_vol_mix_1_;
				this->ref_tex_light_vol_mix_1_.set_null();
			}
			else
				this->_tex_dev_sz_pool_release(this->ref_tex_light_vol_mix_1_);

		}
		if(!this->ref_tex_light_vol_mix_2_.is_null())
		{
			if(this->shdr_dr_light_vol_mix_->idx_rtt() == 1)
			{
				this->ref_tex_light_vol_mix_ = this->ref_tex_light_vol_mix_2_;
				this->ref_tex_light_vol_mix_2_.set_null();
			}
			else
				this->_tex_dev_sz_pool_release(this->ref_tex_light_vol_mix_2_);
		}
	}

	{
		if(!this->ref_tex_light_mix_1_.is_null())
		{
			if(this->shdr_dr_light_mix_->idx_mix() == 0)
			{
				this->ref_tex_light_mix_ = this->ref_tex_light_mix_1_;
				this->ref_tex_light_mix_1_.set_null();
			}
			else
				this->_tex_dev_sz_pool_release(this->ref_tex_light_mix_1_);

		}
		if(!this->ref_tex_light_mix_2_.is_null())
		{
			if(this->shdr_dr_light_mix_->idx_mix() == 1)
			{
				this->ref_tex_light_mix_ = this->ref_tex_light_mix_2_;
				this->ref_tex_light_mix_2_.set_null();
			}
			else
				this->_tex_dev_sz_pool_release(this->ref_tex_light_mix_2_);
		}
	}
}

void sence_render_test::_render_light_one_lighting(const light_mgr::light_info::ptr& li)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->_tex_dev_sz_pool_get(this->ref_tex_light_);
	this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_);

	dev->active_shdr(this->shdr_dr_light_);
	this->shdr_dr_light_->pre_frame(this->sence_);

	this->shdr_dr_light_->set_light(li, this->sence_);
	this->shdr_dr_light_->render_screen_quad(this->sence_);

	this->shdr_dr_light_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_light_one_shadow_cast(const light_mgr::light_info::ptr& li)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	//得到shadow map
	dev->push_cull_info();
	dev->enable_cull_front();

	dev->active_shdr(this->shdr_shadow_cast_);
	this->shdr_shadow_cast_->pre_frame(this->sence_);

	this->shdr_shadow_cast_->set_light_info(li);
	const sence_mgr::MAP_ROBJ& render_obj = this->sence_->render_group_obj(L3_RENDER_GROUP_DEFAULT);
	for(sence_mgr::MAP_ROBJ::const_iterator it = render_obj.begin(); it != render_obj.end(); it ++)
	{
		robj_base * robj = it->second.get();

		this->shdr_shadow_cast_->prepare(*robj);
		robj->render();
	}
	this->shdr_shadow_cast_->unset_light_info();

	this->shdr_shadow_cast_->post_frame(this->sence_);
	dev->set_active_shdr_null();

	dev->pop_cull_info();
}

void sence_render_test::_render_light_one_shadow_recv(const light_mgr::light_info::ptr& li)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->_tex_dev_sz_pool_get(this->ref_tex_light_shadow_);	
	this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_shadow_);

	if(!this->ref_tex_light_.is_null())
		this->shdr_shadow_recv_->set_sence_light_tex(this->ref_tex_light_->obj_id());

	//处理光照与阴影
	dev->active_shdr(this->shdr_shadow_recv_);
	this->shdr_shadow_recv_->pre_frame(this->sence_);

	this->shdr_shadow_recv_->set_light_info(li);
	this->shdr_shadow_recv_->render_screen_quad(this->sence_);

	this->shdr_shadow_recv_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_light_one_vol(const light_mgr::light_info::ptr& li)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->_tex_dev_sz_pool_get(this->ref_tex_light_vol_);
	this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_vol_);

	// 体积光处理
	dev->active_shdr(this->shdr_vol_);
	this->shdr_vol_->pre_frame(this->sence_);

	this->shdr_vol_->set_light_info(li);
	this->shdr_vol_->render_screen_quad(this->sence_);

	this->shdr_vol_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_light_one_mix(const light_mgr::light_info::ptr& li)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();
	if(this->shdr_dr_light_mix_->idx_mix() == 1)
		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_mix_1_);
	else
		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_mix_2_);

	//混合光照与阴影
	dev->active_shdr(this->shdr_dr_light_mix_);
	this->shdr_dr_light_mix_->pre_frame(this->sence_);

	if(li->l_->enable_shadow())
		this->shdr_dr_light_mix_->tex_light_shadow(this->ref_tex_light_shadow_->obj_id());
	else
		this->shdr_dr_light_mix_->tex_light_shadow(this->ref_tex_light_->obj_id());

	this->shdr_dr_light_mix_->render_screen_quad();

	this->shdr_dr_light_mix_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_light_one_vol_mix(const light_mgr::light_info::ptr& li)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();
	if(this->shdr_dr_light_vol_mix_->idx_rtt() == 1)
		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_vol_mix_1_);
	else
		this->rtt_dev_size_->add_tex_clr(this->ref_tex_light_vol_mix_2_);

	//混合体积光
	dev->active_shdr(this->shdr_dr_light_vol_mix_);
	this->shdr_dr_light_vol_mix_->pre_frame(this->sence_);

	this->shdr_dr_light_vol_mix_->tex_vol(this->ref_tex_light_vol_->obj_id());

	this->shdr_dr_light_vol_mix_->render_screen_quad();

	this->shdr_dr_light_vol_mix_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_light_one(const light_mgr::light_info::ptr& li)
{
	this->_render_light_one_lighting(li);

	//阴影caster(在光源处构造摄影机,得到深度纹理)
	if(li->l_->enable_shadow() || li->l_->enable_vol())
		this->_render_light_one_shadow_cast(li);

	//阴影receiver(全屏处理)
	if(li->l_->enable_shadow())
		this->_render_light_one_shadow_recv(li);

	//体积光全屏处理
	if(li->l_->enable_vol())
	{
		this->_render_light_one_vol(li);
		this->_render_light_one_vol_mix(li);
	}

	this->_render_light_one_mix(li);
}

void sence_render_test::_render_final(texture_base::ptr& tex_out_put)
{
	if(tex_out_put.is_null())
		return;

	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->rtt_dev_size_->add_tex_clr(tex_out_put);

	//处理光照与阴影
	dev->active_shdr(this->shdr_dr_final_);
	this->shdr_dr_final_->pre_frame(this->sence_);

	if(!this->ref_tex_light_mix_.is_null())
		this->shdr_dr_final_->tex_light_shadow(this->ref_tex_light_mix_->obj_id());
	else
		this->shdr_dr_final_->tex_light_shadow(base_obj::INVALID_OBJ_ID);

	if(!this->ref_tex_light_vol_mix_.is_null())
	{
		this->shdr_dr_final_->has_light_vol(1);
		this->shdr_dr_final_->tex_light_vol(this->ref_tex_light_vol_mix_->obj_id());
	}
	else
	{
		this->shdr_dr_final_->has_light_vol(0);
		this->shdr_dr_final_->tex_light_vol(base_obj::INVALID_OBJ_ID);
	}

	if(!this->ref_tex_ssao_.is_null())
	{
		this->shdr_dr_final_->has_ssao(1);
		this->shdr_dr_final_->tex_ssao(this->ref_tex_ssao_->obj_id());
	}
	else
	{
		this->shdr_dr_final_->has_ssao(0);
		this->shdr_dr_final_->tex_ssao(base_obj::INVALID_OBJ_ID);
	}

	if(!this->ref_tex_atmospheric_.is_null())
	{
		this->shdr_dr_final_->has_atmospheric(1);
		this->shdr_dr_final_->atmospheric_scale(1.0f);
		this->shdr_dr_final_->tex_atmospheric(this->ref_tex_atmospheric_->obj_id());
	}
	else
	{
		this->shdr_dr_final_->has_atmospheric(0);
		this->shdr_dr_final_->atmospheric_scale(0.0f);
		this->shdr_dr_final_->tex_atmospheric(base_obj::INVALID_OBJ_ID);
	}

	this->shdr_dr_final_->tex_sence_clr(this->tex_gbuffer_diff_->obj_id());
	this->shdr_dr_final_->tex_view_depth(this->tex_gbuffer_dep_line_->obj_id());
	this->shdr_dr_final_->tex_view_proj_depth(this->dep_tex_gbuffer_proj_->obj_id());

	this->shdr_dr_final_->render_screen_quad();

	this->shdr_dr_final_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_forward(texture_base::ptr& tex_out_put)
{
	if(tex_out_put.is_null())
		return;

	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->rtt_dev_size_->add_tex_clr(tex_out_put);
	this->rtt_dev_size_->set_tex_dep(this->dep_tex_gbuffer_proj_);
	
	dev->enable_rtt(this->rtt_dev_size_->obj_id());
	const sence_mgr::MAP_ROBJ& render_obj = this->sence_->render_group_obj(L3_RENDER_GROUP_DEFAULT);
	for(sence_mgr::MAP_ROBJ::const_iterator it = render_obj.begin(); it != render_obj.end(); it ++)
	{
		robj_base * robj = it->second.get();
		l3_bool need_process = !robj->defer_render();
		if(!need_process)
			continue;

		robj->render();
	}
	dev->disable_rtt(this->rtt_dev_size_->obj_id());
}

void sence_render_test::_render_atmospheric()
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->_tex_dev_sz_pool_get(this->ref_tex_atmospheric_);
	this->rtt_dev_size_->add_tex_clr(this->ref_tex_atmospheric_);

	dev->active_shdr(this->shdr_dr_atmospheric_);
	this->shdr_dr_atmospheric_->pre_frame(this->sence_);

	this->shdr_dr_atmospheric_->render_screen_quad();

	this->shdr_dr_atmospheric_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_hdr(OBJ_ID tex_input)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	//处理光照与阴影
	dev->active_shdr(this->shdr_hdr_);
	this->shdr_hdr_->pre_frame(this->sence_);

	this->shdr_hdr_->tex_final(tex_input);

	this->shdr_hdr_->pix_offset_x(1.0f / dev->width());
	this->shdr_hdr_->pix_offset_y(1.0f / dev->height());

	this->shdr_hdr_->render_screen_quad();

	this->shdr_hdr_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::_render_ssr(texture_base::ptr& tex_reflect_src)
{
	win_device * dev = this->sence_->eng()->dev();
	assert(dev);

	this->rtt_dev_size_->del_all_tex_clr();
	this->rtt_dev_size_->unset_tex_dep();

	this->_tex_dev_sz_pool_get(this->ref_tex_ssr_);
	this->rtt_dev_size_->add_tex_clr(this->ref_tex_ssr_);

	this->shdr_ssr_->tex_reflect_src(tex_reflect_src->obj_id());
	this->shdr_ssr_->tex_final(this->ref_tex_final_->obj_id());

	dev->active_shdr(this->shdr_ssr_);
	this->shdr_ssr_->pre_frame(this->sence_);

	this->shdr_ssr_->render_screen_quad(this->sence_);

	this->shdr_ssr_->post_frame(this->sence_);
	dev->set_active_shdr_null();
}

void sence_render_test::set_sun_light(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree)
{
	light::ptr l = this->sence_->get_light_mgr()->create_light_dir(x_degree, y_degree, z_degree);
	if (l.is_null())
		return;
	outdoor_light_.sun_light_ = l;
}

void sence_render_test::_render_sunlight_cascaded_shadowmap_cast()
{
	//分割摄像机视截体
	//this->sence_->cam_cur()-> // frustum_mesh_t
	//为每个视截体生成包围盒,并得到平行光摄影机变换矩阵x4
	//虚拟平行光摄影机得到深度纹理
}

void sence_render_test::_render_sunlight_cascaded_shadowmap_recv()
{
	//传入4个深度纹理,以及对应的摄影机变换矩阵 
	//每个像素点根据深度,用对应的变换矩阵与深度纹理,查询出阴影
}

void sence_render_test::_debug()
{
	l3_f32 y = 0.0f;
	this->sence_->render_show_tex(this->tex_gbuffer_normal_->obj_id(),
		0.0f, y,
		0.1f, 0.1f);
	y += 0.1f;

	this->sence_->render_show_tex(this->tex_gbuffer_dep_line_->obj_id(),
		0.0f, y,
		0.1f, 0.1f);
	y += 0.1f;

	this->sence_->render_show_tex(this->tex_gbuffer_diff_->obj_id(),
		0.0f, y,
		0.1f, 0.1f);
	y += 0.1f;

	this->sence_->render_show_tex(this->dep_tex_gbuffer_proj_->obj_id(),
		0.0f, y,
		0.15f, 0.1f);
	y += 0.1f;	

	if(!this->ref_tex_light_mix_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_light_mix_->obj_id(),
			0.0f, y,
			0.2f, 0.1f);
		y += 0.1f;
	}

	if(!this->ref_tex_light_vol_mix_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_light_vol_mix_->obj_id(),
			0.0f, y,
			0.1f, 0.1f);
		y += 0.1f;
	}

	if(!this->ref_tex_atmospheric_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_atmospheric_->obj_id(),
			0.0f, y,
			0.2f, 0.1f);
		y += 0.1f;
	}

	if(!this->ref_tex_final_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_final_->obj_id(),
			0.0f, y,
			0.1f, 0.1f);
		y += 0.1f;
	}

	if(!this->ref_tex_ssao_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_ssao_->obj_id(),
			0.0f, y,
			0.1f, 0.1f);
		y += 0.1f;
	}

	//if(!this->ref_tex_obj_far_.is_null())
	//{
	//	this->sence_->render_show_tex(this->ref_tex_obj_far_->obj_id(),
	//		0.0f, y,
	//		0.1f, 0.1f);
	//	y += 0.1f;
	//}

	if(!this->ref_tex_ssr_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_ssr_->obj_id(),
			0.0f, y,
			0.2f, 0.2f);
		y += 0.2f;
	}

	for(MAP_TEX_REFLECT::iterator it = this->map_tex_reflect_.begin(); it != this->map_tex_reflect_.end(); it ++)
	{
		this->sence_->render_show_tex(it->second->obj_id(),
			0.0f, y,
			0.5f, 0.5f);
		y += 0.5f;
	}

	for(MAP_TEX_REFLECT::iterator it = this->map_tex_refract_.begin(); it != this->map_tex_refract_.end(); it ++)
	{
		this->sence_->render_show_tex(it->second->obj_id(),
			0.0f, y,
			0.5f, 0.5f);
		y += 0.5f;
	}

	win_device * dev = this->sence_->eng()->dev();
	assert(dev);
	dev->set_proj_matrix(this->sence_->cam_cur()->proj_mtx());
	dev->set_view_matrix(this->sence_->cam_cur()->view_mtx());

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
			break;
		}
	}
}

void sence_render_test::_debug_light(l3_int32 idx)
{
	l3_f32 debug_width = 0.4f;
	l3_f32 debug_height = 0.4f;
	l3_f32 x_base = 0.5f;

	l3_f32 debug_x = x_base;

	if(!this->ref_tex_light_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_light_->obj_id(),
			debug_x, 0.0f + idx * debug_height,
			debug_width, debug_height);
		debug_x += debug_width;	
	}

	if(!ref_tex_light_shadow_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_light_shadow_->obj_id(),
			debug_x, 0.0f + idx * debug_height,
			debug_width, debug_height);
		debug_x += debug_width;	
	}

	this->sence_->render_show_tex(this->dep_tex_shadow_cast_->obj_id(),
		debug_x, 0.0f + idx * debug_height,
		debug_width, debug_height);
	debug_x += debug_width;


	if(!this->ref_tex_light_vol_.is_null())
	{
		this->sence_->render_show_tex(this->ref_tex_light_vol_->obj_id(),
			debug_x, 0.0f + idx * debug_height,
			debug_width, debug_height);
		debug_x += debug_width;
	}

}


}

#endif