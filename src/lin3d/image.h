/**
* @file image.h
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
#ifndef __L3ENG_IMAGE_H__
#define __L3ENG_IMAGE_H__

#include <string>
#include <vector>
#include "l3_type.h"
#include "base_obj.h"
#include "l3_ref_ptr.h"
#include "l3_image.h"

//#pragma warning(disable:4819)

namespace l3eng{

class color;

class image : public base_obj
{
public:

	typedef ref_ptr_thread<image> ptr;

	OBJ_TYPE_DEFINE(base_obj, OBJ_IMG)

public:

	static void _init();

public:

	image(const char * file_path = NULL);

	virtual ~image();

	/* @brief 创建一个空图像 */
	void create(const l3_uint32 width, const l3_uint32 height, const e_pixel_format format);

	l3_uint32 width()const{ return this->width_; }
	l3_uint32 height()const{ return this->height_; }
	l3_uint32 bpp()const{ return this->bpp_; }
	e_pixel_format pixel_format()const{ return this->pixel_format_; }
	const std::string& file_path()const{ return this->file_path_; }

	std::vector<l3_uint8>& data()const{ return this->vd_; }

	virtual l3_int32 load(const std::string& file_path);
	virtual l3_int32 save(const std::string& file_path);

	l3_int32 load_raw(const std::string& file_path, const l3_uint32 width, const l3_uint32 height, const l3_uint32 bpp);
	l3_int32 load_raw_memory(const std::vector<l3_uint8>& vdata,
		const l3_uint32 width, const l3_uint32 height, const e_pixel_format pix_format = E_PIXEL_LUMINANCE_8BIT);

	static void get_rand_normal_2d_name(std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_int32 rand_seed = 0, const l3_uint32 part = 16);
	l3_int32 load_rand_normal_2d(const std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_int32 rand_seed = 0, const l3_uint32 part = 16);

	static void get_rand_img_name(std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_int32 rand_seed = 0);
	l3_int32 load_rand_img(const std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_int32 rand_seed = 0);

	l3_int32 gen_normal_img_lum8bit(image& img_dst, l3_f32 scale = 1.0);
	l3_int32 gen_normal_img(image& img_dst, l3_f32 scale = 1.0);

	/* @brief 生成高斯 */
	l3_int32 gen_gussian_img(const std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_f32 pA = 0.5f, const l3_f32 pB = 0.0f, const l3_f32 vA = 0.0f, const l3_f32 vB = 0.0f);

	/*
	* @param radius1:圆的直径
	* @param radius2:开始衰减
	*/
	l3_int32 load_round_cake(const std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_uint32 radius1 = 32,
		const l3_uint32 radius2 = 16);
	static void get_round_cake_name(std::string& file_virtual_name,
		const l3_uint32 width = 64, const l3_uint32 height = 64,
		const l3_uint32 radius1 = 32,
		const l3_uint32 radius2 = 16);


	/* @brief 获取子图像 */
	l3_int32 get_sub_img(image& img_dst,
		const l3_uint32 pos_x, const l3_uint32 pos_y,
		const l3_uint32 width, const l3_uint32 height);

	/* @brief rgb转黑白(0.299*r + 0.587*g + 0.114*b) */
	l3_int32 conver_to_gray(image& img_dst);
	/* @brief 将位图扩展成rgba */
	l3_int32 conver_to_rgba(image& img_dst);

	/* @brief 添加alpha赋 */
	l3_int32 apply_alpha(image& img_alpha);

	//l3_uint32 cal_bytes_pitch()
	//{
	//	l3_uint32 pitch_bytes = this->width_ * (this->bpp_ / 8);
	//	
	//	if(0 != pitch_bytes % 4)
	//		pitch_bytes = 4 - pitch_bytes % 4 + pitch_bytes;

	//	return pitch_bytes;
	//}

private:

	l3_int32 _get_pixel_lum8bit(l3_int32 x, l3_int32 y, l3_f32 scale, l3_f32& val);
	l3_int32 _get_pixel(l3_int32 x, l3_int32 y, l3_f32 scale, color& clr);

	static l3_int32 _get_bpp_by_format(const e_pixel_format format, l3_uint32& bpp)
	{
		bpp = 8;
		switch(format)
		{
		case E_PIXEL_LUMINANCE_8BIT:
			bpp = 8;
			break;

		case E_PIXEL_RGB:
			bpp = 24;
			break;

		case E_PIXEL_RGBA:
			bpp = 32;
			break;

		default:
			return -1;
			break;
		}

		return 0;
	}
	l3_uint32 _cal_bytes_per_pix()
	{
		if(this->bpp_ <= 0)
			this->bpp_ = _get_bpp_by_format(pixel_format_, this->bpp_);
		return this->bpp_ / 8;
	}
	l3_uint32 _cal_bytes_pitch()
	{
		if(this->pitch_ <= 0)
		{
			this->pitch_ = _cal_bytes_per_pix() * this->width_;
			if(0 != this->pitch_ % 4)
				this->pitch_ = 4 - this->pitch_ % 4 + this->pitch_;
		}

		return this->pitch_;
	}

private:

	mutable std::vector<l3_uint8> vd_;

	l3_uint32 width_;
	l3_uint32 height_;
	l3_uint32 bpp_;
	l3_uint32 pitch_;

	e_pixel_format pixel_format_;

	std::string file_path_;
};

}

#endif


















