/**
* @file robj_skeleton.h
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
#ifndef __ANTISPACE_ROBJ_SKELETON_H__
#define __ANTISPACE_ROBJ_SKELETON_H__

#include "render_obj.h"
#include "mesh.h"
#include "mesh_loader_x.h"
#include "quaternion.h"

namespace l3eng{

struct x_joint_t : public render_obj
{
	x_joint_t(sence_mgr * sence):render_obj(sence),parent_(NULL){
		this->msh_.ib().idx_type(E_ITYPE_LINES);
	}

	virtual const vertex_buf& get_vertex_buf()const{
		return this->msh_.vb();
	}

	virtual const index_buf& get_index_buf()const{
		return this->msh_.ib();
	}

	void cal_global_mat(const matrix4& mat, l3_uint8 r, l3_uint8 g, l3_uint8 b)
	{
		vector3 p;
		this->m_.mult(mat, this->m_global_);
		this->m_global_.mult_v3(p, this->p_);

		{
			//device_buf_base::ptr buf_ptr(new device_common_buf);

			//this->msh_.vb().buf_pos(buf_ptr);

			//buf_ptr->resize_buf_sz(2 * 12);
			//l3_f32 * point = (l3_f32 *)buf_ptr->get_buf();
			vertex_buf& vb = this->msh_.vb();

			vb.add_vertex_pos(this->p_.x(), this->p_.y(), this->p_.z());
			//point[0] = this->p_.x();
			//point[1] = this->p_.y();
			//point[2] = this->p_.z();

			if(this->parent_)
			{
				vb.add_vertex_pos(this->parent_->p_.x(), this->parent_->p_.y(), this->parent_->p_.z());
				//point[3] = this->parent_->p_.x();
				//point[4] = this->parent_->p_.y();
				//point[5] = this->parent_->p_.z();
			}
			else
			{
				vb.add_vertex_pos(0, 0, 0);
				//point[3] = 0;
				//point[4] = 0;
				//point[5] = 0;
			}
		}

		{
			//device_buf_base::ptr color_ptr(new device_common_buf);

			//this->msh_.vb().buf_color(color_ptr);

			//color_ptr->resize_buf_sz(4 * 2);
			//l3_uint8 * color = (l3_uint8 *)color_ptr->get_buf();
			vertex_buf& vb = this->msh_.vb();

			vb.add_vertex_clr(r, g, b, 0);
			//color[0] = r;
			//color[1] = g;
			//color[2] = b;
			//color[3] = 0;

			vb.add_vertex_clr(r, g, b, 0);
			//color[4] = r;
			//color[5] = g;
			//color[6] = b;
			//color[7] = 0;
		}

		{
			{
				//device_buf_base::ptr idx_ptr(new device_common_buf);

				//idx_ptr->resize_buf_sz(sizeof(l3_int32) * 2);
				//l3_int32 * index = (l3_int32 *)(idx_ptr->get_buf());
				index_buf& ib = this->msh_.ib();

				ib.add_index_i32(0);
				ib.add_index_i32(1);
				//index[0] = 0;
				//index[1] = 1;

				//this->msh_.ib().set_buf(idx_ptr);
			}
		}
	}

	/* @brief 连结点的名字 */
	std::string joint_name_;

	/* @brief 父节点 */
	x_joint_t * parent_;
	std::string parent_joint_name_;

	std::list<x_joint_t *> sub_joint_;

	/* @brief 相对父节点的变换矩阵 */
	matrix4 m_;
	matrix4 m_global_;

	mesh msh_;

	vector3 p_;
};

struct x_skin_t
{
	struct ver_idx_m
	{
		l3_int32 m_idx_;//哪个material list
		l3_int32 v_idx_;//对应的顶点索引
	};

	/* @brief 依附于哪个连结点 */
	std::string ref_joint_name_;

	/* @brief 包含的点的索引 */
	std::vector<ver_idx_m> v_ver_idx_;

	/* @brief 用于转换至父节点的空间矩阵 */
	matrix4 global_inverse_mat_;
};

struct x_animation_joint_t
{
	std::string joint_name_;
	std::vector<quaternion> rotation_key_;
	std::vector<vector3> pos_key_;
};

struct x_animation_t
{
	std::map<std::string/* joint name */, x_animation_joint_t> map_animation_joint_; 
};

class robj_skeleton : public render_obj
{
public:

	typedef ref_ptr_thread<robj_skeleton> ptr;

	OBJ_TYPE_DEFINE(render_obj, OBJ_ROBJ_SKELETON)

	typedef std::map<l3_int32/* material list */, std::map<l3_int32/* idx from file */, l3_int32/* sub idx */> > MAP_IDX;

public:

	robj_skeleton(sence_mgr * sence):
		render_obj(sence),
		root_(NULL),
		current_mesh_(NULL)
	{}

	virtual ~robj_skeleton(){}

public:

	virtual void render();

	virtual const vertex_buf& get_vertex_buf()const;

	virtual const index_buf& get_index_buf()const;

	virtual void dump(std::string& info){}

	virtual void animation(const std::string& name, l3_uint32 frame_num, l3_f32 t);

public:

	l3_int32 load_mesh(const std::string& file_path);

private:

	void _gen_joint_tree(const std::map<std::string, frame_info_t>& map_frame);
	void _gen_skin(const x_mesh& x_msh, const MAP_IDX& map_i);
	void _gen_animation(const std::list<x_animation_set>& lst_ani_set);
	void _gen_join_msh(x_joint_t * joint, l3_uint8 clr_r);

	void _re_cal_msh();

	static void _gen_new_matrix(const quaternion * q1, const vector3 * v1,
		const quaternion * q2, const vector3 * v2, matrix4& m, l3_f32 t);

private:

	/* @brief 根节点 */
	x_joint_t * root_;

	/* @brief 节点字典 */
	std::map<std::string, x_joint_t *> map_joint_;

	/* @brief 各顶点所附着的骨骼节点数据(顶点索引数组,与对应的骨骼节点joint名称) */
	std::vector<x_skin_t> v_skin_;

	/* @brief 原始顶点数据 */
	mesh * current_mesh_;
	struct sub_mesh_info
	{
		mesh msh_org_;
		mesh msh_skin_;
		OBJ_ID tex_obj_;
		//std::string tex_;
	};
	std::map<l3_int32, sub_mesh_info> map_msh_;

	/* @brief 动画关键帧数据 */
	std::map<std::string/* 动画名称 */, x_animation_t> map_animation_;
};

}

#endif
