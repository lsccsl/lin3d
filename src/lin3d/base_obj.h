/**
* @file base_obj.h object base define
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
#ifndef __L3ENG_BASE_OBJ_H__
#define __L3ENG_BASE_OBJ_H__

#include <stdlib.h>
#include <string>
#include "l3_type.h"
#include "l3_ref_ptr.h"
#include "l3_dllexp.h"

namespace l3eng{

enum OBJ_TYPE{
	OBJ_BASE,

	OBJ_CAMERA, //摄影机对像
	OBJ_IMG, //图片对像
	OBJ_MESH, //mesh对像

	OBJ_LIGHT, //灯光对像
	OBJ_LIGHT_CAM,

	OBJ_TEXTURE_BASE, //纹理对像
	OBJ_TEXTURE_WIN32GL,

	OBJ_RENDER, //渲染对像
	OBJ_ROBJ_BASE,
	OBJ_ROBJ_GEOMETRY,
	OBJ_ROBJ_SPHERE,
	OBJ_ROBJ_CONE,
	OBJ_ROBJ_SKELETON,
	OBJ_ROBJ_SKYDOME,
	OBJ_ROBJ_WATER,
	OBJ_ROBJ_PLANE,
	OBJ_ROBJ_VECTOR,

	OBJ_RENDER_TARGET_BASE, //render target 对像
	OBJ_RENDER_TARGET_FBO,

	OBJ_SHADER_PROGRAM, //shader对像
	OBJ_SHADER,
	OBJ_SHADER_TEST,
	OBJ_SHADER_SHOW_TEX,
	OBJ_SHADER_DEFER_RENDER,
	OBJ_SHADER_DEFER_RENDER_GBUFFER,
	OBJ_SHADER_DEFER_RENDER_LIGHT,
	OBJ_SHADER_DEFER_RENDER_LIGHT_MIX,
	OBJ_SHADER_DEFER_RENDER_LIGHT_VOL_MIX,
	OBJ_SHADER_DEFER_ATMOSPHERIC,
	OBJ_SHADER_DEFER_RENDER_FINAL,
	//OBJ_SHADER_SHADOWMAP,
	OBJ_SHADER_SHADOWMAP_CAST,
	OBJ_SHADER_SHADOWMAP_RECV,
	OBJ_SHADER_CASCADED_SHADOWMAP_CAST,
	OBJ_SHADER_CASCADED_SHADOWMAP_RECV,
	OBJ_SHADER_SSAO,
	OBJ_SHADER_VOL,
	OBJ_SHADER_VOL_LIGHTDIR_EXTRUDE,
	OBJ_SHADER_VOL_RADIAL_BLUR,
	OBJ_SHADER_ATMOSPHERIC_GROUND,
	OBJ_SHADER_EASY_HDR,
	OBJ_SHADER_HDR,
	OBJ_SHADER_SSR,
	OBJ_SHADER_REFLECT_FORWARD,
	OBJ_SHADER_PBR_FWD,
	OBJ_SHADER_DISSOLUTION,
};

#define OBJ_TYPE_DEFINE(__parent_class__, __obj_type__) \
	static const OBJ_TYPE _OBJ_TYPE_ = __obj_type__;\
	\
	virtual const OBJ_TYPE obj_type()const{ return __obj_type__; }\
	virtual const l3_bool is_my_type(OBJ_TYPE type)const\
	{\
		if(__obj_type__ == type)\
			return L3_TRUE;\
		\
		return this->__parent_class__::is_my_type(type);\
	}


/**
* base object define
*/
class LIN3D_EX_PORT base_obj
{
public:

	typedef ref_ptr_thread<base_obj> ptr;

	static const OBJ_TYPE _OBJ_TYPE_ = OBJ_BASE;
	static const l3_int64 INVALID_OBJ_ID = -1;

public:

	base_obj():obj_id_(-1),debug_(L3_FALSE){}

	virtual ~base_obj(){}

public:

	virtual void dump(std::string& info){}

	void obj_id(OBJ_ID id){ this->obj_id_ = id; }
	const OBJ_ID obj_id()const{ return this->obj_id_; }

	virtual const OBJ_TYPE obj_type()const{ return OBJ_BASE; }
	virtual const l3_bool is_my_type(OBJ_TYPE type)const{ return this->_is_my_type(type); }

	void obj_name(const std::string& n){ this->obj_name_ = n; }
	const std::string& obj_name(){ return this->obj_name_; }

public:

	void debug(l3_bool d){ this->debug_ = d; }

protected:

	inline const l3_bool _is_my_type(OBJ_TYPE type)const
	{
		if(OBJ_BASE == type)
			return L3_TRUE;

		return L3_FALSE;
	}

protected:
	
	l3_bool debug_;

private:

	OBJ_ID obj_id_;
	std::string obj_name_;
};

}

#endif
