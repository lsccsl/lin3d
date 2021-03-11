/**
* @file shader_screen_quad.cpp
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
#include "shader_screen_quad.h"
#include "l3_engine.h"
#include "robj_geometry.h"
#include "win_device.h"

namespace l3eng{

shader_screen_quad::~shader_screen_quad()
{
	if(this->geo_screen_quad_)
		delete this->geo_screen_quad_;
}

void shader_screen_quad::init_screen_full(shader_program::ptr& shdr_prg, l3_engine * eng)
{
	this->eng_ = eng;

	this->attr_screen_quad_pos_loc_      = shdr_prg->get_attrib_loc("attr_screen_quad_pos"); //attribute vec2
	this->attr_screen_quad_texcoord_loc_ = shdr_prg->get_attrib_loc("attr_screen_quad_texcoord"); //attribute vec2

	this->geo_screen_quad_ = new robj_geometry(eng->sence());

	this->geo_screen_quad_->create_square(1, 1, L3_TRUE);

	{
		this->pos_buf_ = device_buf_base::ptr(new device_common_buf);

		this->pos_buf_->resize_buf_sz(sizeof(l3_f32) * 2 * 4);

		l3_f32 * buf = (l3_f32 *)(this->pos_buf_->get_buf());

		buf[0] = -1.0f;
		buf[1] = 1.0f;

		buf[2] = -1.0f;
		buf[3] = -1.0f;

		buf[4] = 1.0f;
		buf[5] = -1.0f;

		buf[6] = 1.0f;
		buf[7] = 1.0f;
	}

	{
		this->texcoord_buf_ = device_buf_base::ptr(new device_common_buf);

		this->texcoord_buf_->resize_buf_sz(sizeof(l3_f32) * 2 * 4);

		l3_f32 * buf = (l3_f32 *)(this->texcoord_buf_->get_buf());

		buf[0] = 0.0f;
		buf[1] = 1.0f;

		buf[2] = 0.0f;
		buf[3] = 0.0f;

		buf[4] = 1.0f;
		buf[5] = 0.0f;

		buf[6] = 1.0f;
		buf[7] = 1.0f;
	}

	shdr_prg->attr_bind(this->attr_screen_quad_pos_loc_, this->pos_buf_);
	shdr_prg->attr_bind(this->attr_screen_quad_texcoord_loc_, this->texcoord_buf_);
}

void shader_screen_quad::set_screen_rect(shader_program::ptr& shdr_prg, l3_engine * eng,
	l3_f32 x_start, l3_f32 y_start,
	l3_f32 x_len, l3_f32 y_len)
{
	this->eng_ = eng;

	this->attr_screen_quad_pos_loc_      = shdr_prg->get_attrib_loc("attr_screen_quad_pos"); //attribute vec2
	this->attr_screen_quad_texcoord_loc_ = shdr_prg->get_attrib_loc("attr_screen_quad_texcoord"); //attribute vec2

	if(!this->geo_screen_quad_)
	{
		this->geo_screen_quad_ = new robj_geometry(eng->sence());
		this->geo_screen_quad_->create_square(1, 1, L3_TRUE);
	}

	x_start =  x_start + (-1.0f);
	y_start = -y_start + ( 1.0f);

	{
		if(this->pos_buf_.is_null())
			this->pos_buf_ = device_buf_base::ptr(new device_common_buf);

		this->pos_buf_->resize_buf_sz(sizeof(l3_f32) * 2 * 4);

		l3_f32 * buf = (l3_f32 *)(this->pos_buf_->get_buf());

		buf[0] = x_start;
		buf[1] = y_start;

		buf[2] = x_start;
		buf[3] = y_start - y_len;

		buf[4] = x_start + x_len;
		buf[5] = y_start - y_len;

		buf[6] = x_start + x_len;
		buf[7] = y_start;
	}

	{
		if(this->texcoord_buf_.is_null())
			this->texcoord_buf_ = device_buf_base::ptr(new device_common_buf);

		this->texcoord_buf_->resize_buf_sz(sizeof(l3_f32) * 2 * 4);

		l3_f32 * buf = (l3_f32 *)(this->texcoord_buf_->get_buf());

		buf[0] = 0.0f;
		buf[1] = 1.0f;

		buf[2] = 0.0f;
		buf[3] = 0.0f;

		buf[4] = 1.0f;
		buf[5] = 0.0f;

		buf[6] = 1.0f;
		buf[7] = 1.0f;
	}

	shdr_prg->attr_bind(this->attr_screen_quad_pos_loc_, this->pos_buf_);
	shdr_prg->attr_bind(this->attr_screen_quad_texcoord_loc_, this->texcoord_buf_);
}

void shader_screen_quad::render_screen_quad(shader_program::ptr& shdr_prg)
{
	if(!this->eng_)
		return;

	shdr_prg->attr_bind(this->attr_screen_quad_pos_loc_, this->pos_buf_);
	shdr_prg->attr_bind(this->attr_screen_quad_texcoord_loc_, this->texcoord_buf_);

	win_device * dev = this->eng_->dev();
	assert(dev);

	dev->push_cull_info();
	dev->enable_cull_back();

	this->geo_screen_quad_->render();

	dev->pop_cull_info();
}

}
