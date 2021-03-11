/**
* @file shader_show_tex.cpp
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
#include "shader_show_tex.h"
#include "win_device.h"

namespace l3eng{

shader_show_tex::shader_show_tex(l3_engine * eng):shader(eng)
{
}

void shader_show_tex::init()
{
	this->uni_tex_loc_ = this->shdr_prg_->get_uniform_tex_loc("uni_tex");
	this->uni_dep_loc_ = this->shdr_prg_->get_uniform_loc("uni_dep");
}

void shader_show_tex::pre_frame(sence_mgr * sence)
{
	this->shdr_prg_->active();
}

void shader_show_tex::post_frame(sence_mgr * sence)
{
	this->shdr_prg_->inactive();
}

void shader_show_tex::render_debug_rtt(OBJ_ID tex,
	const l3_f32 x_start, const l3_f32 y_start,
	const l3_f32 x_len, const l3_f32 y_len,
	const l3_f32 dep)
{
	this->shdr_prg_->uni_bind_tex(this->uni_tex_loc_, tex);
	this->shdr_prg_->uni_bind_float(this->uni_dep_loc_, dep);

	this->screen_quad_.set_screen_rect(this->shdr_prg_, this->eng_,
		x_start, y_start,
		x_len, y_len);

	//win_device * dev = this->eng_->dev();
	//assert(dev);

	//dev->push_cull_info();
	//dev->enable_cull_back();

	this->screen_quad_.render_screen_quad(this->shdr_prg_);

	//dev->pop_cull_info();
}

}
