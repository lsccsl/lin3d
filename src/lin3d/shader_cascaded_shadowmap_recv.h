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


namespace l3eng {

	class shader_cascaded_shadowmap_recv : public shader
	{
	public:

		typedef ref_ptr_thread<shader_cascaded_shadowmap_recv> ptr;
		OBJ_TYPE_DEFINE(shader, OBJ_SHADER_CASCADED_SHADOWMAP_RECV)

	public:

		shader_cascaded_shadowmap_recv(l3_engine* eng):shader(eng)
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

	private:
	};

}

#endif
