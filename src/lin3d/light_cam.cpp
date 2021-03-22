/**
* @file light_cam.cpp
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
#include "light_cam.h"
#include "sence_mgr.h"
#include "win_device.h"
#include "l3_engine.h"
#include "render_target_mgr.h"

namespace l3eng{

light_cam::light_cam(light::ptr l, sence_mgr * sence):l_(l),
	z_near_(0.1f),
	z_far_(1000.0f),
	sence_(sence),
	virtual_cam_(0.02f),
	virtual_dir_(1.0f),
	virtual_width_(0.2f)
{
	l3_engine * eng = this->sence_->eng();
	render_target_mgr * rtt_mgr = eng->rtt_mgr();

	recal_light_mtx();
}

void light_cam::recal_light_mtx()
{
	l3_engine * eng = this->sence_->eng();
	win_device * dev = eng->dev();

	switch(this->l_->light_type())
	{
	case light::E_LIGHT_DIR_OUTDOOR:
		{
			this->z_far_ = this->l_->light_max_len();
			if (this->z_far_ <= this->z_near_)
				this->z_far_ = this->z_near_ + 10.0f;

			this->view_mtx_ = this->l_->get_mtx_view();
			matrix4::gen_ortho_proj_matrix(
				dev->width(), dev->height(),
				this->z_near_, this->z_far_,
				this->proj_mtx_);
		}
		break;

	case light::E_LIGHT_DIR:
		{
			this->z_far_ = this->l_->light_max_len();
			if(this->z_far_ <= this->z_near_)
				this->z_far_ = this->z_near_ + 10.0f;

#if 0
			this->view_mtx_ = this->l_->get_mtx_view();
			matrix4::gen_ortho_proj_matrix(
				dev->width(), dev->height(),
				this->z_near_, this->z_far_,
				this->proj_mtx_);
#else
			//l3_f32 virtual_cam   = 0.02f;
			//l3_f32 virtual_dir   = 0.5f;
			//l3_f32 virtual_width = 0.2f;

			camera * cam = this->sence_->cam_cur();
			vector3 virtual_pos(cam->x_pos(), cam->y_pos(), cam->z_pos());
			vector3 cam_z;
			cam->get_cam_z(cam_z);
			virtual_pos = virtual_pos + cam_z * cam->z_far() * this->virtual_cam_;
			virtual_pos = virtual_pos - this->l_->dir() * this->l_->light_max_len() * this->virtual_dir_;
			virtual_pos.xyz(-virtual_pos.x(), -virtual_pos.y(), -virtual_pos.z());

			this->z_far_ = this->l_->light_max_len() + this->l_->light_max_len() * this->virtual_dir_;

			this->view_mtx_ = this->l_->get_mtx_view();
			vector3 virtual_tran_pos;
			this->view_mtx_.trans_rotate_vect3(virtual_pos, virtual_tran_pos);
			matrix4::move_matrix(this->view_mtx_,
				virtual_tran_pos.x(), virtual_tran_pos.y(), virtual_tran_pos.z());

			//this->l_->set_pos(virtual_pos.x(), virtual_pos.y(), virtual_pos.z());
			matrix4::gen_ortho_proj_matrix(
				dev->width() * this->virtual_width_, dev->height() * (this->virtual_width_ * cam->aspect()),
				this->z_near_, this->z_far_,
				this->proj_mtx_);
#endif
		}
		break;

	case light::E_LIGHT_SPOT:
		{
			l3_f32 fov = this->l_->cutoff() * 2;

			this->z_far_ = this->l_->light_max_len();
			if(this->z_far_ <= this->z_near_)
				this->z_far_ = this->z_near_ + 10.0f;
			//this->z_far_ = 200.0f;
			this->view_mtx_ = this->l_->get_mtx_view();
			matrix4::gen_pers_proj_matrix(fov, 1.0f, this->z_near_, this->z_far_, this->proj_mtx_);
		}
		break;

	case light::E_LIGHT_POINT:
		break;
	}
}

}
