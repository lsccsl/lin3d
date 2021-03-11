/**
* @file robj_skeleton.cpp
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
#include "robj_skeleton.h"
#include "l3_log.h"
#include "texture_mgr.h"
#include "sence_mgr.h"
#include "l3_engine.h"

namespace l3eng{


const vertex_buf& robj_skeleton::get_vertex_buf()const
{
	return this->current_mesh_->vb();
}

const index_buf& robj_skeleton::get_index_buf()const
{
	return this->current_mesh_->ib();
}

void robj_skeleton::render()
{
	for(std::map<l3_int32, sub_mesh_info>::iterator it = this->map_msh_.begin();
		it != this->map_msh_.end(); it ++)
	{
		this->current_mesh_ = &(it->second.msh_skin_);
		this->mtrl_.set_tex_obj_id(it->second.tex_obj_);
		this->_render();
	}

#if 0
	for(std::map<std::string, x_joint_t *>::iterator it = this->map_joint_.begin(); it != this->map_joint_.end(); it ++)
	{
		this->_render(it->second);
	}
#endif
}

void robj_skeleton::_gen_animation(const std::list<x_animation_set>& lst_ani_set)
{
	for(std::list<x_animation_set>::const_iterator it = lst_ani_set.begin(); it != lst_ani_set.end(); it ++)
	{
		x_animation_t ani;

		for(std::list<x_animation>::const_iterator it_x = it->lst_ani_.begin(); it_x != it->lst_ani_.end(); it_x ++)
		{
			x_animation_joint_t ani_joint;

			ani_joint.joint_name_ = it_x->frame_name_;

			for(std::list<x_animation_key>::const_iterator it_k = it_x->lst_key_.begin(); it_k != it_x->lst_key_.end(); it_k ++)
			{
				switch(it_k->type_)
				{
				case x_animation_key::E_X_ANIMATION_ROTATION:
					{
						for(std::list<x_time_float_key_t>::const_iterator it_tk = it_k->lst_key_.begin();
							it_tk != it_k->lst_key_.end();
							it_tk ++)
						{
							quaternion q(it_tk->float_keys_[0],
								it_tk->float_keys_[1], it_tk->float_keys_[2], it_tk->float_keys_[3]);

							ani_joint.rotation_key_.push_back(q);
						}
					}
					break;

				case x_animation_key::E_X_ANIMATION_MOVE:
					{
						for(std::list<x_time_float_key_t>::const_iterator it_tk = it_k->lst_key_.begin();
							it_tk != it_k->lst_key_.end();
							it_tk ++)
						{
							vector3 v(it_tk->float_keys_[0], it_tk->float_keys_[1], it_tk->float_keys_[2]);

							ani_joint.pos_key_.push_back(v);
						}
					}
					break;
				
				case x_animation_key::E_X_ANIMATION_TRAN_MATRIX:
				case x_animation_key::E_X_ANIMATION_SCALE:
				default:
					break;
				}
			}

			ani.map_animation_joint_[it_x->frame_name_] = ani_joint;

		}

		this->map_animation_[it->name_] = ani;
	}
}

void robj_skeleton::_gen_skin(const x_mesh& x_msh, const MAP_IDX& map_i)
{
	x_skin_t s;
	for(std::list<x_skin_weight>::const_iterator it = x_msh.lst_sw_.begin();
		it != x_msh.lst_sw_.end(); it ++)
	{
		s.ref_joint_name_ = it->ref_frame_name_;

		x_skin_t::ver_idx_m vim;
		s.v_ver_idx_.resize(0);

		//要将顶点索引重定向相应的子mesh对应的顶点索引
		for(size_t i = 0; i < it->v_vertex_idx_.size(); i ++)
		{
			l3_int32 v_idx = it->v_vertex_idx_[i];
			for(MAP_IDX::const_iterator it_map = map_i.begin(); it_map != map_i.end(); it_map ++)
			{
				vim.m_idx_ = it_map->first;

				std::map<l3_int32, l3_int32>::const_iterator it_map1 = it_map->second.find(v_idx);
				if(it_map1 == it_map->second.end())
					continue;

				vim.v_idx_ = it_map1->second;

				s.v_ver_idx_.push_back(vim);
			}
		}

		s.global_inverse_mat_ = it->reserve_m_;

		this->v_skin_.push_back(s);
	}
}

void robj_skeleton::_gen_joint_tree(const std::map<std::string, frame_info_t>& map_frame)
{
	{
		x_joint_t * joint = NULL;
		for(std::map<std::string, frame_info_t>::const_iterator it = map_frame.begin(); it != map_frame.end(); it ++)
		{
			joint = new x_joint_t(this->sence_);

			joint->joint_name_ = it->first;
			joint->m_ = it->second.m_;
			joint->parent_joint_name_ = it->second.parent_;
			this->map_joint_[joint->joint_name_] = joint;
		}
	}

	{
		for(std::map<std::string, x_joint_t *>::iterator it = this->map_joint_.begin(); it != this->map_joint_.end(); it ++)
		{
			x_joint_t * j = it->second;
			if(j->parent_joint_name_.empty())
			{
				this->root_ = j;
				continue;
			}

			std::map<std::string, x_joint_t *>::iterator it_parent = this->map_joint_.find(j->parent_joint_name_);
			if(this->map_joint_.end() == it_parent)
			{
				L3_LOG_ENG_DEBUG(("joint:%s 's parent:%s not found",
					it->second->joint_name_.c_str(), it->second->parent_joint_name_.c_str()));
				continue;
			}
			x_joint_t * j_p = it_parent->second;

			j->parent_ = j_p;

			j_p->sub_joint_.push_back(j);
		}
	}

	{
		this->_gen_join_msh(this->root_, 255);
	}
}

l3_int32 robj_skeleton::load_mesh(const std::string& file_path)
{
	mesh_loader_x mloader;
	if(0 != mloader.load_mesh(file_path))
		return -1;

	const std::map<std::string, frame_info_t>& map_frame = mloader.map_frame();

	x_mesh * xmsh = NULL;

	for(std::map<std::string, frame_info_t>::const_iterator it = map_frame.begin();
		it != map_frame.end(); it ++)
	{
		if(it->second.xmsh_.face_.size())
		{
			xmsh = (x_mesh *)&(it->second.xmsh_);
			break;
		}
	}

	if(NULL == xmsh)
		return -1;

	//根据material list,将三角形分别拷贝直map_msh_
	//保存idx映射表
	MAP_IDX map_idx;
	for(size_t i = 0; i < xmsh->material_list_.v_material_list_.size(); i ++)
	{
		l3_int32 mtrl_idx = xmsh->material_list_.v_material_list_[i];

		std::map<l3_int32, l3_int32>& map_sub_idx = map_idx[mtrl_idx];

		x_face& face = xmsh->face_[i];

		//取出对应的三个点,生成顶点,以及对应的面索引
		if(face.idx1_ >= (l3_int32)(xmsh->vertex_.size()))
			continue;
		if(face.idx2_ >= (l3_int32)(xmsh->vertex_.size()))
			continue;
		if(face.idx3_ >= (l3_int32)(xmsh->vertex_.size()))
			continue;

		sub_mesh_info& sub_mesh = this->map_msh_[mtrl_idx];

		l3_int32 new_idx1 = -1;
		if(map_sub_idx.find(face.idx1_) != map_sub_idx.end())
		{
			//说明对应的顶点已经有了,直接用相应的索引
			new_idx1 = map_sub_idx[face.idx1_];
		}
		else
		{
			vector3& v1 = xmsh->vertex_[face.idx1_];
			vertex_texcoord_t& texcoord = xmsh->texcoord_.v_texcoord_[face.idx1_];

			sub_mesh.msh_org_.vb().add_vertex_pos(v1.x(), v1.y(), v1.z());
			sub_mesh.msh_org_.vb().add_texcoord0(texcoord);

			new_idx1 = sub_mesh.msh_org_.vb().get_vertex_count() - 1;

			map_sub_idx[face.idx1_] = new_idx1;
		}

		l3_int32 new_idx2 = -1;
		if(map_sub_idx.find(face.idx2_) != map_sub_idx.end())
		{
			new_idx2 = map_sub_idx[face.idx2_];
		}
		else
		{
			vector3& v2 = xmsh->vertex_[face.idx2_];
			vertex_texcoord_t& texcoord = xmsh->texcoord_.v_texcoord_[face.idx2_];

			sub_mesh.msh_org_.vb().add_vertex_pos(v2.x(), v2.y(), v2.z());
			sub_mesh.msh_org_.vb().add_texcoord0(texcoord);

			new_idx2 = sub_mesh.msh_org_.vb().get_vertex_count() - 1;

			map_sub_idx[face.idx2_] = new_idx2;
		}

		l3_int32 new_idx3 = -1;
		if(map_sub_idx.find(face.idx3_) != map_sub_idx.end())
		{
			new_idx3 = map_sub_idx[face.idx3_];
		}
		else
		{
			vector3& v3 = xmsh->vertex_[face.idx3_];
			vertex_texcoord_t& texcoord = xmsh->texcoord_.v_texcoord_[face.idx3_];

			sub_mesh.msh_org_.vb().add_vertex_pos(v3.x(), v3.y(), v3.z());
			sub_mesh.msh_org_.vb().add_texcoord0(texcoord);

			new_idx3 = sub_mesh.msh_org_.vb().get_vertex_count() - 1;

			map_sub_idx[face.idx3_] = new_idx3;
		}

		sub_mesh.msh_org_.ib().add_index_i32(new_idx1);
		sub_mesh.msh_org_.ib().add_index_i32(new_idx2);
		sub_mesh.msh_org_.ib().add_index_i32(new_idx3);
	}

	//备份所有的顶点数据,计算法线
	for(std::map<l3_int32, sub_mesh_info>::iterator it = this->map_msh_.begin();
		it != this->map_msh_.end(); it ++)
	{
		it->second.msh_skin_.deep_copy(it->second.msh_org_);

		it->second.msh_org_.cal_normal();
		it->second.msh_skin_.cal_normal();

		if(it->first < (l3_int32)(xmsh->material_list_.v_material_.size())){
			x_material& x_mtrl = xmsh->material_list_.v_material_[it->first];

			texture_base::ptr tex;
			this->sence_->eng()->tex_mgr()->load_tex(x_mtrl.tex_.filename_, tex);
			if(!tex.is_null())
				it->second.tex_obj_ = tex->obj_id();
			else
				L3_LOG_ENG_DEBUG(("fail load %s", x_mtrl.tex_.filename_.c_str()));
		}
	}

	this->_gen_joint_tree(map_frame);

	if(xmsh)
		this->_gen_skin(*xmsh, map_idx);

	this->_gen_animation(mloader.lst_ani_set());

	return 0;
}

void robj_skeleton::_gen_new_matrix(const quaternion * q1, const vector3 * v1,
	const quaternion * q2, const vector3 * v2, matrix4& m, l3_f32 t)
{
	matrix4 m_rotate;
	if(NULL == q2)
	{
		q1->get_matrix(m_rotate);
	}
	else
	{
		quaternion q_slerp;
		quaternion::slerp(*q1, *q2, q_slerp, t);
		q_slerp.get_matrix(m_rotate);
	}

	matrix4 m_move;
	matrix4::gen_move_matrix(m_move, v1->x(), v1->y(), v1->z());

	m_rotate.mult(m_move, m);
}

void robj_skeleton::animation(const std::string& name, l3_uint32 frame_num, l3_f32 t)
{
	std::map<std::string/* 动画名称 */, x_animation_t>::iterator it_ani =
		this->map_animation_.find(name);

	if(this->map_animation_.end() == it_ani)
		return;

	x_animation_t& ani = it_ani->second;

	for(std::map<std::string/* joint name */, x_animation_joint_t>::iterator it_ani_joint = ani.map_animation_joint_.begin();
		it_ani_joint != ani.map_animation_joint_.end(); it_ani_joint ++)
	{
		x_animation_joint_t& ani_joint = it_ani_joint->second;

		std::map<std::string, x_joint_t *>::iterator it_joint = this->map_joint_.find(ani_joint.joint_name_);
		if(this->map_joint_.begin() == it_joint)
			continue;

		x_joint_t * joint = it_joint->second;

		l3_uint32 max_key_num = (l3_uint32)ani_joint.pos_key_.size();
		if(max_key_num < ani_joint.rotation_key_.size())
			max_key_num = (l3_uint32)ani_joint.rotation_key_.size();

		frame_num = frame_num % max_key_num;

		quaternion * q1 = NULL;
		vector3 * v1 = NULL;

		quaternion * q2 = NULL;
		vector3 * v2 = NULL;

		if(frame_num < (ani_joint.pos_key_.size() - 1))
		{
			v1 = &ani_joint.pos_key_[frame_num];
			v2 = &ani_joint.pos_key_[frame_num + 1];
		}
		else
		{
			v1 = &ani_joint.pos_key_[ani_joint.pos_key_.size() - 1];
			v2 = NULL;
		}

		if(frame_num < (ani_joint.rotation_key_.size() - 1))
		{
			q1 = &ani_joint.rotation_key_[frame_num];
			q2 = &ani_joint.rotation_key_[frame_num + 1];
		}
		else
		{
			q1 = &ani_joint.rotation_key_[ani_joint.rotation_key_.size() - 1];
			q2 = NULL;
		}

		assert(q1 && v1);

		robj_skeleton::_gen_new_matrix(q1, v1, q2, v2, joint->m_, t);
	}

	this->_gen_join_msh(this->root_, 255);
	this->_re_cal_msh();
}

void robj_skeleton::_gen_join_msh(x_joint_t * joint, l3_uint8 clr_r)
{
	l3_uint8 r = clr_r;
	l3_uint8 g = 0/*clr_r*/;
	l3_uint8 b = 0/*clr_r*/;

	if(joint->parent_)
	{
		joint->cal_global_mat(joint->parent_->m_global_, r, g, b);
	}
	else
	{
		matrix4 m;
		joint->cal_global_mat(m, 255, 0, 0);
	}


	for(std::list<x_joint_t *>::iterator it = joint->sub_joint_.begin(); it != joint->sub_joint_.end(); it ++)
	{
		this->_gen_join_msh(*it, clr_r - 10);
	}
}

void robj_skeleton::_re_cal_msh()
{
	vector3 vpos_org;
	vector3 vpos_skin;

	for(l3_int32 i = 0; i < (l3_int32)(this->v_skin_.size()); i ++)
	{
		x_skin_t& skin = this->v_skin_[i];
		std::map<std::string, x_joint_t *>::iterator it_joint = this->map_joint_.find(skin.ref_joint_name_);
		if(this->map_joint_.end() == it_joint)
			continue;

		printf("%s:%d\r\n", skin.ref_joint_name_.c_str(), skin.v_ver_idx_.size());
		/* 根据m_global_计算每个点的变换,然后填入 */
		x_joint_t& joint = *(it_joint->second);

		for(l3_int32 j = 0; j < (l3_int32)(skin.v_ver_idx_.size()); j ++)
		{
			x_skin_t::ver_idx_m& vim = skin.v_ver_idx_[j];

			std::map<l3_int32, sub_mesh_info>::iterator it_map_mesh =
				this->map_msh_.find(vim.m_idx_);

			if(it_map_mesh == this->map_msh_.end())
				continue;

			vertex_buf& vb_org = it_map_mesh->second.msh_org_.vb();
			vertex_buf& vb_skin = it_map_mesh->second.msh_skin_.vb();

			if(0 != vb_org.get_vertex_pos(vim.v_idx_, vpos_org))
				continue;

			{
				matrix4 m;
				skin.global_inverse_mat_.mult(joint.m_global_, m);
				m.trans_vect3(vpos_org, vpos_skin);
			}

			vb_skin.set_vertex_pos(vim.v_idx_, vpos_skin.x(), vpos_skin.y(), vpos_skin.z());

#if 0
			device_buf_base::ptr& buf_org = it_map_mesh->second.msh_org_.vb().buf_pos();
			if(buf_org.is_null())
				continue;
			ant_f32 * vd_org = (ant_f32 *)buf_org->get_buf();
			if(NULL == vd_org)
				continue;

			device_buf_base::ptr& buf_skin = it_map_mesh->second.msh_skin_.vb().buf_pos();
			if(buf_skin.is_null())
				continue;
			l3_f32 * vd_skin = (ant_f32 *)buf_skin->get_buf();
			if(NULL == vd_skin)
				continue;

			vd_org += vim.v_idx_ * 3;
			vd_skin += vim.v_idx_ * 3;

			vector3 v_org(vd_org[0], vd_org[1], vd_org[2]);
			vector3 v_temp;
			vector3 v_new;

			{
				matrix4 m;
				skin.global_inverse_mat_.mult(joint.m_global_, m);
				m.trans_vect3(v_org, v_new);
			}

			vd_skin[0] = v_new.x();
			vd_skin[1] = v_new.y();
			vd_skin[2] = v_new.z();
#endif
		}
	}
}

}








