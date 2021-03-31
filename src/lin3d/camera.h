/**
* @file camera.h
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
#ifndef __L3ENG_CAMERA_H__
#define __L3ENG_CAMERA_H__

#include "base_obj.h"
#include "matrix4.h"
#include "plane.h"
#include "robj_geometry.h"
#include "aabb.h"
#include "l3_type.h"

namespace l3eng{

class ray;
class robj_geometry;
class sence_mgr;

/* @brief
* ��Щ�������ɵ���Դ�ļ�,��z���ʾ�߶�,����quake,����openglĬ�ϵĹ۲����ͬ
* (opengl��ԭ�㿴�� z=-1����,����quake���ļ���Ĭ�������,�������Ĵ�ֱ������)
* ��Ϊ��ʼ�Ĺ۲췽��ͬ,�ᵼ�²�ͬ��yaw pitch����,�˴���,����ֱ�۵�openglĬ�Ϸ�ʽ����yaw pitch
*/
class camera : public base_obj
{
public:

	enum{
		FRUSTUM_PLANE_FAR = 0,
		FRUSTUM_PLANE_NEAR,
		FRUSTUM_PLANE_LEFT,
		FRUSTUM_PLANE_RIGHT,
		FRUSTUM_PLANE_TOP,
		FRUSTUM_PLANE_BOTTOM,
		FRUSTUM_PLANE_COUNT,
	};

	enum CAMTYPE{
		CAMTYPE_none  = 0,
		CAMTYPE_pers  = 1,
		CAMTYPE_ortho = 2,
	};

public:

	typedef ref_ptr_thread<camera> ptr; 

	OBJ_TYPE_DEFINE(base_obj, OBJ_CAMERA)

public:

	inline camera(sence_mgr * sence):
		x_degree_(0),y_degree_(0),z_degree_(0),
		x_pos_(0),y_pos_(0),z_pos_(10),need_cal_view_mtx_(1),
		active_(1),
		fovy_(90.0f),aspect_(1.0f),z_near_(1.0f),z_far_(100.0f),
		fm_(sence),
		cam_type_(CAMTYPE_pers)
	{
		matrix4::gen_pers_proj_matrix(this->fovy_, this->aspect_, this->z_near_, this->z_far_, this->proj_mtx_);
	}

	virtual ~camera(){}

	camera& operator = (const camera& r);

public:

	inline const matrix4& view_mtx()const{ return this->view_mtx_; }

	inline const matrix4& proj_mtx()const{ return this->proj_mtx_; }

	void view_mtx(const matrix4& m)
	{
		this->view_mtx_ = m;
		this->clear_cam_view_mtx_dirty();
	}
	void proj_mtx(const matrix4& m)
	{
		this->proj_mtx_ = m;
	}


	/* @brief ������Ӱ����ת
	* opengl����,ӭ��x/y/z�������,��ʱ��Ϊ��,˳ʱ��Ϊ��,������
	* opengl��Ӱ��Ĭ��λ��0,0,0��,������ z=-1
	*/
	inline void set_rotate(const l3_f32 x_degree,
		const l3_f32 y_degree,
		const l3_f32 z_degree = 0)
	{
		this->x_degree_ = x_degree;
		this->y_degree_ = y_degree;
		this->z_degree_ = z_degree;

		this->set_cam_view_mtx_dirty();
	}

	/* @brief ������Ӱ��"��ǰ/�����ƶ�"(������Ӱ����ǰ�����ķ���x/y/z_degree,�Զ������x/y/z_pos)
	* ��ǰdistΪ��ֵ
	* ����distΪ��ֵ
	*/
	void move_toward(const l3_f32 dist);

	/* @brief ���� ��+ ��- */
	void move_sideward(const l3_f32 dist);

	/* @brief ��ֱ�ƶ� */
	void move_vertical(const l3_f32 dist);

	/* @brief ��Ӱ����λ */
	inline void reset()
	{
		this->x_degree_ = 0;
		this->y_degree_ = 0;
		this->z_degree_ = 0;
		this->x_pos_ = 0;
		this->y_pos_ = 0;
		this->z_pos_ = 0;

		this->set_cam_view_mtx_dirty();
	}

	inline void x_degree(l3_f32 x){ this->x_degree_ = x; this->set_cam_view_mtx_dirty(); }
	inline void y_degree(l3_f32 y){ this->y_degree_ = y; this->set_cam_view_mtx_dirty(); }
	inline void x_pos(l3_f32 x){ this->x_pos_ = x; this->set_cam_view_mtx_dirty(); }
	inline void y_pos(l3_f32 y){ this->y_pos_ = y; this->set_cam_view_mtx_dirty(); }
	inline void z_pos(l3_f32 z){ this->z_pos_ = z; this->set_cam_view_mtx_dirty(); }
	inline l3_f32 x_degree()const{ return this->x_degree_; }
	inline l3_f32 y_degree()const{ return this->y_degree_; }
	inline l3_f32 x_pos()const{ return this->x_pos_; }
	inline l3_f32 y_pos()const{ return this->y_pos_; }
	inline l3_f32 z_pos()const{ return this->z_pos_; }

	inline const l3_f32 fovy()const{ return this->fovy_; }
	inline const l3_f32 aspect()const{ return this->aspect_; }
	inline const l3_f32 z_near()const{ return this->z_near_; }
	inline const l3_f32 z_far()const{ return this->z_far_; }

	CAMTYPE cam_type() const{
		return cam_type_;
	}

	inline void set_pers_proj(l3_f32 fovy, l3_f32 aspect, l3_f32 z_near, l3_f32 z_far)
	{
		this->cam_type_ = CAMTYPE_pers;

		this->fovy_ = fovy;
		this->aspect_ = aspect;
		this->z_near_ = z_near;
		this->z_far_ = z_far;

		matrix4::gen_pers_proj_matrix(this->fovy_, this->aspect_, this->z_near_, this->z_far_, this->proj_mtx_);

		this->set_frustum_dirty();
	}

	inline void set_ortho_proj(l3_f32 view_width,
		l3_f32 view_height,
		l3_f32 z_near, l3_f32 z_far)
	{
		this->cam_type_ = CAMTYPE_ortho;

		this->view_width_ = view_width;
		this->view_height_ = view_height;

		matrix4::gen_ortho_proj_matrix(view_width,
			view_height,
			z_near, z_far,
			this->proj_mtx_);

		this->set_frustum_dirty();
	}

	/* 
	* @brief ����ʾ������,����һ������ڽ�ƽ�������,ͨ������ʰȡ
	*        ����ӿ�(һ������¾���window����)���Ϊ 500x500
	*        ��������õ��Ĵ�������Ϊ (400,400) Ӧ����Ϊ (08,0.8) (0.5,0.5)�ͱ�ʾ����Ӱ��ָ����Ļ���ĵ�һ������
	* @param x:ȡֵ(-1,1)
	* @param y:ȡֵ(-1,1)
	*/
	void get_ray_from_screen(const l3_f32 x_normalize, const l3_f32 y_normalize,
		ray& r);

	/* @brief ���¼���任���� */
	virtual void check_and_cal_mtx();
	inline void set_cam_view_mtx_dirty()
	{
		this->need_cal_view_mtx_ = 1;
		this->set_frustum_dirty();
	}
	inline void clear_cam_view_mtx_dirty()
	{
		this->need_cal_view_mtx_ = 0;
	}
	inline void set_frustum_dirty()
	{
		this->f_.need_re_cal_ = 1;
	}
	inline void clear_frustum_dirty()
	{
		this->f_.need_re_cal_ = 0;
	}

	/* @brief �����ӽ����6����Χƽ��(״��ȱ���Ľ�����) */
	void check_and_cal_frustum();

	/* @brief �ж�aab���Ƿ����ӽ��巶Χ�� */
	enum enum_cam_visible{
		OUT_OF_CAM = 0,
		TOTAL_IN_CAM = 1,
		PART_IN_CAM = 2,
	};
	enum_cam_visible is_visible(const aabbox& box)const;

	robj_geometry& get_fg();
	void get_fg_point(vector3& near_top_left,
		vector3& near_top_right,
		vector3& near_bottom_left,
		vector3& near_bottom_right,
		vector3& far_top_left,
		vector3& far_top_right,
		vector3& far_bottom_left,
		vector3& far_bottom_right);

	inline const l3_int32 active()const{ return this->active_; }
	inline void active(l3_int32 a){ this->active_ = a; }

	/* @brief ��ȡ��Ӱ������������ϵ�е���x y z */
	void get_cam_x(vector3& v)const;
	void get_cam_y(vector3& v)const;
	void get_cam_z(vector3& v)const;

protected:

	/* @brief ��Ӱ����ת��Ϣ */
	l3_f32 x_degree_;
	l3_f32 y_degree_;
	l3_f32 z_degree_;
	/* @brief todo ����������ת quaternion */

	/* @brief ��Ӱ��λ����Ϣ */
	l3_f32 x_pos_;
	l3_f32 y_pos_;
	l3_f32 z_pos_;

	/* @brief ͶӰ��Ϣ */
	l3_f32 fovy_;
	l3_f32 aspect_;
	l3_f32 z_near_;
	l3_f32 z_far_;

	/* @brief orthoʹ�� */
	l3_f32 view_width_;
	l3_f32 view_height_;

	/* @brief �ӽ����6��ƽ�� */
	struct frustum_t
	{
		frustum_t():need_re_cal_(1){}

		plane plane_[FRUSTUM_PLANE_COUNT];
		l3_int32 need_re_cal_;
	};
	frustum_t f_;
	struct frustum_mesh_t
	{
		frustum_mesh_t(sence_mgr * sence):dirty_(1), fg_(sence)
		{}

		l3_int32 dirty_;

		vector3 near_top_left;
		vector3 near_top_right;
		vector3 near_bottom_left;
		vector3 near_bottom_right;

		vector3 far_top_left;
		vector3 far_top_right;
		vector3 far_bottom_left;
		vector3 far_bottom_right;

		robj_geometry fg_;
	};
	frustum_mesh_t fm_;

	/* @brief �ӿ���Ϣ,��Ⱦ�豸��Ϣ,λ����Ϣ(�任����),�ӽ��� */
	matrix4 view_mtx_;
	/* @brief �Ƿ���Ҫ����任���� */
	l3_int32 need_cal_view_mtx_;

	/* @brief ͸��ͶӰ�任���� */
	matrix4 proj_mtx_;

	/* @brief �Ƿ�������Ӱ�� */
	l3_int32 active_;

	/* @brief ��Ӱ������ */
	CAMTYPE cam_type_;
};

}

#endif
