/**
* @file l3_image.cpp
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
#include "l3_image.h"
#include "image.h"

namespace l3eng{

l3_image::l3_image():img_(new image)
{
	
}

l3_image::~l3_image()
{
}

l3_int32 l3_image::load(const char * file_path)
{
	return img_->load(file_path);
}

image& l3_image::img()
{
	return *img_;
}

}

