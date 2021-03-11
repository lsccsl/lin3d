/**
* @file l3_type.h
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
#ifndef __L3ENG_TYPE_H__
#define __L3ENG_TYPE_H__

namespace l3eng
{

typedef char l3_int8;
typedef unsigned char l3_uint8;

typedef short l3_int16;
typedef unsigned short l3_uint16;

typedef int l3_int32;
typedef unsigned int l3_uint32;

typedef unsigned long long l3_uint64;
typedef long long l3_int64;

typedef float l3_f32;
typedef double l3_f64;

typedef bool l3_bool;

#define L3_TRUE (true)
#define L3_FALSE (false)

typedef l3_int64 OBJ_ID;

#pragma pack(push)
#pragma pack(1)

/* @brief 顶点纹理映射 */
struct vertex_texcoord_t
{
	l3_f32 u_;
	l3_f32 v_;
};

/* @brief 顶点坐标 */
struct vertex_pos_t
{
	l3_f32 x_;
	l3_f32 y_;
	l3_f32 z_;
};

/* @brief 顶点颜色(0~255) */
struct vertex_color_t
{
	vertex_color_t(l3_uint8 r = 255, l3_uint8 g = 255, l3_uint8 b = 255, l3_uint8 a = 255):
		r_(r),
		g_(g),
		b_(b),
		a_(a)
	{}
	l3_uint8 r_;
	l3_uint8 g_;
	l3_uint8 b_;
	l3_uint8 a_;
};

/* @brief 法线信息 */
struct vertex_normal_t
{
	l3_f32 x_;
	l3_f32 y_;
	l3_f32 z_;
};

/* @brief 顶点索引定义 */
struct vertex_idx_t
{
	l3_uint32 idx_;
};

/* @brief render target color attachment format */
enum e_rtt_clr_type{
	e_rtt_clr_rgba_u8, //8bit
	e_rtt_clr_rgba_f16, //浮点16bit
	e_rtt_clr_rgba_f32, //浮点32bit
};

/* @brief  */
enum e_rtt_depth_type{
	e_rtt_depth_f16,
	e_rtt_depth_f32,
};

#pragma pack(pop)

}

#endif
