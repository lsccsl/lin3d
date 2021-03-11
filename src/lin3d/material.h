/**
* @file material.h
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
#ifndef __l3eng_MATERIAL_H__
#define __l3eng_MATERIAL_H__

#include <string>
#include "l3_color.h"
#include "blend.h"
#include "base_obj.h"

namespace l3eng{

class material
{
public:

	enum e_poly_mode{
		E_POLY_POIN = 0,
		E_POLY_LINE = 1,
		E_POLY_FILL = 2,
	};

	enum e_tex_filter
	{
		E_TEX_FILTER_LINEAR,
		E_TEX_FILTER_NEAREST,
	};

	const static l3_int32 __MAX_TEX_LAY__ = 8;

public:

	inline material():poly_mode_(E_POLY_FILL),
		ambient_(1.0f, 1.0f, 1.0f),
		diffuse_(1.0f, 1.0f, 1.0f),
		specular_(1.0f, 1.0f, 1.0f),
		emission_(0.0f, 0.0f, 0.0f),
		shininess_(1.0f),
		blend_mode_(L3_BLEND_MODE_DISABLE),
		force_light_off_(L3_FALSE),
		enable_depth_test_(L3_TRUE),
		tex_filter_(E_TEX_FILTER_LINEAR),
		depth_write_(L3_TRUE),
		enable_reflect_(L3_FALSE),
		is_hollow_(L3_FALSE)
	{
		for(l3_int32 i = 0; i < sizeof(this->tex_)/sizeof(this->tex_[0]); i ++)
		{
			this->tex_[i].tex_obj_id_ = base_obj::INVALID_OBJ_ID;
		}
	}
	inline ~material(){}

	inline const OBJ_ID get_tex_obj_id(l3_uint32 lay = 0){ return this->tex_[lay].tex_obj_id_; }
	inline void set_tex_obj_id(const OBJ_ID tex_obj, l3_uint32 lay = 0){ this->tex_[lay].tex_obj_id_ = tex_obj; }

	inline const e_poly_mode poly_mode()const{ return this->poly_mode_; }
	inline void poly_mode(const e_poly_mode poly){ this->poly_mode_ = poly; }

	inline color& ambient(){ return this->ambient_; }
	inline color& diffuse(){ return this->diffuse_; }
	inline color& specular(){ return this->specular_; }
	inline color& emission(){ return this->emission_; }

	inline l3_f32 shininess()const{ return this->shininess_; }
	inline void shininess(l3_f32 s){ this->shininess_ = s; }

	inline l3_blend_mode_t blend_mode()const{ return this->blend_mode_; }
	inline void blend_mode(l3_blend_mode_t bm){ this->blend_mode_ = bm; }

	inline l3_bool force_light_off()const{ return this->force_light_off_; }
	inline void force_light_off(l3_bool b){ this->force_light_off_ = b; }

	inline l3_bool enable_depth_test()const{ return this->enable_depth_test_; }
	inline void enable_depth_test(l3_bool b){ this->enable_depth_test_ = b; }

	inline e_tex_filter tex_filter()const{ return this->tex_filter_; }
	inline void tex_filter(e_tex_filter f){ this->tex_filter_ = f; }

	inline l3_bool depth_write(){ return this->depth_write_; }
	inline void depth_write(l3_bool b){ this->depth_write_ = b; }

	inline l3_bool enable_reflect(){ return this->enable_reflect_; }
	inline void enable_reflect(l3_bool b){ this->enable_reflect_ = b; }

	inline const l3_bool is_tranparent()const
	{
		switch(this->blend_mode_)
		{
		case L3_BLEND_MODE_NONE:
		case L3_BLEND_MODE_DISABLE:
			return L3_FALSE;
		}
		return L3_TRUE;
	}

	inline const l3_bool is_hollow()const
	{
		return is_hollow_;
	}

	inline void set_hollow(l3_bool h)
	{
		is_hollow_ = h;
	}

private:

	struct tex_info_t
	{
		tex_info_t():tex_obj_id_(base_obj::INVALID_OBJ_ID){}

		OBJ_ID tex_obj_id_;
	};
	tex_info_t tex_[__MAX_TEX_LAY__];

	e_poly_mode poly_mode_;

	color ambient_;
	color diffuse_;
	color specular_;
	color emission_;
	/* @brief 0-128, 128表示亮 */
	l3_f32 shininess_;

	l3_blend_mode_t blend_mode_;

	l3_bool force_light_off_;

	l3_bool enable_depth_test_;

	e_tex_filter tex_filter_;

	/* @brief 是否在渲染时写入深度缓存 */
	l3_bool depth_write_;

	l3_bool enable_reflect_;

	l3_bool is_hollow_;

	/* @brief 粗糙度 */
	l3_f32 roughness_;
	/* @brief 金属度 */
	l3_f32 metalic_;
};

}

#endif







