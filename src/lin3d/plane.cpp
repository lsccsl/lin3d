/**
* @file plane.h
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
#include "plane.h"
#include "matrix3.h"

namespace l3eng{

plane::plane(const vector3& p1, const vector3& p2, const vector3& p3)
{
	/* p1 p2 p3 按右手法则,逆时针绕序,得到法线
	* 按逆时针方式取向量进行叉乘
	*/

	vector3 v1 = p2 - p1;
	vector3 v2 = p1 - p3;

	v2.cross_product(v1, this->normal_);

	this->normal_.normalise();

	this->dist_ = p1.dot_product(this->normal_);
}

/* @brief 与另外一个平面的相交线 */
l3_int32 plane::intersection_with_plane(const plane& p, vector3& point, vector3& direction)const
{
	/* 已知两个平面方程 p_this, p
	* 以及另外一平面,即两个平面法线所构成的平面 p_c
	* 可得到三个三元一次方程组,解这个方程组,可以得到三个平面的交点
	* direction 就是两个法线的 cross product
	* 
	* 方程组如下: a b c是未知数
	* p_this.x * a + p_this.y * b + p_this.z * c = -dist_this
	* p.x *      a + p.y *      b + p.z *      c = -dist_p
	* p_c.x *    a + p_c.y *    b + p_c.z *    c = 0
	*
	* 不知道irrlicht是怎么处理,不像是用逆矩阵解方程组,里面用到了能量,与自相关,莱文森-德宾递推算法?
	*/

	this->normal_.cross_product(p.normal_, direction);

	/* 构造一个矩阵 */
	matrix3 m(direction.x(), direction.y(), direction.z(),
		this->normal_.x(), this->normal_.y(), this->normal_.z(),
		p.normal_.x(), p.normal_.y(), p.normal_.z());

	/* 得到逆矩阵,矩阵不是满秩的情况下,可能无解 */
	matrix3 im;
	if(0 != m.gen_inverse(im))
		return -1;

	/* 然后用向量(-dist_this, -dist_p, 0)与im相乘,即可得到方程组的解,也就是三个平面的交点 */
	vector3 vd(0, -this->dist_, -p.dist_);

	im.mult_vector(vd, point);

	return 0;
}

/* @brief 求平面与直线的交点 */
l3_int32 plane::intersection_with_line(const vector3& line_point,
	const vector3& line_dir,
	vector3& point_out)const
{
	/* 直线可以表示为 t为变量
	* x = p_x + d_x * t
	* y = p_y + d_y * t
	* z = p_z + d_z * t
	*
	* 另有平面方程
	* a*x + b*y + c*z + d = 0
	* 解出这个四元一次方程组即可得到平面与直线交点
	*/

	/* 判断直线是否与平面平行 */
	l3_f32 cp = line_dir.dot_product(this->normal_);
	if(l3_is_zero(cp))
		return -1;

	l3_f32 t = (-(line_point.dot_product(this->normal_) + this->dist_)) / cp;

	point_out.x(t * line_dir.x() + line_point.x());
	point_out.y(t * line_dir.y() + line_point.y());
	point_out.z(t * line_dir.z() + line_point.z());

	return 0;
}

/* @brief 求三个平面的交点 */
l3_int32 plane::intersection_with_2planes(const plane& p1, const plane& p2, vector3& out_point)const
{
	vector3 point;
	vector3 dir;
	if(0 != this->intersection_with_plane(p1, point, dir))
		return -1;

	if(0 != p2.intersection_with_line(point, dir, out_point))
		return -1;

	return 0;
}

/* @brief 求点在平面的前面还是后面 */
const plane::E_SIDE plane::point_on_plane_side(const vector3& point)const
{
	l3_f32 dist = point.dot_product(this->normal_);

	/* dist - this->dist_ 小于零,在平面之前,大于零在平面之后 */
	l3_f32 det = dist- this->dist_;

	if(l3_is_zero(det))
		return ON_PLANE;

	if(det > 0)
		return BACK_SIDE;
	else
		return FRONT_SIDE;
}

/* @brief 位平面的前面还是后面 */
const plane::E_SIDE plane::aabb_on_plane_side(const aabbox& box)const
{
	vector3 vc;
	vector3 vh;

	if(0 != box.get_center(vc))
		return ERR_SIDE;
	if(0 != box.get_half(vh))
		return ERR_SIDE;

	l3_f32 dist = vc.dot_product(this->normal_) + this->dist_;

	l3_f32 max_dist = vh.abs_dot_product(this->normal_);

	/* 实际上要判断8个顶点与平面的关系,通过简化处理,只计算两次点积,ogre irrlicht在这方面代码描述不同,但逻辑相似.*/

	if(dist - max_dist > 0)
		return FRONT_SIDE;

	if(dist + max_dist < 0)
		return BACK_SIDE;

	return BOTH_SIDE;
}

/* @brief 向量与平面是否相对 */
l3_bool plane::is_front_facing(const vector3& v)
{
	l3_f32 proj = this->normal_.dot_product(v);

	if(proj <= 0.0F)
		return L3_TRUE;

	return L3_FALSE;
}

/* @brief 获取点到平面的距离 */
l3_f32 plane::get_distance(const vector3& point)
{
	return this->normal_.dot_product(point) - this->dist_;
}

}
