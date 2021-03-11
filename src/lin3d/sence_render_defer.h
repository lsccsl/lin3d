/**
* @file sence_render_defer.h
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
#ifndef __L3ENG__SENCE_RENDER_DEFER_H__
#define __L3ENG__SENCE_RENDER_DEFER_H__

#include <set>
#include "render_target_base.h"
#include "light_mgr.h"
#include "shader_defer_render_gbuffer.h"
#include "shader_defer_render_light.h"
#include "shader_shadowmap_cast.h"
#include "shader_shadowmap_recv.h"
#include "shader_vol.h"
#include "shader_defer_render_light_mix.h"
#include "shader_defer_render_light_vol_mix.h"
#include "shader_defer_render_final.h"
#include "shader_defer_render_atmospheric.h"
#include "shader_hdr.h"
#include "shader_ssao.h"
#include "shader_ssr.h"
#include "camera.h"
#include "shader_reflect_forward.h"

/*
gbuffer 法线/漫反射/深度  深度(透视) width x height --- done

shadow cast 深度 1000x1000 --- done
shadow recv width x height --- done

光照+shadow width x height 
ssao width x height  
atmospheric width x height  


ssr width x height  
volumelight width x height

hdr


light mix / light vol mix  x 2 = 6个
*/


namespace l3eng{

class sence_mgr;

class sence_render_defer
{
public:

	typedef ref_ptr_thread<sence_render_defer> ptr;

	sence_render_defer(sence_mgr * sence);
	~sence_render_defer(){}

	void init();

	void render_sence(const L3_RENDER_GROUP render_group);

	inline void enable_hdr(l3_bool e)
	{
		this->enable_hdr_ = e;
	}

	inline void enable_atmospheric(l3_bool e)
	{
		this->enable_atmospheric_ = e;
	}

	inline void enable_ssao(l3_bool e)
	{
		this->enable_ssao_  = e;
	}

private:

	void _init_shader();
	void _init_rtt();

	//void _render_far();
	//void _render_far_gbuffer();

	void _render_sence();

	void _render_gbuffer();
	void _render_ssao();
	void _render_light();
	void _render_atmospheric();
	void _render_final(texture_base::ptr& tex_out_put);
	void _render_forward(texture_base::ptr& tex_out_put);
	void _render_hdr(texture_base::ptr& tex_input);
	void _render_ssr(texture_base::ptr& tex_reflect_src);

	void _render_light_one            (const light_mgr::light_info::ptr& li);
	void _render_light_one_lighting   (const light_mgr::light_info::ptr& li);
	void _render_light_one_shadow_cast(const light_mgr::light_info::ptr& li);
	void _render_light_one_shadow_recv(const light_mgr::light_info::ptr& li);
	void _render_light_one_vol        (const light_mgr::light_info::ptr& li);
	void _render_light_one_mix        (const light_mgr::light_info::ptr& li);
	void _render_light_one_vol_mix    (const light_mgr::light_info::ptr& li);

	void _render_reflect_tex();
	void _render_refract_tex();
	void _render_reflect_obj(texture_base::ptr& tex_out_put);

	void _release_all_tex();
	void _release_reflect_tex();
	void _release_refract_tex();

	void _debug();
	void _debug_light(l3_int32 idx);

private:

	void _tex_dev_sz_pool_init();
	void _tex_dev_sz_pool_get(texture_base::ptr& tex);
	void _tex_dev_sz_pool_release(texture_base::ptr& tex);

private:

	/* @brief gbuffer shader */
	shader_defer_render_gbuffer::ptr shdr_dr_gbuffer_;

	/* @brief 光照渲染 */
	shader_defer_render_light::ptr shdr_dr_light_;

	/* @brief 纹理阴影shader */
	shader_shadowmap_cast::ptr shdr_shadow_cast_;
	shader_shadowmap_recv::ptr shdr_shadow_recv_;

	/* @brief 体积光处理 */
	shader_vol::ptr shdr_vol_;

	/* @brief ssao shader */
	shader_ssao::ptr shdr_ssao_;

	/* @brief 混合光照阴影以及体积光 */
	shader_defer_render_light_mix::ptr shdr_dr_light_mix_;
	shader_defer_render_light_vol_mix::ptr shdr_dr_light_vol_mix_;

	/* @brief 合并光照阴影等 */
	shader_defer_render_final::ptr shdr_dr_final_;

	/* @brief 大气散射 */
	shader_defer_render_atmospheric::ptr shdr_dr_atmospheric_;

	/* @brief hdr */
	shader_hdr::ptr shdr_hdr_;

	/* @brief ssr shader */
	shader_ssr::ptr shdr_ssr_;

	/* @brief 反射 */
	shader_reflect_forward::ptr shdr_reflect_;


	/* @brief 窗口大小的render target */
	render_target_base::ptr rtt_dev_size_;
	std::set<texture_base::ptr> set_free_tex_dev_sz_;
	std::set<texture_base::ptr> set_tex_dev_sz_;
	std::set<texture_base::ptr> set_dep_tex_dev_sz_;

	/* @brief gbuffer */
	render_target_base::ptr rtt_gbuffer_;
	texture_base::ptr tex_gbuffer_diff_;
	texture_base::ptr tex_gbuffer_normal_;
	texture_base::ptr tex_gbuffer_dep_line_;
	texture_base::ptr dep_tex_gbuffer_proj_;

	/* @brief shadow cast */
	render_target_base::ptr rtt_shadow_cast_;
	texture_base::ptr tex_shadow_cast_;
	texture_base::ptr dep_tex_shadow_cast_;


	/* @brief 光照渲染结果 */
	texture_base::ptr ref_tex_light_;
	/* @brief 光照阴影结果 */
	texture_base::ptr ref_tex_light_shadow_;
	/* @brief 光照阴影混合 */
	texture_base::ptr ref_tex_light_mix_1_;
	texture_base::ptr ref_tex_light_mix_2_;
	texture_base::ptr ref_tex_light_mix_;

	/* @brief 体积光处理结果 */
	texture_base::ptr ref_tex_light_vol_;
	/* @brief 体积光混合 */
	texture_base::ptr ref_tex_light_vol_mix_1_;
	texture_base::ptr ref_tex_light_vol_mix_2_;
	texture_base::ptr ref_tex_light_vol_mix_;

	/* @brief 大气散射 */
	texture_base::ptr ref_tex_atmospheric_;

	/* @brief 合并光照阴影等 final tex */
	texture_base::ptr ref_tex_final_;

	/* @brief ssao */
	texture_base::ptr ref_tex_ssao_;

	/* @brief ssr */
	texture_base::ptr ref_tex_ssr_;

	/* @brief 所有的反射纹理 */
	typedef std::map<OBJ_ID, texture_base::ptr> MAP_TEX_REFLECT;
	MAP_TEX_REFLECT map_tex_reflect_;
	/* @brief 所有折射纺理 */
	typedef std::map<OBJ_ID, texture_base::ptr> MAP_TEX_REFRACT;
	MAP_TEX_REFLECT map_tex_refract_;


	/* @brief 远景天空盒等处理结果纹理 */
	//texture_base::ptr ref_tex_obj_far_;
	///* @brief 近景处理结果纹理 */
	//texture_base::ptr ref_tex_obj_;


	sence_mgr * sence_;
	l3_bool enable_atmospheric_;
	l3_bool enable_hdr_;
	l3_bool enable_ssr_;
	l3_bool enable_ssao_;
};

}

#endif
