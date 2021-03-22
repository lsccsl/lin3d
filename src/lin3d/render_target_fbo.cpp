/**
* @file win32gl_rtt_fbo.cpp
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
#include "render_target_fbo.h"
#include "l3_engine.h"
#include "l3_log.h"
#include "win_device.h"

namespace l3eng{

l3_int32 render_target_fbo::_enable_mult(l3_bool need_clear)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glViewport(0, 0, this->width_, this->height_);

	glDrawBuffersARB((GLsizei)(this->v_attachment_.size()), &this->v_attachment_[0]);
	//GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
	//glDrawBuffersARB(2, buffers);

	//if(need_clear)
	//{
	//	glClearColor(this->clear_clr_.r(),
	//		this->clear_clr_.g(),
	//		this->clear_clr_.b(),
	//		this->clear_clr_.a());

	//	glClearDepth(1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//}

	return 0;
}

l3_int32 render_target_fbo::_enable_one(l3_bool need_clear)
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glViewport(0, 0, this->width_, this->height_);

	//if(need_clear)
	//{
	//	glClearColor(this->clear_clr_.r(),
	//		this->clear_clr_.g(),
	//		this->clear_clr_.b(),
	//		this->clear_clr_.a());

	//	glClearDepth(1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//}

	return 0;
}

l3_int32 render_target_fbo::enable(l3_bool need_clear)
{
	win_device::print_error(__FILE__, __LINE__);

	l3_int32 ret = -1;
	if(this->v_tex_.size() > 1)
		ret = _enable_mult(need_clear);
	else
		ret = _enable_one(need_clear);

	win_device::print_error(__FILE__, __LINE__);

	return ret;
}

l3_int32 render_target_fbo::disable()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	return 0;
}

l3_int32 render_target_fbo::clear_fbo()
{
	glClearColor(this->clear_clr_.r(),
		this->clear_clr_.g(),
		this->clear_clr_.b(),
		this->clear_clr_.a());

	glClearDepth(this->clear_dep_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT/* | GL_STENCIL_BUFFER_BIT*/);
	return 0;
}

//l3_int32 render_target_fbo::init_with_dep(const l3_int32 width,
//	const l3_int32 height,
//	const l3_int32 clr_attach_count,
//	const render_target_base::e_rtt_color_format format,
//	const color& clear_clr,
//	const render_target_base::e_rtt_depth_format depth_format,
//	const texture_base::e_tex_compare dep_cmp,
//	const l3_f32 clear_dep)
//{
//	//this->width_ = width;
//	//this->height_ = height;
//	this->clear_clr_ = clear_clr;
//	this->clear_dep_ = clear_dep;
//
//	assert(this->eng_);
//	if(clr_attach_count > this->eng_->dev()->fbo_max_attach_count())
//	{
//		L3_LOG_ENG_DEBUG(("dev is null"));
//		return -1;
//	}
//
//	this->v_tex_.resize(clr_attach_count);
//
//	/* 创建颜色 */
//	for(size_t i = 0; i < this->v_tex_.size(); i ++)
//	{
//		texture_base::e_tex_type type;
//		texture_base::e_tex_inter_type inter_type;
//
//		switch(format)
//		{
//		case render_target_base::e_rtt_color_format_rgba_f32:
//			type = texture_base::e_tex_type_f32;
//			inter_type = texture_base::e_tex_type_rgba_f32;
//			break;			
//
//		case render_target_base::e_rtt_color_format_rgba_f16:
//			type = texture_base::e_tex_type_f32;
//			inter_type = texture_base::e_tex_type_rgba_f16;
//			break;
//
//		case render_target_base::e_rtt_color_format_luminance_f16:
//			type = texture_base::e_tex_type_f32;
//			inter_type = texture_base::e_tex_type_luminance_16;
//			break;
//
//		case render_target_base::e_rtt_color_format_luminance_f32:
//			type = texture_base::e_tex_type_f32;
//			inter_type = texture_base::e_tex_type_luminance_32;
//			break;
//
//		case render_target_base::e_rtt_color_format_rgba:
//		default:
//			type = texture_base::e_tex_type_u8;
//			inter_type = texture_base::e_tex_type_rgba_u8;
//			break;
//		}
//
//		this->eng_->tex_mgr()->create_tex(this->v_tex_[i], width, height,
//			type, texture_base::e_tex_format_rgba, NULL,
//			inter_type,
//			texture_base::e_tex_filter_linear, texture_base::e_tex_wrap_clamp_edge);
//	}
//
//	/* 创建深度 */
//	switch(depth_format)
//	{
//	case e_rtt_depth_format_16:
//		this->eng_->tex_mgr()->create_dep_tex(this->depth_tex_, width, height, L3_FALSE, dep_cmp);
//		break;
//
//	case e_rtt_depth_format_32:
//	default:
//		this->eng_->tex_mgr()->create_dep_tex(this->depth_tex_, width, height, L3_TRUE, dep_cmp);
//		break;
//	}
//
//	/* 绑定 */
//	//glGenFramebuffersEXT(1, &this->fbo_);
//	this->_init_dev_fbo(width, height);
//
//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);
//
//	GLenum clr_attach_0 = GL_COLOR_ATTACHMENT0_EXT;
//	for(size_t i = 0; i < this->v_tex_.size(); i ++)
//	{
//		GLenum attach = clr_attach_0 + i;
//		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, this->v_tex_[i]->tex_dev_id(), 0);
//		this->v_attachment_.push_back(attach);
//	}
//
//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depth_tex_->tex_dev_id(), 0);
//
//	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
//	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
//		return -1;
//
//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//
//	return 0;
//}

l3_int32 render_target_fbo::_init_clr(const l3_int32 width,
	const l3_int32 height,
	const l3_int32 clr_attach_count,
	const render_target_base::e_rtt_color_format format,
	const color& clear_clr)
{
	assert(this->eng_);

	this->_init_dev_fbo(width, height);

	this->clear_clr_ = clear_clr;

	if(clr_attach_count > this->eng_->dev()->fbo_max_attach_count())
	{
		L3_LOG_ENG_DEBUG(("dev is null"));
		return -1;
	}

	this->v_tex_.resize(clr_attach_count);

	/* 创建颜色 */
	for(size_t i = 0; i < this->v_tex_.size(); i ++)
	{
		texture_base::e_tex_type type;
		texture_base::e_tex_inter_type inter_type;

		switch(format)
		{
		case render_target_base::e_rtt_color_format_rgba_f32:
			type = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_rgba_f32;
			break;

		case render_target_base::e_rtt_color_format_rgba_f16:
			type = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_rgba_f16;
			break;

		case render_target_base::e_rtt_color_format_luminance_f16:
			type = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_luminance_16;
			break;

		case render_target_base::e_rtt_color_format_luminance_f32:
			type = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_luminance_32;
			break;

		case render_target_base::e_rtt_color_format_rgba:
		default:
			type = texture_base::e_tex_type_u8;
			inter_type = texture_base::e_tex_inter_type_rgba_u8;
			break;
		}

		this->eng_->tex_mgr()->create_tex(this->v_tex_[i], width, height,
			type, texture_base::e_tex_format_rgba, NULL,
			inter_type,
			texture_base::e_tex_filter_linear, texture_base::e_tex_wrap_clamp_edge);
	}

	/* 绑定 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	GLenum clr_attach_0 = GL_COLOR_ATTACHMENT0_EXT;
	for(size_t i = 0; i < this->v_tex_.size(); i ++)
	{
		GLenum attach = (GLenum)(clr_attach_0 + i);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, this->v_tex_[i]->tex_dev_id(), 0);
		this->v_attachment_.push_back(attach);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

l3_int32 render_target_fbo::_init_dep(const l3_int32 width,
	const l3_int32 height,
	const render_target_base::e_rtt_depth_format depth_format,
	const texture_base::e_tex_compare dep_cmp,
	const l3_f32 clear_dep)
{
	assert(eng);

	if(this->fbo_ == (l3_uint32)-1)
		return 0;
	//this->_init_dev_fbo(width, height);

	this->clear_dep_ = clear_dep;

	/* 创建深度 */
	switch(depth_format)
	{
	case e_rtt_depth_format_16:
		this->eng_->tex_mgr()->create_dep_tex(this->depth_tex_, width, height, L3_FALSE, dep_cmp);
		break;

	case e_rtt_depth_format_32:
	default:
		this->eng_->tex_mgr()->create_dep_tex(this->depth_tex_, width, height, L3_TRUE, dep_cmp);
		break;
	}

	/* 绑定 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);


	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depth_tex_->tex_dev_id(), 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

l3_int32 render_target_fbo::_init_dev_fbo(const l3_int32 width,
	const l3_int32 height)
{
	if(this->fbo_ != (l3_uint32)-1)
		return 0;

	this->width_ = width;
	this->height_ = height;
	glGenFramebuffersEXT(1, &this->fbo_);//glDeleteFramebuffersEXT

	this->pix_offset_x_ = 1.0f / this->width_;
	this->pix_offset_y_ = 1.0f / this->height_;;

	return 0;
}

//l3_int32 render_target_fbo::init_no_dep(const l3_int32 width,
//	const l3_int32 height,
//	const l3_int32 clr_attach_count,
//	const render_target_base::e_rtt_color_format format,
//	const color& clear_clr)
//{
//	return this->_init_clr(width,
//		height,
//		clr_attach_count,
//		format,
//		clear_clr);
//}

l3_int32 render_target_fbo::init_empty(const l3_int32 width, const l3_int32 height,
	const color& clear_clr,
	const l3_f32 clear_dep)
{
	this->clear_clr_ = clear_clr;
	this->clear_dep_ = clear_dep;
	return _init_dev_fbo(width, height);
}

l3_int32 render_target_fbo::add_tex_clr(
	//const l3_int32 width,
	//const l3_int32 height,
	const render_target_base::e_rtt_color_format format)
{
	if(this->fbo_ == (l3_uint32)-1)
		return 0;

	if(this->v_tex_.size() >= (size_t)(this->eng_->dev()->fbo_max_attach_count()))
	{
		L3_LOG_ENG_DEBUG(("dev is null"));
		return -1;
	}


	/* 创建颜色 */
	{
		texture_base::e_tex_type type;
		texture_base::e_tex_inter_type inter_type;

		switch(format)
		{
		case render_target_base::e_rtt_color_format_rgba_f32:
			type       = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_rgba_f32;
			break;

		case render_target_base::e_rtt_color_format_rgba_f16:
			type       = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_rgba_f16;
			break;

		case render_target_base::e_rtt_color_format_luminance_f16:
			type       = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_luminance_16;
			break;

		case render_target_base::e_rtt_color_format_luminance_f32:
			type       = texture_base::e_tex_type_f32;
			inter_type = texture_base::e_tex_inter_type_luminance_32;
			break;

		case render_target_base::e_rtt_color_format_rgba:
		default:
			type       = texture_base::e_tex_type_u8;
			inter_type = texture_base::e_tex_inter_type_rgba_u8;
			break;
		}

		texture_base::ptr tex;
		this->eng_->tex_mgr()->create_tex(tex, this->width_, this->height_,
			type, texture_base::e_tex_format_rgba, NULL,
			inter_type,
			texture_base::e_tex_filter_linear, texture_base::e_tex_wrap_clamp_edge);

		this->v_tex_.push_back(tex);
	}

	/* 绑定 todo : option 不能重新绑定所有 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	this->v_attachment_.resize(0);
	GLenum clr_attach_0 = GL_COLOR_ATTACHMENT0_EXT;
	for(size_t i = 0; i < this->v_tex_.size(); i ++)
	{
		GLenum attach = (GLenum)(clr_attach_0 + i);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, this->v_tex_[i]->tex_dev_id(), 0);
		this->v_attachment_.push_back(attach);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

l3_int32 render_target_fbo::add_tex_clr(texture_base::ptr& tex_ptr)
{
	if(this->fbo_ == (l3_uint32)-1)
		return 0;

	if(this->v_tex_.size() >= (size_t)(this->eng_->dev()->fbo_max_attach_count()))
	{
		L3_LOG_ENG_DEBUG(("dev is null"));
		return -1;
	}

	this->v_tex_.push_back(tex_ptr);

	/* 绑定 todo : option 不能重新绑定所有 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	this->v_attachment_.resize(0);
	GLenum clr_attach_0 = GL_COLOR_ATTACHMENT0_EXT;
	for(size_t i = 0; i < this->v_tex_.size(); i ++)
	{
		GLenum attach = (GLenum)(clr_attach_0 + i);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, this->v_tex_[i]->tex_dev_id(), 0);
		this->v_attachment_.push_back(attach);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

l3_int32 render_target_fbo::add_tex_clr(OBJ_ID tex_id)
{
	texture_base::ptr tex_ptr;
	this->eng_->tex_mgr()->find_tex(tex_id, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	return this->add_tex_clr(tex_ptr);
}

l3_int32 render_target_fbo::del_tex_clr(texture_base::ptr& tex_ptr)
{
	if(this->fbo_ == (l3_uint32)-1)
		return 0;

	{
		for(std::vector<texture_base::ptr>::iterator it = this->v_tex_.begin(); it != this->v_tex_.end(); it ++)
		{
			texture_base::ptr& tex_tmp = *it;
			if(tex_ptr == tex_tmp)
				continue;

			this->v_tex_.erase(it);
			break;
		}
	}

	/* 绑定 todo : option 不能重新绑定所有 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	this->v_attachment_.resize(0);
	GLenum clr_attach_0 = GL_COLOR_ATTACHMENT0_EXT;
	for(size_t i = 0; i < this->v_tex_.size(); i ++)
	{
		GLenum attach = (GLenum)(clr_attach_0 + i);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attach, GL_TEXTURE_2D, this->v_tex_[i]->tex_dev_id(), 0);
		this->v_attachment_.push_back(attach);
	}

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

l3_int32 render_target_fbo::del_all_tex_clr()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	for(size_t i = 0; i < this->v_attachment_.size(); i ++)
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, this->v_attachment_[i], GL_TEXTURE_2D, 0, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status && GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT != status)
	{
		L3_LOG_ENG_WARN(("del_all_tex_clr err status:%x", status));
		win_device::print_error(__FILE__, __LINE__);
		return -1;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	this->v_attachment_.resize(0);
	this->v_tex_.resize(0);

	return 0;
}

l3_int32 render_target_fbo::set_tex_dep(
	//const l3_int32 width,
	//const l3_int32 height,
	const render_target_base::e_rtt_depth_format depth_format,
	const texture_base::e_tex_compare dep_cmp,
	const l3_f32 clear_dep)
{
	return this->_init_dep(this->width_,
		this->height_,
		depth_format,
		dep_cmp,
		clear_dep);
}
	
l3_int32 render_target_fbo::set_tex_dep(texture_base::ptr& tex_ptr)
{
	if(this->fbo_ == (l3_uint32)-1)
		return 0;

	this->depth_tex_ = tex_ptr;

	/* 绑定 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->depth_tex_->tex_dev_id(), 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

l3_int32 render_target_fbo::set_tex_dep(OBJ_ID tex_id)
{
	texture_base::ptr tex_ptr;
	this->eng_->tex_mgr()->find_tex(tex_id, tex_ptr);
	if(tex_ptr.is_null())
		return -1;

	return this->set_tex_dep(tex_ptr);
}

l3_int32 render_target_fbo::unset_tex_dep()
{
	if(this->fbo_ == (l3_uint32)-1)
		return 0;

	this->depth_tex_.set_null();

	/* 绑定 */
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->fbo_);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(GL_FRAMEBUFFER_COMPLETE_EXT != status)
		return -1;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return 0;
}

}
