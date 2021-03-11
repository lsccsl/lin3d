/**
* @file shader_easy_hdr.h
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
#ifndef __L3ENG_SHADER_EASY_HDR_H__
#define __L3ENG_SHADER_EASY_HDR_H__

#include "shader.h"
#include "shader_screen_quad.h"
#include "shader_program_inter.h"

namespace l3eng{

class shader_easy_hdr : public shader
{
public:

	typedef ref_ptr_thread<shader_easy_hdr> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_EASY_HDR)

public:

	shader_easy_hdr(l3_engine * eng):shader(eng){}
	virtual ~shader_easy_hdr(){}

	virtual void init()override;

	virtual void pre_frame(sence_mgr * sence)override;
	virtual void post_frame(sence_mgr * sence)override;
	//virtual void prepare(robj_base& robj)override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_easy_hdr);
		return n;
	}

public:

	void render_screen_quad();

	void tex_final(OBJ_ID tex)
	{ this->tex_final_ = tex; }

	OBJ_ID rtt_out_put()
	{ return this->rtt_out_put_; }

private:

	/* @brief 输入的hdr图像 */
	OBJ_ID tex_final_;

	/* @brief 输出的ldr图像 */
	OBJ_ID rtt_out_put_;

	l3_int32 uni_tex_final_loc_; //uniform sampler2D 

	/* @brief screen quad */
	shader_screen_quad screen_quad_;
};

}

#endif
