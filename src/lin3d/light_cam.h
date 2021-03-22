/**
* @file light_cam.h
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
#ifndef __L3ENG_LIGHT_CAM_H__
#define __L3ENG_LIGHT_CAM_H__

#include "base_obj.h"
#include "matrix4.h"
#include "light.h"

namespace l3eng{

class sence_mgr;

class light_cam : public base_obj
{
public:

	typedef ref_ptr_thread<light_cam> ptr; 
	OBJ_TYPE_DEFINE(base_obj, OBJ_LIGHT_CAM)

public:

	light_cam(light::ptr l, sence_mgr * sence);

	void recal_light_mtx();

	inline const matrix4& view_mtx(){ return this->view_mtx_; }
	inline const matrix4& proj_mtx(){ return this->proj_mtx_; }

	inline const l3_f32 z_near(){ return this->z_near_; }
	inline const l3_f32 z_far(){ return this->z_far_; }

	inline void virtual_cam(l3_f32 f)
	{ this->virtual_cam_ = f; }
	inline void virtual_dir(l3_f32 f)
	{ this->virtual_dir_ = f; }
	inline void virtual_width(l3_f32 f)
	{ this->virtual_width_ = f; }

private:

	sence_mgr * sence_;

	/* @brief 光源的观察矩阵 */
	matrix4 view_mtx_;
	/* @brief 投影 */
	matrix4 proj_mtx_;

	/* @brief 远近截面 */
	l3_f32 z_near_;
	l3_f32 z_far_;

	l3_f32 virtual_cam_;
	l3_f32 virtual_dir_;
	l3_f32 virtual_width_;


	light::ptr l_;
};

}

#endif
