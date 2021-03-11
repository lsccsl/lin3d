/**
* @file win_device.h
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
#ifndef __L3ENG_WIN_DEVICE_H__
#define __L3ENG_WIN_DEVICE_H__

#include <string>
#include "l3_type.h"
#include "l3_engine.h"
#include "matrix4.h"
#include "shader.h"
#include "blend.h"

namespace l3eng{

class robj_base;

class win_device
{
public:

	enum en_cull_type
	{
		en_cull_type_none = 0,
		en_cull_type_front = 1,
		en_cull_type_back  = 2,
	};

public:

	win_device(l3_engine * eng);
	~win_device();

	l3_int32 init();

	/* @brief windows proc for per window */
	l3_int32 wnd_proc(unsigned int message, l3_uint64 wParam, l3_uint64 lParam);
	
	l3_int32 msg_loop();
	const l3_event_t& win_event()
	{ return this->win_event_; }

	l3_int32 begin_render();
	l3_int32 end_render();

	l3_int32 render(robj_base * obj);

	void active_shdr(shader::ptr shdr)
	{ this->active_shdr_ = shdr; }
	void set_active_shdr_null()
	{ this->active_shdr_.set_null(); }

	void set_view_matrix(const matrix4& m);
	void set_proj_matrix(const matrix4& m);
	const matrix4& get_view_matrix()
	{
		return this->mtx_view_;
	}
	const matrix4& get_proj_matrix()
	{
		return this->mtx_proj_;
	}

	const l3_f32 width(){ return this->width_; }
	const l3_f32 height(){ return this->height_; }

	void enable_rtt(OBJ_ID rtt);
	void disable_rtt(OBJ_ID rtt);
	void clear_fbo(OBJ_ID rtt);

	/* @brief 绑定张惠,返回绑定的位置(current_tex_unit_ ++之前的值, -1表示绑定失败) */
	l3_int32 bind_user_tex(OBJ_ID tex_id, const l3_bool bind_shadow = L3_FALSE);


	void disable_cull_face();
	/* @brief 不渲染前面 */
	void enable_cull_front();
	/* @brief 不渲染背面 */
	void enable_cull_back();

	l3_bool is_cull_face()const
	{
		return this->cull_face_info_.is_cull_face_;
	}
	en_cull_type cull_face_type()const
	{
		return this->cull_face_info_.cull_face_type_;
	}

	void push_cull_info();
	void pop_cull_info();


	void blend_mode(l3_blend_mode_t blend_mode){
		this->global_blend_mode_ = blend_mode;
	}


	void enable_poly_offset_fill(l3_f32 factor, l3_f32 unit);
	void disable_poly_offset_fill();


	inline l3_bool have_event(){
		return have_event_;
	}
	inline void set_no_event(){
		have_event_ = L3_FALSE;
	}

public:

	l3_int32 fbo_max_attach_count(){ return this->fbo_max_attach_count_; }

	void set_title(const char * sz_title);
	
	l3_uint32 get_millsecond();

	static l3_int32 print_error(char *file, int line);

private:

	/* @brief 初始化window */
	l3_int32 _init_window();
	/* @brief 初始化opengl */
	l3_int32 _init_gl();

	/* @brief 开启alpha以及其它混合方式 */
	void _enable_blend(l3_blend_mode_t blend_mode);

private:

	l3_engine * eng_;

	/* @brief HWND */
	void * hwnd_;
	void * hdc_;

	/* @brief window info */
	l3_f32 top_left_x_;
	l3_f32 top_left_y_;
	l3_f32 width_;
	l3_f32 height_;
	std::string class_name_;
	std::string module_name_;

	/* @brief window event */
	l3_bool win_exit_;
	l3_bool have_event_;
	l3_event_t win_event_;

	/* @brief 固定管线纹理数量 */
	l3_int32 max_tex_units_fixed_pipe_;
	/* @brief 可编程管线纹理数是 */
	l3_int32 max_tex_units_;
	l3_int32 current_tex_unit_;

	/* @brief fbo颜色纹理最大数 */
	l3_int32 fbo_max_attach_count_;

	/* @brief 观察矩阵与世界矩阵 */
	matrix4 mtx_view_;
	/* @brief 投影矩阵 */
	matrix4 mtx_proj_;

	/* @brief 面朝向栽剪 */
	struct cull_face_info_t
	{
		cull_face_info_t(en_cull_type cull_type = en_cull_type_none, l3_bool is_cull_face = L3_FALSE):
			cull_face_type_(cull_type),
			is_cull_face_(is_cull_face)
		{}

		l3_bool is_cull_face_;
		en_cull_type cull_face_type_;
	};
	cull_face_info_t cull_face_info_;
	std::list<cull_face_info_t> lst_cfi_;

	shader::ptr active_shdr_;

	l3_blend_mode_t global_blend_mode_;
};

}

#endif
