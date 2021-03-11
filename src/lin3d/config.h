/**
* @file config.h
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
#ifndef __L3ENG_CONFIG_H__
#define __L3ENG_CONFIG_H__

#include <list>
#include <string>
#include "l3_type.h"

namespace l3eng{

class common_file;

class config
{
public:

	config();
	~config(){}

	void add_sys_folder(const std::string& folder_name);

	//std::list<std::string>& lst_sys_folder(){ return this->lst_sys_folder_; }

	l3_int32 read_file(std::string& content, const std::string& file_path);

	l3_int32 find_file_full_path(const std::string& file_short, std::string& file_path_full);

	const std::string& get_default_fresnel_tex();

private:

	std::list<std::string> lst_sys_folder_;
};

}

#endif





