/**
* @file mesh_loader_xfile.cpp
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
#include "mesh_loader_x.h"

#include <sstream>

#include "l3_file.h"
#include "l3_log.h"

namespace l3eng{

const char * mesh_loader_x::_get_token(const char * p, const l3_int32 sz, std::string& token)
{
	char * p_token_begin = (char *)p;
	l3_int32 i = 0;
	for(; i < sz && *p_token_begin; i ++, p_token_begin ++)
	{
		if(mesh_loader_x::_check_comment(p_token_begin, sz - i))
			break;

		if(::isspace(*p_token_begin))
			continue;

		break;
	}

	if(p_token_begin >= (p + sz))
	{
		return (p_token_begin + 1);
	}

	switch(*p_token_begin)
	{
	case ';':
	case '}':
	case '{':
	case ',':
		{
			token.insert(0, 1, *p_token_begin);
			return (p_token_begin + 1);
		}
		break;

	default:
		break;
	}

	char * p_token_end = p_token_begin;
	for(; i < sz && *p_token_end; i ++, p_token_end ++)
	{
		if(mesh_loader_x::_check_comment(p_token_end, sz - i))
			break;

		if(::isspace(*p_token_end) ||
			';' == *p_token_end ||
			'}' == *p_token_end ||
			'{' == *p_token_end ||
			',' == *p_token_end)
			break;
	}

	assert(p_token_end > p_token_begin);
	token.insert(0, p_token_begin, p_token_end - p_token_begin);

	return p_token_end;
}

const l3_bool mesh_loader_x::_check_comment(const char * p, const l3_int32 sz)
{
	if('#' == *p)
		return L3_TRUE;

	if(sz >= 2)
	{
		if('/' == *p && '/' == *(p+1))
			return L3_TRUE;
	}

	return L3_FALSE;
}

const char * mesh_loader_x::_read_line(const char * p, const l3_int32 sz)
{
	char * ptmp = (char *)p;
	for(l3_int32 i = 0; i < sz && *ptmp; i ++, ptmp ++)
	{
		switch(*ptmp)
		{
		case '\r':
			return ptmp + 1;
			break;

		case '\n':
			this->current_line_ ++;
			return ptmp + 1;
			break;
		}
	}

	return ptmp;
}

l3_int32 mesh_loader_x::load_mesh(const std::string& file_path)
{
	if(file_path.empty())
		return -1;

	common_file f;
	if(0 != f.open(file_path))
		return -1;

	l3_int32 f_sz = f.get_file_size();
	if(f_sz < 0)
		return -1;

	std::vector<l3_uint8> v;
	v.resize(f_sz);
	if(f.read(&v[0], (l3_uint32)v.size()) < 0)
		return -1;

	return this->_parse((const char *)&v[0], (l3_uint32)v.size());
}

l3_int32 mesh_loader_x::_parse(const char * p, const l3_int32 sz)
{
	L3_LOG_ENG_DEBUG(("sz:%d %x", sz, p));

	if(sz < 12)
		return -1;

	if(0 != strncmp(p, "xof ", 4))
		return -1;

	char * ptmp = (char *)(p + 4);

	{
		char actemp[3] = {0};
		actemp[0] = ptmp[0];
		actemp[1] = ptmp[1];
		this->ver1_ = atoi(actemp);
		ptmp += 2;

		actemp[0] = ptmp[0];
		actemp[1] = ptmp[1];
		this->ver2_ = atoi(actemp);
		ptmp += 2;
	}

	if(0 == strncmp(ptmp, "txt ", 4))
	{
		L3_LOG_DEBUG(MODULE_ENG, ("txt format"));
	}
	else
	{
		L3_LOG_TRACE(MODULE_ENG, ("unknow x file format"));
		return -1;
	}

	ptmp += 4;

	if(0 == strncmp(ptmp, "0032", 4))
	{
		this->f_sz_ = X_FLOAT_32;
	}
	else
	{
		L3_LOG_TRACE(MODULE_ENG, ("unknown float sz"));
	}

	ptmp += 4;

	char * p_end_line = (char *)ptmp;
	const char * p_doc_end = p + sz;

	do{
		ptmp = p_end_line;

		p_end_line = (char *)this->_read_line(ptmp, sz - (l3_int32)(ptmp - p));

		this->_process_line(ptmp, (l3_int32)(p_end_line - ptmp));
	}
	while(ptmp < p_doc_end);

{
//FILE * pfile = fopen("x.txt", "aw");
//ant_int32 line = 0;
//for(std::list<token_info>::iterator it = lst_token_.begin(); it != this->lst_token_.end(); it ++)
//{
//	token_info& token = *it;
//
//	if(token.line_ != line)
//		fprintf(pfile, "\r\n");
//
//	fprintf(pfile, "[%d-%d:%s]", token.line_, token.row_, token.token_.c_str());
//
//	line = token.line_;
//}
//fclose(pfile);
}
	return this->_parse_token();
}

void mesh_loader_x::_process_line(const char * p, const l3_int32 sz)
{
	std::string token;

	char * ptmp = (char *)p;
	do{
		if(mesh_loader_x::_check_comment(ptmp, sz - (l3_int32)(ptmp - p)))
			break;

		token.resize(0);
		ptmp = (char *)(this->_get_token(ptmp, sz - (l3_int32)(ptmp - p), token));

		if(!token.empty())
		{
			token_info ti;
			ti.token_ = token;
			ti.line_ = this->current_line_;
			ti.row_ = (l3_int32)(ptmp - p);
			this->lst_token_.push_back(ti);
		}

	}while(ptmp - p < sz);

}

l3_int32 mesh_loader_x::_parse_unknown(std::list<token_info>::iterator& it)
{
	l3_int32 brace = 0;

	if(it == this->lst_token_.end())
		return -1;

	token_info * token = &(*it);

	for(;it != this->lst_token_.end(); it ++)
	{
		token = &(*it);
		if("{" != token->token_)
			continue;

		break;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		token = &(*it);

		if("{" == token->token_)
			brace ++;
		else if("}" == token->token_)
			brace --;

		if(0 == brace)
		{
			return 0;
			break;
		}

		if(brace < 0)
		{
			L3_LOG_TRACE(MODULE_ENG, ("brace not match %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
			break;
		}
	}

	return 0;
}

l3_int32 mesh_loader_x::_parse_template(std::list<token_info>::iterator& it)
{
	return this->_parse_unknown(it);
}

l3_int32 mesh_loader_x::_parse_int(const std::string& token, l3_int32& val)
{
	l3_bool negative = L3_FALSE;
	switch(token[0])
	{
	case '-':
		negative = L3_TRUE;
		break;

	case '+':
		negative = L3_FALSE;
		break;

	default:
		{
			if(token[0] >= '0' && token[0] <= '9')
				val = token[0] - '0';
			else
				return -1;
		}
		break;
	}

	l3_int32 i = 1;
	for(; i < (l3_int32)(token.size()); i ++)
	{
		l3_int32 c = token[i];

		if(c >= '0' && c <= '9')
			val = (c - '0') + val * 10;
		else
			break;
	}

	if(negative)
		val = (-val);

	return 0;
}

l3_int32 mesh_loader_x::_parse_float(const std::string& token, l3_f32& f)
{
	static const l3_f32 fast_atof_table[17] = {
		0.f,
		0.1f,
		0.01f,
		0.001f,
		0.0001f,
		0.00001f,
		0.000001f,
		0.0000001f,
		0.00000001f,
		0.000000001f,
		0.0000000001f,
		0.00000000001f,
		0.000000000001f,
		0.0000000000001f,
		0.00000000000001f,
		0.000000000000001f,
		0.0000000000000001f
	};

	if(token.empty())
	{
		f = 0;
		return 0;
	}

	l3_int32 val = 0;
	l3_bool negative = L3_FALSE;
	switch(token[0])
	{
	case '-':
		negative = L3_TRUE;
		break;

	case '+':
		negative = L3_FALSE;
		break;

	default:
		{
			if(token[0] >= '0' && token[0] <= '9')
				val = token[0] - '0';
		}
		break;
	}

	l3_int32 i = 1;
	for(; i < (l3_int32)(token.size()); i ++)
	{
		l3_int32 c = token[i];

		if(c >= '0' && c <= '9')
			val = (c - '0') + val * 10;
		else
			break;
	}

	f = (l3_f32)val;

	if(i >= (l3_int32)(token.size()))
		return 0;

	if('.' != token[i])
		return 0;

	i ++;

	val = 0;
	l3_int32 j = 0;
	for(; i < (l3_int32)(token.size()) && j < (sizeof(fast_atof_table)/sizeof(fast_atof_table[0]));
		i ++, j++)
	{
		l3_int32 c = token[i];

		if(c >= '0' && c <= '9')
			val = (c - '0') + val * 10;
		else
			break;
	}

	if(j > 0)
	{
		l3_f32 f1 = (l3_f32)val;
		f += f1 * fast_atof_table[j];
	}

	if(negative)
		f = -f;

	return 0;
}

l3_int32 mesh_loader_x::_parse_matrix(std::list<token_info>::iterator& it, matrix4& m)
{
	if(it == this->lst_token_.end())
		return -1;

	l3_int32 ret = 0;

	token_info * token = &(*it);
	l3_f32 fa[16] = {0.0f};
	{
		l3_int32 k = 0;
		for(; it != this->lst_token_.end(); it ++)
		{
			token = &(*it);
			if("," == token->token_)
				continue;

			if(";" == token->token_)
				break;

			if(k < sizeof(fa)/sizeof(fa[0]))
			{
				if(0 != this->_parse_float(token->token_, fa[k]))
				{
					ret = -1;
					goto end_;
				}
				k ++;
			}
		}

		it ++;
		if(0 != this->_get_symbol(it, ";"))
		{
			ret = -1;
			goto end_;
		}
	}

	m.assign(fa[0],
		fa[1],
		fa[2],
		fa[3],
		fa[4],
		fa[5],
		fa[6],
		fa[7],
		fa[8],
		fa[9],
		fa[10],
		fa[11],
		fa[12],
		fa[13],
		fa[14],
		fa[15]);

end_:

	if(0 != ret)
		L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_parse_coords2d(std::list<token_info>::iterator& it, vertex_texcoord_t& coord)
{
	/*
	* template Coords2d
	* {
	*	FLOAT u;
	*	FLOAT v;
	* }
	* 例:
	* 0.280615;0.558309;,
	*/
	token_info * token = NULL;
	l3_int32 k = 0;

	l3_int32 ret = 0;

	l3_f32 f[2] = {0};
	token_info * token_float = NULL;
	for(; it != this->lst_token_.end() && k < sizeof(f)/sizeof(f[0]); it ++)
	{
		token = &(*it);
		if(";" != token->token_)
		{
			token_float = token;
			continue;
		}

		if(token_float)
		{
			if(0 != this->_parse_float(token_float->token_, f[k]))
			{
				ret = -1;
				goto end_;
			}
		}

		/* 处理3个';',然后结束 */
		k ++;
	}

	if(k < sizeof(f)/sizeof(f[0]))
	{
		ret = -1;
		goto end_;
	}

	coord.u_ = f[0];
	coord.v_ = f[1];

end_:

	if(0 != ret)
		L3_LOG_ENG_TRACE(("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_parse_frame_matrix(std::list<token_info>::iterator& it, matrix4& m)
{
	/*
	* matrix4X4模板定义
	* template Matrix4X4
	* {
	* 	<uid>
	*	array FLOAT matrix[16];
	* }
	*
	* template FrameTransformMatrix
	* {
	*	<uid>
	*	Matrix4X4 frameMatrix;
	* }
	*
	* 例:
	* FrameTransformMatrix
	* {
	*	1.000000,0.000000,0.000000,0.000000,
	*	0.000000,1.000000,0.000000,0.000000,
	*	0.000000,0.000000,1.000000,0.000000,
	*	-5.191007,-3.469861,0.000000,1.000000;;
	* }
	*/

	if(it == this->lst_token_.end())
		return -1;

	l3_int32 ret = 0;

	token_info * token = &(*it);

	if("FrameTransformMatrix" != token->token_)
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_parse_matrix(it, m))
	{
		ret = -1;
		goto end_;
	}
	//ant_f32 fa[16] = {0.0f};
	//{
	//	ant_int32 k = 0;
	//	for(; it != this->lst_token_.end(); it ++)
	//	{
	//		token = &(*it);
	//		if("," == token->token_)
	//			continue;

	//		if(";" == token->token_)
	//			break;

	//		if(k < sizeof(fa)/sizeof(fa[0]))
	//		{
	//			if(0 != this->_parse_float(token->token_, fa[k]))
	//			{
	//				ret = -1;
	//				goto end_;
	//			}
	//			k ++;
	//		}
	//	}

	//	it ++;
	//	if(0 != this->_get_symbol(it, ";"))
	//	{
	//		ret = -1;
	//		goto end_;
	//	}
	//}

	it ++;
	if(0 != this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}

end_:

	if(0 != ret)
		L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_parse_mesh_face(std::list<token_info>::iterator& it, x_face& face)
{
	/*
	* 模板定义如下
	* template MeshFace
	* {
	*	<uid>
	*	DWORD face_count;
	*	array DWORD face_idx[face_count];
	* }
	* 例:3;0,1,2;
	*/
	l3_int32 ret = 0;

	/* 暂时只支持三角形 */
	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 idx_count = 0;
	if(0 != this->_parse_int(it->token_, idx_count))
		return -1;

	it ++;
	if(0 != this->_get_symbol(it, ";"))
		return -1;
	it ++;

	if(3 != idx_count)
		return -1;

	l3_int32 i = 0;
	token_info * token = NULL;
	l3_int32 * val = (l3_int32 *)&face;
	for(; i< idx_count && it != this->lst_token_.end(); it ++)
	{
		if("," != it->token_ && ";" != it->token_)
		{
			token = &(*it);
			continue;
		}

		if(NULL == token)
			return -1;

		if(0 != this->_parse_int(token->token_, val[i]))
			return -1;

		i ++;
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_vector(std::list<token_info>::iterator& it, vector3& v)
{
	/*
	* 例
	* 9.163393;52.592094;5.311051;
	*
	* template Vector
	* {
	*	<uid>
	*	FLOAT x;
	*	FLOAT y;
	*	FLOAT z;
	* }
	*/
	token_info * token = NULL;
	l3_int32 k = 0;

	l3_int32 ret = 0;

	l3_f32 f[3] = {0};
	token_info * token_float = NULL;
	for(; it != this->lst_token_.end() && k < 3; it ++)
	{
		token = &(*it);
		if(";" != token->token_)
		{
			token_float = token;
			continue;
		}

		if(token_float)
		{
			if(0 != this->_parse_float(token_float->token_, f[k]))
			{
				ret = -1;
				goto end_;
			}
		}

		/* 处理3个';',然后结束 */
		k ++;
	}

	if(k < 3)
	{
		ret = -1;
		goto end_;
	}

	v.xyz(f[0], f[1], f[2]);

end_:

	if(0 != ret)
		L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_get_symbol(std::list<token_info>::iterator& it, const char * symbol)
{
	assert(symbol);

	if(it == this->lst_token_.end())
		return -1;

	token_info * token = &(*it);
	if(symbol != token->token_)
		return -1;

	return 0;
}

l3_int32 mesh_loader_x::_parse_skin_weight(std::list<token_info>::iterator& it, x_skin_weight& sw)
{
	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 ret = 0;
	token_info * token = &(*it);

	if("SkinWeights" != it->token_)
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}
	it ++;
	if(this->lst_token_.end() == it)
	{
		ret = -1;
		goto end_;
	}

	sw.ref_frame_name_ = it->token_;
	if(sw.ref_frame_name_.size() > 2)
	{
		if('\"' == sw.ref_frame_name_[0])
			sw.ref_frame_name_ = sw.ref_frame_name_.substr(1, sw.ref_frame_name_.size() - 2);
	}
	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	l3_int32 weight_count = 0;
	if(0 != this->_parse_int(it->token_, weight_count))
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	{
		l3_int32 i = 0;
		for(; it != this->lst_token_.end() && i < weight_count; it ++)
		{
			l3_int32 idx = 0;
			if(0 != this->_parse_int(it->token_, idx))
			{
				ret = -1;
				goto end_;
			}

			sw.v_vertex_idx_.push_back(idx);

			it ++;
			if(0 != this->_get_symbol(it, ","))
			{
				if(0 == this->_get_symbol(it, ";"))
				{
					it ++;
					break;
				}

				ret = -1;
				goto end_;
			}
		}

		if(sw.v_vertex_idx_.size() != weight_count)
		{
			ret = -1;
			goto end_;
		}
	}

	{
		l3_int32 i = 0;
		for(; it != this->lst_token_.end() && i < weight_count; it ++)
		{
			l3_f32 weight = 0.0f;
			if(0 != this->_parse_float(it->token_, weight))
			{
				ret = -1;
				goto end_;
			}

			sw.v_weight_.push_back(weight);

			it ++;
			if(0 != this->_get_symbol(it, ","))
			{
				if(0 == this->_get_symbol(it, ";"))
				{
					it ++;
					break;
				}

				ret = -1;
				goto end_;
			}
		}

		if(sw.v_weight_.size() != weight_count)
		{
			ret = -1;
			goto end_;
		}
	}

	/* 解析矩阵 */
	if(0 != this->_parse_matrix(it, sw.reserve_m_))
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_animation_key(std::list<token_info>::iterator& it, x_animation_key& ani_key)
{
	l3_int32 ret = 0;

	if(this->lst_token_.end() == it)
		return -1;

	token_info * token = &(*it);

	if("AnimationKey" != it->token_)
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	{
		l3_int32 key_count = 0;

		/* 获关键帧类型(旋转or平移等) */
		l3_int32 type;
		if(0 != this->_parse_int(it->token_, type))
		{
			ret = -1;
			goto end_;
		}
		it ++;
		ani_key.type_ = (x_animation_key::e_x_animation_type)type;

		if(0 != this->_get_symbol(it, ";"))
		{
			ret = -1;
			goto end_;
		}
		it ++;

		/* 获取关键帧个数 */
		if(0 != this->_parse_int(it->token_, key_count))
		{
			ret = -1;
			goto end_;
		}
		it ++;

		if(0 != this->_get_symbol(it, ";"))
		{
			ret = -1;
			goto end_;
		}
		it ++;

		for(l3_int32 i = 0; i < key_count; i ++)
		{
			x_time_float_key_t x_key;
			if(0 != this->_parse_animation_time_float_keys(it, x_key))
			{
				ret = -1;
				goto end_;
			}

			ani_key.lst_key_.push_back(x_key);

			if(0 != this->_get_symbol(it, ","))
			{
				if(0 == this->_get_symbol(it, ";"))
				{
					it ++;
					break;
				}

				ret = -1;
				goto end_;
			}
			it ++;
		}
	}

	if(0 != this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}
	//it ++;

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_animation_time_float_keys(std::list<token_info>::iterator& it, x_time_float_key_t& x_key)
{
	l3_int32 ret = 0;
	l3_int32 count = 0;

	if(this->lst_token_.end() == it)
		return -1;

	token_info * token = &(*it);

	if(0 != this->_parse_int(it->token_, x_key.t_))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_parse_int(it->token_, count))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	{
		for(; it != this->lst_token_.end() && count > 0; it ++)
		{
			if(";" == it->token_)
				break;

			if("," == it->token_)
				continue;

			l3_f32 f;
			if(0 != this->_parse_float(it->token_, f))
			{
				ret = -1;
				goto end_;
			}

			x_key.float_keys_.push_back(f);

			count --;
		}
	}

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_animation(std::list<token_info>::iterator& it, x_animation& ani)
{
	l3_int32 ret = 0;

	if(this->lst_token_.end() == it)
		return -1;

	token_info * token = &(*it);

	if("Animation" != it->token_)
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(this->lst_token_.end() == it)
	{
		ret = -1;
		goto end_;
	}

	if("{" == it->token_)
	{
	}
	else
	{
		ani.name_ = it->token_;
		it ++;
	}

	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	/* 解析出引用的frame,重要 */
	{
		if(0 != this->_get_symbol(it, "{"))
		{
			ret = -1;
			goto end_;
		}
		it ++;

		if(this->lst_token_.end() == it)
		{
			ret = -1;
			goto end_;
		}
		ani.frame_name_ = it->token_;
		it ++;

		if(0 != this->_get_symbol(it, "}"))
		{
			ret = -1;
			goto end_;
		}
		it ++;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		if("AnimationKey" == it->token_)
		{
			x_animation_key ani_key;
			if(0 != this->_parse_animation_key(it, ani_key))
			{
				ret = -1;
				goto end_;
			}
			ani.lst_key_.push_back(ani_key);
		}
		else if("}" == it->token_)
		{
			break;
		}
		else
		{
			if(0 != this->_parse_unknown(it))
			{
				ret = -1;
				goto end_;
			}
		}
	}

	if(this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}
	//it ++;

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_ani_set(std::list<token_info>::iterator& it, x_animation_set& ani_set)
{
	l3_int32 ret = 0;

	if(this->lst_token_.end() == it)
		return -1;

	token_info * token = &(*it);

	if("AnimationSet" != it->token_)
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(this->lst_token_.end() == it)
	{
		ret = -1;
		goto end_;
	}

	ani_set.name_ = it->token_;
	it ++;
	if(this->lst_token_.end() == it)
	{
		ret = -1;
		goto end_;
	}

	if(this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	for(; this->lst_token_.end() != it; it ++)
	{
		if("Animation" == it->token_)
		{
			x_animation ani;
			if(0 != this->_parse_animation(it, ani))
			{
				ret = -1;
				goto end_;
			}
			ani_set.lst_ani_.push_back(ani);
		}
		else if("}" == it->token_)
		{
			break;
		}
		else
		{
			if(0 != this->_parse_unknown(it))
			{
				ret = -1;
				goto end_;
			}
		}
	}

	if(this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}
	//it ++;

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_mesh_texture_coords(std::list<token_info>::iterator& it, x_texcoord& tc)
{
	//ANT_LOG_ENG_DEBUG((""));

	if(this->lst_token_.end() == it)
		return -1;

	token_info& token = (*it);

	l3_int32 ret = 0;

	if("MeshTextureCoords" != token.token_)
	{
		ret = -1;
		goto end_;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		token = (*it);
		if("{" == token.token_)
			break;
		
		continue;
	}

	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}

	l3_int32 tc_count = 0;
	it ++;
	if(it == this->lst_token_.end())
	{
		ret = -1;
		goto end_;
	}

	token = (*it);
	if(0 != this->_parse_int(token.token_, tc_count))
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	l3_int32 i = 0;
	for(; i < tc_count && it != this->lst_token_.end(); i ++, it ++)
	{
		vertex_texcoord_t coord;
		if(0 != this->_parse_coords2d(it, coord))
		{
			ret = -1;
			goto end_;
		}

		tc.v_texcoord_.push_back(coord);

		if(0 != this->_get_symbol(it, ","))
		{
			if(0 == this->_get_symbol(it, ";"))
			{
				it ++;
				break;
			}
			else
			{
				ret = -1;
				goto end_;
			}
		}
	}
	if(tc_count != tc.v_texcoord_.size())
	{
		L3_LOG_ENG_TRACE(("vertex count not match %d - %d", tc_count, tc.v_texcoord_.size()));
		ret = -1;
		goto end_;
	}


	if(0 != this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_ENG_TRACE(("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_ENG_TRACE(("reach end, parse err %s line:%d - row:%d", token.token_.c_str(), token.line_, token.row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_clr_rgba(std::list<token_info>::iterator& it, x_clr_rgba& clr_rgba)
{
	/*
	* 例
	* 1;1;1;1;
	*
	* template Vector
	* {
	*	<uid>
	*	FLOAT r;
	*	FLOAT g;
	*	FLOAT b;
	*	FLOAT a;
	* }
	*/

	if(this->lst_token_.end() == it)
		return -1;

	token_info * token = &(*it);
	l3_int32 k = 0;

	l3_int32 ret = 0;

	l3_f32 f[4] = {0};
	token_info * token_float = NULL;
	for(; it != this->lst_token_.end() && k < 4; it ++)
	{
		token = &(*it);
		if(";" != token->token_)
		{
			token_float = token;
			continue;
		}

		if(token_float)
		{
			if(0 != this->_parse_float(token_float->token_, f[k]))
			{
				ret = -1;
				goto end_;
			}
		}

		/* 处理4个';',然后结束 */
		k ++;
	}

	if(k < 4)
	{
		ret = -1;
		goto end_;
	}

	clr_rgba.r_ = f[0];
	clr_rgba.g_ = f[1];
	clr_rgba.b_ = f[2];
	clr_rgba.a_ = f[3];

end_:

	if(0 != ret)
		L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_parse_clr_rgb(std::list<token_info>::iterator& it, x_clr_rgb& clr_rgb)
{
	/*
	* 例
	* 1;1;1;
	*
	* template Vector
	* {
	*	<uid>
	*	FLOAT r;
	*	FLOAT g;
	*	FLOAT b;
	* }
	*/
	l3_int32 k = 0;

	l3_int32 ret = 0;

	l3_f32 f[3] = {0};

	if(this->lst_token_.end() == it)
		return -1;

	token_info * token = &(*it);

	token_info * token_float = NULL;
	for(; it != this->lst_token_.end() && k < 3; it ++)
	{
		token = &(*it);
		if(";" != token->token_)
		{
			token_float = token;
			continue;
		}

		if(token_float)
		{
			if(0 != this->_parse_float(token_float->token_, f[k]))
			{
				ret = -1;
				goto end_;
			}
		}

		/* 处理4个';',然后结束 */
		k ++;
	}

	if(k < 3)
	{
		ret = -1;
		goto end_;
	}

	clr_rgb.r_ = f[0];
	clr_rgb.g_ = f[1];
	clr_rgb.b_ = f[2];

end_:

	if(0 != ret)
		L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_parse_TextureFilename(std::list<token_info>::iterator& it, x_texture_filename& tex_filename)
{
	/*
	TextureFilename {
		"d0b80068.bmp";
	}
	*/

	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 ret = 0;
	token_info * token = &(*it);

	if(0 != _strnicmp("TextureFilename", token->token_.c_str(), token->token_.size()))
	{
		ret = -1;
		goto end_;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		token = &(*it);
		if("{" == token->token_)
			break;
		
		continue;
	}
	it ++;

	ret = this->_parse_string(it, tex_filename.filename_);

	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;
	
	if(0 != this->_get_symbol(it, "}"))
	{
		ret = -1;
		goto end_;
	}

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_ENG_TRACE(("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_ENG_TRACE(("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_string(std::list<token_info>::iterator& it, std::string& str)
{
	/*"d0b80068.bmp"*/

	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 ret = 0;
	token_info * token = &(*it);

	if('"' == token->token_[0] && '"' == token->token_[token->token_.size() - 1])
	{
		str = token->token_.substr(1, token->token_.size() - 2);
	}
	else
	{
		str = token->token_;
	}

//end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_ENG_TRACE(("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_ENG_TRACE(("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_Material(std::list<token_info>::iterator& it, x_material& mtrl)
{
	/*
	Material Material__2028_d0b80068_ddsSub0 {
		1.000000;1.000000;1.000000;1.000000;;
		3.200000;
		0.100000;0.100000;0.100000;;
		0.000000;0.000000;0.000000;;

		TextureFilename {
			"d0b80068.bmp";
		}
	}
	*/

	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 ret = 0;
	token_info * token = &(*it);

	if("Material" != token->token_)
	{
		ret = -1;
		goto end_;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		token = &(*it);
		if("{" == token->token_)
			break;
		
		continue;
	}

	it ++;
	this->_parse_clr_rgba(it, mtrl.face_clr_);

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_parse_float(it->token_, mtrl.pow_))
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_parse_clr_rgb(it, mtrl.specular_clr_))
	{
		ret = -1;
		goto end_;
	}

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	if(0 != this->_parse_clr_rgb(it, mtrl.emissive_clr_))
	{
		ret = -1;
		goto end_;
	}

	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;


	for(; it != this->lst_token_.end(); it ++)
	{
		if(0 == _strnicmp("TextureFilename", it->token_.c_str(), it->token_.size()))
		{
			if(0 != this->_parse_TextureFilename(it, mtrl.tex_))
			{
				ret = -1;
				goto end_;
			}
			continue;
		}

		if("}" == it->token_)
			break;
	}

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_ENG_TRACE(("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_ENG_TRACE(("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_MeshMaterialList(std::list<token_info>::iterator& it, x_material_list& m_list)
{
	/*
	MeshMaterialList  {
		2;
		2002;
		0,
		0,
		1;

		Material Material__2028_d0b80068_ddsSub0 {
			1.000000;1.000000;1.000000;1.000000;;
			3.200000;
			0.100000;0.100000;0.100000;;
			0.000000;0.000000;0.000000;;

			TextureFilename {
				"d0b80068.bmp";
			}
		}

		Material Material__2028_2029bc38_ddsSub1 {
			1.000000;1.000000;1.000000;1.000000;;
			3.200000;
			0.100000;0.100000;0.100000;;
			0.000000;0.000000;0.000000;;

			TextureFilename {
				"2029bc38.bmp";
			}
		}
	}
	*/

	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 ret = 0;

	token_info * token = &(*it);
	if("MeshMaterialList" != token->token_)
	{
		ret = -1;
		goto end_;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		token = &(*it);
		if("{" == token->token_)
			break;
		
		continue;
	}

	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}

	/* 读取material个数 */
	l3_int32 material_count;
	{
		it ++;
		if(it == this->lst_token_.end())
		{
			ret = -1;
			goto end_;
		}

		token = &(*it);
		if(0 != this->_parse_int(token->token_, material_count))
		{
			ret = -1;
			goto end_;
		}

		it ++;
		if(0 != this->_get_symbol(it, ";"))
		{
			ret = -1;
			goto end_;
		}
		it ++;
	}

	/* 读取三角面所属的material idx count */
	l3_int32 material_list_count = 0;
	{
		token = &(*it);
		if(0 != this->_parse_int(token->token_, material_list_count))
		{
			ret = -1;
			goto end_;
		}

		it ++;
		if(0 != this->_get_symbol(it, ";"))
		{
			ret = -1;
			goto end_;
		}
		it ++;
	}

	/* 循环读出每个三角面的material idx */
	for(l3_int32 i = 0; i < material_list_count; i ++, it ++)
	{
		l3_int32 idx;
		token = &(*it);
		if(0 != this->_parse_int(token->token_, idx))
		{
			ret = -1;
			goto end_;
		}

		m_list.v_material_list_.push_back(idx);

		it ++;

		if(0 != this->_get_symbol(it, ","))
		{
			if(0 == this->_get_symbol(it, ";"))
			{
				break;
			}
			else
			{
				ret = -1;
				goto end_;
			}
		}
	}

	/* 顶点个数是否符合 */
	if(material_list_count != m_list.v_material_list_.size())
	{
		L3_LOG_TRACE(MODULE_ENG, ("material list count not match %d - %d", material_list_count, m_list.v_material_list_.size()));
		ret = -1;
		goto end_;
	}

	/* 读出多边形的个数 */
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	for(; it != this->lst_token_.end(); it ++)
	{
		if("Material" == it->token_)
		{
			x_material x_mtrl;
			if(0 != this->_parse_Material(it, x_mtrl))
			{
				ret = -1;
				goto end_;
			}

			m_list.v_material_.push_back(x_mtrl);
			continue;
		}

		if("}" == it->token_)
			break;
	}

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_mesh(std::list<token_info>::iterator& it, x_mesh& xmsh)
{
	//ANT_LOG_DEBUG(MODULE_ENG, (""));

	if(this->lst_token_.end() == it)
		return -1;

	l3_int32 ret = 0;

	token_info * token = &(*it);
	if("Mesh" != token->token_)
	{
		ret = -1;
		goto end_;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		token = &(*it);
		if("{" == token->token_)
			break;
		
		continue;
	}

	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}

	/* 读取顶点个数 */
	l3_int32 vertex_count;
	it ++;
	if(it == this->lst_token_.end())
	{
		ret = -1;
		goto end_;
	}

	token = &(*it);
	if(0 != this->_parse_int(token->token_, vertex_count))
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	/* 循环取出vertex_count个顶点(x;y;z;) */
	l3_int32 i = 0;
	for(; i < vertex_count && it != this->lst_token_.end(); i ++, it ++)
	{
		vector3 v;
		if(0 != this->_parse_vector(it, v))
		{
			ret = -1;
			goto end_;
		}

		xmsh.vertex_.push_back(v);

		if(0 != this->_get_symbol(it, ","))
		{
			if(0 == this->_get_symbol(it, ";"))
			{
				break;
			}
			else
			{
				ret = -1;
				goto end_;
			}
		}
	}

	/* 顶点个数是否符合 */
	if(vertex_count != xmsh.vertex_.size())
	{
		L3_LOG_TRACE(MODULE_ENG, ("vertex count not match %d - %d", vertex_count, xmsh.vertex_.size()));
		ret = -1;
		goto end_;
	}

	/* 读出多边形的个数 */
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	l3_int32 face_count = 0;
	token = &(*it);
	if(0 != this->_parse_int(token->token_, face_count))
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(0 != this->_get_symbol(it, ";"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	for(i = 0; it != this->lst_token_.end(); i ++, it ++)
	{
		x_face face;
		if(0 != this->_parse_mesh_face(it, face))
		{
			ret = -1;
			goto end_;
		}

		xmsh.face_.push_back(face);

		if(0 != this->_get_symbol(it, ","))
		{
			if(0 == this->_get_symbol(it, ";"))
			{
				it ++;
				break;
			}
			else
			{
				ret = -1;
				goto end_;
			}
		}
	}

	if(face_count != xmsh.face_.size())
	{
		L3_LOG_TRACE(MODULE_ENG, ("vertex count not match %d - %d", vertex_count, xmsh.face_.size()));
		ret = -1;
		goto end_;
	}

	for(; it != this->lst_token_.end(); it ++)
	{
		if("MeshNormals" == it->token_ ||
			"VertexDuplicationIndices" == it->token_ ||
			"XSkinMeshHeader" == it->token_ ||
			"FVFData" == it->token_)
		{
			if(0 != this->_parse_unknown(it))
			{
				ret = -1;
				goto end_;
			}
			continue;
		}

		if("MeshMaterialList" == it->token_)
		{
			if(0 != _parse_MeshMaterialList(it, xmsh.material_list_))
			{
				ret = -1;
				goto end_;
			}
			continue;
		}

		if("MeshTextureCoords" == it->token_)
		{
			if(0 != this->_parse_mesh_texture_coords(it, xmsh.texcoord_))
			{
				ret = -1;
				goto end_;
			}
			continue;
		}

		if("SkinWeights" == it->token_)
		{
			x_skin_weight sw;
			if(0 != this->_parse_skin_weight(it, sw))
			{
				ret = -1;
				goto end_;
			}
			xmsh.lst_sw_.push_back(sw);
			continue;
		}

		if("}" == it->token_)
			break;
	}

end_:

	if(0 != ret)
	{
		if(it != this->lst_token_.end())
			L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", it->token_.c_str(), it->line_, it->row_));
		else
			L3_LOG_TRACE(MODULE_ENG, ("reach end, parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));
	}

	return ret;
}

l3_int32 mesh_loader_x::_parse_frame(std::list<token_info>::iterator& it, std::string& frame_name, const std::string& parent)
{
	l3_int32 ret = 0;

	if(it == this->lst_token_.end())
		return -1;

	token_info * token = &(*it);

	frame_info_t f;
	f.parent_ = parent;

	if("Frame" != token->token_)
	{
		ret = -1;
		goto end_;
	}

	it ++;
	if(this->lst_token_.end() != it)
	{
		token = &(*it);
		f.name_ = token->token_;
	}
	else
	{
		ret = -1;
		goto end_;
	}

	frame_name = f.name_;

	it ++;
	if(0 != this->_get_symbol(it, "{"))
	{
		ret = -1;
		goto end_;
	}
	it ++;

	{
		std::list<std::string> lst_frame_;
		std::string current_frame;
		for(; it != this->lst_token_.end(); it ++)
		{
			token = &(*it);
			if("Frame" == token->token_)
			{
				std::string sub_frame_name;
				if(0 != this->_parse_frame(it, sub_frame_name, f.name_))
				{
					ret = -1;
					goto end_;
				}
				f.lst_sub_.push_back(sub_frame_name);
				continue;
			}

			if("FrameTransformMatrix" == token->token_)
			{
				if(0 != this->_parse_frame_matrix(it, f.m_))
				{
					ret = -1;
					goto end_;
				}
				continue;
			}

			if("Mesh" == token->token_)
			{
				this->_parse_mesh(it, f.xmsh_);
				continue;
			}

			if("}" == token->token_)
				break;
		}
	}

	//it ++;
	//if(this->lst_token_.end() != it)
	//{
	//	token = &(*it);
	//	if("}" != token->token_)
	//	{
	//		ret = -1;
	//		goto end_;
	//	}
	//}

end_:

	if(0 == ret)
		this->map_frame_[f.name_] = f;
	else
		L3_LOG_TRACE(MODULE_ENG, ("parse err %s line:%d - row:%d", token->token_.c_str(), token->line_, token->row_));

	return ret;
}

l3_int32 mesh_loader_x::_parse_token()
{
	L3_LOG_ENG_DEBUG(("_parse_token"));
	std::list<token_info>::iterator it = this->lst_token_.begin();
	while(it != this->lst_token_.end())
	{
		token_info& token = *it;
		if("template" == token.token_)
		{
			if(0 != this->_parse_template(it))
				return -1;
		}
		else if("Frame" == token.token_)
		{
			std::string root_frame;
			if(0 != this->_parse_frame(it, root_frame, ""))
				return -1;
		}
		else if("AnimationSet" == token.token_)
		{
			x_animation_set ani_set;
			if(0 != this->_parse_ani_set(it, ani_set))
				return -1;

			this->lst_ani_set_.push_back(ani_set);
		}
		else
		{	
			this->_parse_unknown(it);
		}

		it ++;
	}
	L3_LOG_ENG_DEBUG(("_parse_token"));

	this->_debug_dump_frame_all();

	return 0;
}

void mesh_loader_x::_debug_dump_frame_all()const
{
return;
	L3_LOG_DEBUG(MODULE_ENG, ("----------------------"));

	{
		for(std::map<std::string, frame_info_t>::const_iterator it = this->map_frame_.begin();
			it != this->map_frame_.end(); it ++)
		{
			const x_mesh& xmsh = it->second.xmsh_;
			if(!xmsh.vertex_.empty())
			{
				std::ostringstream oss;
				oss << "mesh vertex " << xmsh.vertex_.size() << " " << it->second.name_ << ":\r\n";
				for(l3_int32 i = 0; i < (l3_int32)(xmsh.vertex_.size()); i ++)
				{
					if(i > 50 && i < (l3_int32)(xmsh.vertex_.size() - 50))
						continue;

					const vector3& v = xmsh.vertex_[i];
					oss << i << " " << v.x() << "," << v.y() << "," << v.z() << "\r\n";
				}

				L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
			}

			if(!xmsh.face_.empty())
			{
				std::ostringstream oss;
				oss << "mesh vertex idx " << xmsh.face_.size() << " " << it->second.name_ << ":\r\n";
				for(l3_int32 i = 0; i < (l3_int32)(xmsh.face_.size()); i ++)
				{
					if(i > 50 && i < (l3_int32)(xmsh.face_.size() - 50))
						continue;

					const x_face& face = xmsh.face_[i];
					oss << i << " " << face.idx1_ << "," << face.idx2_ << "," << face.idx3_ << "\r\n";
				}
				L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
			}

			if(!xmsh.lst_sw_.empty())
			{
				for(std::list<x_skin_weight>::const_iterator it = xmsh.lst_sw_.begin();
					it != xmsh.lst_sw_.end(); it ++)
				{
					const x_skin_weight& sw = *it;
					std::ostringstream oss;

					oss << "skin weight:" << sw.ref_frame_name_ << "\r\n";
					oss << sw.v_vertex_idx_.size() << "{";
					for(l3_int32 i = 0; i < (l3_int32)(sw.v_vertex_idx_.size()); i ++)
					{
						oss << sw.v_vertex_idx_[i] << " ";
					}
					oss << "}\r\n";

					oss << sw.v_weight_.size() << "{";
					for(l3_int32 j = 0; j < (l3_int32)(sw.v_weight_.size()); j ++)
					{
						oss << sw.v_weight_[j] << " ";
					}
					oss << "}\r\n";

					std::string m_info;
					sw.reserve_m_.dump(m_info);

					oss << "reserve matrix:" << m_info << "\r\n\r\n";


					L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
				}
			}

			if(!xmsh.material_list_.v_material_list_.empty())
			{
				std::ostringstream oss;
				oss << "\r\n";

				for(size_t i = 0; i < xmsh.material_list_.v_material_list_.size(); i ++)
				{
					oss << xmsh.material_list_.v_material_list_[i] << ",";
				}
				L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
			}

			if(!xmsh.material_list_.v_material_.empty())
			{
				std::ostringstream oss;
				for(size_t i = 0; i < xmsh.material_list_.v_material_.size(); i ++)
				{
					const x_material& xm = xmsh.material_list_.v_material_[i];

					oss << "face clr:["
						<< xm.face_clr_.r_ << ","
						<< xm.face_clr_.g_ << ","
						<< xm.face_clr_.b_ << ","
						<< xm.face_clr_.a_
						<< "]";

					oss << " pow:" << xm.pow_;

					oss << "specular clr:["
						<< xm.specular_clr_.r_ << ","
						<< xm.specular_clr_.g_ << ","
						<< xm.specular_clr_.b_ 
						<< "]";

					oss << "emissive clr:["
						<< xm.emissive_clr_.r_ << ","
						<< xm.emissive_clr_.g_ << ","
						<< xm.emissive_clr_.b_
						<< "]";

					oss << xm.tex_.filename_;
				}
				L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
			}
		}
	}

	{
		for(std::map<std::string, frame_info_t>::const_iterator it = this->map_frame_.begin();
			it != this->map_frame_.end(); it ++)
		{
			std::ostringstream oss;
			const frame_info_t& fi = it->second;
			oss << "frame:" << fi.name_ << " parent:" << fi.parent_;

			std::string m_info;
			fi.m_.dump(m_info);

			oss << "\r\ntrans matrix:" << m_info << "\r\n";

			oss << "sub:\r\n";
			for(std::list<std::string>::const_iterator it_lst = fi.lst_sub_.begin();
				it_lst != fi.lst_sub_.end(); it_lst ++)
			{
				oss << *it_lst << ",";
			}

			oss << "\r\nend: " << fi.name_ << "---------------------\r\n\r\n";
			L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
		}
	}

	{
		for(std::list<x_animation_set>::const_iterator it = this->lst_ani_set_.begin();
			it != this->lst_ani_set_.end(); it ++)
		{
			const x_animation_set& ani_set = *it;
			L3_LOG_DEBUG(MODULE_ENG, (ani_set.name_.c_str()));
			for(std::list<x_animation>::const_iterator it1 = ani_set.lst_ani_.begin();
				it1 != ani_set.lst_ani_.end(); it1 ++)
			{
				const x_animation& ani = *it1;

				std::ostringstream oss;
				oss << ani.name_ << " " << ani.frame_name_ << " " << ani.lst_key_.size() << "\r\n";

				for(std::list<x_animation_key>::const_iterator it2 = ani.lst_key_.begin();
					it2 != ani.lst_key_.end(); it2 ++)
				{
					const x_animation_key& ani_key = *it2;

					oss << "type:" << ani_key.type_ << " key count" << ani_key.lst_key_.size() << "\r\n";

					for(std::list<x_time_float_key_t>::const_iterator it3 = ani_key.lst_key_.begin();
						it3 != ani_key.lst_key_.end(); it3 ++)
					{
						const x_time_float_key_t& t_key = *it3;

						oss << "time:" << t_key.t_ << " ";

						for(l3_int32 i = 0; i < (l3_int32)(t_key.float_keys_.size()); i ++)
						{
							char actemp[256] = {0};
							::_snprintf_s(actemp, sizeof(actemp) - 1, "%f ", t_key.float_keys_[i]);
							oss << actemp << " ";
							//L3_LOG_DEBUG(MODULE_ENG, ("%f", t_key.float_keys_[i]));
						}

						oss << "\r\n";
					}
				}

				L3_LOG_DEBUG(MODULE_ENG, (oss.str().c_str()));
			}
		}
	}

}

}
