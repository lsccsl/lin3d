/**
* @file shader_cascaded_shadowmap_cast.h
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
#ifndef __L3ENG_SHADER_CASCADED_SHADOWMAP_CAST_H__
#define __L3ENG_SHADER_CASCADED_SHADOWMAP_CAST_H__

#include "shader.h"
#include "shader_program_inter.h"
#include "light_mgr.h"
#include "robj_geometry.h"
#include "robj_vector.h"
#include "l3_compile_config.h"
#include "shader_csm_common.h"


namespace l3eng{

class shader_cascaded_shadowmap_cast : public shader
{
public:

	typedef ref_ptr_thread<shader_cascaded_shadowmap_cast> ptr;
	OBJ_TYPE_DEFINE(shader, OBJ_SHADER_CASCADED_SHADOWMAP_CAST)

public:

	shader_cascaded_shadowmap_cast(l3_engine* eng);
	virtual ~shader_cascaded_shadowmap_cast(){}

	virtual void init() override;

	virtual void pre_frame(sence_mgr * sence) override;
	virtual void post_frame(sence_mgr * sence) override;
	virtual void prepare(robj_base& robj) override;

	virtual const std::string& shader_name() override
	{
		static std::string n(shader_program_inter::_l3eng_inter_cascaded_shadowmap_cast);
		return n;
	}

public:

	void set_light_info(OBJ_ID light_obj);

	bool next_cam_seg() {
		cur_cam_seg_++;
		if (cur_cam_seg_ >= outdoor_light_.v_cam_seg_.size())
		{
			cur_cam_seg_ = 0;
			return false;
		}
		return true;
	}

	void cal_cam_seg();

	OBJ_ID get_cam_seg_depthtex(const l3_int32 cam_seg);

	inline const l3_f32 offset_x() const{
		return this->offset_x_;
	}
	inline const l3_f32 offset_y() const{
		return this->offset_y_;
	}

	void get_csm_cam_seg(const l3_int32 idx_seg, csm_cam_seg_info& csm_cam_seg)const
	{
		if (idx_seg >= outdoor_light_.v_cam_seg_.size())
			return;

		csm_cam_seg = outdoor_light_.v_cam_seg_[idx_seg].csm_cam_seg;
	}

public:

#ifdef HAS_TEST_MODE
	void debug_geo_bbox(aabbox& bbox, robj_geometry& robj, l3_bool b);
	void debug_geo_pos(const vector3& pos, robj_geometry& robj, l3_bool b);
	void debug_vector(const vector3& v_start, const vector3& dir, const l3_f32 len, robj_vector& robj, l3_bool b);
	void debug_render();
	void debug_enble_gen_bbox() {
		this->debug_gen_bbox_ = L3_TRUE;
	}
	void debug_geo_cam_seg(const vector3& tmp_top_left,
		const vector3& tmp_top_right,
		const vector3& tmp_bottom_left,
		const vector3& tmp_bottom_right,
		const vector3& tmp1_top_left,
		const vector3& tmp1_top_right,
		const vector3& tmp1_bottom_left,
		const vector3& tmp1_bottom_right
	);
#endif

private:

	/* @brief 分辨率 */
	l3_f32 csm_shadow_solution_;
	l3_f32 offset_x_;
	l3_f32 offset_y_;

	/* @brief 每个分割视截体距离权重 */
	std::vector<l3_f32> v_seg_weight_;
	l3_int32 seg_weight_total_;

	/* @brief out door main light(sun) */
	struct outdoor_light_cam_seg
	{
		outdoor_light_cam_seg()
		{}

		csm_cam_seg_info csm_cam_seg;

		/* @brief rtt对象 */
		OBJ_ID rtt_csm_cast_;
	};
	struct outdoor_light
	{
		outdoor_light() {
			v_cam_seg_.resize(CSM_SEG_COUNT);
		}

		OBJ_ID sun_light_;

		std::vector<outdoor_light_cam_seg> v_cam_seg_;
	};
	outdoor_light outdoor_light_;
	l3_int32 cur_cam_seg_ = 0;

	l3_int32 uni_world_mtx_loc_; // uniform mat4
	l3_int32 uni_light_dep_range_loc_; //光源的depth range uniform float
	l3_int32 uni_light_view_mtx_loc_; //光源观察 uniform mat4
	l3_int32 uni_light_proj_mtx_loc_; //光源投影 uniform mat4

#ifdef HAS_TEST_MODE
	robj_geometry debug_geo_bbox_;
	robj_geometry debug_geo_lightspace_bbox_;
	robj_geometry debug_geo_campos_;
	robj_geometry debug_geo_campos_lightspace_;

	robj_geometry debug_geo_near_top_left_;
	robj_geometry debug_geo_near_top_right;
	robj_geometry debug_geo_near_bottom_left;
	robj_geometry debug_geo_near_bottom_right;
	robj_geometry debug_geo_far_top_left;
	robj_geometry debug_geo_far_top_right;
	robj_geometry debug_geo_far_bottom_left;
	robj_geometry debug_geo_far_bottom_right;

	//robj_geometry debug_geo_cam_seg_;
	robj_vector debug_vec_zfar_;
	l3_bool debug_gen_bbox_ = L3_FALSE;
#endif
};

}

#endif
