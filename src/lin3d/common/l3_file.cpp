/**
* @file l3_file.cpp
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
#include "l3_file.h"
#include <sys/types.h>
#include <sys/stat.h>

namespace l3eng{

common_file::common_file():f_(NULL)
{}

common_file::~common_file()
{
	if(this->f_)
		fclose(this->f_);
}

l3_int32 common_file::open(const std::string& path)
{
	if(this->f_)
		fclose(this->f_);

	this->f_ = NULL;

	fopen_s(&this->f_, path.c_str(), "rb");
	if(NULL == this->f_)
		return -1;

	this->path_ = path;

	return 0;
}

l3_int32 common_file::read(void * data, l3_uint32 data_sz)
{
	if(NULL == this->f_)
		return -1;

	l3_int32 ret = (l3_int32)fread(data, 1, data_sz, this->f_);

	if(ret < 0)
		return -1;

	return ret;
}

l3_int32 common_file::get_file_size()
{
	if(NULL == this->f_)
		return -1;

	struct _stat s;
	if(0 != _stat(this->path_.c_str(), &s))
		return -1;

	return s.st_size;
}

}
