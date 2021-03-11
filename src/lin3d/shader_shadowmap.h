///**
//* @file shader_shadowmap.h
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
//#ifndef __L3ENG_SHADER_SHADOWMAP_H__
//#define __L3ENG_SHADER_SHADOWMAP_H__
//
//#include "shader.h"
//#include "shader_program_inter.h"
//#include "light_mgr.h"
//
//enum EN_SHADOWMAP_STAGE{
//	EN_SHADOWMAP_STAGE_CAST = 0,
//	EN_SHADOWMAP_STAGE_RECV = 1,
//};
//
//namespace l3eng{
//
//class robj_geometry;
//
//class shader_shadowmap : public shader
//{ 
//public:
//
//	typedef ref_ptr_thread<shader_shadowmap> ptr;
//	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_SHADOWMAP)
//
//public:
//
//	shader_shadowmap(l3_engine * eng):shader(eng),
//		stage_(EN_SHADOWMAP_STAGE_CAST)
//	{}
//	virtual ~shader_shadowmap(){}
//
//	virtual void init() override;
//
//	virtual void pre_frame(sence_mgr * sence) override;
//	virtual void post_frame(sence_mgr * sence) override;
//	virtual void prepare(robj_base& robj) override;
//
//	virtual const std::string& shader_name() override
//	{
//		static std::string n(shader_program_inter::_l3eng_inter_shadowmap);
//		return n;
//	}
//
//public:
//
//	void render_screen_quad();
//
//	void set_light_info(const light_mgr::light_info& li);
//
//	inline void set_sence_dep_tex(OBJ_ID sence_dep)
//	{ this->sence_dep_tex_ = sence_dep; }
//	inline void set_sence_nml_tex(OBJ_ID sence_nml)
//	{ this->sence_nml_tex_ = sence_nml; }
//	inline void set_sence_clr_tex(OBJ_ID sence_clr)
//	{ this->sence_clr_tex_ = sence_clr; }
//	inline void set_sence_light_tex(OBJ_ID sence_light)
//	{ this->sence_light_tex_ = sence_light; }
//
//	inline void set_stage_cast()
//	{ this->stage_ = EN_SHADOWMAP_STAGE_CAST; }
//	inline void set_stage_recv()
//	{ this->stage_ = EN_SHADOWMAP_STAGE_RECV; }
//
//private:
//
//	void _set_shader_loc();
//
//	void _init_screen_quad();
//
//private:
//
//	/* @brief 分辨率 */
//	l3_f32 offset_x_;
//	l3_f32 offset_y_;
//	/* @brief 深度缓存 */
//	OBJ_ID rtt_light_depth_;
//
//	EN_SHADOWMAP_STAGE stage_;
//
//	//OBJ_ID rtt_dep_; //光源深度纹理rtt
//	OBJ_ID sence_dep_tex_; //场景深度
//	OBJ_ID sence_nml_tex_; //场景法线
//	OBJ_ID sence_clr_tex_; //场景纹理
//	OBJ_ID sence_light_tex_; //场景光照
//	light_cam::ptr l_cam_;
//
//	l3_int32 uni_shader_stage_loc_; // uniform int 
//	l3_int32 uni_dep_range_loc_; // vec2
//	//l3_int32 uni_view_mtx_loc_; // mat4
//	l3_int32 uni_view_inv_mtx_loc_; //观察逆矩阵 uniform mat4 
//	//l3_int32 uni_proj_mtx_loc_; //投影矩阵 uniform mat4
//	l3_int32 uni_aspect_loc_; // 屏幕宽高比 uniform float 
//	l3_int32 uni_tan_half_fov_loc_; // 视角一半的tan值 uniform float 
//	l3_int32 uni_world_mtx_loc_; // mat4
//
//	l3_int32 uni_tex_light_depth_tex_loc_; //光源空间深度 uniform sampler2D 
//	l3_int32 uni_tex_light_depth_shadow_loc_; //同一个纹理,深度 uniform sampler2DShadow
//	l3_int32 uni_tex_view_depth_loc_; //摄影机空间深度纹理 uniform sampler2D
//	l3_int32 uni_tex_nml_loc_; //摄影机空间场景法线 uniform sampler2D
//	l3_int32 uni_tex_clr_loc_; // uniform sampler2D
//	l3_int32 uni_tex_light_loc_; //场景光照 unifrom sampler2D 
//
//	l3_int32 uni_light_dep_range_loc_; //光源的depth range uniform float
//	l3_int32 uni_light_view_mtx_loc_; //光源观察 uniform mat4
//	l3_int32 uni_light_proj_mtx_loc_; //光源投影 uniform mat4
//	l3_int32 uni_light_dep_pix_offset_loc_; //光源深度纹理分辨率 uniform vec2
//
//
//	l3_int32 attr_screen_quad_pos_loc_; //attribute vec2
//	l3_int32 attr_screen_quad_texcoord_loc_; //attribute vec2
//
//
//	/* @brief screen quad */
//	device_buf_base::ptr pos_buf_;
//	device_buf_base::ptr texcoord_buf_;
//
//	/* @brief screen quad */
//	robj_geometry * geo_screen_quad_;
//};
//
//}
//
//#endif
//
//
//
//
//
//
//
//
//
//
