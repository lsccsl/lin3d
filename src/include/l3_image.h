/**
* @file l3_image.h
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
#ifndef __L3ENG_L3_IMAGE_H__
#define __L3ENG_L3_IMAGE_H__

#include "l3_type.h"
#include "l3_dllexp.h"
#include "l3_ref_ptr.h"

namespace l3eng{

enum e_pixel_format{
	E_PIXEL_LUMINANCE_8BIT,
	E_PIXEL_RGB,
	E_PIXEL_RGBA,
};

class image;

class LIN3D_EX_PORT l3_image
{
public:

public:

	l3_image();
	~l3_image();

public:

	l3_int32 load(const char * file_path);
	image& img();

private:

	image * img_;
};

}

#endif
