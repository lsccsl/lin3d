/**
* @file math.cpp
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
#include "l3_math.h"
#include <stdlib.h>

namespace l3eng{

/* @brief ƽ���� */
const l3_f32 l3_sqrt(l3_f32 f)
{
	return sqrt(f); //todo : q3's what the fuck ?
}

/* @brief ���� 1/f */
const l3_f32 l3_reciprocal(l3_f32 f)
{
	return ((1.0f)/f);
}

/* @brief ��ƽ�����ĵ��� */
const l3_f32 l3_reciprocal_sqrt(l3_f32 f)
{
	return ((1.0f)/sqrt(f));
}

/* @brief ֵ�Ƿ�ӽ����� */
l3_int32 l3_is_zero(l3_f32 f, l3_f32 epsilon)
{
	if(fabs(f) < epsilon)
		return 1;

	return 0;
}

/* @brief ȡ����ֵ */
l3_f32 l3_fabs(l3_f32 f)
{
	return fabs(f);
}

/* @brief 0-1֮�������� */
l3_f32 l3_unit_rand()
{
	l3_f32 f = (l3_f32)(rand() & 0x7fff);
	return (f / 0x7fff);
}

/* @brief ��ȡ���η��̵ĸ��н�С��һ��
* @return ���ظ�ֵ,��ʾ�޽�
*/
l3_int32 ant_get_root(l3_f32 a, l3_f32 b, l3_f32 c,
	l3_f32& r1, l3_f32& r2)
{
	l3_f32 delt = b * b - 4.0f * a * c;

	if(delt < 0.0f)
		return -1;

	l3_f32 sqrt_delt = l3_sqrt(delt);

	l3_f32 r = 1 / (2 * a);/* todo a����0 */

	r1 = (-b + sqrt_delt) * r;
	r2 = (-b - sqrt_delt) * r;

	return 0;
}

}
