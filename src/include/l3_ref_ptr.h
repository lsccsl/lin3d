/**
* @file ref_ptr.h
*
* @author lin shao chuan (email:lsccsl@tom.com)
*
* @brief if it works, it was written by lin shao chuan, if not, i don't know who wrote it
*     指针引用计数封装,缺省情况下,为多线程不安全
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
#ifndef __L3ENG_REF_PTR_H__
#define __L3ENG_REF_PTR_H__

#include <assert.h>
#include <stdlib.h>
#include "l3_null_lock.h"
#include "l3_type.h"

namespace l3eng{

/* @brief  */
template<class __T_, class __lock_>
class ref_ptr
{
public:

	ref_ptr():count_(NULL),ptr_(NULL),l_(NULL)
	{}

	virtual ~ref_ptr(){
		release();
	}

	template<class __X_>
	explicit ref_ptr(__X_ * ptr):ptr_(ptr),count_(new l3_int32(1)),l_(new __lock_)
	{
		assert(ptr);
	}

	ref_ptr(const ref_ptr& r):ptr_(NULL), count_(NULL),l_(NULL)
	{
		this->assign_static_cast(r);
	}

	ref_ptr& operator = (const ref_ptr& r)
	{
		this->assign_static_cast(r);
		return *this;
	}

	template<class __X_, class __XLOCK__>
	ref_ptr(const ref_ptr<__X_, __XLOCK__>& r):ptr_(NULL),l_(NULL),count_(NULL)
	{
		this->assign_static_cast(r);
	}

	template<class __X_, class __XLOCK__>
	ref_ptr& operator=(const ref_ptr<__X_, __XLOCK__>& r)
	{
		this->assign_static_cast(r);
		return *this;
	}

	template<class __X_, class __XLOCK__>
	ref_ptr& assign_static_cast(const ref_ptr<__X_, __XLOCK__>& r)
	{
		//assert(r.l_);
		//assert(r.count_);
		//assert(r.ptr_);
		if(r.ptr_ == NULL)
		{
			this->release();

			this->l_ = r.l_;
			this->ptr_ = static_cast<__T_ *>(r.ptr_);
			this->count_ = r.count_;

			return *this;
		}
		else
		{
			if(r.ptr_ == this->ptr_)
				return *this;

			release();

			r.l_->lock();

			this->l_ = r.l_;
			this->ptr_ = static_cast<__T_ *>(r.ptr_);
			this->count_ = r.count_;

			(*this->count_) ++;

			r.l_->unlock();
	 
			return *this;
		}
	}

	template<class PTR>
	inline PTR cast_static()
	{
		PTR ptr;

		ptr.assign_static_cast(*this);

		return ptr;
	}

	inline __T_* operator->() const { assert(ptr_); return ptr_; }
	inline __T_* get() const { return ptr_; }
	inline __T_& operator*() const { assert(this->ptr_); return *ptr_; }

	template<class __X_, class __XLOCK__>
	inline bool operator==(const ref_ptr<__X_, __XLOCK__>& r)const{ return this->ptr_ == r.ptr_; }

	template<class __X_, class __XLOCK__>
	inline bool operator!=(const ref_ptr<__X_, __XLOCK__>& r)const{ return this->ptr_ != r.ptr_; }

	template<class __X_, class __XLOCK__>
	inline bool operator<(const ref_ptr<__X_, __XLOCK__>& r)const{ return this->ptr_ < r.ptr_; }

	inline bool is_null(void) const { return this->ptr_ == NULL; }

	inline void set_null(void){
		if(this->ptr_)
		{
			release();
		}
	}

protected:

	void release()
	{
		bool need_delete = false;

		if(this->l_)
		{
			assert(this->count_);
			assert(this->ptr_);

			this->l_->lock();

			(*this->count_) --;

			if(0 == *this->count_)
				need_delete = true;

			this->l_->unlock();

			if(need_delete)
			{
				assert(this->count_);
				assert(this->l_);

				delete this->ptr_;
				delete this->count_;
				delete this->l_;
			}
		}

		this->l_ =	NULL;
		this->ptr_ = NULL;
		this->count_ = NULL;
	}

protected:

	template<class __X_, class __lock_> friend class ref_ptr;

	l3_int32 * count_;/* 或者模仿boost InterlockedIncrement ? */
	__T_ * ptr_;
	__lock_ * l_;
};


/* @brief  */
template<class __X_>
class ref_ptr_thread : public ref_ptr<__X_, null_lock>
{
public:

	ref_ptr_thread():ref_ptr<__X_, null_lock>()
	{}

	template<class __Y_>
	explicit ref_ptr_thread(__Y_ * ptr):ref_ptr<__X_, null_lock>(ptr){}

	template<class __X_>
	ref_ptr_thread(const ref_ptr_thread<__X_>& r)
	{
		this->assign_static_cast(r);
	}

	template<class __X_>
	ref_ptr_thread& operator=(const ref_ptr_thread<__X_>& r){
		this->assign_static_cast(r);
		return *this;
	}
};

}

#endif
