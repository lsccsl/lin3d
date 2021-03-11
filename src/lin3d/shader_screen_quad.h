/**
* @file shader_screen_quad.h
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
#ifndef __L3ENG_SHADER_SCREEN_QUAD_H__
#define __L3ENG_SHADER_SCREEN_QUAD_H__

#include "l3_type.h"
#include "shader_program.h"

namespace l3eng{

class l3_engine;
class robj_geometry;

class shader_screen_quad
{
public:

	typedef ref_ptr_thread<shader_screen_quad> ptr;

	shader_screen_quad():geo_screen_quad_(NULL),eng_(NULL){}
	~shader_screen_quad();

	void init_screen_full(shader_program::ptr& shdr_prg, l3_engine * eng);

	void set_screen_rect(shader_program::ptr& shdr_prg, l3_engine * eng,
		l3_f32 x_start = 0.0f, l3_f32 y_start = 0.0f,
		l3_f32 x_len = 2.0f, l3_f32 y_len = 2.0f);

	void render_screen_quad(shader_program::ptr& shdr_prg);

private:

	l3_engine * eng_;

	l3_int32 attr_screen_quad_pos_loc_; //attribute vec2
	l3_int32 attr_screen_quad_texcoord_loc_; //attribute vec2
	/* @brief screen quad */
	device_buf_base::ptr pos_buf_;
	device_buf_base::ptr texcoord_buf_;
	/* @brief screen quad */
	robj_geometry * geo_screen_quad_;
};

}

#endif
