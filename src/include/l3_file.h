/**
* @file l3_file.h
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
#ifndef __L3ENG_FILE_H__
#define __L3ENG_FILE_H__

#include <stdio.h>
#include <string>
#include "l3_type.h"

namespace l3eng{

class common_file
{
public:

	common_file();

	virtual ~common_file();

public:

	virtual l3_int32 open(const std::string& path);

	virtual l3_int32 read(void * data, l3_uint32 data_sz);

	virtual l3_int32 get_file_size();

private:

	FILE * f_;
	std::string path_;
};

}

#endif
