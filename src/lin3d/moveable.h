/**
* @file moveable.h
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
#ifndef __L3ENG_MOVEABLE_H__
#define __L3ENG_MOVEABLE_H__

#include "l3_type.h"
#include "matrix4.h"
#include "aabb.h"
#include "sphere.h"
#include "l3_ref_ptr.h"

namespace l3eng{

class moveable_obj
{
public:

	typedef ref_ptr_thread<moveable_obj> ptr;

public:

	enum e_cal_mode_t{
		e_cal_mode_view = 1,
		e_cal_mode_world = 2,
		e_cal_mode_all = 0xffffffff,
	};

public:

	moveable_obj(const e_cal_mode_t e = e_cal_mode_all):
	  e_cal_mode_(e),
	  dirty_view_(L3_FALSE),
	  dirty_world_(L3_FALSE),
		x_degree_(0.0f),
		y_degree_(0.0f),
		z_degree_(0.0f),
		x_scale_(1.0f),
		y_scale_(1.0f),
		z_scale_(1.0f),
		bchange_(L3_FALSE)
	{}


	virtual void set_dirty(){
		this->dirty_view_ = L3_TRUE;
		this->dirty_world_ = L3_TRUE;
	}

	inline l3_bool get_dirty()const
	{
		if(this->e_cal_mode_ & e_cal_mode_view)
		{
			if(this->dirty_view_)
				return L3_TRUE;
		}

		if(this->e_cal_mode_ & e_cal_mode_world)
		{
			if(this->dirty_world_)
				return L3_TRUE;
		}

		return L3_FALSE;
	}


	inline void x_degree(l3_f32 x){ this->x_degree_ = x; this->set_dirty(); }
	inline void y_degree(l3_f32 y){ this->y_degree_ = y; this->set_dirty(); }
	inline void z_degree(l3_f32 z){ this->z_degree_ = z; this->set_dirty(); }
	inline void set_degree(l3_f32 x, l3_f32 y, l3_f32 z)
	{
		this->x_degree_ = x;
		this->y_degree_ = y;
		this->z_degree_ = z;
		this->set_dirty();
	}

	inline l3_f32 x_degree()const{ return this->x_degree_; }
	inline l3_f32 y_degree()const{ return this->y_degree_; }
	inline l3_f32 z_degree()const{ return this->z_degree_; }


	inline void x_pos(l3_f32 x){ this->v_pos_.x(x); this->set_dirty(); }
	inline void y_pos(l3_f32 y){ this->v_pos_.y(y); this->set_dirty(); }
	inline void z_pos(l3_f32 z){ this->v_pos_.z(z); this->set_dirty(); }
	inline void set_pos(l3_f32 x, l3_f32 y, l3_f32 z)
	{
		this->v_pos_.xyz(x, y, z);
		this->set_dirty();
	}

	inline l3_f32 x_pos()const{ return this->v_pos_.x(); }
	inline l3_f32 y_pos()const{ return this->v_pos_.x(); }
	inline l3_f32 z_pos()const{ return this->v_pos_.z(); }
	inline const vector3& v_pos()const{ return this->v_pos_; }


	inline const matrix4& mtx_view()const
	{
		this->_recal_mtx();
		return this->mtx_view_;
	}
	inline const matrix4& mtx_world()const
	{
		this->_recal_mtx();
		return this->mtx_world_;
	}


	/* @brief ��ת */
	void rotate(const l3_f32 x_degree, const l3_f32 y_degree, const l3_f32 z_degree);

	/* @brief ƽ�� */
	void move_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z);

	/* @brief ƽ����x y z */
	void move_to_xyz(const l3_f32 x, const l3_f32 y, const l3_f32 z);

	/* @brief ���� */
	void scale(const l3_f32 x_scale, const l3_f32 y_scale, const l3_f32 z_scale);


	/* @brief ��ȡ����ĳ���,������ */
	void dir_world_up(vector3& v) const;

	/* @brief ��ȡ����ĳ���,ǰ���� */
	void dir_world_toward(vector3& v) const;

	/* @brief ��ȡ����ĳ���,������ */
	void dir_world_left(vector3& v) const;


	inline const e_cal_mode_t e_cal_mode(){ return this->e_cal_mode_; }
	inline void e_cal_mode(e_cal_mode_t e){ this->e_cal_mode_ = e; }


	inline const l3_bool bchange()const
	{ return this->bchange_; }
	inline void bchange(l3_bool b)const
	{ this->bchange_ = b; }

	/* @brief ���¼���任���� */
	inline l3_bool recal_mtx()const
	{ return _recal_mtx(); }

protected:

	/* @brief ���¼���任���� */
	l3_bool _recal_mtx()const;

private:

	/* @brief ŷ����(��������������) */
	l3_f32 x_degree_;
	l3_f32 y_degree_;
	l3_f32 z_degree_;

	vector3 v_pos_;

	/* @brief ���� */
	l3_f32 x_scale_;
	l3_f32 y_scale_;
	l3_f32 z_scale_;

	/* @brief todo:����������ת(��Ԫ��) */
	//vector3 axis_;
	//ant_f32 angle_degreee_;

	e_cal_mode_t e_cal_mode_;

	/* @brief �۲���ģʽ����(��ƽ��,����ת,��������Ӱ��) */
	mutable l3_bool dirty_view_;
	mutable matrix4 mtx_view_;
	/* @brief ���۲�ģʽ(����ת,��ƽ��,�����ڷ�������) */
	mutable l3_bool dirty_world_;
	mutable matrix4 mtx_world_;

	mutable l3_bool bchange_;
};

class moveable_entity : public moveable_obj
{
public:

	typedef ref_ptr_thread<moveable_entity> ptr;

public:

	moveable_entity(const e_cal_mode_t e = e_cal_mode_all):
		moveable_obj(e)
	{}


	/* @brief ���±任����,���Χ��Ϣ */
	inline void update_all()const
	{
		this->_recal_mtx();
		this->_recal_bound_info();
	}


	/* @brief ����һ������ */
	inline void merge_local(const vector3& point){
		this->bi_.local_box_.merge(point);
		this->bi_.dirty_ = 1;
	}
	inline void merge_local(const l3_f32 x,
		const l3_f32 y,
		const l3_f32 z)
	{
		this->bi_.local_box_.merge(x, y, z);
		this->bi_.dirty_ = 1;
	}


	/* @brief ��ȡ����任 */
	inline const matrix4& get_world_matrix()const
	{ return this->mtx_world(); }

	/* @brief ��ȡ�����Χ�� */
	inline const aabbox& get_world_aabbox()const
	{
		this->_recal_bound_info();
		return this->bi_.world_box_;
	}

	/* @brief ��ȡ�ֲ���Χ�� */
	inline const aabbox& get_local_aabbox()const
	{ return this->bi_.local_box_; }

	/* @brief ��ȡ��ΧԲ */
	inline const sphere& get_world_sphere()const
	{
		this->_recal_bound_info();
		return this->bi_.world_sphere_;
	}

	/* ��ȡ����λ�� */
	inline const vector3& get_world_pos()const
	{
		return this->v_pos();
	}

protected:

	/* @brief ���¼����Χ��Ϣ */
	inline void _recal_bound_info()const
	{
		if(!this->bi_.dirty_ && !this->get_dirty())
			return;

		this->_recal_mtx();

		//�����Χ��
		{
			this->bi_.local_box_.transform(this->mtx_world(), this->bi_.world_box_);
		}

		//�����Χ��
		{
			vector3 v_half;
			this->bi_.local_box_.get_half(v_half);
			this->bi_.local_box_.get_center(this->bi_.local_sphere_.center());

			this->bi_.local_sphere_.radius() = v_half.get_length();
			this->bi_.world_sphere_.radius() = this->bi_.local_sphere_.radius();

			this->mtx_world().trans_vect3(this->bi_.local_sphere_.center(), this->bi_.world_sphere_.center());
		}

		this->bi_.dirty_ = L3_FALSE;
	}

	virtual void set_dirty(){
		moveable_obj::set_dirty();
		this->bi_.dirty_ = L3_TRUE;
	}

private:

	struct bound_info
	{
		bound_info():dirty_(1){}

		/* @brief �ھֲ�����ϵ�еİ�Χ�� */
		aabbox local_box_;
		mutable l3_bool dirty_;

		/* @brief ��local_box_��ת��,�õ��µİ�Χ�� */
		mutable aabbox world_box_;
		mutable sphere world_sphere_;
		mutable sphere local_sphere_;
	};
	mutable bound_info bi_;
};

}

#endif











