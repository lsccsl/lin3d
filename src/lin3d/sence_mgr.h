/**
* @file sence_mgr.h
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
#ifndef __L3ENG__SENCE_MGR_H__
#define __L3ENG__SENCE_MGR_H__

#include <map>
#include <set>
#include "l3_type.h"
#include "camera.h"
#include "robj_base.h"
#include "shader_defer_render.h"
#include "shader_defer_render_gbuffer.h"
#include "shader_defer_render_light.h"
#include "shader_defer_render_light_mix.h"
#include "shader_defer_render_light_vol_mix.h"
#include "shader_defer_render_atmospheric.h"
#include "shader_defer_render_final.h"
#include "light_mgr.h"
#include "shader_shadowmap.h"
#include "shader_shadowmap_cast.h"
#include "shader_shadowmap_recv.h"
#include "shader_ssao.h"
#include "shader_vol.h"
#include "render_target_base.h"
#include "shader_easy_hdr.h"
#include "shader_show_tex.h"
#include "shader_hdr.h"
#include "shader_ssr.h"
#include "sence_render_defer.h"
#include "sence_render_forward.h"
#include "sence_render_test.h"
#include "l3_compile_config.h"

namespace l3eng{

class l3_engine;
class triangle;


class sence_mgr
{
public:

	typedef std::map<OBJ_ID, robj_base::ptr> MAP_ROBJ;
	friend class sence_render_defer;
	friend class sence_render_forward;
	friend class sence_render_test;

public:

	sence_mgr(l3_engine * eng);
	~sence_mgr();

	void init();

	void pre_render();
	void post_render();

	void render(const L3_RENDER_GROUP render_group = L3_RENDER_GROUP_ALL);

	void robj_add(robj_base::ptr robj);
	void robj_del(OBJ_ID oid);
	void robj_set_render_group(OBJ_ID oid, const L3_RENDER_GROUP render_group);

public:

	/* @brief 射线拾取 */
	l3_bool ray_pickup(const ray& r, robj_base::ptr& obj, triangle& tr_hit, l3_f32& t)const;

public:

	inline l3_engine * eng(){ return this->eng_; }

	/* @brief 场景的主摄影机,供l3_engine提供应用程序设置属性(内部不使用此数据) */
	inline camera * cam_main(){ return this->cam_main_.get(); }

	/* @brief 当前摄影机,所有的shader取这个进行绘制
	* (正常绘制时是main cam,绘制水体反射与折射是临时生成的一个摄影机)
	*/
	inline camera * cam_cur(){ return this->cam_cur_.get(); }
	inline void reset_cam_cur()
	{
		this->cam_cur_ = this->cam_main_;
	}
	inline void set_cam_cur(camera::ptr cam)
	{
		this->cam_cur_ = cam;
	}

	/* @brief 灯光管理 */
	inline light_mgr * get_light_mgr(){ return this->light_mgr_; }

	sence_render_forward * sence_frender()
	{
		return this->sence_frender_.get();
	}

	sence_render_defer * sence_drender()
	{
		return this->sence_render_.get();
	}

public:

	inline void enable_defer_render(l3_bool e)
	{
		this->enable_defer_render_ = e;
	}

	inline void enable_hdr(l3_bool e)
	{
		if(!this->sence_render_.is_null())
			this->sence_render_->enable_hdr(e);
		else
		{
#ifdef HAS_TEST_MODE
			if(!this->sence_test_render_.is_null())
				this->sence_test_render_->enable_hdr(e);
#endif
		}
	}

	inline void enable_atmospheric(l3_bool e)
	{
		if(!this->sence_render_.is_null())
			this->sence_render_->enable_atmospheric(e);
		else
		{
#ifdef HAS_TEST_MODE
			if(!this->sence_test_render_.is_null())
				this->sence_test_render_->enable_atmospheric(e);
#endif
		}
	}

	inline void enable_ssao(l3_bool e)
	{
		if(!this->sence_render_.is_null())
			this->sence_render_->enable_ssao(e);
		else
		{
#ifdef HAS_TEST_MODE
			if(!this->sence_test_render_.is_null())
				this->sence_test_render_->enable_ssao(e);
#endif
		}
	}

	inline void enable_post_vol_rb(l3_bool e)
	{
		if (!this->sence_render_.is_null())
			this->sence_render_->enable_post_vol_rb(e);
		else
		{
#ifdef HAS_TEST_MODE
			//if (!this->sence_test_render_.is_null())
			//	this->sence_test_render_->enable_post_vol_rb(e);
#endif
		}
	}

	void render_show_tex(OBJ_ID rtt_tex,
		const l3_f32 x_start = 0.0f, const l3_f32 y_start = 0.0f,
		const l3_f32 x_len = 2.0f, const l3_f32 y_len = 2.0f,
		const l3_f32 dep = 0.0f);

	inline void render_show_rtt_tex(OBJ_ID rtt, const l3_uint32 tex_idx,
		const l3_f32 x_start, const l3_f32 y_start,
		const l3_f32 x_len, const l3_f32 y_len,
		const l3_f32 dep = 0.0f)
	{
		_render_show_rtt_tex(rtt, tex_idx,
			x_start, y_start,
			x_len, y_len,
			dep);
	}

	void test_render_mode(l3_bool t)
	{
		this->test_render_mode_ = t;
	}

public:

	inline void rtt_sence(OBJ_ID rtt)
	{
		this->rtt_sence_ = rtt;
	}
	inline OBJ_ID rtt_sence()
	{
		return this->rtt_sence_;
	}

protected:

	const MAP_ROBJ& map_obj(){ return this->map_obj_; }

	const MAP_ROBJ& render_group_obj(const L3_RENDER_GROUP group)const;

private:

	void _render_show_rtt_tex(OBJ_ID rtt, const l3_uint32 tex_idx,
		const l3_f32 x_start, const l3_f32 y_start,
		const l3_f32 x_len, const l3_f32 y_len,
		const l3_f32 dep = 0.0f);


private:

	/* @brief  */
	l3_engine * eng_;

	/* @brief 所有的渲染对象 */
	MAP_ROBJ map_obj_;

	/* @brief 对象分组 */
	struct render_group_t
	{
		MAP_ROBJ map_group_obj_;
	};
	typedef std::map<L3_RENDER_GROUP, render_group_t> MAP_RENDER_GROUP;
	MAP_RENDER_GROUP map_render_group_;

	/* @brief 灯光管理 */
	light_mgr * light_mgr_;

	/* @brief 默认摄影机 */
	camera::ptr cam_main_;

	/* @brief 当前使用的摄像机 */
	camera::ptr cam_cur_;

	/* @brief 延迟渲染 */
	l3_bool enable_defer_render_;
	sence_render_defer::ptr sence_render_;

	/* @brief for test only */
	sence_render_forward::ptr sence_frender_;

	/* @brief for test only */
	l3_bool test_render_mode_;
#ifdef HAS_TEST_MODE
	sence_render_test::ptr sence_test_render_;
#endif

	/* @brief show tex shader */
	shader_show_tex::ptr shdr_show_;

	std::set<OBJ_ID> set_rtt_;
	OBJ_ID rtt_sence_;
};

}

#endif
