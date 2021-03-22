/**
* @file shader_program.h
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
#ifndef __L3ENG_SHADER_PROGRAM_H__
#define __L3ENG_SHADER_PROGRAM_H__

#include <string>
#include <map>
#include <list>
#include "l3_type.h"
#include "l3_ref_ptr.h"
#include "base_obj.h"
#include "device_buf.h"
#include "matrix4.h"
#include "vector2.h"

namespace l3eng{

class l3_engine;

enum enum_shader_source_type{
	/* @brief 顶点着色器 */
	SHADER_SOURCE_TYPE_VERTEX = 0,
	/* @brief 像素(片元)着色器 */
	SHADER_SOURCE_TYPE_FRAGMENT = 1,
	/* @brief 曲面细分控制 */
	SHADER_SOURCE_TYPE_TESS_CONTRL = 2,
	/* @brief 曲面细分计算 */
	SHADER_SOURCE_TYPE_TESS_EVAL = 3,
};

class shader_source
{
public:

	typedef ref_ptr_thread<shader_source> ptr;

public:

	shader_source(enum_shader_source_type stype):stype_(stype),sobj_(-1){}
	~shader_source(){}

	l3_int32 load_from_file(const std::string& file_path);
	l3_int32 load_from_string(const std::string& data, const std::string& vpath);

	const l3_int32 sobj()const{ return this->sobj_; }
	const enum_shader_source_type stype()const{ return this->stype_; }

private:

	/* @brief 着色器代码路径 */
	std::string path_;
	enum_shader_source_type stype_;

	/* @brief opengl句柄 */
	l3_int32 sobj_;
};

class shader_program : public base_obj
{
public:

	typedef ref_ptr_thread<shader_program> ptr;
	OBJ_TYPE_DEFINE(base_obj, OBJ_SHADER_PROGRAM)

public:

	struct glsl_param_base_t
	{
		glsl_param_base_t():
			b_bind_(L3_FALSE),
			loc_(-1),
			gl_type_(-1),
			size_(0)
		{}

		l3_int32 loc_;
		l3_int32 gl_type_;
		l3_int32 size_;
		std::string name_;

		l3_bool b_bind_; //是否已绑定数据
	};

	struct glsl_param_t : public glsl_param_base_t
	{
		device_buf_base::ptr buf_;
	};

	struct glsl_param_tex_t : public glsl_param_base_t
	{
		glsl_param_tex_t():tex_obj_(base_obj::INVALID_OBJ_ID){}		

		OBJ_ID tex_obj_;
	};

	typedef std::map<l3_int32/* glsl loc */, glsl_param_t> MAP_GLSL_PARAM;
	typedef std::map<l3_int32/* glsl loc */, glsl_param_tex_t> MAP_GLSL_TEX;
	typedef std::map<std::string/* param name */, l3_int32/* glsl loc */> MAP_GLSL_NAME_LOC;

public:

	shader_program(l3_engine * eng, const std::string& name):eng_(eng), name_(name){}
	virtual ~shader_program(){}

	l3_int32 init();

	l3_int32 attach(shader_source::ptr sobj);

	l3_int32 link();

	l3_int32 active()const;
	l3_int32 inactive();

	/* @brief 所有其它类型uniform变量 */
	l3_int32 get_uniform_info(const std::string& name, glsl_param_t& info)const;
	l3_int32 get_uniform_loc(const std::string& name)const;

	/* @brief 纹理uniform专用 */
	l3_int32 get_uniform_tex(const std::string& name, glsl_param_tex_t& info)const;
	l3_int32 get_uniform_tex_loc(const std::string& name)const;

	l3_int32 get_attrib_info(const std::string& name, glsl_param_t& info)const;
	l3_int32 get_attrib_loc(const std::string& name)const;

	void get_all_uniform_val();

	inline const MAP_GLSL_PARAM& get_all_user_uni_info()
	{ return this->map_uniform_info_; }
	inline const MAP_GLSL_TEX& get_all_user_uni_tex()
	{ return this->map_uniform_tex_; }
	inline const MAP_GLSL_PARAM& get_all_user_attr_info()
	{ return this->map_attrib_info_; }

public:

	/* @brief 解绑uniform */
	//l3_int32 uni_unbind(const l3_int32 loc);
	l3_int32 uni_unbind_all();
	//l3_int32 uni_unbind_tex(const l3_int32 loc);
	l3_int32 uni_unbind_tex_all();


	/* @brief 绑定纹理uniform */
	l3_int32 uni_bind_tex(const l3_int32 loc,
		const OBJ_ID tex_obj);

	/* @brief 绑定二维向量 */
	l3_int32 uni_bind_float_ve2(const l3_int32 loc,
		const l3_f32 f1,
		const l3_f32 f2);
	/* @brief 绑定二维向量数组 */
	l3_int32 uni_bind_float_vec2_array(const l3_int32 loc,
		const std::vector<vector2>& vf);

	/* @brief 绑定三维向量 */
	l3_int32 uni_bind_float_vec3(const l3_int32 loc,
		const l3_f32 f1,
		const l3_f32 f2,
		const l3_f32 f3);

	/* @brief 绑定四维向量 */
	l3_int32 uni_bind_float_vec4(const l3_int32 loc,
		const l3_f32 f1,
		const l3_f32 f2,
		const l3_f32 f3,
		const l3_f32 f4);

	/* @brief 绑定浮点数 */
	l3_int32 uni_bind_float(const l3_int32 loc,
		const l3_f32 f);
	/* @brief 绑定浮点数组 */
	l3_int32 uni_bind_float_array(const l3_int32 loc,
		const l3_f32 * pf, const l3_uint32 pf_sz);

	/* @brief 绑定4x4矩阵 */
	l3_int32 uni_bind_mat4(const l3_int32 loc,
		const matrix4& mtx);
	/* @brief 绑定4x4矩阵数组 */
	l3_int32 uni_bind_mat4_array(const l3_int32 loc,
		const std::vector<matrix4>& v_mtx);

	/* @brief 绑定整型数 */
	l3_int32 uni_bind_int(const l3_int32 loc,
		const l3_int32 i);


	/* @brief uniform绑定至glsl */
	l3_int32 uni_dev_bind();
	l3_int32 uni_dev_bind_tex();

public:

	/* @brief 绑定顶点属性 */
	l3_int32 attr_bind(const l3_int32 loc, device_buf_base::ptr& buf);
	l3_int32 attr_unbind_all();

	/* @brief 顶点属性绑定至glsl */
	l3_int32 attr_dev_bind();
	l3_int32 attr_dev_unbind()const;

private:

	/* @brief */
	l3_engine * eng_;

	/* @brief 顶点着色器 像素(片元)着色器 */
	std::list<shader_source::ptr> lst_sobj_;

	/* @brief 顶点属性的location */
	MAP_GLSL_PARAM map_attrib_info_;
	MAP_GLSL_NAME_LOC map_attrib_name_;
	MAP_GLSL_PARAM map_gl_attrib_info_;
	MAP_GLSL_NAME_LOC map_gl_attrib_name_;

	/* @brief 一次draw call呼叫的全局变量uniform location */
	MAP_GLSL_PARAM map_uniform_info_;
	MAP_GLSL_NAME_LOC map_uniform_name_;
	/* @brief opengl自带的变量 */
	MAP_GLSL_PARAM map_gl_uniform_info_;
	MAP_GLSL_NAME_LOC map_gl_uniform_name_;

	/* @brief uniform tex */
	MAP_GLSL_TEX map_uniform_tex_;
	MAP_GLSL_NAME_LOC map_uniform_tex_name_;

	/* @brief opengl句柄 */
	l3_int32 prog_;

	/* @brief */
	std::string name_;
};


}

#endif












