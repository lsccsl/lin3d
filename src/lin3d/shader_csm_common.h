/**
* @file shader_csm_common.h
*
* @author lin shao chuan (email:czhama@163.com)
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
#ifndef __L3ENG_SHADER_CSM_COMMON_H__
#define __L3ENG_SHADER_CSM_COMMON_H__

#include "l3_type.h"
#include "matrix4.h"

namespace l3eng {

struct csm_cam_seg_info
{
	matrix4 mtx_proj_;
	matrix4 mtx_view_;
	l3_f32 z_near_;
	l3_f32 z_far_;
	/* @brief 光源空间的深度纹理 */
	OBJ_ID tex_csm_depth_;
	OBJ_ID tex_csm_depth_clr_;
};

}

#endif
