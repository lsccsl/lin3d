/**
* @file shader_cascaded_shadowmap_cast.cpp
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
#include "shader_cascaded_shadowmap_cast.h"
#include "sence_mgr.h"
#include "win_device.h"
#include "render_target_mgr.h"
#include "texture_mgr.h"

namespace l3eng {


shader_cascaded_shadowmap_cast::shader_cascaded_shadowmap_cast(l3_engine* eng) :shader(eng), csm_shadow_solution_(CSM_SHADOWTEX_SOLUTION)
#ifdef HAS_TEST_MODE
	, debug_geo_bbox_(eng->sence())
	, debug_geo_lightspace_bbox_(eng->sence())
	, debug_geo_campos_(eng->sence())
	, debug_geo_campos_lightspace_(eng->sence())
	, debug_vec_zfar_(eng->sence())
	//, debug_geo_cam_seg_(eng->sence())
	, debug_geo_near_top_left_(eng->sence())
	, debug_geo_near_top_right(eng->sence())
	, debug_geo_near_bottom_left(eng->sence())
	, debug_geo_near_bottom_right(eng->sence())
	, debug_geo_far_top_left(eng->sence())
	, debug_geo_far_top_right(eng->sence())
	, debug_geo_far_bottom_left(eng->sence())
	, debug_geo_far_bottom_right(eng->sence())
#endif
{}

void shader_cascaded_shadowmap_cast::init()
{
	assert(this->eng_);

	render_target_mgr* rtt_mgr = this->eng_->rtt_mgr();
	assert(rtt_mgr);
	texture_mgr* tex_mgr = this->eng_->tex_mgr();
	assert(tex_mgr);

	this->v_seg_weight_.push_back(1);
	this->v_seg_weight_.push_back(2);
	this->v_seg_weight_.push_back(4);
	this->v_seg_weight_.push_back(10);
	this->seg_weight_total_ = 10;

	for (l3_int32 i = 0; i < this->outdoor_light_.v_cam_seg_.size(); i++)
	{
		outdoor_light_cam_seg& cam_seg = this->outdoor_light_.v_cam_seg_[i];
		render_target_base::ptr rtt;
		rtt_mgr->create_fbo_empty(rtt,
			this->csm_shadow_solution_, this->csm_shadow_solution_,
			color(0.0f, 0.0f, 0.0f, 0.0f), 1.0f);
		if (rtt.is_null())
			return;
		cam_seg.rtt_csm_cast_ = rtt->obj_id();

		texture_base::ptr tex_clr;
		tex_mgr->create_tex_null(tex_clr, this->csm_shadow_solution_, this->csm_shadow_solution_,
			texture_base::e_tex_inter_type_rgba_f16);
		rtt->add_tex_clr(tex_clr);
		cam_seg.csm_cam_seg.tex_csm_depth_clr_ = tex_clr->obj_id();

		texture_base::ptr tex_depth;
		tex_mgr->create_dep_tex(tex_depth, this->csm_shadow_solution_, this->csm_shadow_solution_, L3_FALSE, texture_base::e_tex_compare_less);
		rtt->set_tex_dep(tex_depth);
		cam_seg.csm_cam_seg.tex_csm_depth_ = tex_depth->obj_id();
	}

	{
		this->uni_light_dep_range_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dep_range");
		this->uni_light_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_view_mtx");
		this->uni_light_proj_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_proj_mtx");

		this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx");
	}

	this->offset_x_ = 1.0f / this->csm_shadow_solution_;
	this->offset_y_ = 1.0f / this->csm_shadow_solution_;
}

void shader_cascaded_shadowmap_cast::cal_cam_seg()
{
	assert(this->eng_);
	sence_mgr* sence = this->eng_->sence();
	if (!sence)
		return;

	//分割摄像机视截体
	vector3 near_top_left;
	vector3 near_top_right;
	vector3 near_bottom_left;
	vector3 near_bottom_right;
	vector3 far_top_left;
	vector3 far_top_right;
	vector3 far_bottom_left;
	vector3 far_bottom_right;
	sence->cam_cur()->get_fg_point(near_top_left,
		near_top_right,
		near_bottom_left,
		near_bottom_right,
		far_top_left,
		far_top_right,
		far_bottom_left,
		far_bottom_right);

	l3_f32 fact = 1.0f / this->seg_weight_total_;// this->outdoor_light_.cascaded_count_;
	vector3 diff_top_left = (far_top_left - near_top_left) * fact;
	vector3 diff_top_right = (far_top_right - near_top_right) * fact;
	vector3 diff_bottom_left = (far_bottom_left - near_bottom_left) * fact;
	vector3 diff_bottom_right = (far_bottom_right - near_bottom_right) * fact;

	vector3 tmp_top_left;
	vector3 tmp_top_right;
	vector3 tmp_bottom_left;
	vector3 tmp_bottom_right;
	vector3 tmp1_top_left;
	vector3 tmp1_top_right;
	vector3 tmp1_bottom_left;
	vector3 tmp1_bottom_right;

	vector3 tmp_lightspace_top_left;
	vector3 tmp_lightspace_top_right;
	vector3 tmp_lightspace_bottom_left;
	vector3 tmp_lightspace_bottom_right;
	vector3 tmp1_lightspace_top_left;
	vector3 tmp1_lightspace_top_right;
	vector3 tmp1_lightspace_bottom_left;
	vector3 tmp1_lightspace_bottom_right;

	light_mgr::light_info::ptr light_info;
	sence->get_light_mgr()->get_light_info(this->outdoor_light_.sun_light_, light_info);
	if (light_info.is_null())
		return;

	aabbox tmp_lightspace_bbox; //光源空间的包围盒
	aabbox tmp_bbox; //光源空间的包围盒
	for (l3_int32 i = 0; i < this->outdoor_light_.v_cam_seg_.size() && i < this->v_seg_weight_.size(); i++)
	{
		//生成正交摄影机参数
		tmp_lightspace_bbox.extent(aabbox::AABB_NULL);

		outdoor_light_cam_seg& cam_seg = this->outdoor_light_.v_cam_seg_[i];

		//视截体分段变换到光源空间
		tmp_top_left = near_top_left;// +diff_top_left * i;
		tmp_top_right = near_top_right;// + diff_top_right * i;
		tmp_bottom_left = near_bottom_left;// + diff_bottom_left * i;
		tmp_bottom_right = near_bottom_right;// + diff_bottom_right * i;

		if (this->outdoor_light_.v_cam_seg_.size() - 1 == i)
		{
			tmp1_top_left = far_top_left;
			tmp1_top_right = far_top_right;
			tmp1_bottom_left = far_bottom_left;
			tmp1_bottom_right = far_bottom_right;
		}
		else
		{
			tmp1_top_left  = tmp_top_left  + diff_top_left * (this->v_seg_weight_[i]);
			tmp1_top_right = tmp_top_right + diff_top_right * (this->v_seg_weight_[i]);
			tmp1_bottom_left  = tmp_bottom_left  + diff_bottom_left  * (this->v_seg_weight_[i]);
			tmp1_bottom_right = tmp_bottom_right + diff_bottom_right * (this->v_seg_weight_[i]);
		}

		tmp_bbox.merge(tmp_top_left);
		tmp_bbox.merge(tmp_top_right);
		tmp_bbox.merge(tmp_bottom_left);
		tmp_bbox.merge(tmp_bottom_right);
		tmp_bbox.merge(tmp1_top_left);
		tmp_bbox.merge(tmp1_top_right);
		tmp_bbox.merge(tmp1_bottom_left);
		tmp_bbox.merge(tmp1_bottom_right);

		matrix4 view_mtx_light_rotate;
		light_info->l_cam_->view_mtx().get_rotate(view_mtx_light_rotate);

		//变换到光源空间
		view_mtx_light_rotate.trans_rotate_vect3(tmp_top_left, tmp_lightspace_top_left);
		view_mtx_light_rotate.trans_rotate_vect3(tmp_top_right, tmp_lightspace_top_right);
		view_mtx_light_rotate.trans_rotate_vect3(tmp_bottom_left, tmp_lightspace_bottom_left);
		view_mtx_light_rotate.trans_rotate_vect3(tmp_bottom_right, tmp_lightspace_bottom_right);

		view_mtx_light_rotate.trans_rotate_vect3(tmp1_top_left, tmp1_lightspace_top_left);
		view_mtx_light_rotate.trans_rotate_vect3(tmp1_top_right, tmp1_lightspace_top_right);
		view_mtx_light_rotate.trans_rotate_vect3(tmp1_bottom_left, tmp1_lightspace_bottom_left);
		view_mtx_light_rotate.trans_rotate_vect3(tmp1_bottom_right, tmp1_lightspace_bottom_right);

		//求出光源空间包围盒
		tmp_lightspace_bbox.merge(tmp_lightspace_top_left);
		tmp_lightspace_bbox.merge(tmp_lightspace_top_right);
		tmp_lightspace_bbox.merge(tmp_lightspace_bottom_left);
		tmp_lightspace_bbox.merge(tmp_lightspace_bottom_right);
		tmp_lightspace_bbox.merge(tmp1_lightspace_top_left);
		tmp_lightspace_bbox.merge(tmp1_lightspace_top_right);
		tmp_lightspace_bbox.merge(tmp1_lightspace_bottom_left);
		tmp_lightspace_bbox.merge(tmp1_lightspace_bottom_right);

		//包围盒光源空间的中心点
		vector3 vc_lightspace;
		tmp_lightspace_bbox.get_center(vc_lightspace);
		vector3 v_campos_lightspace = vc_lightspace;
		v_campos_lightspace.z(tmp_lightspace_bbox.max_z());

		//摄像机位置变换回世界空间
		vector3 v_campos;
		matrix4 inv_view_mtx_light_rotate;
		view_mtx_light_rotate.gen_inverse_matrix(inv_view_mtx_light_rotate);
		inv_view_mtx_light_rotate.trans_rotate_vect3(v_campos_lightspace, v_campos);

		cam_seg.csm_cam_seg.z_near_ = 0;// -1.0;
		cam_seg.csm_cam_seg.z_far_ = (tmp_lightspace_bbox.max_z() - tmp_lightspace_bbox.min_z())/* + 1.0f*/;

#ifdef HAS_TEST_MODE
		if (0 == i && this->debug_gen_bbox_)
		{
			debug_geo_bbox(tmp_lightspace_bbox, debug_geo_lightspace_bbox_, L3_TRUE);
			debug_geo_bbox(tmp_bbox, debug_geo_bbox_, L3_FALSE);
			debug_geo_pos(v_campos, debug_geo_campos_, L3_TRUE);
			debug_geo_pos(v_campos_lightspace, debug_geo_campos_lightspace_, L3_FALSE);
			debug_vector(v_campos, vector3(0, -1, 0), cam_seg.csm_cam_seg.z_far_ - cam_seg.csm_cam_seg.z_near_, debug_vec_zfar_, 0);
			debug_geo_cam_seg(tmp_top_left,
				tmp_top_right,
				tmp_bottom_left,
				tmp_bottom_right,
				tmp1_top_left,
				tmp1_top_right,
				tmp1_bottom_left,
				tmp1_bottom_right
			);
			this->debug_gen_bbox_ = L3_FALSE;
		}
#endif

		//移动光源虚拟摄影机位置
		cam_seg.csm_cam_seg.mtx_view_ = light_info->l_cam_->view_mtx();

		matrix4 move_mtx;
		matrix4::gen_move_matrix(move_mtx, -v_campos.x(), -v_campos.y(), -v_campos.z());
		move_mtx.mult(view_mtx_light_rotate, cam_seg.csm_cam_seg.mtx_view_);//见camera::check_and_cal_mtx,类似处理

		//proj_mtx
		matrix4::gen_ortho_proj_matrix(tmp_lightspace_bbox.max_x() - tmp_lightspace_bbox.min_x(),
			tmp_lightspace_bbox.max_y() - tmp_lightspace_bbox.min_y(),
			cam_seg.csm_cam_seg.z_near_, cam_seg.csm_cam_seg.z_far_,
			cam_seg.csm_cam_seg.mtx_proj_);
	}

	//为每个视截体生成包围盒,并得到平行光摄影机变换矩阵x4
	//虚拟平行光摄影机得到深度纹理
}

void shader_cascaded_shadowmap_cast::pre_frame(sence_mgr* sence)
{
	if (this->cur_cam_seg_ >= this->outdoor_light_.v_cam_seg_.size())
		return;
	this->shdr_prg_->active();

	assert(this->eng_);
	win_device* dev = this->eng_->dev();
	assert(eng);
	assert(dev);

	outdoor_light_cam_seg& cam_seg = this->outdoor_light_.v_cam_seg_[cur_cam_seg_];
	dev->enable_rtt(cam_seg.rtt_csm_cast_);
	dev->clear_fbo(cam_seg.rtt_csm_cast_);
}

void shader_cascaded_shadowmap_cast::post_frame(sence_mgr* sence)
{
	if (this->cur_cam_seg_ >= this->outdoor_light_.v_cam_seg_.size())
		return;
	this->shdr_prg_->inactive();

	outdoor_light_cam_seg& cam_seg = this->outdoor_light_.v_cam_seg_[cur_cam_seg_];
	l3_engine* eng = sence->eng();
	win_device* dev = eng->dev();
	assert(eng);
	assert(dev);
	dev->disable_rtt(cam_seg.rtt_csm_cast_);
}

void shader_cascaded_shadowmap_cast::prepare(robj_base& robj)
{
	if (this->cur_cam_seg_ >= this->outdoor_light_.v_cam_seg_.size())
		return;

	outdoor_light_cam_seg& cam_seg = this->outdoor_light_.v_cam_seg_[this->cur_cam_seg_];

	this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
		robj.get_world_matrix());

	this->shdr_prg_->uni_bind_mat4(this->uni_light_view_mtx_loc_,
		cam_seg.csm_cam_seg.mtx_view_);
	this->shdr_prg_->uni_bind_mat4(this->uni_light_proj_mtx_loc_,
		cam_seg.csm_cam_seg.mtx_proj_);
	this->shdr_prg_->uni_bind_float_ve2(this->uni_light_dep_range_loc_,
		cam_seg.csm_cam_seg.z_near_,//0,//cam_seg.z_near_,
		cam_seg.csm_cam_seg.z_far_ - cam_seg.csm_cam_seg.z_near_//100//cam_seg.z_far_ - cam_seg.z_near_
	);
}

void shader_cascaded_shadowmap_cast::set_light_info(OBJ_ID light_obj)
{
	this->outdoor_light_.sun_light_ = light_obj;
}

OBJ_ID shader_cascaded_shadowmap_cast::get_cam_seg_depthtex(const l3_int32 cam_seg_idx)
{
	if (cam_seg_idx >= this->outdoor_light_.v_cam_seg_.size())
		return base_obj::INVALID_OBJ_ID;

	outdoor_light_cam_seg& cam_seg = this->outdoor_light_.v_cam_seg_[cam_seg_idx];
	
	return cam_seg.csm_cam_seg.tex_csm_depth_clr_;
}

#ifdef HAS_TEST_MODE
void shader_cascaded_shadowmap_cast::debug_geo_bbox(aabbox& bbox, robj_geometry& robj, l3_bool b)
{
	robj.clear_mesh();
	if (b)
	{
		robj.create_cube_by_aabb(bbox, L3_TRUE,
			255, 255, 0,
			127);
	}
	else
	{
		robj.create_cube_by_aabb(bbox, L3_TRUE,
			255, 0, 255,
			127);
	}
	robj.get_material().blend_mode(L3_BLEND_MODE_ALPHA1);
	robj.get_material().enable_depth_test(L3_FALSE);
	robj.get_material().poly_mode(material::E_POLY_LINE);
}

void shader_cascaded_shadowmap_cast::debug_geo_pos(const vector3& pos, robj_geometry& robj, l3_bool b)
{
	robj.clear_mesh();

	if (b)
	{
		robj.create_cube(10, 10, 10, L3_TRUE,
			0, 255, 255,
			127);
	}
	else
	{
		robj.create_cube(10, 10, 10, L3_TRUE,
			255, 0, 255,
			127);
	}

	robj.move_to_xyz(pos.x(), pos.y(), pos.z());

	robj.get_material().blend_mode(L3_BLEND_MODE_ALPHA1);
	robj.get_material().enable_depth_test(L3_FALSE);
	robj.get_material().poly_mode(material::E_POLY_LINE);
}

void shader_cascaded_shadowmap_cast::debug_vector(const vector3& v_start, const vector3& dir, const l3_f32 len, robj_vector& robj, l3_bool b)
{
	robj.set_vector(dir, len);
	robj.set_start(v_start);
	robj.get_material().enable_depth_test(L3_FALSE);
}
void shader_cascaded_shadowmap_cast::debug_geo_cam_seg(const vector3& near_top_left,
	const vector3& near_top_right,
	const vector3& near_bottom_left,
	const vector3& near_bottom_right,
	const vector3& far_top_left,
	const vector3& far_top_right,
	const vector3& far_bottom_left,
	const vector3& far_bottom_right)
{
	debug_geo_pos(near_top_left,     this->debug_geo_near_top_left_,    L3_FALSE);
	debug_geo_pos(near_top_right,    this->debug_geo_near_top_right,    L3_FALSE);
	debug_geo_pos(near_bottom_left,  this->debug_geo_near_bottom_left,  L3_FALSE);
	debug_geo_pos(near_bottom_right, this->debug_geo_near_bottom_right, L3_FALSE);
	debug_geo_pos(far_top_left,      this->debug_geo_far_top_left,      L3_TRUE);
	debug_geo_pos(far_top_right,     this->debug_geo_far_top_right,     L3_TRUE);
	debug_geo_pos(far_bottom_left,   this->debug_geo_far_bottom_left,   L3_TRUE);
	debug_geo_pos(far_bottom_right,  this->debug_geo_far_bottom_right,  L3_TRUE);

	//this->debug_geo_cam_seg_.clear_mesh();

	//this->debug_geo_cam_seg_.add_triangle(near_top_left,
	//	near_bottom_left,
	//	near_top_right);

	//this->debug_geo_cam_seg_.add_triangle(near_top_right,
	//	near_bottom_left,
	//	near_bottom_right);

	//this->debug_geo_cam_seg_.add_triangle(far_top_left,
	//	far_bottom_left,
	//	far_top_right);

	//this->debug_geo_cam_seg_.add_triangle(far_top_right,
	//	far_bottom_left,
	//	far_bottom_right);

	//this->debug_geo_cam_seg_.get_material().blend_mode(L3_BLEND_MODE_ALPHA1);
	//this->debug_geo_cam_seg_.get_material().enable_depth_test(L3_FALSE);
	//this->debug_geo_cam_seg_.get_material().poly_mode(material::E_POLY_LINE);
}

void shader_cascaded_shadowmap_cast::debug_render()
{
	this->debug_geo_bbox_.render();
	//this->debug_geo_lightspace_bbox_.render();
	//this->debug_geo_campos_.render();
	//this->debug_geo_campos_lightspace_.render();
	//this->debug_vec_zfar_.render();
	//this->debug_geo_cam_seg_.render();

	this->debug_geo_near_top_left_.render();
	this->debug_geo_near_top_right.render();
	this->debug_geo_near_bottom_left.render();
	this->debug_geo_near_bottom_right.render();
	this->debug_geo_far_top_left.render();
	this->debug_geo_far_top_right.render();
	this->debug_geo_far_bottom_left.render();
	this->debug_geo_far_bottom_right.render();
}
#endif
}
