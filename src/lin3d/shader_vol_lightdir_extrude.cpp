/**
* @file shader_vol_lightdir_extrude.cpp
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
#include "shader_vol_lightdir_extrude.h"
#include "l3_engine.h"
#include "robj_base.h"
#include "camera.h"
#include "sence_mgr.h"
#include "win_device.h"

namespace l3eng{

void shader_vol_lightdir_extrude::init()
{
	this->uni_light_dir_loc_ = this->shdr_prg_->get_uniform_loc("uni_light_dir");
	this->uni_vol_max_loc_ = this->shdr_prg_->get_uniform_loc("uni_vol_max");
	this->uni_dis_from_light_loc_ = this->shdr_prg_->get_uniform_loc("uni_dis_from_light");

	this->uni_world_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_world_mtx"); //世界变换
	this->uni_view_mtx_loc_ = this->shdr_prg_->get_uniform_loc("uni_view_mtx"); //观察
	this->uni_proj_mtx_loc_= this->shdr_prg_->get_uniform_loc("uni_proj_mtx"); //投影矩阵

	this->v_light_pos_dir_ = vector4(0.0f, 30.0f, 0.0f, 1.0f);
	this->vol_max_ = 100.0f;
	this->dis_from_light_ = 1.0f;
	this->is_dir_light_ = L3_FALSE;
}

void shader_vol_lightdir_extrude::pre_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	if(!dev)
		return;

	dev->blend_mode(L3_BLEND_MODE_ALPHA);
	this->shdr_prg_->active();

	dev->push_cull_info();
	//dev->disable_cull_face();
	//dev->enable_cull_front();
	dev->enable_cull_back();


	dev->enable_poly_offset_fill(-1.1f, -1.1f);//防止z-fighting
}

void shader_vol_lightdir_extrude::post_frame(sence_mgr * sence)
{
	win_device * dev = this->eng_->dev();
	if(!dev)
		return;

	this->eng_->dev()->blend_mode(L3_BLEND_MODE_NONE);
	this->shdr_prg_->inactive();

	dev->pop_cull_info();
	this->eng_->dev()->disable_poly_offset_fill();
}

void shader_vol_lightdir_extrude::prepare(robj_base& robj)
{
	this->shdr_prg_->uni_bind_mat4(this->uni_world_mtx_loc_,
		robj.get_world_matrix());

	{
		camera * cam = robj.sence()->cam_cur();

		this->shdr_prg_->uni_bind_mat4(this->uni_proj_mtx_loc_,
			cam->proj_mtx());
		this->shdr_prg_->uni_bind_mat4(this->uni_view_mtx_loc_,
			cam->view_mtx());
	}

	this->shdr_prg_->uni_bind_float_vec4(this->uni_light_dir_loc_,
		this->v_light_pos_dir_.x(),
		this->v_light_pos_dir_.y(),
		this->v_light_pos_dir_.z(),
		this->v_light_pos_dir_.w());

	this->shdr_prg_->uni_bind_float(this->uni_vol_max_loc_, this->vol_max_);

	if(this->is_dir_light_)
	{
		this->shdr_prg_->uni_bind_float(this->uni_dis_from_light_loc_, 1.0f);
	}
	else
	{
		vector3 v_dis = robj.get_world_pos() - vector3(this->v_light_pos_dir_.x(), this->v_light_pos_dir_.y(), this->v_light_pos_dir_.z());
		this->dis_from_light_ = v_dis.get_length();
		this->shdr_prg_->uni_bind_float(this->uni_dis_from_light_loc_, this->dis_from_light_);
	}
}

void shader_vol_lightdir_extrude::set_light_dir(const vector3 light_pos_dir, const l3_bool is_dir)
{
	this->is_dir_light_ = is_dir;
	this->v_light_pos_dir_ = vector4(
		light_pos_dir.x(),
		light_pos_dir.y(),
		light_pos_dir.z(),
		is_dir ? 0 : 1.0f
		);
}

void shader_vol_lightdir_extrude::set_vol_max(const l3_f32 vm)
{
	this->vol_max_ = vm;
}

void shader_vol_lightdir_extrude::set_param_f32(const l3_int32 param_type, const l3_f32 param_val)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_VOL_LIGHTDIR_EXTRUDE_VOL_MAX:
		this->vol_max_ = param_val;
		break;

	default:
		break;
	}
}

void shader_vol_lightdir_extrude::set_param_vec3(const l3_int32 param_type, const vector3& param_val)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_VOL_LIGHTDIR_EXTRUDE_LIGHT_POS_DIR:
		this->v_light_pos_dir_.x(param_val.x());
		this->v_light_pos_dir_.y(param_val.y());
		this->v_light_pos_dir_.z(param_val.z());
		break;

	default:
		break;
	}
}

void shader_vol_lightdir_extrude::set_param_bool(const l3_int32 param_type, const l3_bool b)
{
	switch(param_type)
	{
	case EN_SHADER_PARAM_VOL_LIGHTDIR_EXTRUDE_LIGHT_IS_DIR:
		this->is_dir_light_ = b;
		break;

	default:
		break;
	}
}

}