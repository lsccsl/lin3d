///**
//* @file shader_defer_render.h
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
//#ifndef __L3ENG_SHADER_DEFER_RENDER_H__
//#define __L3ENG_SHADER_DEFER_RENDER_H__
//
//#include "base_obj.h"
//#include "shader.h"
//#include "shader_program_inter.h"
//#include "light.h"
//
///*
//Depth R32F
//
//Normal + scattering A2R10G10B10
//
//Diffuse color + emissive A8R8G8B8
//*/
//
//enum EN_DR_STAGE{
//	EN_DR_STAGE_DEFER_RENDER_GBUFFER = 0,
//	EN_DR_STAGE_DEFER_RENDER_LIGHTING = 1,
//};
//
//namespace l3eng{
//
//class l3_engine;
//class robj_geometry;
//
//class shader_defer_render : public shader
//{ 
//public:
//
//	typedef ref_ptr_thread<shader_defer_render> ptr;
//	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_DEFER_RENDER)
//
//public:
//
//	shader_defer_render(l3_engine * eng):shader(eng),
//		uni_dep_range_loc_(-1),
//		stage_(EN_DR_STAGE_DEFER_RENDER_GBUFFER)
//	{}
//	virtual ~shader_defer_render(){}
//
//	virtual void init() override;
//
//	virtual void pre_frame(sence_mgr * sence) override;
//	virtual void post_frame(sence_mgr * sence) override;
//	virtual void prepare(robj_base& robj) override;
//
//	virtual const std::string& shader_name() override
//	{
//		static std::string n(shader_program_inter::_l3eng_inter_defer_render);
//		return n;
//	}
//
//public:
//
//	void set_stage_gbuffer()
//	{ this->stage_ = EN_DR_STAGE_DEFER_RENDER_GBUFFER; }
//	void set_stage_light()
//	{ this->stage_ = EN_DR_STAGE_DEFER_RENDER_LIGHTING; }
//
//	inline OBJ_ID rtt_gbuffer(){ return this->rtt_gbuffer_; }
//	inline OBJ_ID rtt_light(){ return this->rtt_light_; }
//
//	void set_light(const light& l, sence_mgr * sence);
//	void render_screen_quad();
//
//private:
//
//	void _set_shader_loc();
//
//	void _pre_frame_gbuffer(sence_mgr * sence);
//	void _post_frame_gbuffer(sence_mgr * sence);
//
//	void _pre_frame_light(sence_mgr * sence);
//	void _post_frame_light(sence_mgr * sence);
//
//	void _prepare_gbuffer(robj_base& robj);
//
//private:
//
//	/* @brief uniform location */
//	l3_int32 uni_dep_range_loc_;
//	l3_int32 uni_world_mtx_loc_;
//	l3_int32 uni_view_mtx_loc_;
//	l3_int32 uni_shader_stage_loc_;
//	l3_int32 uni_tex1_loc_; //����1 sampler2D
//	l3_int32 uni_tex2_loc_; //����2 sampler2D
//
//	l3_int32 uni_light_type_loc_; //������� int
//	l3_int32 uni_light_view_pos_loc_; // �ƹ�(λ��/����, w=0��ʾ���� w=1��ʾλ��) uniform vec4
//	l3_int32 uni_light_clr_loc_; // uniform vec3
//	l3_int32 uni_light_attenuation_loc_; //������ڴ�ֵ��ʼ˥�� uniform float
//	l3_int32 uni_light_max_len_loc_; //������ڴ�ֵ����˥��Ϊ0 uniform float
//	l3_int32 uni_light_view_spot_dir_loc_;//Spotlight���� uniform vec3
//	l3_int32 uni_light_cutoff_cos_loc_; //�۹��,��׶�������Ե�нǵ�cosֵ uniform float
//	l3_int32 uni_light_exponent_loc_; //�۹�����������Ե˥�� uniform float
//
//
//	l3_int32 uni_tex_nml_loc_; // uniform sampler2D
//	l3_int32 uni_tex_clr_loc_; // uniform sampler2D
//	l3_int32 uni_tex_view_depth_loc_; // ��Ӱ���ռ�������� uniform sampler2D
//	l3_int32 uni_aspect_loc_; // ��Ļ��߱� uniform float 
//	l3_int32 uni_tan_half_fov_loc_; // �ӽ�һ���tanֵ uniform float 
//
//	l3_int32 attr_screen_quad_pos_loc_; //attribute vec2
//	l3_int32 attr_screen_quad_texcoord_loc_; //attribute vec2
//
//
//	/* @brief ���� */
//	EN_DR_STAGE stage_;
//
//	/* @brief gbuffer */
//	OBJ_ID rtt_gbuffer_;
//	OBJ_ID rtt_light_;
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
