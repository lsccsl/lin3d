/**
* @file log.cpp
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
#include "l3_log.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <windows.h>
#include <assert.h>

namespace l3eng{


struct log_conf_t
{
	log_conf_t():log_level_(0),b_sync_(1),log_to_(255){
		strncpy_s(this->log_file_, "log.log", sizeof(this->log_file_));
	}

	char log_file_[32];
	int log_level_;
	int log_to_;
	int b_sync_;

	std::list<std::string> lst_log_;
};

struct log_conf
{
	static const int MAX_LOG_MODULE = 32;

	log_conf(){
		strcpy_s(log_base_path_, "./log");

		for(int i = 0; i < sizeof(this->lf_)/sizeof(this->lf_[0]); i ++)
		{
			_snprintf_s(this->lf_[i].log_file_, sizeof(this->lf_[i].log_file_) - 1, "%s/log_%d.log", log_base_path_, i);
			this->lf_[i].log_level_ = 0;
		}
	}

	char log_base_path_[260];
	log_conf_t lf_[MAX_LOG_MODULE];
};

static log_conf __lgcf_;

bool log_print::check_log_level(const int module, const int lv)
{
	assert(module < sizeof(__lgcf_.lf_)/sizeof(__lgcf_.lf_[0]));
	return __lgcf_.lf_[module].log_level_ <= module;
}

const char * log_print::get_log_dir_base()
{
	return __lgcf_.log_base_path_;
}

void log_print::set_log_dir_base(const char * base_dir)
{
	strcpy_s(__lgcf_.log_base_path_, base_dir ? base_dir : "./");

	for(int i = 0; i < sizeof(__lgcf_.lf_)/sizeof(__lgcf_.lf_[0]); i ++)
		_snprintf_s(__lgcf_.lf_[i].log_file_, sizeof(__lgcf_.lf_[i].log_file_) - 1, "%s/log_%d.log", __lgcf_.log_base_path_, i);
}

void log_print::write_log(const char * fmt, ...)
{
	va_list var;

	char actemp[8192];
	actemp[sizeof(actemp) - 1] = 0;

	struct tm ptm;
	timeb tb;
	{
		ftime(&tb);

		time_t t = tb.time;
		localtime_s(&ptm, &t);
	}
	
	int thread_id = GetCurrentThreadId();
	int ret = ::_snprintf_s(actemp, sizeof(actemp) - 32, sizeof(actemp) - 32, "[%04d-%02d-%02d %02d:%02d:%02d %03d]%s:%d tid:%d ",
		ptm.tm_year + 1900,
		ptm.tm_mon + 1,
		ptm.tm_mday,
		ptm.tm_hour,
		ptm.tm_min,
		ptm.tm_sec,
		tb.millitm,
		this->func_, this->line_, thread_id);

	va_start(var, fmt);
	int ret1 = ::_vsnprintf_s(actemp + ret, sizeof(actemp) - 32 - ret, sizeof(actemp) - 32 - ret, fmt, var);
	va_end(var);

	if(ret1 >= 0)
	{
		ret += ret1;
	}
	else
	{
		ret = sizeof(actemp) - 32;

		ret += _snprintf_s(actemp + ret, 31, 31,"\r\n[more...]\r\n");
	}

	actemp[ret] = '\r';
	actemp[ret + 1] = '\n';
	actemp[ret + 2] = 0;

	log_conf_t& lf = __lgcf_.lf_[this->module_];

	if(lf.log_to_ & LOG_TO_SCREEN)
	{
		printf(actemp);
	}

	if(lf.log_to_ & LOG_TO_MEM)
	{
		lf.lst_log_.push_back(actemp);
	}

	if(lf.log_to_ & LOG_TO_FILE)
	{
		FILE * pfile = NULL;
		fopen_s(&pfile, lf.log_file_, "a+");
		if(NULL == pfile)
			return;

		fprintf(pfile, actemp);
		fflush(pfile);
		fclose(pfile);
	}
}

void test_print_log()
{
	L3_LOG(1, 0, ("aa%d %s", 1, "aa"));
	L3_LOG_DEBUG(1, ("aa%d %s", 1, "aa"));
	L3_LOG_TRACE(1, ("aa%d %s", 1, "aa"));
	L3_LOG_INFO(1, ("aa%d %s", 1, "aa"));
	L3_LOG_WARN(1, ("aa%d %s", 1, "aa"));
	L3_LOG_ERR(1, ("aa%d %s", 1, "aa"));
}

void L3_LOG_DUMP_MEM()
{
	for(int i = 0; i < sizeof(__lgcf_.lf_)/sizeof(__lgcf_.lf_[0]); i++)
	{
		log_conf_t& lf = __lgcf_.lf_[i];

		if(lf.lst_log_.empty())
			continue;

		FILE * pfile = NULL;
		fopen_s(&pfile, lf.log_file_, "a+");
		if(NULL == pfile)
			continue;
		for(std::list<std::string>::iterator it = lf.lst_log_.begin();
			it != lf.lst_log_.end(); it ++)
		{
			fprintf(pfile, it->c_str());
		}
		fflush(pfile);
		fclose(pfile);

		lf.lst_log_.clear();
	}
}

extern void L3_LOG_ENG_DST(int to)
{
	__lgcf_.lf_[MODULE_ENG].log_to_ = to;
}

}
