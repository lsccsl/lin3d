/**
* @file shader_program_inter.h
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
//#programonce

#ifndef __L3ENG_SHADER_PROGRAM_INTER_H__
#define __L3ENG_SHADER_PROGRAM_INTER_H__

#include <list>
#include <string>
#include "l3_type.h"

namespace l3eng
{

struct glsl_shader
{
	std::string shader_name_;

	std::list<std::string> lst_vs_;
	std::list<std::string> lst_fs_;
	std::list<std::string> lst_tcs_;
	std::list<std::string> lst_tes_;
};

class shader_program_inter
{
public:

	static std::string _l3eng_inter_test_program;
	static std::string _l3eng_inter_defer_render_gbuffer;
	static std::string _l3eng_inter_defer_render_light;
	static std::string _l3eng_inter_defer_render_light_mix;
	static std::string _l3eng_inter_defer_render_light_vol_mix;
	static std::string _l3eng_inter_defer_render_atmospheric;
	static std::string _l3eng_inter_defer_render_final;
	static std::string _l3eng_inter_shadowmap_cast;
	static std::string _l3eng_inter_shadowmap_recv;
	static std::string _l3eng_inter_cascaded_shadowmap_cast;
	static std::string _l3eng_inter_cascaded_shadowmap_recv;
	static std::string _l3eng_inter_ssao;
	static std::string _l3eng_inter_volume_light;
	static std::string _l3eng_inter_volume_lightdir_extrude;
	static std::string _l3eng_inter_volume_light_radial_blur;
	static std::string _l3eng_inter_atmospheric_ground;
	static std::string _l3eng_inter_easy_hdr;
	static std::string _l3eng_inter_hdr;
	static std::string _l3eng_inter_ssr;
	static std::string _l3eng_inter_reflect_forward;
	static std::string _l3eng_inter_pbr_forward;
	static std::string _l3eng_inter_dissolution;

	static std::string _l3eng_inter_show_tex;


	shader_program_inter();
	~shader_program_inter();

	void init(l3_bool test_mode);

	const std::list<glsl_shader>& lst_shader(){ return this->lst_shader_; }

private:

	std::list<glsl_shader> lst_shader_;
};

}

#endif








