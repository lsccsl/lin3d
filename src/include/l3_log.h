/**
* @file log.h
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
#ifndef __L3ENG_LOG_H__
#define __L3ENG_LOG_H__

#include <string.h>
#include <stdio.h>
#include <string>
#include <list>
#include "l3_dllexp.h"

namespace l3eng{

#define LOG_TO_SCREEN (0x01)
#define LOG_TO_FILE (0x02)
#define LOG_TO_MEM (0x04)

class LIN3D_EX_PORT log_print
{
public:

	log_print(const char * func, const int line):module_(0),func_(func),line_(line){}

	static void set_log_dir_base(const char * base_dir);

public:

	void write_log(const char * format, ...);

	static const char * get_log_dir_base();
	static bool check_log_level(const int module, const int lv);

	int module_;
	const char * func_;
	int line_;
};

#define L3_LOG(__module__, __level__, __x__) do{\
		int __real_module__ = __module__ & 0x1f;\
		if(l3eng::log_print::check_log_level(__real_module__, __level__)){\
			l3eng::log_print __lp__(__FUNCTION__, __LINE__);\
			__lp__.module_ = __real_module__;\
			__lp__.write_log __x__;\
		}\
	}\
	while(0)

#define L3_LOG_DEBUG(__module__, __x__) L3_LOG(__module__, 0, __x__)
#define L3_LOG_TRACE(__module__, __x__) L3_LOG(__module__, 1, __x__)
#define L3_LOG_INFO(__module__, __x__) L3_LOG(__module__, 2, __x__)
#define L3_LOG_WARN(__module__, __x__) L3_LOG(__module__, 3, __x__)
#define L3_LOG_ERR(__module__, __x__) L3_LOG(__module__, 4, __x__)

#define L3_LOG_BASE() (l3eng::log_print::get_log_dir_base())

#define MODULE_ENG (1)

#define L3_LOG_ENG_DEBUG(__x__) L3_LOG(MODULE_ENG, 0, __x__)
#define L3_LOG_ENG_TRACE(__x__) L3_LOG(MODULE_ENG, 1, __x__)
#define L3_LOG_ENG_INFO(__x__) L3_LOG(MODULE_ENG, 2, __x__)
#define L3_LOG_ENG_WARN(__x__) L3_LOG(MODULE_ENG, 3, __x__)
#define L3_LOG_ENG_ERR(__x__) L3_LOG(MODULE_ENG, 4, __x__)

extern void L3_LOG_ENG_DST(int to);

extern void L3_LOG_DUMP_MEM();

}

#endif
