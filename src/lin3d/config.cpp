/**
* @file config.cpp
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
#include "config.h"
#include "l3_file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "l3_log.h"

namespace l3eng{

config::config()
{
	this->lst_sys_folder_.push_back("./");
	this->lst_sys_folder_.push_back("../");
	this->lst_sys_folder_.push_back("../../");
	this->lst_sys_folder_.push_back("../../../");
	this->lst_sys_folder_.push_back("../../../resource/");
	this->lst_sys_folder_.push_back("../resource/");
}

void config::add_sys_folder(const std::string& folder_name)
{
	this->lst_sys_folder_.push_back(folder_name);
}

l3_int32 config::read_file(std::string& content, const std::string& file_path)
{
	common_file file;
	std::string file_full_path;
	for(std::list<std::string>::iterator it = this->lst_sys_folder_.begin();
		it != this->lst_sys_folder_.end(); it ++)
	{
		file_full_path = *it;
		file_full_path.append(file_path);

		if(0 != file.open(file_full_path))
			continue;

		content.resize(file.get_file_size());
		if(file.read((void *)(content.data()), (l3_uint32)content.size()) >= 0)
			return 0;
	}

	return -1;
}

l3_int32 config::find_file_full_path(const std::string& file_short, std::string& file_full_path)
{
	for(std::list<std::string>::iterator it = this->lst_sys_folder_.begin();
		it != this->lst_sys_folder_.end(); it ++)
	{
		file_full_path = *it;
		file_full_path.append(file_short);

		struct _stat buffer;
		if(0 != ::_stat(file_full_path.c_str(), &buffer))
			continue;

		return 0;
	}

	L3_LOG_ENG_DEBUG(("fail find tex %s~~~~~~~\r\n", file_short.c_str()));

	return -1;
}

const std::string& config::get_default_fresnel_tex()
{
	static std::string __str_fresnel_ = "../resource/WaterFresnel.psd";
	return __str_fresnel_;
}

}

