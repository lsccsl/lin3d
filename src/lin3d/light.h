/**
* @file light.h
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
#ifndef __L3ENG_LIGHT_H__
#define __L3ENG_LIGHT_H__

#include "l3_vector3.h"
#include "l3_color.h"
#include "moveable.h"
#include "base_obj.h"

namespace l3eng{

class light : public base_obj
{
public:

	typedef ref_ptr_thread<light> ptr;
	OBJ_TYPE_DEFINE(base_obj, OBJ_LIGHT)

	enum enum_light_type
	{
		E_LIGHT_DIR = 0, //平行光
		E_LIGHT_SPOT = 1, //聚光灯
		E_LIGHT_POINT = 2, //点光源
		E_LIGHT_DIR_OUTDOOR = 3
	};

public:

	light():
		//pos_(0.0f, 0.0f, 1.0f),
		dir_(0.0f, 0.0f, -1.0f),
		enable_(L3_FALSE),
		enable_vol_(L3_FALSE),
		enable_shadow_(L3_TRUE),
		light_type_(E_LIGHT_SPOT),
		attenuation_const_(1.0f),
		attenuation_linear_(0.01f),
		attenuation_quadratic_(0.01f),
		exponent_(2.0f),
		cutoff_(45.0f),
		cutoff_inner_(30.0f),
		cutoff_outer_(45.0f),
		attenuation_begin_(10.0f),
		light_max_len_(100.0f),
		enable_light_debug_(L3_FALSE)
	{
		set_pos(0.0f, 0.0f, 1.0f);
	}
	~light(){}

	//inline const vector3& pos()const{ return this->pos_; }
	inline const vector3& pos()const{ return this->mov_.v_pos(); }
	inline const vector3& dir()const{ return this->dir_; }

	inline color& diffuse(){ return this->diffuse_; }
	inline const color& diffuse()const{ return this->diffuse_; }
	inline color& specular(){ return this->specular_; }
	inline const color& specular()const{ return this->specular_; }
	inline color& ambient(){ return this->ambient_; }
	inline const color& ambient()const{ return this->ambient_; }

	inline l3_f32 exponent()const{ return this->exponent_; }
	inline void exponent(l3_f32 e){ this->exponent_ = e; }

	inline l3_f32 cutoff()const{ return this->cutoff_; }
	inline void cutoff(l3_f32 c)
	{
		this->cutoff_ = c;
		if(this->cutoff_ > 90.0f)
			this->cutoff_ = 90.0f;
	}


	inline l3_f32 cutoff_inner(){ return this->cutoff_inner_; }
	inline void cutoff_inner(l3_f32 ci)
	{
		this->cutoff_inner_ = ci;
		if(this->cutoff_inner_ > 90.0f)
			this->cutoff_inner_ = 90.0f;

		if(this->cutoff_ < this->cutoff_inner_)
			this->cutoff_ = this->cutoff_inner_;
	}

	inline l3_f32 cutoff_outer(){ return this->cutoff_outer_; }
	inline void cutoff_outer(l3_f32 ci)
	{
		this->cutoff_outer_ = ci;
		if(this->cutoff_outer_ > 90.0f)
			this->cutoff_outer_ = 90.0f;

		if(this->cutoff_ < this->cutoff_outer_)
			this->cutoff_ = this->cutoff_outer_;
	}


	inline l3_bool enable()const{ return this->enable_; }
	inline void enable(l3_bool b){ this->enable_ = b; }

	inline l3_bool enable_vol()const{ return this->enable_vol_; }
	inline void enable_vol(l3_bool b){ this->enable_vol_ = b; }

	inline l3_bool enable_shadow()const{ return this->enable_shadow_; }
	inline void enable_shadow(l3_bool b){ this->enable_shadow_ = b; }


	inline const enum_light_type light_type()const{ return this->light_type_; }
	inline void light_type(enum_light_type t){ this->light_type_ = t; }

	void set_pos(const l3_f32 x, const l3_f32 y, const l3_f32 z);

	void set_rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree);
	void get_rotate(l3_f32& x_degree, l3_f32& y_degree, l3_f32& z_degree);

	const matrix4& get_mtx_view()const{ return this->mov_.mtx_view(); }

	inline const l3_f32 attenuation_const()const
	{ return this->attenuation_const_; }
	inline const l3_f32 attenuation_linear()const
	{ return this->attenuation_linear_; }
	inline const l3_f32 attenuation_quadratic()const
	{ return this->attenuation_quadratic_; }

	inline const l3_f32 attenuation_begin()const
	{ return this->attenuation_begin_; }

	inline void light_max_len(l3_f32 max_len)
	{ this->light_max_len_ = max_len; }
	inline const l3_f32 light_max_len()const
	{ return this->light_max_len_; }

public:

	void enable_light_debug(l3_bool e)
	{
		this->enable_light_debug_ = e;
	}

	l3_bool enable_light_debug()
	{
		return this->enable_light_debug_;
	}

private:

	l3_bool enable_light_debug_;

	///* @brief 位置,点光源/聚光灯 */
	//vector3 pos_;
	/* @brief 方向,平行光/聚光灯 */
	vector3 dir_;

	moveable_obj mov_;

	/* @brief 漫射,反射,环境光系数 */
	color diffuse_;
	color specular_;
	color ambient_;

	/* @brief 光源衰减 */
	l3_f32 attenuation_const_;
	l3_f32 attenuation_linear_;
	l3_f32 attenuation_quadratic_;

	/* @brief 距离大于此值开始衰减 */
	l3_f32 attenuation_begin_;
	/* @brief 距离大于此值光线衰减为0 */
	l3_f32 light_max_len_;


	/* @brief sportlight 光椎发散衰减指数  0-128 0:表示均匀是发散光线 */
	l3_f32 exponent_;

	/* @brief spotlight 光椎轴线与边的夹角(角度,不超过90) */
	l3_f32 cutoff_;
	l3_f32 cutoff_inner_;
	l3_f32 cutoff_outer_;

	l3_bool enable_; // 是否启用
	l3_bool enable_vol_; // 是否启用体积光
	l3_bool enable_shadow_; // 是否产生阴影

	enum_light_type light_type_;
};

}

#endif
