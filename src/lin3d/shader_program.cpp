/**
* @file shade_program.cpp
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
#include "shader_program.h"
#include "win32gl_glee.h"
#include <gl/glu.h>
#include "l3_file.h"
#include "l3_log.h"
#include "win_device.h"

namespace l3eng{

static int printError(char *file, int line)
{
	GLenum glErr;
	int retCode = 0;

	glErr = glGetError();
	while (glErr != GL_NO_ERROR)
	{
		L3_LOG_ENG_TRACE(("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr)));
		retCode = 1;
		glErr = glGetError();
	}
	return retCode;
}

static void printInfoLog(GLhandleARB obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	GLcharARB *infoLog;
	printError(__FILE__, __LINE__);

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
	printError(__FILE__, __LINE__);

	if(infologLength > 0)
	{
		infoLog = (GLcharARB*)malloc(infologLength);
		if(infoLog == NULL)
		{
			L3_LOG_ENG_TRACE(("ERROR: Could not allocate InfoLog buffer\n"));
			exit(1);
		}
		glGetInfoLogARB(obj,infologLength,&charsWritten,infoLog);
		L3_LOG_ENG_TRACE(("InfoLog:\n%s\n\n",infoLog));
		free(infoLog);
	}
	printError(__FILE__, __LINE__);
}

l3_int32 shader_source::load_from_file(const std::string& file_path)
{
	L3_LOG_ENG_TRACE(("load glsl:%s", file_path.c_str()));
	common_file f;
	l3_int32 ret = f.open(file_path);
	if(0 != ret)
	{
		L3_LOG_ENG_TRACE(("open file fail:%d %s", ret, file_path.c_str()));
		return ret;
	}

	ret = f.get_file_size();
	L3_LOG_ENG_TRACE(("file sz", ret));
	if(ret < 0)
	{
		L3_LOG_ENG_TRACE(("get file size err:%s",  file_path.c_str()));
		return -1;
	}

	std::string data;
	data.resize(ret);
	f.read((void *)data.data(), (l3_uint32)data.size());

	L3_LOG_ENG_TRACE(("glsl program:%s", data.c_str()));

	return this->load_from_string(data, file_path);
}

l3_int32 shader_source::load_from_string(const std::string& data, const std::string& vpath)
{
	switch(this->stype_)
	{
	case SHADER_SOURCE_TYPE_VERTEX:
		this->sobj_ = glCreateShader(GL_VERTEX_SHADER);
		break;

	case SHADER_SOURCE_TYPE_FRAGMENT:
		this->sobj_ = glCreateShader(GL_FRAGMENT_SHADER);
		break;

	case SHADER_SOURCE_TYPE_TESS_CONTRL:
		this->sobj_ = glCreateShader(GL_TESS_CONTROL_SHADER);
		break;

	case SHADER_SOURCE_TYPE_TESS_EVAL:
		this->sobj_ = glCreateShader(GL_TESS_EVALUATION_SHADER);
		break;

	default:
		break;
	}

	const char* sources[1];
	sources[0] = data.c_str();

	glShaderSource(this->sobj_, 1, sources, NULL);
	glCompileShader(this->sobj_);

	printError(__FILE__, __LINE__);
	
	GLint compiled = 0;
	glGetShaderiv(this->sobj_, GL_COMPILE_STATUS, &compiled);
	printError(__FILE__, __LINE__);

	if(!compiled)
	{
		printInfoLog(this->sobj_);
		printError(__FILE__, __LINE__);
		L3_LOG_ENG_TRACE(("compile fail:%d", compiled));
		return -1;
	}
	
	printInfoLog(this->sobj_);
	
	return 0;
}


l3_int32 shader_program::init()
{
	//L3_LOG_ENG_TRACE((""));

	this->prog_ = glCreateProgram();
	if(this->prog_ < 0)
		return -1;

	printError(__FILE__, __LINE__);

	return 0;
}

l3_int32 shader_program::attach(shader_source::ptr sobj)
{
	//L3_LOG_ENG_TRACE((""));

	if(sobj.is_null())
		return -1;

	this->lst_sobj_.push_back(sobj);

	glAttachShader(this->prog_, sobj->sobj());

	printError(__FILE__, __LINE__);

	return 0;
}

l3_int32 shader_program::link()
{
	//L3_LOG_ENG_TRACE((""));

	glLinkProgram(this->prog_);
	printInfoLog(this->prog_);

	GLint linked = 0;
	glGetProgramiv(this->prog_, GL_LINK_STATUS, &linked);

	printInfoLog(this->prog_);

	if(!linked)
		return -1;

	/* just test */
	l3_int32 attr_count;
	glGetProgramiv(this->prog_, GL_ACTIVE_ATTRIBUTES, &attr_count);
	for(l3_int32 i = 0; i < attr_count; i ++)
	{
		GLsizei name_len = 256;
		GLint size_attr = 0;
		GLenum type;
		char actemp[256] = {0};
		glGetActiveAttrib(this->prog_, i, sizeof(actemp), &name_len, &size_attr, &type, actemp);
		l3_int32 loc = glGetAttribLocationARB(this->prog_, actemp);

		for(int i = 0; (i < sizeof(actemp)) && actemp[i]; i ++)
		{
			if('[' == actemp[i])
				actemp[i] = 0;
		}

		l3_bool is_gl = L3_FALSE;
		if(strlen(actemp) >= 2)
		{
			if('g' == actemp[0] && 'l' == actemp[1])
			{
				is_gl = L3_TRUE;
			}
		}

		glsl_param_t gpt;
		gpt.loc_ = loc;
		gpt.gl_type_ = type;
		gpt.size_ = size_attr;
		gpt.name_ = actemp;

		if(is_gl)
		{
			this->map_gl_attrib_info_[gpt.loc_] = gpt;
			this->map_gl_attrib_name_[gpt.name_] = gpt.loc_;
		}
		else
		{
			this->map_attrib_info_[gpt.loc_] = gpt;
			this->map_attrib_name_[gpt.name_] = gpt.loc_;
		}
	}

	l3_int32 unf_count = 0;
	glGetProgramiv(this->prog_, GL_ACTIVE_UNIFORMS, &unf_count);
	for(l3_int32 i = 0; i < unf_count; i ++)
	{
		GLsizei name_len = 256;
		GLint size_attr = 0;
		GLenum type;
		char actemp[256] = {0};
		glGetActiveUniform(this->prog_, i, sizeof(actemp), &name_len, &size_attr, &type, actemp);
		l3_int32 loc = glGetUniformLocationARB(this->prog_, actemp);

		for(int i = 0; (i < sizeof(actemp)) && actemp[i]; i ++)
		{
			if('[' == actemp[i])
				actemp[i] = 0;
		}

		l3_bool is_gl = L3_FALSE;
		if(strlen(actemp) >= 2)
		{
			if('g' == actemp[0] && 'l' == actemp[1])
			{
				is_gl = L3_TRUE;
			}
		}


		switch(type)
		{
		case GL_SAMPLER_2D:
		case GL_SAMPLER_2D_SHADOW:
			{
				glsl_param_tex_t gpt;

				gpt.loc_ = loc;
				gpt.gl_type_ = type;
				gpt.size_ = size_attr;
				gpt.name_ = actemp;

				this->map_uniform_tex_[gpt.loc_] = gpt;
				this->map_uniform_tex_name_[gpt.name_] = gpt.loc_;
			}
			break;

		default:
			{
				glsl_param_t gpt;

				gpt.loc_ = loc;
				gpt.gl_type_ = type;
				gpt.size_ = size_attr;
				gpt.name_ = actemp;

				if(is_gl)
				{
					this->map_gl_uniform_info_[gpt.loc_] = gpt;
					this->map_gl_uniform_name_[gpt.name_] = gpt.loc_;
				}
				else
				{
					this->map_uniform_info_[gpt.loc_] = gpt;
					this->map_uniform_name_[gpt.name_] = gpt.loc_;
				}
			}
			break;
		}
	}

	return 0;
}

void shader_program::get_all_uniform_val()
{
	for(MAP_GLSL_PARAM::iterator it = this->map_uniform_info_.begin();
		it != this->map_uniform_info_.end(); it ++)
	{
		glsl_param_t& gp = it->second;

		if(gp.loc_ < 0)
			continue;

		switch(gp.gl_type_)
		{
		//case GL_FLOAT_MAT4_ARB:
		case GL_FLOAT_MAT4:
			{
				l3_f32 f[20] = {0.0f};

				//glGetUniformfvARB(this->prog_, gp.loc_, f);
				glGetUniformfv(this->prog_, gp.loc_, f);

				int a = 0;
			}
			break;

		case GL_FLOAT:
			{
				l3_f32 f[1024] = {0.0f};

				//glGetUniformfvARB(this->prog_, gp.loc_, f);
				glGetUniformfv(this->prog_, gp.loc_, f);

				int a = 0;
			}
			break;
		}

		//glGetUniformfvARB(this->prog_,
		//	gp.loc_,
		//	GLfloat *params);
	}
}

l3_int32 shader_program::active()const
{
	glUseProgram(this->prog_);
	return 0;
}

l3_int32 shader_program::inactive()
{
	glUseProgram(0);
	return 0;
}

l3_int32 shader_program::get_uniform_info(const std::string& name, glsl_param_t& info)const
{
	MAP_GLSL_NAME_LOC::const_iterator it = this->map_uniform_name_.find(name);
	if(this->map_uniform_name_.end() == it)
		return -1;

	MAP_GLSL_PARAM::const_iterator it1 = this->map_uniform_info_.find(it->second);
	if(this->map_uniform_info_.end() == it1)
		return -1;

	info = it1->second;
	return 0;
}
l3_int32 shader_program::get_uniform_loc(const std::string& name)const
{
	MAP_GLSL_NAME_LOC::const_iterator it = this->map_uniform_name_.find(name);
	if(this->map_uniform_name_.end() == it)
		return -1;

	return it->second;
}

l3_int32 shader_program::get_uniform_tex(const std::string& name, glsl_param_tex_t& info)const
{
	MAP_GLSL_NAME_LOC::const_iterator it = this->map_uniform_tex_name_.find(name);
	if(this->map_uniform_tex_name_.end() == it)
		return -1;

	MAP_GLSL_TEX::const_iterator it1 = this->map_uniform_tex_.find(it->second);
	if(this->map_uniform_tex_.end() == it1)
		return -1;

	info = it1->second;
	return 0;
}
l3_int32 shader_program::get_uniform_tex_loc(const std::string& name)const
{
	MAP_GLSL_NAME_LOC::const_iterator it = this->map_uniform_tex_name_.find(name);
	if(this->map_uniform_tex_name_.end() == it)
		return -1;

	return it->second;
}

l3_int32 shader_program::get_attrib_info(const std::string& name, glsl_param_t& info)const
{
	MAP_GLSL_NAME_LOC::const_iterator it = this->map_attrib_name_.find(name);
	if(this->map_attrib_name_.end() == it)
		return -1;

	MAP_GLSL_PARAM::const_iterator it1 = this->map_gl_attrib_info_.find(it->second);
	if(this->map_gl_attrib_info_.end() == it1)
		return -1;

l3_int32 loc = glGetAttribLocation(this->prog_, name.c_str());

	info = it1->second;
	return 0;
}
l3_int32 shader_program::get_attrib_loc(const std::string& name)const
{
	MAP_GLSL_NAME_LOC::const_iterator it = this->map_attrib_name_.find(name);
	if(this->map_attrib_name_.end() == it)
		return -1;

	return it->second;
}

l3_int32 shader_program::uni_unbind_all()
{
	for(MAP_GLSL_PARAM::iterator it = this->map_uniform_info_.begin(); it != this->map_uniform_info_.end(); it ++)
		it->second.b_bind_ = L3_FALSE;
	return 0;
}

l3_int32 shader_program::uni_unbind_tex_all()
{
	for (MAP_GLSL_TEX::iterator it = this->map_uniform_tex_.begin(); it != this->map_uniform_tex_.end(); it++)
	{
		it->second.b_bind_ = L3_FALSE;
		it->second.v_tex_obj_.resize(0);
	}
	return 0;
}

l3_int32 shader_program::uni_bind_tex(const l3_int32 loc,
	const OBJ_ID tex_obj)
{
	MAP_GLSL_TEX::iterator it = this->map_uniform_tex_.find(loc);
	if(this->map_uniform_tex_.end() == it)
		return -1;
	if (it->second.size_ > 1)
	{
		L3_LOG_ENG_WARN(("glsl:%s uni_name:%s size err", this->name_.c_str(), it->second.name_.c_str()));
		return -1;
	}

	it->second.b_bind_ = L3_TRUE;
	it->second.tex_obj_ = tex_obj;

	return 0;
}
l3_int32 shader_program::uni_bind_tex_array(const l3_int32 loc,
	const std::vector<OBJ_ID>& v_tex_obj)
{
	MAP_GLSL_TEX::iterator it = this->map_uniform_tex_.find(loc);
	if (this->map_uniform_tex_.end() == it)
		return -1;
	if (it->second.size_ <= 1)
	{
		L3_LOG_ENG_WARN(("glsl:%s uni_name:%s size err", this->name_.c_str(), it->second.name_.c_str()));
		return -1;
	}

	it->second.b_bind_ = L3_TRUE;
	it->second.v_tex_obj_ = v_tex_obj;
	l3_int32 cur_size = it->second.v_tex_obj_.size();
	if (cur_size < it->second.size_)
	{
		for(l3_int32 i = 0; i < (it->second.size_ - cur_size); i ++)
			it->second.v_tex_obj_.push_back(-1);
	}

	return 0;
}

l3_int32 shader_program::uni_bind_float_ve2(const l3_int32 loc,
	const l3_f32 f1,
	const l3_f32 f2)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
	{
		buf = device_buf_base::ptr(new device_common_buf);
	}

	buf->resize_buf_sz(sizeof(l3_f32) * 2);
	l3_f32 * f = (l3_f32 *)buf->get_buf();

	f[0] = f1;
	f[1] = f2;

	return 0;
}
l3_int32 shader_program::uni_bind_float_vec2_array(const l3_int32 loc,
	const std::vector<vector2>& vf)
{
	MAP_GLSL_PARAM::iterator it = this->map_uniform_info_.find(loc);
	if (it == this->map_uniform_info_.end())
		return -1;
	if (vf.size() > it->second.size_)
	{
		L3_LOG_ENG_WARN(("float array size not match input size:%d glsl size:%d", vf.size(), it->second.size_));
		return -1;
	}
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if (buf.is_null())
		buf = device_buf_base::ptr(new device_common_buf);

	buf->clear();
	for (l3_int32 i = 0; i < vf.size(); i++)
		buf->append_buf((void *)vf[i].get_vector2_data(), vf[i].get_vector2_data_sz());

	if (it->second.size_ > vf.size())
		buf->resize_buf_sz(it->second.size_ * vector2::vector2_data_sz());

	return 0;
}

l3_int32 shader_program::uni_bind_float_vec3(const l3_int32 loc,
	const l3_f32 f1,
	const l3_f32 f2,
	const l3_f32 f3)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
	{
		buf = device_buf_base::ptr(new device_common_buf);
	}

	buf->resize_buf_sz(sizeof(l3_f32) * 3);
	l3_f32 * f = (l3_f32 *)buf->get_buf();

	f[0] = f1;
	f[1] = f2;
	f[2] = f3;

	return 0;
}

l3_int32 shader_program::uni_bind_float_vec4(const l3_int32 loc,
	const l3_f32 f1,
	const l3_f32 f2,
	const l3_f32 f3,
	const l3_f32 f4)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
	{
		buf = device_buf_base::ptr(new device_common_buf);
	}

	buf->resize_buf_sz(sizeof(l3_f32) * 4);
	l3_f32 * f = (l3_f32 *)buf->get_buf();

	f[0] = f1;
	f[1] = f2;
	f[2] = f3;
	f[3] = f4;

	return 0;
}

l3_int32 shader_program::uni_bind_float(const l3_int32 loc,
	const l3_f32 f)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
	{
		buf = device_buf_base::ptr(new device_common_buf);
	}

	buf->resize_buf_sz(sizeof(l3_f32));
	l3_f32 * buf_f = (l3_f32 *)buf->get_buf();

	*buf_f = f;

	return 0;
}

l3_int32 shader_program::uni_bind_float_array(const l3_int32 loc,
	const l3_f32 * pf, const l3_uint32 pf_sz)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	if (pf_sz > it->second.size_)
	{
		L3_LOG_ENG_WARN(("float array size not match input size:%d glsl size:%d", pf_sz, it->second.size_));
		return -1;
	}

	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
		buf = device_buf_base::ptr(new device_common_buf);

	buf->clear();

	buf->append_buf((void *)pf, sizeof(l3_f32) * pf_sz);
	if (pf_sz < it->second.size_)
		buf->resize_buf_sz(it->second.size_ * sizeof(l3_f32));

	return 0;
}

l3_int32 shader_program::uni_bind_mat4(const l3_int32 loc,
	const matrix4& mtx)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
	{
		buf = device_buf_base::ptr(new device_common_buf);
	}

	matrix4 mtx_transpose;
	mtx.transpose(mtx_transpose);

	buf->resize_buf_sz(sizeof(l3_f32) * 16);
	l3_f32 * pbuf = (l3_f32 *)buf->get_buf();

	memcpy(pbuf, mtx_transpose.get_matrix_data(), mtx_transpose.get_matrix_data_sz()/*sizeof(l3_f32) * 16*/);

	return 0;
}

/* @brief °ó¶¨4x4¾ØÕóÊý×é */
l3_int32 shader_program::uni_bind_mat4_array(const l3_int32 loc,
	const std::vector<matrix4>& v_mtx)
{
	MAP_GLSL_PARAM::iterator it = this->map_uniform_info_.find(loc);
	if (it == this->map_uniform_info_.end())
		return -1;
	if (v_mtx.size() > it->second.size_)
	{
		L3_LOG_ENG_WARN(("mtx array size not match input size:%d glsl size:%d", v_mtx.size(), it->second.size_));
		return -1;
	}

	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if (buf.is_null())
		buf = device_buf_base::ptr(new device_common_buf);

	buf->clear();
	matrix4 mtx_transpose;
	for (l3_int32 i = 0; i < v_mtx.size(); i++)
	{
		v_mtx[i].transpose(mtx_transpose);
		buf->append_buf((void *)mtx_transpose.get_matrix_data(), mtx_transpose.get_matrix_data_sz());
	}

	if (it->second.size_ > v_mtx.size())
		buf->resize_buf_sz(it->second.size_ * matrix4::matrix_data_sz());
}

l3_int32 shader_program::uni_bind_int(const l3_int32 loc,
	const l3_int32 i)
{
	MAP_GLSL_PARAM::iterator it =  this->map_uniform_info_.find(loc);
	if(it == this->map_uniform_info_.end())
		return -1;
	it->second.b_bind_ = L3_TRUE;

	device_buf_base::ptr& buf = it->second.buf_;
	if(buf.is_null())
	{
		buf = device_buf_base::ptr(new device_common_buf);
	}

	buf->resize_buf_sz(sizeof(l3_int32));
	l3_int32 * pbuf = (l3_int32 *)buf->get_buf();

	*pbuf = i;

	return 0;
}

l3_int32 shader_program::uni_dev_bind()
{
	win_device::print_error(__FILE__, __LINE__);
	for(MAP_GLSL_PARAM::iterator it = this->map_uniform_info_.begin(); it != this->map_uniform_info_.end(); it ++)
	{
		glsl_param_t& info = it->second;

		if(!info.b_bind_ || info.buf_.is_null())
			continue;

		if(info.loc_ < 0)
			continue;

		switch(info.gl_type_)
		{
		case GL_INT:
			{
				if (1 == info.size_)
				{
					if (4 * info.size_ <= info.buf_->get_buf_sz())
						glUniform1i(info.loc_, *(GLint*)(info.buf_->get_buf()));
					win_device::print_error(__FILE__, __LINE__);
				}
				else
				{
					if (4 * info.size_ <= info.buf_->get_buf_sz())
						glUniform1iv(info.loc_, info.size_, (GLint*)(info.buf_->get_buf()));
					win_device::print_error(__FILE__, __LINE__);
				}
			}
			break;

		case GL_FLOAT:
			{
				if(4 * info.size_ <= info.buf_->get_buf_sz())
					glUniform1fv(info.loc_, info.size_, (GLfloat *)(info.buf_->get_buf()));

				win_device::print_error(__FILE__, __LINE__);
			}
			break;

		case GL_FLOAT_VEC4_ARB:
			{
				glUniform4fv(info.loc_, info.size_, (GLfloat *)(info.buf_->get_buf()));
				win_device::print_error(__FILE__, __LINE__);
			}
			break;

		case GL_FLOAT_VEC3_ARB:
			{
				glUniform3fv(info.loc_, info.size_, (GLfloat *)(info.buf_->get_buf()));
				win_device::print_error(__FILE__, __LINE__);
			}
			break;

		case GL_FLOAT_VEC2_ARB:
			{
				glUniform2fv(info.loc_, info.size_, (GLfloat *)(info.buf_->get_buf()));
				win_device::print_error(__FILE__, __LINE__);
			}
			break;

		case GL_FLOAT_MAT4_ARB:
			{
				glUniformMatrix4fv(info.loc_, info.size_, GL_TRUE, (GLfloat *)(info.buf_->get_buf()));
				win_device::print_error(__FILE__, __LINE__);
			}
			break;

		default:
			{
				L3_LOG_ENG_ERR(("unknow type:%d", info.gl_type_));
			}
			break;
		}
	}

	return 0;
}

l3_int32 shader_program::uni_dev_bind_tex()
{
	for(MAP_GLSL_TEX::iterator it = this->map_uniform_tex_.begin(); it != this->map_uniform_tex_.end(); it ++)
	{
		if(!it->second.b_bind_)
			continue;

		glsl_param_tex_t& param_tex = it->second;

		l3_bool bind_shadow = L3_FALSE;
		switch(param_tex.gl_type_)
		{
		case GL_SAMPLER_2D:
			break;

		case GL_SAMPLER_2D_SHADOW:
			bind_shadow = L3_TRUE;
			break;
		}

		if (1 == param_tex.size_)
		{
			l3_int32 tex_unit = this->eng_->dev()->bind_user_tex(param_tex.tex_obj_, bind_shadow);
			if (tex_unit < 0)
				continue;

			glUniform1i(param_tex.loc_, tex_unit);
		}
		else
		{
			std::vector<GLint> v_tex_unit;
			for (l3_int32 i = 0; i < param_tex.v_tex_obj_.size(); i++)
			{
				GLint tex_unit = this->eng_->dev()->bind_user_tex(param_tex.v_tex_obj_[i], bind_shadow);
				if (tex_unit < 0)
					v_tex_unit.push_back(0);
				else
					v_tex_unit.push_back(tex_unit);
			}
			if(!v_tex_unit.empty())
				glUniform1iv(param_tex.loc_, param_tex.size_, &v_tex_unit[0]);
		}
	}

	return 0;
}

l3_int32 shader_program::attr_dev_bind()
{
	for(MAP_GLSL_PARAM::iterator it = this->map_attrib_info_.begin(); it != this->map_attrib_info_.end(); it ++)
	{
		l3_int32 stride_sz = 0;
		l3_int32 size = 1;

		glsl_param_t& info = it->second;
		if(info.buf_.is_null())
			continue;

		switch(info.gl_type_)
		{
		case GL_FLOAT:
			{
				stride_sz = 4 * info.size_;
				size = 1;
				//if(stride_sz > info.buf_->get_buf_sz())
				//{
				//	L3_LOG_ENG_ERR(("buffer not big enough"));
				//	continue;
				//}
			}
			break;

		case GL_FLOAT_VEC3:
			{
				stride_sz = 12 * info.size_;
				size = 3;
			}
			break;

		case GL_FLOAT_VEC2:
			{
				stride_sz = 8 * info.size_;
				size = 2;
			}
			break;

		default:
			continue;
			break;
		}

		void * pbuf = info.buf_->get_buf();

		glVertexAttribPointer(info.loc_,
			size,
			GL_FLOAT,
			GL_FALSE,
			stride_sz,
			pbuf);


		win_device::print_error(__FILE__, __LINE__);

		glEnableVertexAttribArray(info.loc_);
		win_device::print_error(__FILE__, __LINE__);
	}

	return 0;
}

l3_int32 shader_program::attr_dev_unbind()const
{
	for(MAP_GLSL_PARAM::const_iterator it = this->map_attrib_info_.begin(); it != this->map_attrib_info_.end(); it ++)
	{
		const glsl_param_t& info = it->second;
		glDisableVertexAttribArray(info.loc_);
	}

	return 0;
}

l3_int32 shader_program::attr_bind(const l3_int32 loc, device_buf_base::ptr& buf)
{
	MAP_GLSL_PARAM::iterator it = this->map_attrib_info_.find(loc);
	if(this->map_attrib_info_.end() == it)
		return -1;

	it->second.buf_ = buf;

	return 0;
}

l3_int32 shader_program::attr_unbind_all()
{
	for(MAP_GLSL_PARAM::iterator it = this->map_attrib_info_.begin(); it != this->map_attrib_info_.end(); it ++)
		it->second.b_bind_ = L3_FALSE;
	return 0;
}

}




