/**
* @file math.h
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
#ifndef __L3ENG_MATH_H__
#define __L3ENG_MATH_H__

#include <math.h>
#include <float.h>
#include "l3_type.h"

#define L3_RAND rand
#define L3_FMOD fmod

namespace l3eng{

/* @brief 派 */
const l3_f32 L3_PI = 3.14159265359f;

/* @brief 角度转换成弧度 即 派/180 */
const l3_f32 L3_AR_CONVERT = (L3_PI / 180.0f);//0.01745329252;

/* @brief 平方根 */
const l3_f32 l3_sqrt(l3_f32 f);

/* @brief 求倒数 1/f */
const l3_f32 l3_reciprocal(l3_f32 f);
 
/* @brief 求平方根的倒数 */
const l3_f32 l3_reciprocal_sqrt(l3_f32 f);

/* @brief 值是否接近于零 */
l3_int32 l3_is_zero(l3_f32 f, l3_f32 epsilon = 0.000001f);

/* @brief 取绝对值 */
l3_f32 l3_fabs(l3_f32 f);

/* @brief 0-1之间的随机数 */
l3_f32 l3_unit_rand();

/* @brief 获取二次方程的根中较小的一个
* @return 返回负值,表示无解
*/
l3_int32 l3_get_root(l3_f32 a, l3_f32 b, l3_f32 c,
	l3_f32& r1, l3_f32& r2);

/* @brief 求f32的符号位 */
#define L3_SIGN_OF_F32(__f_) ( ( (*(l3_uint32*)&(__f_)) >> 31 )&0x1)

#ifndef FLT_MAX
#define FLT_MAX 3.402823466E+38F
#endif

}

#endif
