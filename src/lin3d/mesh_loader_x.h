/**
* @file mesh_loader_x.h
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
#ifndef __L3ENG_MESH_LOADER_X_H__
#define __L3ENG_MESH_LOADER_X_H__

#include <string>
#include <vector>
#include "l3_type.h"
#include "mesh.h"
#include "matrix4.h"

namespace l3eng{

struct x_face
{
	l3_int32 idx1_;
	l3_int32 idx2_;
	l3_int32 idx3_;
};

struct x_clr_rgba
{
	l3_f32 r_;
	l3_f32 g_;
	l3_f32 b_;
	l3_f32 a_;
};

struct x_clr_rgb
{
	l3_f32 r_;
	l3_f32 g_;
	l3_f32 b_;
};

struct x_texture_filename
{
	std::string filename_;
};

struct x_material
{
	x_clr_rgba face_clr_;    //材质颜色
	l3_f32 pow_;            //高光强度
	x_clr_rgb specular_clr_; //反射光
	x_clr_rgb emissive_clr_; //自发光

	x_texture_filename tex_; //纹理路径
};

struct x_material_list
{
	std::vector<l3_int32> v_material_list_;
	std::vector<x_material> v_material_;
};

struct x_skin_weight
{
	std::string ref_frame_name_;
	std::vector<l3_int32> v_vertex_idx_;
	std::vector<l3_f32> v_weight_;
	matrix4 reserve_m_;
};

struct x_texcoord
{
	std::vector<vertex_texcoord_t> v_texcoord_;
};

struct x_mesh
{
	std::vector<vector3> vertex_;
	std::vector<x_face> face_;
	std::list<x_skin_weight> lst_sw_;
	x_texcoord texcoord_;
	x_material_list material_list_;
};

struct x_time_float_key_t
{
	l3_int32 t_;
	std::vector<l3_f32> float_keys_;
};

struct x_animation_key
{
	enum e_x_animation_type
	{
		E_X_ANIMATION_ROTATION,
		E_X_ANIMATION_SCALE,
		E_X_ANIMATION_MOVE,
		E_X_ANIMATION_TRAN_MATRIX,
	};

	e_x_animation_type type_;

	std::list<x_time_float_key_t> lst_key_;
};
struct x_animation
{
	std::string name_;/* 动画本身的名字 */
	std::string frame_name_;/* 引用的frame名字 */
	std::list<x_animation_key> lst_key_;
};

struct x_animation_set
{
	std::string name_;
	std::list<x_animation> lst_ani_;
};

struct frame_info_t
{
	std::string parent_;
	std::string name_;
	matrix4 m_;
	x_mesh xmsh_;

	std::list<std::string> lst_sub_;
};

class mesh_loader_x
{
public:

	enum enum_float_sz
	{
		X_FLOAT_32,
		X_FLOAT_64,
	};

	mesh_loader_x():current_line_(0){}
	virtual ~mesh_loader_x(){}

public:

	void _debug_dump_frame_all()const;

	virtual l3_int32 load_mesh(const std::string& file_path);

	const std::map<std::string, frame_info_t>& map_frame()const{ return this->map_frame_; }

	const std::list<x_animation_set>& lst_ani_set()const{ return this->lst_ani_set_; }

private:

	struct token_info
	{
		token_info():line_(0),row_(0){}

		std::string token_;
		l3_int32 line_;
		l3_int32 row_;
	};

private:

	l3_int32 _parse(const char * p, const l3_int32 sz);
	const char * _read_line(const char * p, const l3_int32 sz);

	void _process_line(const char * p, const l3_int32 sz);
	const char * _get_token(const char * p, const l3_int32 sz, std::string& token);
	const l3_bool _check_comment(const char * p, const l3_int32 sz);


	l3_int32 _parse_token();

	l3_int32 _parse_unknown(std::list<token_info>::iterator& it);
	l3_int32 _parse_template(std::list<token_info>::iterator& it);
	l3_int32 _parse_frame(std::list<token_info>::iterator& it, std::string& frame_name, const std::string& parent = "");
	l3_int32 _parse_mesh(std::list<token_info>::iterator& it, x_mesh& xmsh);
	l3_int32 _parse_skin_weight(std::list<token_info>::iterator& it, x_skin_weight& sw);
	l3_int32 _parse_mesh_texture_coords(std::list<token_info>::iterator& it, x_texcoord& tc);
	l3_int32 _parse_MeshMaterialList(std::list<token_info>::iterator& it, x_material_list& m_list);
	l3_int32 _parse_Material(std::list<token_info>::iterator& it, x_material& mtrl);
	l3_int32 _parse_TextureFilename(std::list<token_info>::iterator& it, x_texture_filename& tex_filename);

	l3_int32 _parse_ani_set(std::list<token_info>::iterator& it, x_animation_set& ani_set);
	l3_int32 _parse_animation(std::list<token_info>::iterator& it, x_animation& ani);
	l3_int32 _parse_animation_key(std::list<token_info>::iterator& it, x_animation_key& ani_key);
	l3_int32 _parse_animation_time_float_keys(std::list<token_info>::iterator& it, x_time_float_key_t& x_key);

	l3_int32 _parse_coords2d(std::list<token_info>::iterator& it, vertex_texcoord_t& coord);
	l3_int32 _parse_frame_matrix(std::list<token_info>::iterator& it, matrix4& m);
	l3_int32 _parse_matrix(std::list<token_info>::iterator& it, matrix4& m);
	l3_int32 _parse_vector(std::list<token_info>::iterator& it, vector3& v);
	l3_int32 _parse_float(const std::string& token, l3_f32& f);
	l3_int32 _parse_int(const std::string& token, l3_int32& val);
	l3_int32 _parse_mesh_face(std::list<token_info>::iterator& it, x_face& face);

	l3_int32 _parse_clr_rgba(std::list<token_info>::iterator& it, x_clr_rgba& clr_rgba);
	l3_int32 _parse_clr_rgb(std::list<token_info>::iterator& it, x_clr_rgb& clr_rgb);
	l3_int32 _parse_string(std::list<token_info>::iterator& it, std::string& str);

	l3_int32 _get_symbol(std::list<token_info>::iterator& it, const char * symbol);

private:

	enum_float_sz f_sz_;
	l3_int32 ver1_;
	l3_int32 ver2_;
	l3_int32 current_line_;

	std::list<token_info> lst_token_;

	std::map<std::string, frame_info_t> map_frame_;
	/* Material MeshMaterialList */
	std::list<x_animation_set> lst_ani_set_;
};

}

#endif
