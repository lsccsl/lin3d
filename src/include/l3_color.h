/**
* @file l3_color.h
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
#ifndef __L3ENG_COLOR_H__
#define __L3ENG_COLOR_H__

#include "l3_type.h"
#include "l3_dllexp.h"

namespace l3eng{

class LIN3D_EX_PORT color
{
public:

	color(l3_f32 r = 1.0f,
		l3_f32 g = 1.0f,
		l3_f32 b = 1.0f,
		l3_f32 a = 0.0f):r_(r),g_(g),b_(b),a_(a)
	{}

public:

	const l3_f32 r()const { return this->r_; }
	const l3_f32 g()const { return this->g_; }
	const l3_f32 b()const { return this->b_; }
	const l3_f32 a()const { return this->a_; }

	void r(l3_f32 f){ this->r_ = f; }
	void g(l3_f32 f){ this->g_ = f; }
	void b(l3_f32 f){ this->b_ = f; }
	void a(l3_f32 f){ this->a_ = f; }

	void rgb(l3_f32 r,
		l3_f32 g,
		l3_f32 b)
	{
		this->r_ = r;
		this->g_ = g;
		this->b_ = b;
	}

	void rgba(l3_f32 r,
		l3_f32 g,
		l3_f32 b,
		l3_f32 a = 0.0f)
	{
		this->r_ = r;
		this->g_ = g;
		this->b_ = b;
		this->a_ = a;
	}

	void interpolated(const color& clr, l3_f32 d, color& clr_out)
	{
		l3_f32 inv = 1.0f - d;

		clr_out.r_ = this->r_ * d + clr.r_ * inv;
		clr_out.g_ = this->g_ * d + clr.g_ * inv;
		clr_out.b_ = this->b_ * d + clr.b_ * inv;
		clr_out.a_ = this->a_ * d + clr.a_ * inv;
	}

private:

	l3_f32 r_;
	l3_f32 g_;
	l3_f32 b_;
	l3_f32 a_;
};

}

#endif
