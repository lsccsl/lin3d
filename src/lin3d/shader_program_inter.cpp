/**
* @file shader_program_inter.cpp
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
#include "shader_program_inter.h"
#include <stdlib.h>

namespace l3eng
{

std::string shader_program_inter::_l3eng_inter_test_program = "_l3eng_inter_test_program";
std::string shader_program_inter::_l3eng_inter_defer_render_gbuffer       = "_l3eng_inter_defer_render_gbuffer";
std::string shader_program_inter::_l3eng_inter_defer_render_light         = "_l3eng_inter_defer_render_light";
std::string shader_program_inter::_l3eng_inter_defer_render_light_mix     = "_l3eng_inter_defer_render_light_mix";
std::string shader_program_inter::_l3eng_inter_defer_render_light_vol_mix = "_l3eng_inter_defer_render_light_vol_mix";
std::string shader_program_inter::_l3eng_inter_defer_render_atmospheric   = "_l3eng_inter_defer_render_atmospheric";
std::string shader_program_inter::_l3eng_inter_defer_render_final         = "_l3eng_inter_defer_render_final";
std::string shader_program_inter::_l3eng_inter_shadowmap_cast = "_l3eng_inter_shadowmap_cast";
std::string shader_program_inter::_l3eng_inter_shadowmap_recv = "_l3eng_inter_shadowmap_recv";
std::string shader_program_inter::_l3eng_inter_cascaded_shadowmap_cast = "_l3eng_inter_cascaded_shadowmap_cast";
std::string shader_program_inter::_l3eng_inter_cascaded_shadowmap_recv = "_l3eng_inter_cascaded_shadowmap_recv";
std::string shader_program_inter::_l3eng_inter_ssao = "_l3eng_inter_ssao";
std::string shader_program_inter::_l3eng_inter_volume_light = "_l3eng_inter_volume_light";
std::string shader_program_inter::_l3eng_inter_volume_lightdir_extrude = "_l3eng_inter_volume_lightdir_extrude";
std::string shader_program_inter::_l3eng_inter_volume_light_radial_blur = "_l3eng_inter_volume_light_radial_blur";
std::string shader_program_inter::_l3eng_inter_atmospheric_ground = "_l3eng_inter_atmospheric_ground";
std::string shader_program_inter::_l3eng_inter_easy_hdr = "_l3eng_inter_easy_hdr";
std::string shader_program_inter::_l3eng_inter_hdr = "_l3eng_inter_hdr";
std::string shader_program_inter::_l3eng_inter_ssr = "_l3eng_inter_ssr";
std::string shader_program_inter::_l3eng_inter_reflect_forward = "_l3eng_inter_reflect_forward";
std::string shader_program_inter::_l3eng_inter_pbr_forward = "_l3eng_inter_pbr_forward";
std::string shader_program_inter::_l3eng_inter_dissolution = "_l3eng_inter_dissolution";


std::string shader_program_inter::_l3eng_inter_show_tex = "_l3eng_inter_show_tex";



const char * __test_vertex_program = "file:/shader/gl/test/test.vs";
const char * __test_fragment_program = "file:/shader/gl/test/test.fs";


const char * __common_fun_vs         = "file:/shader/gl/common/common_fun.vs";
const char * __common_fun_fs         = "file:/shader/gl/common/common_fun.fs";
const char * __common_def_shdr       = "file:/shader/gl/common/common_def.shdr";
const char * __common_screen_quad_vs = "file:/shader/gl/common/common_screen_quad.vs";


const char * __defer_render_gbuffer_vs = "file:/shader/gl/defer_render/defer_render_gbuffer.vs";
const char * __defer_render_gbuffer_fs = "file:/shader/gl/defer_render/defer_render_gbuffer.fs";

//const char * __defer_render_light_vs = "file:/shader/gl/defer_render/defer_render_light.vs";
const char * __defer_render_light_fs = "file:/shader/gl/defer_render/defer_render_light.fs";
const char * __test_defer_render_light_fs = "file:/shader/gl/test/test_defer_render_light.fs";


const char * __defer_render_light_mix_vs = "file:/shader/gl/defer_render/defer_render_light_mix.vs";
const char * __defer_render_light_mix_fs = "file:/shader/gl/defer_render/defer_render_light_mix.fs";
const char * __defer_render_light_vol_mix_fs = "file:/shader/gl/defer_render/defer_render_light_vol_mix.fs";
const char * __defer_render_atmospheric_vs = "file:/shader/gl/defer_render/defer_render_atmospheric.vs";
const char * __defer_render_atmospheric_fs = "file:/shader/gl/defer_render/defer_render_atmospheric.fs";

const char * __defer_render_final_vs = "file:/shader/gl/defer_render/defer_render_final.vs";
const char * __defer_render_final_fs = "file:/shader/gl/defer_render/defer_render_final.fs";
const char * __test_defer_render_final_fs = "file:/shader/gl/test/test_defer_render_final.fs";


//const char * __shadowmap_recv_vs = "file:/shader/gl/shadowmap/shadowmap_recv.vs";
const char * __shadowmap_recv_fs = "file:/shader/gl/shadowmap/shadowmap_recv.fs";

const char * __shadowmap_cast_vs = "file:/shader/gl/shadowmap/shadowmap_cast.vs";
const char * __shadowmap_cast_fs = "file:/shader/gl/shadowmap/shadowmap_cast.fs";


const char * __cascaded_shadowmap_cast_vs = "file:/shader/gl/shadowmap/csm/cascadedshadowmap_cast.vs";
const char * __cascaded_shadowmap_cast_fs = "file:/shader/gl/shadowmap/csm/cascadedshadowmap_cast.fs";
//const char * __cascaded_shadowmap_recv_vs = "file:/shader/gl/shadowmap/csm/cascadedshadowmap_recv.vs";
const char * __cascaded_shadowmap_recv_fs = "file:/shader/gl/shadowmap/csm/cascadedshadowmap_recv.fs";


const char * __ssao_vs = "file:/shader/gl/ssao/ssao.vs";
const char * __ssao_fs = "file:/shader/gl/ssao/ssao.fs";


const char * __vol_vs =  "file:/shader/gl/volume_light/vol.vs";
const char * __vol_fs =  "file:/shader/gl/volume_light/vol.fs";


const char * __vol_lightdir_extrude_vs = "file:/shader/gl/volume_light/vol_lightdir_vertex_extrude/vol_lightdir_vertex_extrude.vs";
const char * __vol_lightdir_extrude_fs = "file:/shader/gl/volume_light/vol_lightdir_vertex_extrude/vol_lightdir_vertex_extrude.fs";


const char * __vol_radial_blur_fs = "file:/shader/gl/volume_light/vol_radial_blur/vol_radial_blur.fs";


const char * __atmospheric_ground_vs = "file:/shader/gl/atmospheric/atmospheric_ground.vs";
const char * __atmospheric_ground_fs = "file:/shader/gl/atmospheric/atmospheric_ground.fs";


const char * __easy_hdr_fs = "file:/shader/gl/hdr/easy_hdr.fs";
const char * __hdr_fs = "file:/shader/gl/hdr/hdr.fs";


const char * __ssr_fs = "file:/shader/gl/ssr/ssr.fs";
//const char * __ssr_fs = "file:/shader/gl/ssr/ssr.fs_adapt_step";
//const char * __ssr_fs = "file:/shader/gl/ssr/ssr.fs_same_step";


const char * __reflect_forward_fs = "file:/shader/gl/reflect_forward/reflect_forward.fs";
const char * __reflect_forward_vs = "file:/shader/gl/reflect_forward/reflect_forward.vs";


const char * __pbr_forward_fs = "file:/shader/gl/pbr/pbr_forward.fs";
const char * __pbr_forward_vs = "file:/shader/gl/pbr/pbr_forward.vs";


const char * __dissolution_fs = "file:/shader/gl/dissolution/dissolution.fs";
const char * __dissolution_vs = "file:/shader/gl/dissolution/dissolution.vs";


const char * __show_tex_fs = "file:/shader/gl/show_tex/show_tex.fs";


shader_program_inter::shader_program_inter()
{}

shader_program_inter::~shader_program_inter()
{}

void shader_program_inter::init(l3_bool test_mode)
{
	// test
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_test_program;

		//vs
		gs.lst_vs_.push_back(__test_vertex_program);

		//fs
		gs.lst_fs_.push_back(__test_fragment_program);

		this->lst_shader_.push_back(gs);
	}


	// shadowmap cast
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_shadowmap_cast;

		//vs
		gs.lst_vs_.push_back(__shadowmap_cast_vs);

		//fs
		gs.lst_fs_.push_back(__shadowmap_cast_fs);

		this->lst_shader_.push_back(gs);
	}
	// shadowmap recv
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_shadowmap_recv;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs/*__shadowmap_recv_vs*/);

		//fs
		gs.lst_fs_.push_back(__shadowmap_recv_fs);

		this->lst_shader_.push_back(gs);
	}


	//cascade shadowmap cast
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_cascaded_shadowmap_cast;

		//vs
		gs.lst_vs_.push_back(__cascaded_shadowmap_cast_vs);

		//fs
		gs.lst_fs_.push_back(__cascaded_shadowmap_cast_fs);

		this->lst_shader_.push_back(gs);
	}
	//cascaded shadowmap recv
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_cascaded_shadowmap_recv;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__common_fun_fs);
		gs.lst_fs_.push_back(__cascaded_shadowmap_recv_fs);

		this->lst_shader_.push_back(gs);
	}


	// defer render gbuffer
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_defer_render_gbuffer;

		//vs
		gs.lst_vs_.push_back(__defer_render_gbuffer_vs);

		//fs
		gs.lst_fs_.push_back(__defer_render_gbuffer_fs);

		this->lst_shader_.push_back(gs);
	}

	// defer render light
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_defer_render_light;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__common_def_shdr);		

		if(test_mode)
			gs.lst_fs_.push_back(__test_defer_render_light_fs);
		else
			gs.lst_fs_.push_back(__defer_render_light_fs);

		this->lst_shader_.push_back(gs);
	}

	// defer render light mix
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_defer_render_light_mix;

		//vs
		gs.lst_vs_.push_back(__defer_render_light_mix_vs);

		//fs
		gs.lst_fs_.push_back(__defer_render_light_mix_fs);

		this->lst_shader_.push_back(gs);
	}

	// defer render light vol mix
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_defer_render_light_vol_mix;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__defer_render_light_vol_mix_fs);

		this->lst_shader_.push_back(gs);
	}

	// defer render atmospheric
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_defer_render_atmospheric;

		//vs
		gs.lst_vs_.push_back(__common_fun_vs);
		gs.lst_vs_.push_back(__defer_render_atmospheric_vs);

		//fs
		gs.lst_fs_.push_back(__common_fun_fs);
		gs.lst_fs_.push_back(__defer_render_atmospheric_fs);


		this->lst_shader_.push_back(gs);
	}

	// defer render final
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_defer_render_final;

		//vs
		gs.lst_vs_.push_back(__defer_render_final_vs);

		//fs
		if(test_mode)
			gs.lst_fs_.push_back(__test_defer_render_final_fs);
		else
			gs.lst_fs_.push_back(__defer_render_final_fs);

		this->lst_shader_.push_back(gs);
	}


	// ssao
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_ssao;

		//vs
		gs.lst_vs_.push_back(__ssao_vs);

		//fs
		gs.lst_fs_.push_back(__common_fun_fs);
		gs.lst_fs_.push_back(__ssao_fs);

		this->lst_shader_.push_back(gs);
	}


	// volume light
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_volume_light;

		//vs
		gs.lst_vs_.push_back(__common_fun_vs);
		gs.lst_vs_.push_back(__vol_vs);

		//fs
		gs.lst_fs_.push_back(__common_def_shdr);		
		gs.lst_fs_.push_back(__common_fun_fs);
		gs.lst_fs_.push_back(__vol_fs);

		this->lst_shader_.push_back(gs);
	}


	// volume light-dir extrude
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_volume_lightdir_extrude;

		//vs
		gs.lst_vs_.push_back(__vol_lightdir_extrude_vs);

		//fs
		gs.lst_fs_.push_back(__vol_lightdir_extrude_fs);

		this->lst_shader_.push_back(gs);
	}


	// volume light radial blur
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_volume_light_radial_blur;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__vol_radial_blur_fs);

		this->lst_shader_.push_back(gs);		
	}


	// atmospheric
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_atmospheric_ground;

		//vs
		gs.lst_vs_.push_back(__atmospheric_ground_vs);

		//fs
		gs.lst_fs_.push_back(__atmospheric_ground_fs);

		this->lst_shader_.push_back(gs);
	}

#if 0
	// easy hdr
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_easy_hdr;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__easy_hdr_fs);

		this->lst_shader_.push_back(gs);
	}
#endif

	// hdr
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_hdr;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__hdr_fs);

		this->lst_shader_.push_back(gs);
	}

	//ssr
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_ssr;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__common_fun_fs);
		gs.lst_fs_.push_back(__ssr_fs);

		this->lst_shader_.push_back(gs);
	}


	// show texture
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_show_tex;

		//vs
		gs.lst_vs_.push_back(__common_screen_quad_vs);

		//fs
		gs.lst_fs_.push_back(__show_tex_fs);

		this->lst_shader_.push_back(gs);
	}


	//reflect
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_reflect_forward;

		//vs
		gs.lst_vs_.push_back(__reflect_forward_vs);

		//fs
		gs.lst_fs_.push_back(__reflect_forward_fs);

		this->lst_shader_.push_back(gs);
	}

	// pbr
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_pbr_forward;

		//vs
		gs.lst_vs_.push_back(__pbr_forward_vs);

		//fs
		//gs.lst_fs_.push_back(__common_def_shdr);		
		gs.lst_fs_.push_back(__pbr_forward_fs);

		this->lst_shader_.push_back(gs);
	}

	//dissolution
	{
		glsl_shader gs;
		gs.shader_name_ = shader_program_inter::_l3eng_inter_dissolution;

		//vs
		gs.lst_vs_.push_back(__dissolution_vs);

		//fs
		//gs.lst_fs_.push_back(__common_def_shdr);		
		gs.lst_fs_.push_back(__dissolution_fs);

		this->lst_shader_.push_back(gs);
	}
}

}
