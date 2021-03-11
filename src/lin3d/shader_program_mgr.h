/**
* @file shader_program_mgr.h
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
#ifndef __L3ENG_SHADER_PROGRAM_MGR_H__
#define __L3ENG_SHADER_PROGRAM_MGR_H__

#include <string>
#include <map>
#include <list>
#include "shader_program.h"
#include "shader.h"

namespace l3eng{

class l3_engine;

class shader_program_mgr
{
public:

	shader_program_mgr(l3_engine * eng):
		eng_(eng)
	{}
	~shader_program_mgr(){}

	l3_int32 init(l3_bool test_mode);

	inline OBJ_ID get_shader_program_handle(const std::string& name)const
	{
		std::map<std::string, OBJ_ID>::const_iterator it = this->map_name_shader_program_.find(name);
		if(this->map_name_shader_program_.end() == it)
			return base_obj::INVALID_OBJ_ID;

		return it->second;
	}

	inline l3_int32 get_shader_program(const OBJ_ID shdr_prgm_obj, shader_program::ptr& sp_ptr)const
	{
		std::map<OBJ_ID, shader_program::ptr>::const_iterator it = this->map_shader_program_.find(shdr_prgm_obj);
		if(this->map_shader_program_.end() == it)
			return -1;

		sp_ptr = it->second;
		return 0;
	}

	inline l3_int32 get_shader_program_by_name(const std::string& name, shader_program::ptr& shrd_prg)const
	{
		OBJ_ID id = this->get_shader_program_handle(name);
		if(id == base_obj::INVALID_OBJ_ID)
			return -1;

		return this->get_shader_program(id, shrd_prg);
	}


	inline l3_int32 get_shader_by_name(const std::string& name, shader::ptr& shdr_ptr)
	{
		OBJ_ID shdr_id = this->get_shader_handle(name);
		if(base_obj::INVALID_OBJ_ID == shdr_id)
			return -1;

		return this->get_shader(shdr_id, shdr_ptr);
	}

	inline OBJ_ID get_shader_handle(const std::string& name)const
	{
		std::map<std::string, OBJ_ID>::const_iterator it = this->map_name_shader_.find(name);
		if(this->map_name_shader_.end() == it)
			return base_obj::INVALID_OBJ_ID;

		return it->second;
	}

	inline l3_int32 get_shader(const OBJ_ID shdr_obj, shader::ptr& shdr_ptr)
	{
		std::map<OBJ_ID, shader::ptr>::iterator it = this->map_shader_.find(shdr_obj);
		if(this->map_shader_.end() == it)
			return -1;

		shdr_ptr = it->second;
		return 0;
	}

private:

	l3_int32 _init_inter_program(l3_bool test_mode);

	l3_int32 _init_shader();

	void _get_shader_program_str(const std::string& input, std::string& output);

private:

	/* @brief 着色器名称与句柄映射 */
	std::map<std::string, OBJ_ID> map_name_shader_program_;
	/* @brief 着色器句柄表 */
	std::map<OBJ_ID, shader_program::ptr> map_shader_program_;

	/* @brief 着色器封装对象 */
	std::map<std::string, OBJ_ID> map_name_shader_;
	std::map<OBJ_ID, shader::ptr> map_shader_;

	l3_engine * eng_;
};

}

#endif
