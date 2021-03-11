/**
* @file mesh.h
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
#ifndef __L3ENG_MESH_H__
#define __L3ENG_MESH_H__

#include <list>
#include "vertex_buf.h"
#include "index_buf.h"
#include "base_obj.h"

namespace l3eng
{

class mesh : public base_obj
{
public:

    typedef ref_ptr_thread<mesh> ptr;
	OBJ_TYPE_DEFINE(base_obj, OBJ_MESH)

public:

	mesh(){}

	virtual ~mesh(){}

	vertex_buf& vb()const{ return this->vb_; }
	index_buf& ib()const{ return this->ib_; }

	void cal_normal();

	void deep_copy(const mesh& msh);

private:

	/* @brief ¶¥µã»º´æ */
	mutable vertex_buf vb_;
	/* @brief ¶¥Õ¼Ë÷Òý»º´æ */
	mutable index_buf ib_;
};

}

#endif
