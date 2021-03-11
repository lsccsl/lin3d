/**
* @file device_buf.h
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
#ifndef __L3ENG_DEVICE_BUF_H__
#define __L3ENG_DEVICE_BUF_H__

#include <vector>
#include "l3_type.h"
#include "l3_ref_ptr.h"

#define VECTOR_BUF

namespace l3eng{

/* @brief 缓冲区可以从此类派生,利用d3d(Lock)或者opengl(glGenBuffersARB)的缓冲区 */
class device_buf_base
{
public:

	typedef ref_ptr_thread<device_buf_base> ptr;

public:

	device_buf_base(){}

	virtual ~device_buf_base(){}
	/* @brief 获取顶点数据 */
	virtual void * get_buf() = 0;
	/* @brief 缓冲区已使用大小 */
	virtual l3_int32 get_buf_sz() = 0;
	/* @brief 设置顶点缓冲区大小 */
	virtual void resize_buf_sz(l3_uint32 sz) = 0;
	/* @brief 缓冲区最大容量 */
	virtual l3_int32 get_buf_cap() = 0;

	/* @brief 由派生类实现 */
	virtual unsigned long get_buf_type() = 0;
	virtual const char * get_buf_type_name(){ return ""; }

public:

	/* @brief 深拷贝缓冲区 */
	virtual void deep_copy(const device_buf_base::ptr& buf_src);
};

/* @brief 常规顶点组件缓存实现,在主存中缓存相应数据 */
class device_common_buf : public device_buf_base
{
public:

	device_common_buf()//:buf_(256)
	{
#ifdef VECTOR_BUF
		this->buf_.resize(0);
#else
		this->pbuf_ = (l3_uint8 *)malloc(256);
		this->buf_cap_ = 256;
		this->buf_sz_ = 0;
#endif
	}

	virtual ~device_common_buf();

	virtual void * get_buf();
	virtual l3_int32 get_buf_sz();
	virtual void resize_buf_sz(l3_uint32 sz);
	virtual l3_int32 get_buf_cap();

	virtual unsigned long get_buf_type(){ return 1; }
	virtual const char * get_buf_type_name(){ return "device_common_buf"; }

private:

#ifdef VECTOR_BUF
	std::vector<l3_uint8> buf_;
#else
	l3_uint8 * pbuf_;
	l3_uint32 buf_sz_;
	l3_uint32 buf_cap_;
#endif
};

}

#endif
