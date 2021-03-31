/**
* @file win_device.cpp
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
#include "win_device.h"
#include <windows.h>
#include "win32gl_glee.h"
#include "material.h"
#include "robj_base.h"
#include "device_buf.h"
#include "vertex_buf.h"
#include "index_buf.h"
#include "l3_log.h"
#include "texture_base.h"
#include "texture_mgr.h"
#include "render_target_base.h"
#include "render_target_mgr.h"
#include "shader_program_mgr.h"

namespace l3eng{

static LRESULT CALLBACK _win_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	win_device * wind = (win_device *)GetWindowLongPtr(hWnd, GWLP_USERDATA/*GWL_USERDATA*/);
	if(wind)
		wind->wnd_proc(message, wParam, lParam);

	return DefWindowProc(hWnd, message, wParam, lParam);
}

win_device::win_device(l3_engine * eng):
	eng_(eng),
	top_left_x_(200),
	top_left_y_(200),
	width_(808),
	height_(627),
	class_name_("lin3d"),
	module_name_("lin3d"),
	win_exit_(L3_FALSE),
	have_event_(L3_FALSE),
	max_tex_units_(1),
	max_tex_units_fixed_pipe_(1),
	fbo_max_attach_count_(1),
	global_blend_mode_(L3_BLEND_MODE_DISABLE)
{}

win_device::~win_device()
{}

l3_int32 win_device::print_error(char *file, int line)
{
	GLenum gl_err;
	l3_int32 ret = 0;

	gl_err = glGetError();
	while (gl_err != GL_NO_ERROR)
	{
		L3_LOG_ENG_TRACE(("glError in file %s @ line %d: glGetError:%d %s\n", file, line, gl_err, gluErrorString(gl_err)));
		ret += 1;
		gl_err = glGetError();
	}
	return ret;
}

l3_int32 win_device::wnd_proc(unsigned int message, l3_uint64 wParam, l3_uint64 lParam)
{
	//this->have_event_ = L3_FALSE;
	switch(message)
	{
	case WM_CREATE:
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		this->win_exit_ = L3_TRUE;
		break;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		{
			this->win_event_.event_type_ = L3_DEV_EVENT_KEYBOARD;
			this->have_event_ = L3_TRUE;
			this->win_event_.keyboard_.key_ = L3_KEY_UNKNOWN;
			this->win_event_.keyboard_.unknow_key_val_ = -1;
			this->win_event_.keyboard_.pressed_down_ = ((message == WM_KEYDOWN) ? L3_TRUE : L3_FALSE);

			static BYTE allKeys[256];
			GetKeyboardState(allKeys);
			this->win_event_.keyboard_.shift_ = ((allKeys[VK_SHIFT] & 0x80) != 0);
			this->win_event_.keyboard_.control_ = ((allKeys[VK_CONTROL] & 0x80)!=0);

			switch(wParam)
			{
			case VK_DOWN:
				this->win_event_.keyboard_.key_ = L3_KEY_DOWN;
				break;

			case VK_UP:
				this->win_event_.keyboard_.key_ = L3_KEY_UP;
				break;

			case VK_RIGHT:
				this->win_event_.keyboard_.key_ = L3_KEY_RIGHT;
				break;

			case VK_LEFT:
				this->win_event_.keyboard_.key_ = L3_KEY_LEFT;
				break;

			case VK_SPACE:
				this->win_event_.keyboard_.key_ = L3_KEY_SPACE;
				break;

			case 0x41:
			case 0x42:
			case 0x43:
			case 0x44:
			case 0x45:
			case 0x46:
			case 0x47:
			case 0x48:
			case 0x49:
			case 0x4a:
			case 0x4b:
			case 0x4c:
			case 0x4d:
			case 0x4e:
			case 0x4f:
			case 0x50:
			case 0x51:
			case 0x52:
			case 0x53:
			case 0x54:
			case 0x55:
			case 0x56:
			case 0x57:
			case 0x58:
			case 0x59:
			case 0x5a:
				this->win_event_.keyboard_.key_ = (L3_VKEY)(L3_KEY_A + (wParam - 0x41));
				break;

			case 0x30:
			case 0x31:
			case 0x32:
			case 0x33:
			case 0x34:
			case 0x35:
			case 0x36:
			case 0x37:
			case 0x38:
			case 0x39:
				this->win_event_.keyboard_.key_ = (L3_VKEY)(L3_KEY_0 + (wParam - 0x30));
				break;

			case VK_MULTIPLY:
				this->win_event_.keyboard_.key_ = L3_KEY_MULTIPLY;
				break;
			case VK_ADD:
				this->win_event_.keyboard_.key_ = L3_KEY_ADD;
				break;
			case VK_SEPARATOR:
				this->win_event_.keyboard_.key_ = L3_KEY_SEPARATOR;
				break;
			case VK_SUBTRACT:
				this->win_event_.keyboard_.key_ = L3_KEY_SUBTRACT;
				break;
			case VK_DECIMAL:
				this->win_event_.keyboard_.key_ = L3_KEY_DECIMAL;
				break;
			case VK_DIVIDE:
				this->win_event_.keyboard_.key_ = L3_KEY_DIVIDE;
				break;
			case VK_F1:
				this->win_event_.keyboard_.key_ = L3_KEY_F1;
				break;
			case VK_F2:
				this->win_event_.keyboard_.key_ = L3_KEY_F2;
				break;
			case VK_F3:
				this->win_event_.keyboard_.key_ = L3_KEY_F3;
				break;
			case VK_F4:
				this->win_event_.keyboard_.key_ = L3_KEY_F4;
				break;
			case VK_F5:
				this->win_event_.keyboard_.key_ = L3_KEY_F5;
				break;
			case VK_F6:
				this->win_event_.keyboard_.key_ = L3_KEY_F6;
				break;
			case VK_F7:
				this->win_event_.keyboard_.key_ = L3_KEY_F7;
				break;
			case VK_F8:
				this->win_event_.keyboard_.key_ = L3_KEY_F8;
				break;
			case VK_F9:
				this->win_event_.keyboard_.key_ = L3_KEY_F9;
				break;
			case VK_F10:
				this->win_event_.keyboard_.key_ = L3_KEY_F10;
				break;
			case VK_F11:
				this->win_event_.keyboard_.key_ = L3_KEY_F11;
				break;
			case VK_F12:
				this->win_event_.keyboard_.key_ = L3_KEY_F12;
				break;
			
			case VK_NUMPAD0:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD0;
				break;
			case VK_NUMPAD1:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD1;
				break;
			case VK_NUMPAD2:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD2;
				break;
			case VK_NUMPAD3:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD3;
				break;
			case VK_NUMPAD4:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD4;
				break;
			case VK_NUMPAD5:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD5;
				break;
			case VK_NUMPAD6:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD6;
				break;
			case VK_NUMPAD7:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD7;
				break;
			case VK_NUMPAD8:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD8;
				break;
			case VK_NUMPAD9:
				this->win_event_.keyboard_.key_ = L3_KEY_NUMPAD9;
				break;

			case VK_SHIFT:
				this->win_event_.keyboard_.key_ = L3_KEY_SHIFT;
				break;
			case VK_LSHIFT:
				this->win_event_.keyboard_.key_ = L3_KEY_LSHIFT;
				break;
			case VK_RSHIFT:
				this->win_event_.keyboard_.key_ = L3_KEY_RSHIFT;
				break;

			case VK_CONTROL:
				this->win_event_.keyboard_.key_ = L3_KEY_CONTROL;				
				break;
			case VK_LCONTROL:
				this->win_event_.keyboard_.key_ = L3_KEY_LCONTROL;
				break;
			case VK_RCONTROL:
				this->win_event_.keyboard_.key_ = L3_KEY_RCONTROL;
				break;

			case VK_TAB:
				this->win_event_.keyboard_.key_ = L3_KEY_TAB;
				break;

			default:
				this->win_event_.keyboard_.unknow_key_val_ = (int)wParam;
				break;
			}
		}
		break;

	case WM_MOUSEMOVE:
		{
			this->have_event_ = L3_TRUE;
			this->win_event_.event_type_ = L3_DEV_EVENT_MOUSE;
			this->win_event_.mouse_.mouse_event_ = L3_MOUSE_MOVE;

			this->win_event_.mouse_.x_ = LOWORD(lParam);
			this->win_event_.mouse_.y_ = HIWORD(lParam);
		}
		break;
	
	case WM_LBUTTONUP:
		{
			this->have_event_ = L3_TRUE;
			this->win_event_.event_type_ = L3_DEV_EVENT_MOUSE;
			this->win_event_.mouse_.mouse_event_ = L3_MOUSE_LBUTTONUP;
			
			this->win_event_.mouse_.x_ = LOWORD(lParam);
			this->win_event_.mouse_.y_ = HIWORD(lParam);
		}
		break;

	case WM_RBUTTONUP:
		{
			this->have_event_ = L3_TRUE;
			this->win_event_.event_type_ = L3_DEV_EVENT_MOUSE;
			this->win_event_.mouse_.mouse_event_ = L3_MOUSE_RBUTTONUP;

			this->win_event_.mouse_.x_ = LOWORD(lParam);
			this->win_event_.mouse_.y_ = HIWORD(lParam);
		}
		break;

	case WM_SIZE:
		{
			this->height_ = HIWORD(lParam);
			this->width_ = LOWORD(lParam);

#if 1
			glViewport(0, 0, (GLsizei)this->width_, (GLsizei)this->height_);
#else
			//glViewport(-100, -100, (GLsizei)this->width_, (GLsizei)this->height_);
#endif

			this->eng_->dev_window_size(this->width_, this->height_);
		}
		break;

	default:
		break;
	}

	return 0;
}

/* @brief 初始化window */
l3_int32 win_device::_init_window()
{
	HINSTANCE hInst;
	if(this->module_name_.empty())
		hInst = GetModuleHandle(NULL);
	else
		hInst = GetModuleHandle(this->module_name_.c_str());

	WNDCLASSEX wcex = {0};

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)_win_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= this->class_name_.c_str();

	int ret = RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(this->class_name_.c_str(), "", WS_OVERLAPPEDWINDOW,
		(int)this->top_left_x_, (int)this->top_left_y_, (int)this->width_, (int)this->height_, NULL, NULL, hInst, this);

	this->hwnd_ = hWnd;

	SetWindowLongPtr(hWnd, GWLP_USERDATA/*GWL_USERDATA*/, (LONG_PTR)this);

	this->hdc_ = GetDC(hWnd);

    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd =
    {   
        sizeof(PIXELFORMATDESCRIPTOR),  // size
            1,                          // version
            PFD_SUPPORT_OPENGL |        // OpenGL window
            PFD_DRAW_TO_WINDOW |        // render to window
            PFD_DOUBLEBUFFER,           // support double-buffering
            PFD_TYPE_RGBA,              // color type
            32,                         // prefered color depth
            0, 0, 0, 0, 0, 0,           // color bits (ignored)
            0,                          // no alpha buffer
            0,                          // alpha bits (ignored)
            0,                          // no accumulation buffer
            0, 0, 0, 0,                 // accum bits (ignored)
            16,                         // depth buffer
            0,                          // no stencil buffer
            0,                          // no auxiliary buffers
            PFD_MAIN_PLANE,             // main layer
            0,                          // reserved
            0, 0, 0,                    // no layer, visible, damage masks
    };

    pixelFormat = ChoosePixelFormat((HDC)(this->hdc_), &pfd);
    SetPixelFormat((HDC)(this->hdc_), pixelFormat, &pfd);

	if(!IsWindow(hWnd))
		return -1;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return 0;
}

/* @brief 初始化opengl */
l3_int32 win_device::_init_gl()
{
	HGLRC hrc = wglCreateContext((HDC)(this->hdc_));
	wglMakeCurrent((HDC)(this->hdc_), hrc);

	win32gl_glee_init();

	glClearColor(0.0, 0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glDepthMask( GL_TRUE );
	glDepthRange (0, 1);
	this->disable_cull_face();

	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &this->max_tex_units_fixed_pipe_); //固定管线纹理数量
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &this->max_tex_units_); //可编程管理纹理数量
	
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &this->fbo_max_attach_count_); //fbo最多可挂接的颜色纹理

	const GLubyte* OpenGLVersion =glGetString(GL_VERSION);
	printf("max tex program:%d\r\n"
		"max tex fixed pipe:%d\r\n"
		"fbo max attach:%d\r\n",
		this->max_tex_units_,
		this->max_tex_units_fixed_pipe_,
		this->fbo_max_attach_count_);
	printf("opengl version:%s\r\n", OpenGLVersion);

	return 0;
}

l3_int32 win_device::init()
{
	_init_window();

	_init_gl();

	return 0;
}

l3_int32 win_device::msg_loop()
{
	this->have_event_ = L3_FALSE;
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!GetMessage (&msg, NULL, 0, 0))
			return -1;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(this->have_event_)
			break;
	}

	if(this->win_exit_)
		return -1;

	if(this->have_event_)
		return 1;
	else
		return 0;
}

void win_device::set_view_matrix(const matrix4& m)
{
	this->mtx_view_.assign(m);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(this->mtx_view_.get_matrix_data());
}
void win_device::set_proj_matrix(const matrix4& m)
{
#if 1
	this->mtx_proj_ = m;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m.get_matrix_data());
#else
	glMatrixMode(GL_PROJECTION;)
	glLoadIdentity();
	gluPerspective(
		60.0,               /* field of view in degree */
		1.0,                /* aspect ratio */
		0.5, 30.0);

	float view_matri[16];
	glGetFloatv(GL_PROJECTION_MATRIX, view_matri);
#endif
}

void win_device::enable_rtt(OBJ_ID rtt)
{
	render_target_base::ptr rtt_ptr;

	this->eng_->rtt_mgr()->get_render_target(rtt, rtt_ptr);
	if(!rtt_ptr.is_null())
		rtt_ptr->enable();
}

void win_device::disable_rtt(OBJ_ID rtt)
{
	render_target_base::ptr rtt_ptr;

	this->eng_->rtt_mgr()->get_render_target(rtt, rtt_ptr);
	if(!rtt_ptr.is_null())
		rtt_ptr->disable();

#if 1
	glViewport(0, 0, (GLsizei)this->width_, (GLsizei)this->height_);
#else
	//glViewport(this->width_/2, this->height_/2, (GLsizei)this->width_, (GLsizei)this->height_);
#endif
}

void win_device::clear_fbo(OBJ_ID rtt)
{
	render_target_base::ptr rtt_ptr;

	this->eng_->rtt_mgr()->get_render_target(rtt, rtt_ptr);
	if(!rtt_ptr.is_null())
		rtt_ptr->clear_fbo();
}

l3_int32 win_device::begin_render()
{
	glFinish();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return 0;
}
l3_int32 win_device::end_render()
{
	glFlush();
	SwapBuffers((HDC)(this->hdc_));

	return 0;
}

l3_int32 win_device::bind_user_tex(OBJ_ID tex_id, const l3_bool bind_shadow)
{
	if(this->current_tex_unit_ >= this->max_tex_units_)
	{
		L3_LOG_ENG_ERR(("current tex unit:%d, max is:%d", this->current_tex_unit_, this->max_tex_units_));
		return -1;
	}

	texture_base::ptr tex;
	if(this->eng_->tex_mgr()->find_tex(tex_id, tex) == base_obj::INVALID_OBJ_ID)
		return -1;

	l3_int32 ret = this->current_tex_unit_;

	tex->tex_dev_bind(this->current_tex_unit_, bind_shadow);
	this->current_tex_unit_ ++;

	return ret;
}

void win_device::disable_cull_face()
{
	if(!this->cull_face_info_.is_cull_face_)
		return;

	glDisable(GL_CULL_FACE);
	this->cull_face_info_.is_cull_face_ = L3_FALSE;
}

void win_device::enable_cull_front()
{
	if(this->cull_face_info_.is_cull_face_ && en_cull_type_front == this->cull_face_info_.cull_face_type_)
		return;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);//不渲染"前面",只渲染"背面"

	this->cull_face_info_.cull_face_type_ = en_cull_type_front;
	this->cull_face_info_.is_cull_face_ = L3_TRUE;
}

void win_device::enable_cull_back()
{
	if(this->cull_face_info_.is_cull_face_ && en_cull_type_back == this->cull_face_info_.cull_face_type_)
		return;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	this->cull_face_info_.cull_face_type_ = en_cull_type_back;
	this->cull_face_info_.is_cull_face_ = L3_TRUE;
}

void win_device::push_cull_info()
{
	this->lst_cfi_.push_front(this->cull_face_info_);

	if(this->lst_cfi_.size() >= 9)
		L3_LOG_ENG_WARN(("tow many cull info cache"));
}
void win_device::pop_cull_info()
{
	if(this->lst_cfi_.empty())
		return;

	cull_face_info_t cfi = this->lst_cfi_.front();
	this->lst_cfi_.pop_front();

	if(cfi.is_cull_face_)
	{
		switch(cfi.cull_face_type_)
		{
		case en_cull_type_front:
			this->enable_cull_front();
			break;
		
		case en_cull_type_back:
			this->enable_cull_back();
			break;
		}
	}
	else
	{
		this->disable_cull_face();
	}
}

l3_int32 win_device::render(robj_base * obj)
{
	//glDisable(GL_CULL_FACE);
	this->current_tex_unit_ = 0;

	const vertex_buf& vb = obj->get_vertex_buf();
	const index_buf& ib = obj->get_index_buf();

	if(ib.buf_is_null())
		return -1;

	{
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		matrix4 temp(0);
		obj->get_world_matrix().mult(this->mtx_view_, temp);
		glLoadMatrixf(temp.get_matrix_data());
	}

#if 0
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//glFrustum(-2.0, 2.0, -2.0, 2.0, 1.0, 1000.0);
		gluPerspective(45.0, this->width_ / this->height_, 0.1f, 1000.0f);

		float view_matri[16];
		glGetFloatv(GL_PROJECTION_MATRIX, view_matri);
	}
#endif

	material& mtrl = obj->get_material();
	GLenum poly_mode = GL_LINE;
	switch(mtrl.poly_mode())
	{
	case material::E_POLY_POIN:
		poly_mode = GL_POINT;
		glPointSize(2.0f);
		break;

	case material::E_POLY_LINE:
		poly_mode = GL_LINE;
		break;

	case material::E_POLY_FILL:
		poly_mode = GL_FILL;
		break;
	}
	glPolygonMode(GL_FRONT_AND_BACK, poly_mode);

	if(mtrl.enable_depth_test())
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if(mtrl.depth_write())
		glDepthMask(GL_TRUE);
	else
		glDepthMask(GL_FALSE);


	if(L3_BLEND_MODE_NONE == this->global_blend_mode_)
		this->_enable_blend(mtrl.blend_mode());
	else
		this->_enable_blend(this->global_blend_mode_);


	{
		//绑定纹理1
		OBJ_ID tex0_id = mtrl.get_tex_obj_id(0);
		if(tex0_id != base_obj::INVALID_OBJ_ID)
			this->bind_user_tex(tex0_id);

		//绑定纹理2
		OBJ_ID tex1_id = mtrl.get_tex_obj_id(1);
		if(tex1_id != base_obj::INVALID_OBJ_ID)
			this->bind_user_tex(tex1_id);
	}

	{
		//顶点
		const device_buf_base::ptr& pos_buf = vb.buf_pos();
		if(!pos_buf.is_null())
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, pos_buf->get_buf());
		}

		//颜色
		const device_buf_base::ptr& clr_buf = vb.buf_clr();
		if(!clr_buf.is_null())
		{
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, clr_buf->get_buf());
		}

		//纹理映射1
		const device_buf_base::ptr& texcoord_buf0 = vb.buf_texcoord0();
		if(!texcoord_buf0.is_null())
		{
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);

			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_filter);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_filter);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buf0->get_buf());
		}

		//纹理映射2
		const device_buf_base::ptr& texcoord_buf1 = vb.buf_texcoord1();
		if(!texcoord_buf1.is_null())
		{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, texcoord_buf1->get_buf());
		}

		//法线
		const device_buf_base::ptr& nml_buf = vb.buf_nml();
		if(!nml_buf.is_null())
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, nml_buf->get_buf());
		}
	}

	if(!this->active_shdr_.is_null())
	{
		this->active_shdr_->prepare(*obj);
		shader_program::ptr& ptr_prog = this->active_shdr_->get_shader_program();
		if(!ptr_prog.is_null())
		{
			ptr_prog->uni_dev_bind();
			ptr_prog->uni_dev_bind_tex();
			ptr_prog->attr_dev_bind();
		}
	}

#if 0
int ver_count = vb.get_vertex_count();
int * ptmp = (int *)ib.get_buf()->get_buf();
for(int i = 0; i < ib.idx_count(); i ++)
{
	if(ptmp[i] < ver_count)
		continue;
	
	L3_LOG_DEBUG(1, ("err idx"));
	exit(0);
}
L3_LOG_DEBUG(1, ("glDrawElements %d %x %d ver_count:%d", ib.idx_count(), ib.get_buf().get(), ib.get_buf()->get_buf_sz(), ver_count));
#endif

	GLenum mode = GL_TRIANGLES;
	switch(ib.idx_type())
	{
	case E_ITYPE_LINES:
		mode = GL_LINES;
		break;

	case E_ITYPE_TRIANGLES:
		mode = GL_TRIANGLES;
		break;

	case E_ITYPE_POINTS:
	default:
		mode = GL_POINTS;
		break;
	}

	glDrawElements(mode, 
		ib.idx_count(),
		GL_UNSIGNED_INT,
		ib.get_buf()->get_buf());

	if(!this->active_shdr_.is_null())
	{
		shader_program::ptr& ptr_prog = this->active_shdr_->get_shader_program();
		if(!ptr_prog.is_null())
		{
			ptr_prog->uni_unbind_tex_all();
			ptr_prog->uni_unbind_all();
			ptr_prog->attr_unbind_all();

			ptr_prog->attr_dev_unbind();
		}
	}

	for(l3_int32 i = 0; i < this->current_tex_unit_; i ++)
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + i);
		glClientActiveTextureARB(GL_TEXTURE0_ARB + i);
		glEnable(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
	}

	//glActiveTextureARB( GL_TEXTURE0 );
	//glClientActiveTextureARB(GL_TEXTURE0_ARB);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	this->_enable_blend(L3_BLEND_MODE_DISABLE);

	this->current_tex_unit_ = 0;

	return 0;
}

void win_device::set_title(const char * sz_title)
{
	SetWindowText((HWND)(this->hwnd_), sz_title);
}

l3_uint32 win_device::get_millsecond()
{
	/* todo:49.7天,哈哈 */
	return GetTickCount();

#if 0
	LARGE_INTEGER  large_interger;  
	double dff;  
	__int64  c1, c2;  
	QueryPerformanceFrequency(&large_interger);  
	dff = large_interger.QuadPart;  
	QueryPerformanceCounter(&large_interger);  
	c1 = large_interger.QuadPart;  
	Sleep(1000);  
	QueryPerformanceCounter(&large_interger);  
	c2 = large_interger.QuadPart;
#endif
}

/* @brief glBlendFunc(src, dst) src:表示当前要写入的像素,dst:表示当前缓冲区的像素 */
void win_device::_enable_blend(l3_blend_mode_t blend_mode)
{
	if(L3_BLEND_MODE_NONE == blend_mode)
		return;

	if(L3_BLEND_MODE_DISABLE == blend_mode)
	{
		glDisable(GL_BLEND);
		return;
	}

	glEnable(GL_BLEND);

	switch(blend_mode)
	{
	case L3_BLEND_MODE_ALPHA:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case L3_BLEND_MODE_ALPHA1:
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		break;

	case L3_BLEND_MODE_ADD:
		glBlendFunc(GL_ONE, GL_ONE);
		break;

	case L3_BLEND_MODE_MODULATE:
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		break;

	case L3_BLEND_MODE_COLOR:
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		break;

	case L3_BLEND_MODE_COLOR1:
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
		break;

	default:
		break;
	}
}


void win_device::enable_poly_offset_fill(l3_f32 factor, l3_f32 unit)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(factor, unit);
}

void win_device::disable_poly_offset_fill()
{
	glPolygonOffset( 0.0f, 0.0f);
	glDisable(GL_POLYGON_OFFSET_FILL);
}


}

