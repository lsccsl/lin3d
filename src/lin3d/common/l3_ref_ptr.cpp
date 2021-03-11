/**
* @file ref_ptr.cpp
*
* @author lin shao chuan (email:lsccsl@tom.com)
*
* @brief if it works, it was written by lin shao chuan, if not, i don't know who wrote it
*     指针引用计数封装,缺省情况下,为多线程不安全
*     珍爱生命,远离指针
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
#include "l3_ref_ptr.h"

namespace l3eng{

struct just_for_test_ref_ptr
{
	int a;
};

static void just_for_test()
{
	ref_ptr<just_for_test_ref_ptr, null_lock> a(new just_for_test_ref_ptr);
	ref_ptr<just_for_test_ref_ptr, null_lock> b(a);
	ref_ptr<just_for_test_ref_ptr, null_lock> c;
	c = a;

	a->a = 1;

	a.is_null();

	a.set_null();

	//int * ptr = new int;
	//a = ptr;

	ref_ptr_thread<just_for_test_ref_ptr> d;
	ref_ptr_thread<just_for_test_ref_ptr> d1(new just_for_test_ref_ptr);
	ref_ptr_thread<just_for_test_ref_ptr> d2(d1);
	d = d2;

}

}
