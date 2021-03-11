/**
* @file base_obj_mgr.h
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
#ifndef __L3ENG_OBJ_MGR_H__
#define __L3ENG_OBJ_MGR_H__

#include <map>
#include "l3_ref_ptr.h"
#include "l3_type.h"
#include "base_obj.h"

namespace l3eng{

class base_obj_mgr
{
public:

	static const l3_int64 MAX_OBJ_ID = 0x7fffffffffffffff;

public:

	base_obj_mgr():id_seed_(0){}

	~base_obj_mgr(){}

	/* @brief create object with no param */
	template<class __T_>
	ref_ptr_thread<__T_> create_obj(){

		ref_ptr_thread<__T_> ptr(new __T_);

		this->_add_obj<__T_>(ptr);

		return ptr;
	}

	/* @brief one param */
	template<class __T_, class __P1_>
	ref_ptr_thread<__T_> create_obj(__P1_ p){

		ref_ptr_thread<__T_> ptr(new __T_(p));

		this->_add_obj<__T_>(ptr);

		return ptr;
	}

	/* @brief two */
	template<class __T_, class __P1_, class __P2_>
	ref_ptr_thread<__T_> create_obj(__P1_ p1, __P2_ p2){

		ref_ptr_thread<__T_> ptr(new __T_(p1, p2));

		this->_add_obj<__T_>(ptr);

		return ptr;
	}

	/* @brief three */
	template<class __T_, class __P1_, class __P2_, class __P3_>
	ref_ptr_thread<__T_> create_obj(__P1_ p1, __P2_ p2, __P3_ p3){

		ref_ptr_thread<__T_> ptr(new __T_(p1, p2, p3));

		this->_add_obj<__T_>(ptr);

		return ptr;
	}

	/* @brief four */
	template<class __T_, class __P1_, class __P2_, class __P3_, class __P4_>
	ref_ptr_thread<__T_> create_obj(__P1_ p1, __P2_ p2, __P3_ p3, __P4_ p4){

		ref_ptr_thread<__T_> ptr(new __T_(p1, p2, p3, p4));

		this->_add_obj<__T_>(ptr);

		return ptr;
	}

	template<class __T_>
	l3_int32 get_obj(OBJ_ID id, ref_ptr_thread<__T_>& ptr)
	{
		std::map<OBJ_ID, ref_ptr_thread<base_obj> >::iterator it = this->map_obj_.find(id);
		if(this->map_obj_.end() == it)
			return -1;

		{
			__T_ * p = (__T_ *)it->second.get();
			OBJ_TYPE a = __T_::_OBJ_TYPE_;
			OBJ_TYPE b = p->obj_type();
			if(!p->is_my_type(__T_::_OBJ_TYPE_))
			{
				printf("no the type %d - %d", __T_::_OBJ_TYPE_, p->obj_type());
				return -1;
			}
		}

		ptr.assign_static_cast(it->second);
		return 0;
	}

	void obj_del(l3_int64 id)
	{
		this->map_obj_.erase(id);
	}

private:
	
	template<class __T_>
	const void _add_obj(ref_ptr_thread<__T_> obj){

		this->id_seed_ ++;
		if(this->id_seed_ > MAX_OBJ_ID)/* todo */
			this->id_seed_ = 1;

		obj->obj_id(this->id_seed_);
		this->map_obj_[this->id_seed_].assign_static_cast(obj);
	}

private:

	OBJ_ID id_seed_;

	std::map<OBJ_ID, ref_ptr_thread<base_obj> > map_obj_;
};

}

#endif
