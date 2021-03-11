/**
* @file shader_program_mgr.cpp
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
#include "shader_program_mgr.h"
#include "shader_program_inter.h"
#include "l3_log.h"
#include "l3_engine.h"
#include "config.h"
#include "base_obj_mgr.h"
#include "shader_test.h"
#include "shader_defer_render.h"
#include "shader_defer_render_gbuffer.h"
#include "shader_defer_render_light.h"
#include "shader_defer_render_light_mix.h"
#include "shader_defer_render_final.h"
#include "shader_shadowmap.h"
#include "shader_shadowmap_cast.h"
#include "shader_shadowmap_recv.h"
#include "shader_ssao.h"
#include "shader_vol.h"
#include "shader_atmospheric_ground.h"
#include "shader_defer_render_atmospheric.h"
#include "shader_defer_render_light_vol_mix.h"
#include "shader_show_tex.h"
#include "shader_easy_hdr.h"
#include "shader_hdr.h"
#include "shader_ssr.h"
#include "shader_reflect_forward.h"
#include "shader_vol_lightdir_extrude.h"
#include "shader_vol_radial_blur.h"
#include "shader_pbr_fwd.h"
#include "shader_dissolution.h"
#include "shader_cascaded_shadowmap_cast.h"
#include "shader_cascaded_shadowmap_recv.h"

namespace l3eng{

l3_int32 shader_program_mgr::init(l3_bool test_mode)
{
	this->_init_inter_program(test_mode);
	this->_init_shader();

	return 0;
}

void shader_program_mgr::_get_shader_program_str(const std::string& input, std::string& output)
{
	output = input;
	if(input.size() < 5)
		return;

	if(0 != input.find_first_of("file:"))
		return;

	std::string file_path = input.substr(5);

	std::string c;
	if(0 != this->eng_->cfg()->read_file(c, file_path))
		return;

	output = c;
}

l3_int32 shader_program_mgr::_init_inter_program(l3_bool test_mode)
{
	shader_program_inter shader_inter;
	shader_inter.init(test_mode);
	const std::list<glsl_shader>& lst_shader = shader_inter.lst_shader();

	for(std::list<glsl_shader>::const_iterator it = lst_shader.begin(); it != lst_shader.end(); it ++)
	{
		const glsl_shader& glsl_shdr = *it;

		L3_LOG_ENG_DEBUG(("%s====================================\r\n", glsl_shdr.shader_name_.c_str()));

		shader_program::ptr s_prog = this->eng_->obj_mgr()->create_obj<shader_program, l3_engine *, const std::string&>(this->eng_, glsl_shdr.shader_name_);
		s_prog->init();

		if(!glsl_shdr.lst_vs_.empty())
		{
			std::string str_shader;
			for(std::list<std::string>::const_iterator it = glsl_shdr.lst_vs_.begin(); it != glsl_shdr.lst_vs_.end(); it ++)
			{
				L3_LOG_ENG_DEBUG(("vertex shader:%s", it->c_str()));
				std::string output;
				this->_get_shader_program_str(*it, output);

				str_shader.append(output);
				str_shader.append("\r\n");
			}

			char acfile[260] = {0};
			::_snprintf_s(acfile, sizeof(acfile) - 1, "%s/%s.vs", L3_LOG_BASE(), glsl_shdr.shader_name_.c_str());
			FILE * pfile = NULL;
			fopen_s(&pfile, acfile, "wb");
			if(pfile)
			{
				fprintf(pfile, str_shader.c_str());
				fclose(pfile);
			}

			shader_source::ptr vs_obj(new shader_source(SHADER_SOURCE_TYPE_VERTEX));
			vs_obj->load_from_string(str_shader.c_str(), glsl_shdr.shader_name_);
			s_prog->attach(vs_obj);
		}

		if(!glsl_shdr.lst_fs_.empty())
		{
			std::string str_shader;
			for(std::list<std::string>::const_iterator it = glsl_shdr.lst_fs_.begin(); it != glsl_shdr.lst_fs_.end(); it ++)
			{
				L3_LOG_ENG_DEBUG(("fragment shader:%s", it->c_str()));
				std::string output;
				this->_get_shader_program_str(*it, output);

				str_shader.append(output);
				str_shader.append("\r\n");
			}

			char acfile[260] = {0};
			::_snprintf_s(acfile, sizeof(acfile) - 1, "%s/%s.fs", L3_LOG_BASE(), glsl_shdr.shader_name_.c_str());
			FILE * pfile = NULL;
			fopen_s(&pfile, acfile, "wb");
			if(pfile)
			{
				fprintf(pfile, str_shader.c_str());
				fclose(pfile);
			}

			shader_source::ptr fs_obj(new shader_source(SHADER_SOURCE_TYPE_FRAGMENT));
			fs_obj->load_from_string(str_shader.c_str(), glsl_shdr.shader_name_);
			s_prog->attach(fs_obj);
		}

		if(!glsl_shdr.lst_tcs_.empty())
		{
			std::string str_shader;
			for(std::list<std::string>::const_iterator it = glsl_shdr.lst_tcs_.begin(); it != glsl_shdr.lst_tcs_.end(); it ++)
			{
				L3_LOG_ENG_DEBUG(("tess control shader:%s", it->c_str()));
				std::string output;
				this->_get_shader_program_str(*it, output);

				str_shader.append(output);
				str_shader.append("\r\n");
			}

			char acfile[260] = {0};
			::_snprintf_s(acfile, sizeof(acfile) - 1, "%s/%s.tcs", L3_LOG_BASE(), glsl_shdr.shader_name_.c_str());
			FILE * pfile = NULL;
			fopen_s(&pfile, acfile, "wb");
			if(pfile)
			{
				fprintf(pfile, str_shader.c_str());
				fclose(pfile);
			}

			shader_source::ptr tcs_obj(new shader_source(SHADER_SOURCE_TYPE_TESS_CONTRL));
			tcs_obj->load_from_string(str_shader.c_str(), glsl_shdr.shader_name_);
			s_prog->attach(tcs_obj);
		}

		if(!glsl_shdr.lst_tes_.empty())
		{
			std::string str_shader;
			for(std::list<std::string>::const_iterator it = glsl_shdr.lst_tes_.begin(); it != glsl_shdr.lst_tes_.end(); it ++)
			{
				L3_LOG_ENG_DEBUG(("tess eval shader:%s", it->c_str()));
				std::string output;
				this->_get_shader_program_str(*it, output);

				str_shader.append(output);
				str_shader.append("\r\n");
			}

			char acfile[260] = {0};
			::_snprintf_s(acfile, sizeof(acfile) - 1, "%s/%s.tes", L3_LOG_BASE(), glsl_shdr.shader_name_.c_str());
			FILE * pfile = NULL;
			fopen_s(&pfile, acfile, "wb");
			if(pfile)
			{
				fprintf(pfile, str_shader.c_str());
				fclose(pfile);
			}

			shader_source::ptr tes_obj(new shader_source(SHADER_SOURCE_TYPE_TESS_EVAL));
			tes_obj->load_from_string(str_shader.c_str(), glsl_shdr.shader_name_);
			s_prog->attach(tes_obj);
		}

		s_prog->link();

		map_name_shader_program_[glsl_shdr.shader_name_] = s_prog->obj_id();

		map_shader_program_[s_prog->obj_id()] = s_prog;

		L3_LOG_ENG_DEBUG(("%s end====================================\r\n\r\n\r\n\r\n", glsl_shdr.shader_name_.c_str()));
	}

	return 0;
}

l3_int32 shader_program_mgr::_init_shader()
{
	// test
	{
		shader_test::ptr s = this->eng_->obj_mgr()->create_obj<shader_test, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}


	// show texture
	{
		shader_show_tex::ptr s = this->eng_->obj_mgr()->create_obj<shader_show_tex, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();			
	}


	// defer rendering gbuffer
	{
		shader_defer_render_gbuffer::ptr s = this->eng_->obj_mgr()->create_obj<shader_defer_render_gbuffer, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// defer rendering light
	{
		shader_defer_render_light::ptr s = this->eng_->obj_mgr()->create_obj<shader_defer_render_light, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// defer rendering light mix
	{
		shader_defer_render_light_mix::ptr s = this->eng_->obj_mgr()->create_obj<shader_defer_render_light_mix, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// defer rendering light vol mix
	{
		shader_defer_render_light_vol_mix::ptr s = this->eng_->obj_mgr()->create_obj<shader_defer_render_light_vol_mix, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// defer rendering atmospheric
	{
		shader_defer_render_atmospheric::ptr s = this->eng_->obj_mgr()->create_obj<shader_defer_render_atmospheric, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();		
	}

	// defer rendering final
	{
		shader_defer_render_final::ptr s = this->eng_->obj_mgr()->create_obj<shader_defer_render_final, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}


	// shadowmap cast
	{
		shader_shadowmap_cast::ptr s = this->eng_->obj_mgr()->create_obj<shader_shadowmap_cast, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// shadowmap recv
	{
		shader_shadowmap_recv::ptr s = this->eng_->obj_mgr()->create_obj<shader_shadowmap_recv, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}


	// cascaded shadowmap cast
	{
		shader_cascaded_shadowmap_cast::ptr s = this->eng_->obj_mgr()->create_obj<shader_cascaded_shadowmap_cast, l3_engine*>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if (!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}
	// cascaded shadowmap recv
	{
		shader_cascaded_shadowmap_recv::ptr s = this->eng_->obj_mgr()->create_obj<shader_cascaded_shadowmap_recv, l3_engine*>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if (!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();		
	}


	// ssao
	{
		shader_ssao::ptr s = this->eng_->obj_mgr()->create_obj<shader_ssao, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}


	// volume light
	{
		shader_vol::ptr s = this->eng_->obj_mgr()->create_obj<shader_vol, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// volume light-dir extrude
	{
		shader_vol_lightdir_extrude::ptr s = this->eng_->obj_mgr()->create_obj<shader_vol_lightdir_extrude, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}

	// volume light - radial blur
	{
		shader_vol_radial_blur::ptr s = this->eng_->obj_mgr()->create_obj<shader_vol_radial_blur, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
			s->set_shader_program(shrd_prg);
		s->init();
	}


	// atmospheric ground
	{
		shader_atmospheric_ground::ptr s = this->eng_->obj_mgr()->create_obj<shader_atmospheric_ground, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}


#if 0
	// easy hdr
	{
		shader_easy_hdr::ptr s = this->eng_->obj_mgr()->create_obj<shader_easy_hdr, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}
#endif

	// hdr
	{
		shader_hdr::ptr s = this->eng_->obj_mgr()->create_obj<shader_hdr, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}

	// ssr
	{
		shader_ssr::ptr s = this->eng_->obj_mgr()->create_obj<shader_ssr, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}

	// water reflect
	{
		shader_reflect_forward::ptr s = this->eng_->obj_mgr()->create_obj<shader_reflect_forward, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}

	// pbr
	{
		shader_pbr_fwd::ptr s = this->eng_->obj_mgr()->create_obj<shader_pbr_fwd, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}

	// dissolution
	{
		shader_dissolution::ptr s = this->eng_->obj_mgr()->create_obj<shader_dissolution, l3_engine *>(this->eng_);
		this->map_shader_[s->obj_id()] = s;
		this->map_name_shader_[s->shader_name()] = s->obj_id();

		shader_program::ptr shrd_prg;
		this->get_shader_program_by_name(s->shader_name(), shrd_prg);
		if(!shrd_prg.is_null())
		{
			s->set_shader_program(shrd_prg);
			s->init();
		}
	}

	return 0;
}

}
