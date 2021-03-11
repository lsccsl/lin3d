/**
* @file shader.h
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
#ifndef __L3ENG_SHADER_H__
#define __L3ENG_SHADER_H__

#include "l3_type.h"
#include "base_obj.h"
#include "shader_program.h"

namespace l3eng{

class l3_engine;
class robj_base;
class sence_mgr;

class shader : public base_obj
{
public:

	typedef ref_ptr_thread<shader> ptr;
	OBJ_TYPE_DEFINE(base_obj, OBJ_SHADER)

public:

	shader(l3_engine * eng):eng_(eng){}
	virtual ~shader(){}

	virtual void init(){}

	virtual void pre_frame(sence_mgr * sence){}
	virtual void post_frame(sence_mgr * sence){}
	virtual void prepare(robj_base& robj){}

	virtual const std::string& shader_name() = 0;

	virtual void set_param_f32(const l3_int32 param_type, const l3_f32 param_val){}
	virtual void set_param_f64(const l3_int32 param_type, const l3_f64 param_val){}
	virtual void set_param_int32(const l3_int32 param_type, const l3_int32 param_val){}
	virtual void set_param_int64(const l3_int32 param_type, const l3_int64 param_val){}
	virtual void set_param_vec3(const l3_int32 param_type, const vector3& param_val){}
	virtual void set_param_obj(const l3_int32 param_type, const OBJ_ID param_val){}
	virtual void set_param_bool(const l3_int32 param_type, const l3_bool b){}
	virtual void set_param(const l3_int32 param_type, const void * param, const l3_uint32 param_sz){}

public:

	inline void set_shader_program(shader_program::ptr shdr_prg)
	{ this->shdr_prg_ = shdr_prg; }

	inline shader_program::ptr& get_shader_program()
	{ return this->shdr_prg_; }

protected:

	l3_engine * eng_;

	/* @brief shader¾ä±ú */
	shader_program::ptr shdr_prg_;
};

}

#endif

