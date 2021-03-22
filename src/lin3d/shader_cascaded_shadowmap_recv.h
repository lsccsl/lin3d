/**
* @file shader_cascaded_shadowmap_recv.h
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
#ifndef __L3ENG_SHADER_CASCADED_SHADOWMAP_RECV_H__
#define __L3ENG_SHADER_CASCADED_SHADOWMAP_RECV_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "vector2.h"
#include "shader_csm_common.h"
#include "shader_screen_quad.h"


namespace l3eng {

class shader_cascaded_shadowmap_cast;

class shader_cascaded_shadowmap_recv : public shader
{
public:

	typedef ref_ptr_thread<shader_cascaded_shadowmap_recv> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_CASCADED_SHADOWMAP_RECV)

public:

	shader_cascaded_shadowmap_recv(l3_engine* eng) :shader(eng)
		, sample_loop_(7)
	{}
	virtual ~shader_cascaded_shadowmap_recv() {}

	virtual void init() override;

	virtual void pre_frame(sence_mgr* sence) override;
	virtual void post_frame(sence_mgr* sence) override;
	virtual void prepare(robj_base& robj) override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_cascaded_shadowmap_recv);
		return n;
	}

public:

	void render_screen_quad(sence_mgr* sence);

	inline void set_sence_dep_tex(OBJ_ID tex_sence_dep) {
		this->tex_sence_dep_ = tex_sence_dep;
	}
	inline void set_sence_light(OBJ_ID tex) {
		tex_sence_light_ = tex;
	}

	void get_csm_cast_info(const shader_cascaded_shadowmap_cast& shdr_cast);

	OBJ_ID tex_light_shadow() {
		return this->tex_light_shadow_;
	}

private:

	/* @brief �ֱ��� */
	l3_f32 offset_x_;
	l3_f32 offset_y_;

	struct ourdoor_light_recv
	{
		ourdoor_light_recv() {
			v_cam_seg_.resize(2);
		}

		std::vector<csm_cam_seg_info> v_cam_seg_;
	};
	ourdoor_light_recv ourdoor_light_recv_;

	/* @brief ������չ�����Ӱ */
	OBJ_ID rtt_light_shadow_;
	OBJ_ID tex_light_shadow_;

	/* @brief ���볡����� */
	OBJ_ID tex_sence_dep_;
	OBJ_ID tex_sence_light_;

	l3_uint32 sample_loop_;

	l3_int32 uni_tex_light_loc_; //�������� uniform sampler2D
	l3_int32 uni_tex_view_depth_loc_; //��Ӱ���ռ��������

	l3_int32 uni_view_inv_mtx_loc_; //�۲������
	l3_int32 uni_tan_half_fov_loc_; //�ӽ�һ���tanֵ
	l3_int32 uni_aspect_loc_; //��Ļ��߱�
	l3_int32 uni_dep_range_loc_; //������

	//uniform sampler2D uni_tex_light_depth_tex; //��Դ�ռ����
	l3_int32 uni_tex_light_depth_tex1_loc_;
	l3_int32 uni_tex_light_depth_tex2_loc_;
	l3_int32 uni_tex_light_depth_shadow1_loc_; //ͬһ������,���
	l3_int32 uni_tex_light_depth_shadow2_loc_; //ͬһ������,���

	l3_int32 uni_far_thredhold_loc_;

	l3_int32 uni_light_dep_range_loc_; //��Դ��depth range
	l3_int32 uni_light_view_mtx_loc_; //��Դ�۲�
	l3_int32 uni_light_proj_mtx_loc_; //��ԴͶӰ
	l3_int32 uni_light_dep_pix_offset_loc_; //��Դ�������ֱ���
	l3_int32 uni_loop_loc_; //uniform int

	shader_screen_quad screen_quad_;
};

}

#endif
