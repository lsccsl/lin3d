/**
* @file camera.cpp
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
#include "camera.h"
#include <math.h>
#include "math.h"
#include "ray.h"

namespace l3eng{

/* @brief ������Ӱ��"��ǰ/�����ƶ�"(������Ӱ����ǰ�����ķ���x/y/z_degree,�Զ������x/y/z_pos) */
void camera::move_toward(const l3_f32 dist)
{
	//todo : option ��������
	this->y_pos_ += dist * sin(this->x_degree_ * L3_AR_CONVERT);

	l3_f32 xz_dist = (dist * cos((this->x_degree_) * L3_AR_CONVERT));

	this->x_pos_ -= (xz_dist * sin(this->y_degree_ * L3_AR_CONVERT));/* x/z������ʵ���Ǹ��� */
	this->z_pos_ -= (xz_dist * cos(this->y_degree_ * L3_AR_CONVERT));

	this->set_cam_view_mtx_dirty();
}

/* @brief ���� ��+ ��- */
void camera::move_sideward(const l3_f32 dist)
{
	//todo : option ��������
	vector3 dir;

	l3_f32 xz = cos(this->x_degree_ * L3_AR_CONVERT);

	dir.y(sin(this->x_degree_ * L3_AR_CONVERT));
	dir.x(xz * sin(this->y_degree_ * L3_AR_CONVERT));
	dir.z(xz * cos(this->y_degree_ * L3_AR_CONVERT));

	dir.normalise();

	vector3 dir_up;
	vector3 dir_left;
	dir.get_up_left(dir_up, dir_left);

	this->x_pos_ += dist * dir_left.x();
	this->y_pos_ += dist * dir_left.y();
	this->z_pos_ += dist * dir_left.z();

	this->set_cam_view_mtx_dirty();
}

void camera::move_vertical(const l3_f32 dist)
{
	//todo : option ��������
	vector3 dir;

	l3_f32 xz = cos(this->x_degree_ * L3_AR_CONVERT);

	dir.y(sin(this->x_degree_ * L3_AR_CONVERT));
	dir.x(xz * sin(this->y_degree_ * L3_AR_CONVERT));
	dir.z(xz * cos(this->y_degree_ * L3_AR_CONVERT));

	dir.normalise();

	vector3 dir_up;
	vector3 dir_left;
	dir.get_up_left(dir_up, dir_left);

	this->x_pos_ += dist * dir_up.x();
	this->y_pos_ += dist * dir_up.y();
	this->z_pos_ += dist * dir_up.z();

	this->set_cam_view_mtx_dirty();
}

/* @brief ���¼���任���� */
void camera::check_and_cal_mtx()
{
	if(!this->need_cal_view_mtx_)
		return;

	matrix4 x_rotate_mtx;
	matrix4::gen_rotate_x_matrix(-this->x_degree_, x_rotate_mtx);
	matrix4 y_rotate_mtx;
	matrix4::gen_rotate_y_matrix(-this->y_degree_, y_rotate_mtx);

	matrix4 rotate_mtx;
	y_rotate_mtx.mult(x_rotate_mtx, rotate_mtx);

	matrix4 move_mtx;
	matrix4::gen_move_matrix(move_mtx, -this->x_pos_, -this->y_pos_, -this->z_pos_);
	move_mtx.mult(rotate_mtx, this->view_mtx_);

	this->clear_cam_view_mtx_dirty();
}

void camera::get_ray_from_screen(const l3_f32 x_normalize, const l3_f32 y_normalize,
	ray& r)
{
	/*
	ogreʰȡ���߼���:

	����Ӱ����project��λ�ñ任�еõ�,
	�����һ������Ļ����,
	����project����Ӱ��λ�ñ任�������,
	��֪ͶӰ���x,y(ȡֵ-1~1)
	ȡ����ֵz=-1(��ƽ��,�����ߵ����,��㲻����Ӱ�����ڵ�ԭ��),��ȡһ��z=0,�Ϳ���ȷ������

	���������µ�ʽ

	[x,y,z,1] *  M_proj = [x_p * z,y_p * z,z_p * z,w_p]
	����M_projΪͶӰ����,  x_p,y_p,z_pΪ��֪ w_p=z

	�ɱ仯��
	[x/z,y/z,1,1/z] *  M_proj = [x_p,y_p,z_p,1]
	��
	[x_p,y_p,z_p,1] * Inv(M_proj) = [x/z,y/z,1,1/z]
	ȥ���������1/z,�Ϳ��Եõ�x,y,z

	������������ײ���
	*/
	matrix4 m;
	this->proj_mtx_.gen_inverse_matrix(m);

	/* ������任�� -1,1֮�� */
	l3_f32 x_proj = (2.0f * x_normalize) - 1.0f;
	l3_f32 y_proj = 1.0f - (2.0f * y_normalize);

	vector3 v_start(x_proj, y_proj, -1.0f);
	vector3 v_target(x_proj, y_proj, 0.0f);

	vector3 v_start_view;
	vector3 v_target_view;

	m.mult_v3(v_start, v_start_view, L3_TRUE);
	m.mult_v3(v_target, v_target_view, L3_TRUE);

	this->view_mtx_.gen_inverse_matrix(m);
	m.mult_v3(v_start_view, r.v_start());

	vector3 v_target_world;
	m.mult_v3(v_target_view, v_target_world);

	r.v_dir(v_target_world - r.v_start());
	r.v_dir().normalise();
}

/* @brief �����ӽ���,һ��ȱ���Ľ��������� */
void camera::check_and_cal_frustum()
{
	if(!this->f_.need_re_cal_)
		return;

	/*
	* �ӽ��������ƽ��,������Ӱ������תƽ��,����ƽ��ķ����Լ���ԭ��ľ���������仯
	* ����ĳ��ƽ��ķ���Ϊ v = (x, y, z)
	* ���ߵ���ת������ô����
	* v_new = v * this->view_mtx_(ȥ�����һ�е�ƽ�Ʒ���,�Լ����һ������������)
	*
	* �õ�v_new֮��,ƽ���붥��ľ��������ô����, ȡ��ƽ�ƾ���,���һ�е�ƽ�Ʒ���,���������ʵ������Ϊ��ԭ���ƶ��������
	* ��Ϊfrustum��ʼ������ƽ��,�������ת��,��ԭ��ľ�����ȻΪ��.
	* ���ǰ��ƶ����ԭ����� o_new,�����������this->view_mtx_���һ�е�ǰ����ֵ,��ƽ�Ʒ���
	* ��������֪�� o_newһ�������ƶ����ƽ���ϵ�
	* �� o_new����ƽ�淽��,��
	* dist = -(v_new * o_new) �͵õ����� (ƽ�淽�� ax + by + cz + dist = 0)
	* ����,ֻҪ֪��frustum��ת֮ǰ����ƽ��ķ���,�Ϳ��Ը��� this->view_mtx_ ����תƽ����frustum����ƽ��������
	* 
	* �������̵ó�frustum��ʼ�ķ���,���Դ�this->proj_mtx_�еõ�,����������һ��ƽ��,����left,���ķ���Ӧ��������
	* v_n_left = (this->proj_mtx_.mv_[0][0] + this->proj_mtx_.mv_[3][0],
	*             this->proj_mtx_.mv_[0][1] + this->proj_mtx_.mv_[3][1],
	*             this->proj_mtx_.mv_[0][2] + this->proj_mtx_.mv_[3][2])
	* ������ M = this->view_mtx_.transpose() * this->proj_mtx_.transpose()  --- ȡת�þ���
	* �۲�õ���M, v_n_left��ת�������,��ʵ���� M�ĵ�һ�м�������(ȡǰ����ֵ)
	* ͬ�� left/righ  top/bottom z_near/z_far�ķ��߶��������Ƶķ����������
	*
	* Ȼ���Ǽ���dist,��Ȼ��leftΪ��,ǰ���Ѿ�˵��
	* dist = -(v_new * o_new)
	* ����ȡthis->view_mtx_�����Ͻ�3x3����һ���ּ�¼Vr
	* dist = -((v_n_left * Vr) * o_new)
	* ����˷���������
	* dist = - (v_n_left * (Vr * o_new))
	* �۲�ó� Vr * o_new ʵ�ʾ��� M �����һ�е�ǰ����ֵ,dist��Ϊ���ǵ�������(�����и�),proj_mtx_ת�ú�,��Ӧ0��λ�õ�ֵ,����Ҫ
	*
	* ��������,��ʵ����ogre��irrlicht����frustum����ƽ��ķ���
	* ��quake�ķ��������ƺ����������˿�����
	* ����ע��,����ת���ĽǶ���ƽ��ת���ĽǶ��������90��,����ʹ��ת�þ������ÿ�����ȷ���㷨�ߵ�ת��
	*/

	matrix4 proj_t;
	this->proj_mtx_.transpose(proj_t);
	matrix4 view_t;
	this->view_mtx_.transpose(view_t);

	matrix4 M;
	proj_t.mult(view_t, M);

	this->f_.plane_[FRUSTUM_PLANE_LEFT].set_normal(M[3][0] + M[0][0],
			M[3][1] + M[0][1],
			M[3][2] + M[0][2]);
	this->f_.plane_[FRUSTUM_PLANE_LEFT].dist(M[3][3] + M[0][3]);

	this->f_.plane_[FRUSTUM_PLANE_RIGHT].set_normal(M[3][0] - M[0][0],
			M[3][1] - M[0][1],
			M[3][2] - M[0][2]);
	this->f_.plane_[FRUSTUM_PLANE_RIGHT].dist(M[3][3] - M[0][3]);


	this->f_.plane_[FRUSTUM_PLANE_TOP].set_normal(M[3][0] - M[1][0],
			M[3][1] - M[1][1],
			M[3][2] - M[1][2]);
	this->f_.plane_[FRUSTUM_PLANE_TOP].dist(M[3][3] - M[1][3]);

	this->f_.plane_[FRUSTUM_PLANE_BOTTOM].set_normal(M[3][0] + M[1][0],
			M[3][1] + M[1][1],
			M[3][2] + M[1][2]);
	this->f_.plane_[FRUSTUM_PLANE_BOTTOM].dist(M[3][3] + M[1][3]);


	this->f_.plane_[FRUSTUM_PLANE_NEAR].set_normal(M[3][0] + M[2][0],
			M[3][1] + M[2][1],
			M[3][2] + M[2][2]);
	this->f_.plane_[FRUSTUM_PLANE_NEAR].dist(M[3][3] + M[2][3]);

	this->f_.plane_[FRUSTUM_PLANE_FAR].set_normal(M[3][0] - M[2][0],
			M[3][1] - M[2][1],
			M[3][2] - M[2][2]);
	this->f_.plane_[FRUSTUM_PLANE_FAR].dist(M[3][3] - M[2][3]);


	/* ��һ������,�޸�dist */
	l3_f32 rleng;
	for(l3_int32 i = 0; i < sizeof(this->f_.plane_)/sizeof(this->f_.plane_[0]); i ++)
	{
		this->f_.plane_[i].normalise_normal(&rleng);
		this->f_.plane_[i].dist(this->f_.plane_[i].dist() * rleng);
	}

	this->fm_.dirty_ = 1;

	this->clear_frustum_dirty();
}

/* @brief �ж�aab���Ƿ����ӽ��巶Χ�� */
camera::enum_cam_visible camera::is_visible(const aabbox& box)const
{
	enum_cam_visible ret = TOTAL_IN_CAM;
	/* ���boxλ��6��ƽ��֮ǰ,˵�������ӽ��嵱��
	* ���λ��ĳ��ƽ��֮��,˵�������޳�
	*/
	for(l3_int32 i = 0; i < sizeof(this->f_.plane_)/sizeof(this->f_.plane_[0]); i ++)
	{
		plane::E_SIDE side = this->f_.plane_[i].aabb_on_plane_side(box);

		switch(side)
		{
		case plane::BACK_SIDE:
			return OUT_OF_CAM;
			break;

		case plane::BOTH_SIDE:
			ret = PART_IN_CAM;
			break;
		}
	}

	return ret;
}

robj_geometry& camera::get_fg()
{
	if(!this->fm_.dirty_)
		return this->fm_.fg_;

	this->fm_.fg_.clear_mesh();

	/* �����ӽ����8������ */
	this->f_.plane_[FRUSTUM_PLANE_FAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_LEFT],
		this->f_.plane_[FRUSTUM_PLANE_TOP],
		this->fm_.far_top_left);
	this->f_.plane_[FRUSTUM_PLANE_FAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_RIGHT],
		this->f_.plane_[FRUSTUM_PLANE_TOP],
		this->fm_.far_top_right);
	this->f_.plane_[FRUSTUM_PLANE_FAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_LEFT],
		this->f_.plane_[FRUSTUM_PLANE_BOTTOM],
		this->fm_.far_bottom_left);
	this->f_.plane_[FRUSTUM_PLANE_FAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_RIGHT],
		this->f_.plane_[FRUSTUM_PLANE_BOTTOM],
		this->fm_.far_bottom_right);

	this->f_.plane_[FRUSTUM_PLANE_NEAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_LEFT],
		this->f_.plane_[FRUSTUM_PLANE_TOP],
		this->fm_.near_top_left);
	this->f_.plane_[FRUSTUM_PLANE_NEAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_RIGHT],
		this->f_.plane_[FRUSTUM_PLANE_TOP],
		this->fm_.near_top_right);
	this->f_.plane_[FRUSTUM_PLANE_NEAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_LEFT],
		this->f_.plane_[FRUSTUM_PLANE_BOTTOM],
		this->fm_.near_bottom_left);
	this->f_.plane_[FRUSTUM_PLANE_NEAR].intersection_with_2planes(this->f_.plane_[FRUSTUM_PLANE_RIGHT],
		this->f_.plane_[FRUSTUM_PLANE_BOTTOM],
		this->fm_.near_bottom_right);


	/* ��Ӷ���� */
	this->fm_.fg_.add_triangle(this->fm_.near_top_left,
		this->fm_.near_bottom_left,
		this->fm_.near_top_right);

	this->fm_.fg_.add_triangle(this->fm_.near_top_right,
		this->fm_.near_bottom_left,
		this->fm_.near_bottom_right);

	this->fm_.fg_.add_triangle(this->fm_.far_top_left,
		this->fm_.far_bottom_left,
		this->fm_.far_top_right);

	this->fm_.fg_.add_triangle(this->fm_.far_top_right,
		this->fm_.far_bottom_left,
		this->fm_.far_bottom_right);

	this->fm_.dirty_ = 0;

	return this->fm_.fg_;
}

void camera::get_cam_x(vector3& v)const
{
	v.x(this->view_mtx_[0][0]);
	v.y(this->view_mtx_[1][0]);
	v.z(this->view_mtx_[2][0]);
}
void camera::get_cam_y(vector3& v)const
{
	v.x(this->view_mtx_[0][1]);
	v.y(this->view_mtx_[1][1]);
	v.z(this->view_mtx_[2][1]);
}
void camera::get_cam_z(vector3& v)const
{
	v.x(-this->view_mtx_[0][2]);
	v.y(-this->view_mtx_[1][2]);
	v.z(-this->view_mtx_[2][2]);
}

camera& camera::operator = (const camera& r)
{
	this->x_degree_ = r.x_degree_;
	this->y_degree_ = r.y_degree_;
	this->z_degree_ = r.z_degree_;

	this->x_pos_ = r.x_pos_;
	this->y_pos_ = r.y_pos_;
	this->z_pos_ = r.z_pos_;

	this->fovy_ = r.fovy_;
	this->aspect_ = r.aspect_;
	this->z_near_ = r.z_near_;
	this->z_far_ = r.z_far_;

	this->view_width_ = r.view_width_;
	this->view_height_ = r.view_height_;

	this->f_ = r.f_;
	this->fm_ = r.fm_;

	this->view_mtx_ = r.view_mtx_;
	this->need_cal_view_mtx_ = r.need_cal_view_mtx_;

	this->proj_mtx_ = r.proj_mtx_;

	this->active_ = r.active_;

	return *this;
}

}
