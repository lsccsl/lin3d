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

/* @brief 设置摄影机"朝前/朝后移动"(根据摄影机当前所朝的方向x/y/z_degree,自动计算出x/y/z_pos) */
void camera::move_toward(const l3_f32 dist)
{
	//todo : option 保存向量
	this->y_pos_ += dist * sin(this->x_degree_ * L3_AR_CONVERT);

	l3_f32 xz_dist = (dist * cos((this->x_degree_) * L3_AR_CONVERT));

	this->x_pos_ -= (xz_dist * sin(this->y_degree_ * L3_AR_CONVERT));/* x/z的坐标实际是负的 */
	this->z_pos_ -= (xz_dist * cos(this->y_degree_ * L3_AR_CONVERT));

	this->set_cam_view_mtx_dirty();
}

/* @brief 侧移 左+ 右- */
void camera::move_sideward(const l3_f32 dist)
{
	//todo : option 保存向量
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
	//todo : option 保存向量
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

/* @brief 重新计算变换矩阵 */
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
	ogre拾取射线计算:

	从摄影机的project与位置变换中得到,
	传入归一化的屏幕坐标,
	利用project与摄影机位置变换的逆矩阵,
	已知投影后的x,y(取值-1~1)
	取两个值z=-1(近平面,即射线的起点,起点不是摄影机所在的原点),另取一个z=0,就可以确定射线

	我们有以下等式

	[x,y,z,1] *  M_proj = [x_p * z,y_p * z,z_p * z,w_p]
	其中M_proj为投影矩阵,  x_p,y_p,z_p为已知 w_p=z

	可变化成
	[x/z,y/z,1,1/z] *  M_proj = [x_p,y_p,z_p,1]
	即
	[x_p,y_p,z_p,1] * Inv(M_proj) = [x/z,y/z,1,1/z]
	去除齐次因子1/z,就可以得到x,y,z

	射线与多边形碰撞检测
	*/
	matrix4 m;
	this->proj_mtx_.gen_inverse_matrix(m);

	/* 将坐标变换至 -1,1之间 */
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

/* @brief 计算视截体,一个缺顶的金塔六面体 */
void camera::check_and_cal_frustum()
{
	if(!this->f_.need_re_cal_)
		return;

	/*
	* 视截体的六个平面,随着摄影机的旋转平移,六个平面的法线以及与原点的距离均发生变化
	* 假设某个平面的法线为 v = (x, y, z)
	* 法线的旋转可以这么计算
	* v_new = v * this->view_mtx_(去掉最后一行的平移分量,以及最后一列齐次坐标分量)
	*
	* 得到v_new之后,平面与顶点的距离可以这么计算, 取出平移矩阵,最后一行的平移分量,这个分量其实可以认为是原点移动后的坐标
	* 因为frustum初始的六个平面,在完成旋转后,与原点的距离仍然为零.
	* 我们把移动后的原点记做 o_new,它的坐标就是this->view_mtx_最后一行的前三个值,即平移分量
	* 并且我们知道 o_new一定是在移动后的平面上的
	* 将 o_new代入平面方程,即
	* dist = -(v_new * o_new) 就得到距离 (平面方程 ax + by + cz + dist = 0)
	* 这样,只要知道frustum旋转之前六个平面的法线,就可以根据 this->view_mtx_ 将旋转平面后的frustum六个平面计算出来
	* 
	* 可以立刻得出frustum初始的法线,可以从this->proj_mtx_中得到,我们以其中一个平面,例如left,它的法线应该是这样
	* v_n_left = (this->proj_mtx_.mv_[0][0] + this->proj_mtx_.mv_[3][0],
	*             this->proj_mtx_.mv_[0][1] + this->proj_mtx_.mv_[3][1],
	*             this->proj_mtx_.mv_[0][2] + this->proj_mtx_.mv_[3][2])
	* 我们设 M = this->view_mtx_.transpose() * this->proj_mtx_.transpose()  --- 取转置矩阵
	* 观察得到的M, v_n_left旋转后的向量,其实就是 M的第一行减第四行(取前三个值)
	* 同理 left/righ  top/bottom z_near/z_far的法线都可用相似的方法计算出来
	*
	* 然后是计算dist,仍然以left为例,前面已经说明
	* dist = -(v_new * o_new)
	* 我们取this->view_mtx_的左上解3x3的那一部分记录Vr
	* dist = -((v_n_left * Vr) * o_new)
	* 矩阵乘法满足结合律
	* dist = - (v_n_left * (Vr * o_new))
	* 观察得出 Vr * o_new 实际就是 M 的最后一行的前三个值,dist则为它们的组合相加(有正有负),proj_mtx_转置后,对应0的位置的值,都不要
	*
	* 以上做法,其实就是ogre与irrlicht计算frustum六个平面的方法
	* 用quake的方法计算似乎更容易让人看明白
	* 另外注意,法线转动的角度与平面转动的角度正好相差90度,所以使用转置矩阵正好可以正确计算法线的转动
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


	/* 归一化法线,修改dist */
	l3_f32 rleng;
	for(l3_int32 i = 0; i < sizeof(this->f_.plane_)/sizeof(this->f_.plane_[0]); i ++)
	{
		this->f_.plane_[i].normalise_normal(&rleng);
		this->f_.plane_[i].dist(this->f_.plane_[i].dist() * rleng);
	}

	this->fm_.dirty_ = 1;

	this->clear_frustum_dirty();
}

/* @brief 判断aab盒是否处于视截体范围内 */
camera::enum_cam_visible camera::is_visible(const aabbox& box)const
{
	enum_cam_visible ret = TOTAL_IN_CAM;
	/* 如果box位于6个平面之前,说明处于视截体当中
	* 如果位于某个平面之后,说明可以剔除
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

	/* 计算视截体的8个顶点 */
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


	/* 添加多边形 */
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
