/**
* @file l3_engine.h
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
#ifndef __L3ENG_ENGINE_H__
#define __L3ENG_ENGINE_H__

#include <assert.h>
#include "l3_type.h"
#include "l3_vector3.h"
#include "l3_color.h"
#include "l3_dllexp.h"
#include "l3_image.h"
#include "l3_enum_define.h"

namespace l3eng{


class base_obj_mgr;
class win_device;
class sence_mgr;
class render_target_mgr;
class l3_engine;
class texture_mgr;
class config;
class shader_program_mgr;


enum L3_DEV_EVENT_TYPE{
	L3_DEV_EVENT_KEYBOARD,
	L3_DEV_EVENT_MOUSE,
	L3_DEV_EVENT_STOP,
};

enum L3_VKEY{
	L3_KEY_UNKNOWN = 0,
	L3_KEY_DOWN = 1000,
	L3_KEY_UP,
	L3_KEY_RIGHT,
	L3_KEY_LEFT,
	L3_KEY_SPACE,

	L3_KEY_0,
	L3_KEY_1,
	L3_KEY_2,
	L3_KEY_3,
	L3_KEY_4,
	L3_KEY_5,
	L3_KEY_6,
	L3_KEY_7,
	L3_KEY_8,
	L3_KEY_9,

	L3_KEY_A,
	L3_KEY_B,
	L3_KEY_C,
	L3_KEY_D,
	L3_KEY_E,
	L3_KEY_F,
	L3_KEY_G,
	L3_KEY_H,
	L3_KEY_I,
	L3_KEY_J,
	L3_KEY_K,
	L3_KEY_L,
	L3_KEY_M,
	L3_KEY_N,
	L3_KEY_O,
	L3_KEY_P,
	L3_KEY_Q,
	L3_KEY_R,
	L3_KEY_S,
	L3_KEY_T,
	L3_KEY_U,
	L3_KEY_V,
	L3_KEY_W,
	L3_KEY_X,
	L3_KEY_Y,
	L3_KEY_Z,

	L3_KEY_MULTIPLY,
	L3_KEY_ADD,
	L3_KEY_SEPARATOR,
	L3_KEY_SUBTRACT,
	L3_KEY_DECIMAL,
	L3_KEY_DIVIDE,
	L3_KEY_F1,
    L3_KEY_F2,
	L3_KEY_F3,
	L3_KEY_F4,
	L3_KEY_F5,
	L3_KEY_F6,
	L3_KEY_F7,
	L3_KEY_F8,
	L3_KEY_F9,
	L3_KEY_F10,
	L3_KEY_F11,
	L3_KEY_F12,

	L3_KEY_NUMPAD0,
	L3_KEY_NUMPAD1,
	L3_KEY_NUMPAD2,
	L3_KEY_NUMPAD3,
	L3_KEY_NUMPAD4,
	L3_KEY_NUMPAD5,
	L3_KEY_NUMPAD6,
	L3_KEY_NUMPAD7,
	L3_KEY_NUMPAD8,
	L3_KEY_NUMPAD9,

	L3_KEY_SHIFT,
	L3_KEY_LSHIFT,
	L3_KEY_RSHIFT,
	L3_KEY_CONTROL,
	L3_KEY_LCONTROL,
	L3_KEY_RCONTROL,
	L3_KEY_TAB,
};

/* @brief keyboard event data */
struct keyboard_data_t
{
	/* Character corresponding to the key (0, if not a character) */
	char char_;

	/* Key which has been pressed or released */
	L3_VKEY key_;
	int unknow_key_val_;

	/* If not true, then the key was left up */
	bool pressed_down_;

	/* True if shift was also pressed */
	bool shift_;

	/* True if ctrl was also pressed */
	bool control_;

	/* @brief lin3d engine dont't user this data, just for extension */
	void * user_data_;
	void * reserve_for_user_data_;
};

enum L3_MOUSE_EVENT{
	L3_MOUSE_MOVE,
	L3_MOUSE_LBUTTONUP,
	L3_MOUSE_RBUTTONUP,
};

/* @brief mouse event data */
struct mouse_data_t
{
	L3_MOUSE_EVENT mouse_event_;

	l3_int32 x_;
	l3_int32 y_;

	/* @brief lin3d engine dont't user this data, just for extension */
	void * user_data_;
	void * reserve_for_user_data_;
};

struct l3_event_t
{
	/* @brief event: mouse keyboard */
	L3_DEV_EVENT_TYPE event_type_;

	union{
		keyboard_data_t keyboard_;
		mouse_data_t mouse_;
	};
};


class LIN3D_EX_PORT frame_listener
{
public:

	virtual ~frame_listener(){}

	/* @brief 输入事件 */
	virtual l3_int32 dev_event(const l3_event_t& e){ return 0; }
	virtual void pre_render(){}
	virtual void post_render(){}
};


class LIN3D_EX_PORT default_listener : public frame_listener
{
public:

	default_listener(l3_engine * eng):eng_(eng),
		y_degree_(0),x_degree_(0),z_degree_(0),
		mouse_x_(0),mouse_y_(0)
	{}

	virtual ~default_listener(){}

	virtual l3_int32 dev_event(const l3_event_t& e)override;	
	virtual void pre_render()override{}
	virtual void post_render()override{}

	virtual l3_int32 keyboard_event(const keyboard_data_t& keyboard_evt);
	virtual l3_int32 mouse_event(const mouse_data_t& mouse_evt);

public:

	l3_engine * eng_;

	/* @brief 摄影机旋转 */
	l3_f32 x_degree_;
	l3_f32 y_degree_;
	l3_f32 z_degree_;

	/* @brief 记录上次一鼠标移动事件坐标 */
	l3_int32 mouse_x_;
	l3_int32 mouse_y_;
};

class LIN3D_EX_PORT l3_engine
{
public:

	friend class default_listener;

	struct LIN3D_EX_PORT fps_count_t
	{
		fps_count_t():frame_count_(0),fps_start_(-1),fps_(0.0f){}

		l3_uint64 fps_start_;
		l3_uint32 frame_count_;
		l3_f32 fps_;
	};
	fps_count_t fps_;

public:

	l3_engine();
	~l3_engine();

	l3_int32 init_eng(frame_listener * fl, l3_bool test_mode);


	void run();

	inline void run_frame() {
		_run_frame();
	}

	inline void run_frame_eng() {
		_run_frame_eng();
	}

	inline void render_sence(const L3_RENDER_GROUP rgroup, OBJ_ID rtt_obj_id = -1){
		_render_sence(rgroup, rtt_obj_id);
	}


	inline l3_uint64 t_elapse(){
		return t_elapse_;
	}

	//void set_t_start();
	//void set_t_frame_start();
	//void set_t_frame_end();


public:

	/* @brief 从场景中删除 */
	l3_int32 obj_del_from_sence(OBJ_ID obj_id);
	/* @brief 删除对象 */
	l3_int32 obj_del(OBJ_ID obj_id);


	/* @brief 摄影机移动 */
	void camera_move_toward(l3_f32 f);
	void camera_move_sideward(l3_f32 f);
	void camera_set(vector3& angle, vector3& pos);
	void camera_get(vector3& angle, vector3& pos);
	void camera_set_pers(l3_f32 fovy, l3_f32 z_near, l3_f32 z_far);
	void camera_set_ortho(l3_f32 view_width, l3_f32 view_height, l3_f32 z_near, l3_f32 z_far);
	/* @brief 屏幕拾取射线 */
	void cam_get_screen_ray(const l3_int32 x, const l3_int32 y,
		l3_f32& r_start_x, l3_f32& r_start_y, l3_f32& r_start_z,
		l3_f32& r_dir_x, l3_f32& r_dir_y, l3_f32& r_dir_z);


	/* @brief 灯光 */
	OBJ_ID light_create();
	OBJ_ID light_get_rtt(OBJ_ID light_obj);
	l3_int32 light_enable(OBJ_ID obj, const l3_bool enable);
	l3_int32 light_enable_vol(OBJ_ID obj, const l3_bool enable);
	l3_int32 light_enable_shadow(OBJ_ID obj, const l3_bool enable);
	l3_int32 light_type(OBJ_ID obj, const l3_int32 type);
	l3_int32 light_clr_diffuse(OBJ_ID obj, const color& clr);
	l3_int32 light_clr_specular(OBJ_ID obj, const color& clr);
	l3_int32 light_clr_ambient(OBJ_ID obj, const color& clr);
	l3_int32 light_set_pos(OBJ_ID obj, const vector3& vpos);
	l3_int32 light_set_rotate(OBJ_ID obj, l3_f32 x_degree, l3_f32 y_degree, l3_f32 z_degree = 0);
	l3_int32 light_spot_exponent(OBJ_ID obj, const l3_f32 exponent);
	l3_int32 light_spot_cutoff(OBJ_ID obj, const l3_f32 cutoff);
	l3_int32 light_spot_cutoff_inner_outer(OBJ_ID obj, const l3_f32 cutoff_inner, const l3_f32 cutoff_outer);
	l3_int32 light_max_len(OBJ_ID obj, l3_f32 max_len); //点光源最大照亮距离(平行光无效)
	l3_int32 light_dir_virtual_cam(OBJ_ID obj, l3_f32 f); //平行光阴影参数, 摄影机前置距离
	l3_int32 light_dir_virtual_dir(OBJ_ID obj, l3_f32 f); //平行光阴影参数, 光源摄影机后退距离
	l3_int32 light_dir_virtual_width(OBJ_ID obj, l3_f32 f); //平行光阴影参数, 光源摄影机宽度缩放(正交投影的宽度)
	l3_int32 light_dir_virtual_height(OBJ_ID obj, l3_f32 f); //平行光阴影参数, 光源摄影机高度缩放(正交投影的宽度)

	void light_enable_light_debug(OBJ_ID obj, l3_bool e);


	/* @brief material */
	l3_int32 material_poly_mode(OBJ_ID obj, l3_int32 mode);
	l3_int32 material_shininess(OBJ_ID obj, l3_f32 s);
	l3_int32 material_enable_reflect(OBJ_ID obj, l3_bool b);
	l3_int32 material_enable_depth_test(OBJ_ID obj, l3_bool b);


	/* @brief 一个物体位置 */
	l3_int32 robj_get_pos(OBJ_ID obj_id, vector3& pos);
	l3_int32 robj_move_to_xyz(OBJ_ID obj_id, l3_f32 x, l3_f32 y, l3_f32 z);
	l3_int32 robj_move_xyz(OBJ_ID obj_id, l3_f32 x, l3_f32 y, l3_f32 z);

	/* @brief 旋传一个物体 */
	l3_int32 robj_rotate(OBJ_ID obj_id, l3_f32 x_degree, l3_f32 y_degree, l3_f32 z_degree);

	/* @brief 缩放一个物体 */
	l3_int32 robj_scale(OBJ_ID obj_id, l3_f32 x_scale, l3_f32 y_scale, l3_f32 z_scale);

	/* @brief 获取物体的朝向,上向量 */
	l3_int32 dir_world_up(OBJ_ID obj_id, vector3& v);

	/* @brief 获取物体的朝向,前向量 */
	l3_int32 dir_world_toward(OBJ_ID obj_id, vector3& v);

	/* @brief 获取物体的朝向,左向量 */
	l3_int32 dir_world_left(OBJ_ID obj_id, vector3& v);


	/* @brief 设置某个物体的render_group */
	l3_int32 robj_set_render_group(OBJ_ID obj_id, const L3_RENDER_GROUP rgoup);


	/*
	* @brief 设置纹理
	* @param obj_id: render obj id
	* @param tex_obj: 纹理对象id(由load_tex创建)
	*/
	l3_int32 robj_set_tex(OBJ_ID obj_id, OBJ_ID tex_obj, l3_uint32 lay = 0);

	/* @brief set render target	*/
	l3_int32 robj_set_rtt(OBJ_ID obj_id, OBJ_ID rtt_obj);

	/* @brief 设置shader */
	l3_int32 robj_set_sys_shader(OBJ_ID obj_id, const char * name);
	/* @brief add sys shader pass */
	l3_int32 robj_add_sys_shader_pass(OBJ_ID obj_id, const char * name);


	/* @brief add cube render obj */
	OBJ_ID robj_geometry_create_cube(l3_f32 d1, l3_f32 d2, l3_f32 d3,
		l3_bool user_clr = L3_FALSE, l3_uint8 clr_r = 255, l3_uint8 clr_g = 255, l3_uint8 clr_b = 255);
	/* @brief create square obj */
	OBJ_ID robj_geometry_create_square(l3_f32 width, l3_f32 height,
		l3_bool user_clr = 0, l3_uint8 clr_r = 255, l3_uint8 clr_g = 255, l3_uint8 clr_b = 255,
		l3_uint8 alpha = 0);
	/* @brief add triangle */
	OBJ_ID robj_geometry_create_triangle(l3_f32 p1x, l3_f32 p1y, l3_f32 p1z,
		l3_f32 p2x, l3_f32 p2y, l3_f32 p2z,
		l3_f32 p3x, l3_f32 p3y, l3_f32 p3z);

	/* @brief 创建平面 */
	OBJ_ID robj_plane_create(const l3_f32 width, const l3_f32 length,
		const vector3& normal, const vector3& inter_point,
		l3_bool user_clr = L3_FALSE, const vertex_color_t& clr = vertex_color_t());
	/* @brief 设置平面 */
	l3_int32 robj_plane_set_nml_inter_point(OBJ_ID obj_id, const vector3& normal, const vector3& inter_point);
	/* @brief 获取平面参数 nml dist */
	l3_int32 robj_plane_get_plane_info(OBJ_ID obj_id,
		l3_f32& nml_x, l3_f32& nml_y, l3_f32& nml_z, l3_f32& d);

	/* @brief 创建向量 */
	OBJ_ID robj_vector_create(const vector3& v_start, const vector3& v_dir, l3_f32 draw_scale = 1.0f);
	/* @brief 设置向量方向 */
	l3_int32 robj_vector_set_dir(OBJ_ID obj_id, const vector3& v_dir, l3_f32 draw_scale = 1.0f);

	/* @brief create sphere */
	OBJ_ID robj_sphere_create(l3_f32 radius, l3_uint32 resolution = 8, const color& clr = color(1.0f, 1.0f, 1.0f, 0.0f));

	/* @brief create cone */
	OBJ_ID robj_cone_create(const l3_f32 cone_height, const l3_f32 cone_angle_degree, const l3_uint32 resolution = 6, 
		const vector3& v_dir = vector3(0.0f, -1.0f, 0.0f),
		const color& clr = color());

	/* @brief sky dome */
	OBJ_ID robj_skydome_create(l3_f32 radius,
		l3_f32 tex_percent,
		l3_f32 sphere_percent,
		l3_int32 h_resolution,
		l3_int32 v_resolution);


	/* @brief 创建一个skeleton动画 */
	OBJ_ID robj_skeleton_create(const char * file_path);
	/* @brief 动画 */
	l3_int32 robj_skeleton_animation(OBJ_ID obj_id, const char * ani_name, const l3_int32 frame, const l3_f32 t);


	/* @brief 创建render target */
	OBJ_ID rtt_fbo_create(const char * file_path,
		const l3_int32 width,
		const l3_int32 height,
		const l3_int32 clr_attach_count = 1,
		const color& clear_clr = color(0, 0, 0, 0),
		const e_rtt_clr_type clr_type = e_rtt_clr_rgba_f16,
		const e_rtt_depth_type dep_type = e_rtt_depth_f16);

	/* @brief 获取tex */
	OBJ_ID rtt_get_tex_clr(OBJ_ID rtt,
		l3_uint32 idx = 1);

	/* @brief 获取dep */
	OBJ_ID rtt_get_tex_dep(OBJ_ID rtt);

	/* @brief show rrt tex */
	void rtt_show_tex(OBJ_ID rtt, const l3_uint32 tex_idx = 1,
		const l3_f32 x_start = 0.0, const l3_f32 y_start = 0.0,
		const l3_f32 x_len = 0.1, const l3_f32 y_len = 0.1,
		const l3_f32 dep = 0.0f);


	/*
	* @brief 加载纹理
	* @return 返回纹理对象的id
	*/
	OBJ_ID tex_load(const char * file_path);
	OBJ_ID tex_load_img(OBJ_ID img_obj_id);
	OBJ_ID tex_load_empty(const l3_int32 width, const l3_int32 height);
	l3_int32 tex_update(OBJ_ID tex_obj_id,
		const l3_int32 xoffset, const l3_int32 yoffset,
		const l3_int32 width, const l3_int32 height,
		const e_pixel_format pix_format, const void * pixels);

	l3_int32 tex_wrap_repeate(OBJ_ID tex);
	l3_int32 tex_wrap_clamp(OBJ_ID tex);
	l3_int32 tex_wrap_clamp_edge(OBJ_ID tex);

	l3_int32 tex_filter_nearest(OBJ_ID tex);
	l3_int32 tex_filter_linear(OBJ_ID tex);


	/* @brief 创建图像 */
	OBJ_ID image_create(const char * path);
	l3_int32 image_save(OBJ_ID img_obj_id, const char * file_name);

	l3_int32 image_load_raw_memory(OBJ_ID img_obj_id,
		const l3_uint8 * data, const l3_int32 data_sz,
		const l3_uint32 width, const l3_uint32 height, const e_pixel_format pix_format = E_PIXEL_LUMINANCE_8BIT);
	OBJ_ID image_load_raw_memory(const char * path,
		const l3_uint8 * data, const l3_int32 data_sz,
		const l3_uint32 width, const l3_uint32 height, const e_pixel_format pix_format = E_PIXEL_LUMINANCE_8BIT);
	/* @brief 从一个图片里创建一个法线图 */
	OBJ_ID image_load_normal(const char * virtual_img_path, const char * img_path, const l3_f32 scale = 1.0f);
	/* @brief 加载一个图片 */
	OBJ_ID image_load(const char * virtual_img_path, const char * img_path);


	/* @brief 是否延迟渲染 */
	l3_int32 shader_defer_render(OBJ_ID obj_id, l3_bool b);
	l3_int32 shader_defer_light(OBJ_ID obj_id, l3_bool b);

	/* @brief 获取延迟渲染的fbo */
	OBJ_ID shader_defer_gbuffer();

	/* @brief 获取延迟渲染的rtt light */
	OBJ_ID shader_defer_rtt_light();

	/* @brief 获取光照与阴影纹理 */
	OBJ_ID shader_defer_rtt_light_shadow();

	/* @brief 获取shader */
	OBJ_ID shader_get(const char * name);

	/* @brief 设置shader参数 */
	void shader_set_param_f32(OBJ_ID shader_id, const l3_int32 param_type, const l3_f32 param_val);
	void shader_set_param_vec3(OBJ_ID shader_id, const l3_int32 param_type, const vector3& param_val);
	void shader_set_param_bool(OBJ_ID shader_id, const l3_int32 param_type, const l3_bool param_val);
	void shader_set_param(OBJ_ID shader_id, const l3_int32 param_type, const void * param, const l3_uint32 param_sz);
	void shader_set_param_obj(OBJ_ID shader_id, const l3_int32 param_type, OBJ_ID obj);


	/* @brief 射线拾取 */
	l3_bool ray_pickup(const l3_f32 start_x, const l3_f32 start_y, const l3_f32 start_z,
		const l3_f32 dir_x, const l3_f32 dir_y, const l3_f32 dir_z,
		OBJ_ID& obj_id,
		l3_f32& t,
		l3_f32& tr_p1x, l3_f32& tr_p1y, l3_f32& tr_p1z,
		l3_f32& tr_p2x, l3_f32& tr_p2y, l3_f32& tr_p2z,
		l3_f32& tr_p3x, l3_f32& tr_p3y, l3_f32& tr_p3z);


	/* @brief 是否启用延迟渲染 */
	void enable_defer_render(l3_bool e);
	void enable_atmospheric(l3_bool e);
	void enable_hdr(l3_bool e);
	void enable_ssao(l3_bool e);

	void set_title(const char * name);

public:

	void add_sys_folder(const char * folder_name);

public:

	inline base_obj_mgr * obj_mgr()
	{ assert(this->obj_mgr_); return this->obj_mgr_; }

	inline win_device * dev()
	{ assert(this->dev_); return this->dev_; }

	inline texture_mgr * tex_mgr()
	{ assert(this->tex_mgr_); return this->tex_mgr_; }

	inline render_target_mgr * rtt_mgr()
	{ assert(this->rtt_mgr_); return this->rtt_mgr_; }

	inline config * cfg()
	{ assert(this->cfg_); return this->cfg_; }

	inline shader_program_mgr * shdr_mgr()
	{ assert(this->shdr_mgr_); return this->shdr_mgr_; }

	inline sence_mgr * sence(){ return this->sence_; }

	void dev_window_size(l3_f32 width, l3_f32 height);
	l3_f32 dev_window_width();
	l3_f32 dev_window_height();

public:

	inline void enable_debug(l3_bool e)
	{ this->enable_debug_ = e; }
	inline l3_bool enable_debug()
	{ return this->enable_debug_; }

	void robj_enable_debug(OBJ_ID obj, l3_bool b);

public:

	// for debug
	void set_reflect_mode(const l3_f32 nx, const l3_f32 ny, const l3_f32 nz, const l3_f32 d);

	void set_refract_mode(const l3_f32 nx, const l3_f32 ny, const l3_f32 nz, const l3_f32 d);

	void set_normal_mode();

protected:

	l3_int32 _run_frame();
	l3_int32 _run_frame_eng();
	l3_int32 _run_frame_msg();
	
	l3_int32 _render_sence(const L3_RENDER_GROUP rgroup = L3_RENDER_GROUP_ALL, OBJ_ID rtt_obj_id = -1);

protected:

	config * cfg_;

	/* @brief 场景管理 */
	sence_mgr * sence_;

	/* @brief windows窗体,消息循环 */
	win_device * dev_;

	/* @brief 帧循环回调 */
	frame_listener * frm_lsn_;
	default_listener * def_lsn_;

	/* @brief 对象管理器 */
	base_obj_mgr * obj_mgr_;

	/* @brief render target manager */
	render_target_mgr * rtt_mgr_;

	/* @brief 纹理资源管理 */
	texture_mgr * tex_mgr_;

	/* @brief glsl管理 */
	shader_program_mgr * shdr_mgr_;

	/* @brief debug */
	l3_bool enable_debug_;

	l3_uint64 t_start_;
	l3_uint64 t_elapse_;
	l3_uint64 t_frame_start_;
	l3_uint64 t_frame_end_;
	//l3_uint64 t_frame_elapse_;
};

}

#endif
