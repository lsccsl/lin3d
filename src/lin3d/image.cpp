/**
* @file image.cpp
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

#pragma warning(disable:4819)

#include "image.h"
#include <assert.h>
#include "l3_file.h"
#include "FreeImage.h"
#include "l3_vector3.h"
#include "l3_log.h"
#include "l3_color.h"

namespace l3eng{

image::image(const char * file_path):pixel_format_(E_PIXEL_LUMINANCE_8BIT),
	width_(0),
	height_(0),
	bpp_(0),
	pitch_(0),
	file_path_(file_path ? file_path : "")
{}

image::~image()
{}

void image::_init()
{
	FreeImage_Initialise(TRUE); 
}

void image::create(const l3_uint32 width, const l3_uint32 height, const e_pixel_format format)
{
	this->width_ = width;
	this->height_ = height;
	this->pixel_format_ = format;

	image::_get_bpp_by_format(format, this->bpp_);

	l3_uint32 pitch = this->_cal_bytes_pitch();
	//this->vd_.resize(this->width_ * this->height_ * (this->bpp_ / 8));
	this->vd_.resize(pitch * this->height_);
}

l3_int32 image::load(const std::string& file_path)
{
	if(file_path.empty())
		return -1;

	this->file_path_ = file_path;

	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_path.c_str(), 0);
	FIBITMAP * bmp = FreeImage_Load(fifmt, file_path.c_str(), BMP_DEFAULT);
	if(NULL == bmp)
		return -1;

	FREE_IMAGE_TYPE img_type = FreeImage_GetImageType(bmp);
	FREE_IMAGE_COLOR_TYPE clr_type = FreeImage_GetColorType(bmp);
	this->bpp_ = FreeImage_GetBPP(bmp);

	if(FIT_BITMAP != img_type)
	{
		assert(0);
		return -1;
	}

	if(this->bpp_ < 8)
	{
		FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(bmp);
		FreeImage_Unload(bmp);
		bmp = newBitmap;
	}
	else
	{
		switch(clr_type)
		{
		case FIC_MINISWHITE:
		case FIC_MINISBLACK:
			{
				FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(bmp);
				FreeImage_Unload(bmp);
				bmp = newBitmap;
			}
			break;

		case FIC_PALETTE:
		case FIC_CMYK:
			{
				FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(bmp);
				FreeImage_Unload(bmp);
				bmp = newBitmap;
			}
			break;

		default:
			break;
		}
	}
	
	this->bpp_ = FreeImage_GetBPP(bmp);
	clr_type = FreeImage_GetColorType(bmp);
	BYTE * bits = FreeImage_GetBits(bmp);
	this->width_ = FreeImage_GetWidth(bmp);
	this->height_ = FreeImage_GetHeight(bmp);

	int bytes_per_pix = 1;
	switch(this->bpp_)
	{
	case 8:
		bytes_per_pix = 1;
		this->pixel_format_ = E_PIXEL_LUMINANCE_8BIT;
		break;

	case 16:
		assert(0);
		break;

	case 24:
		bytes_per_pix = 3;
		this->pixel_format_ = E_PIXEL_RGB;
		break;

	case 32:
		bytes_per_pix = 4;
		this->pixel_format_ = E_PIXEL_RGBA;
		break;

	default:
		assert(0);
		return -1;
		break;
	}

	unsigned srcPitch = FreeImage_GetPitch(bmp);

	//this->vd_.resize(this->width_ * this->height_ * bytes_per_pix);
	this->vd_.resize(srcPitch * this->height_);	
	unsigned char * pc_image = &this->vd_[0];

	//int line = bytes_per_pix * this->width_;

	//todo:大小码
	for(l3_uint32 i = 0; i < this->height_; i ++)
	{
		int line_start = (this->height_ - i - 1) * srcPitch;
		//int line_start = i * srcPitch;
		int line_start1 = i * srcPitch;
		//int line_start1 = i * this->width_ * bytes_per_pix;
		for(l3_uint32 j = 0; j < this->width_; j++)
		{
			switch(this->pixel_format_)
			{
			case E_PIXEL_RGB:
				{
					unsigned char r = bits[line_start + j * bytes_per_pix + 0];
					unsigned char g = bits[line_start + j * bytes_per_pix + 1];
					unsigned char b = bits[line_start + j * bytes_per_pix + 2];
					pc_image[line_start1 + j * bytes_per_pix + 0] = 
						b;
					pc_image[line_start1 + j * bytes_per_pix + 1] = 
						g;
					pc_image[line_start1 + j * bytes_per_pix + 2] = 
						r;
				}
				break;

			case E_PIXEL_RGBA:
				{
					unsigned char r = bits[line_start + j * bytes_per_pix + 0];
					unsigned char g = bits[line_start + j * bytes_per_pix + 1];
					unsigned char b = bits[line_start + j * bytes_per_pix + 2];
					unsigned char a = bits[line_start + j * bytes_per_pix + 3];
					pc_image[line_start1 + j * bytes_per_pix + 3] = 
						a;

					pc_image[line_start1 + j * bytes_per_pix + 0] = 
						b;
					pc_image[line_start1 + j * bytes_per_pix + 1] = 
						g;
					pc_image[line_start1 + j * bytes_per_pix + 2] = 
						r;
				}
				break;

			case E_PIXEL_LUMINANCE_8BIT:
				pc_image[line_start1 + j] = bits[line_start + j * bytes_per_pix];
				break;
			}
		}
	}

	FreeImage_Unload(bmp);

	return 0;
}

l3_int32 image::save(const std::string& file_path)
{
	FIBITMAP * bmp = FreeImage_AllocateT(FIT_BITMAP, this->width_, this->height_, this->bpp_);

	switch(this->pixel_format_)
	{
	case E_PIXEL_LUMINANCE_8BIT:
		{
			FIBITMAP * tmp = FreeImage_ConvertToGreyscale(bmp);
			FreeImage_Unload(bmp);
			bmp = tmp;
		}
		break;

	default:
		break;
	}

	l3_int32 bytes_per_pix = this->bpp_ / 8;

	l3_uint32 pitch = bytes_per_pix * this->width_;
	if(0 != pitch % 4)
		pitch = 4 - pitch % 4 + pitch;

	/* todo 大小码有区别 */
	for(l3_uint32 i = 0; i < this->height_; i ++)
	{
		BYTE * dst_bits = FreeImage_GetScanLine(bmp, i);
		//ant_int32 line_start = bytes_per_pix * i * this->width_;
		l3_int32 line_start =  i * pitch;
		for(l3_uint32 j = 0; j < this->width_; j ++)
		{
			switch(this->pixel_format_)
			{
			case E_PIXEL_LUMINANCE_8BIT:
				dst_bits[j * bytes_per_pix] = this->vd_[line_start + j * bytes_per_pix];
				break;

			case E_PIXEL_RGB:
				dst_bits[j * bytes_per_pix + 2] = this->vd_[line_start + j * bytes_per_pix + 0];//r
				dst_bits[j * bytes_per_pix + 1] = this->vd_[line_start + j * bytes_per_pix + 1];//g
				dst_bits[j * bytes_per_pix + 0] = this->vd_[line_start + j * bytes_per_pix + 2];//b
				break;

			case E_PIXEL_RGBA:
				dst_bits[j * bytes_per_pix + 0] = this->vd_[line_start + j * bytes_per_pix + 0];//b
				dst_bits[j * bytes_per_pix + 1] = this->vd_[line_start + j * bytes_per_pix + 1];//g
				dst_bits[j * bytes_per_pix + 2] = this->vd_[line_start + j * bytes_per_pix + 2];//r
				dst_bits[j * bytes_per_pix + 3] = this->vd_[line_start + j * bytes_per_pix + 3];//a
				break;
			}
		}
	}

	BOOL ret = FreeImage_Save(FIF_BMP, bmp, file_path.c_str(), 0);

	FreeImage_Unload(bmp);

	return 0;
}

l3_int32 image::load_raw(const std::string& file_path, const l3_uint32 width, const l3_uint32 height, const l3_uint32 bpp)
{
	if(bpp != 8)
		return -1;

	this->file_path_ = file_path;

	common_file f;
	if(0 != f.open(file_path))
		return -1;

	l3_int32 data_sz = (l3_int32)(width * height * (bpp/8));
	this->vd_.resize(data_sz);
	l3_int32 ret = f.read(&this->vd_[0], (l3_uint32)this->vd_.size());
	if(ret < data_sz)
		return -1;

	this->width_ = width;
	this->height_ = height;
	this->bpp_ = bpp;

	return 0;
}

l3_int32 image::load_raw_memory(const std::vector<l3_uint8>& vdata,
	const l3_uint32 width, const l3_uint32 height, const e_pixel_format pix_format)
{
	this->pixel_format_ = pix_format;
	switch(pix_format)
	{
	case E_PIXEL_LUMINANCE_8BIT:
		this->bpp_ = 8;
		break;

	case E_PIXEL_RGB:
		this->bpp_ = 24;
		break;

	case E_PIXEL_RGBA:
		this->bpp_ = 32;
		break;

	default:
		return -1;
		break;
	}

	this->vd_ = vdata;

	this->width_ = width;
	this->height_ = height;

	return 0;
}

void image::get_rand_normal_2d_name(std::string& file_virtual_name,
	const l3_uint32 width, const l3_uint32 height,
	const l3_int32 rand_seed, const l3_uint32 part)
{
	char actemp[256] = {0};
	::_snprintf_s(actemp, sizeof(actemp) - 1, "normal_rand_texture_%d_%d_%d.bmp", width, height, part);
	file_virtual_name = actemp;
}

l3_int32 image::load_rand_normal_2d(const std::string& file_virtual_name,
	const l3_uint32 width, const l3_uint32 height,
	const l3_int32 rand_seed, const l3_uint32 part)
{
	#define SCALE ((1<<8))

	l3_uint32 total = width * height * 3;
	this->vd_.resize(total);

    for(l3_uint32 i=0; i < this->vd_.size(); i += 3)
    {
        l3_f32 rnd = abs((l3_f32)rand());
        l3_f32 angle = (2.0f * L3_PI * rnd / (l3_f32)RAND_MAX) / (l3_f32)part;

		l3_int32 r = (l3_uint8)(SCALE * cos(angle));//(ant_uint8)(SCALE * (cos(angle) * 0.5 + 0.5));
		l3_int32 g = (l3_uint8)(SCALE * sin(angle));//(ant_uint8)(SCALE * (sin(angle) * 0.5 + 0.5));
		l3_int32 b = 0;//(ant_uint8)(SCALE * (ant_f32)(rnd / RAND_MAX));

        this->vd_[i + 0] = r;
        this->vd_[i + 1] = g;
        this->vd_[i + 2] = b;
    }

	this->width_ = width;
	this->height_ = height;
	this->bpp_ = 24;

	this->pixel_format_ = E_PIXEL_RGB;

	this->file_path_ = file_virtual_name;

	return 0;
}

void image::get_rand_img_name(std::string& file_virtual_name,
	const l3_uint32 width,
	const l3_uint32 height,
	const l3_int32 rand_seed)
{
	char actemp[256] = {0};
	::_snprintf_s(actemp, sizeof(actemp) - 1, "rand_texture_%d_%d_%d.bmp", width, height, rand_seed);
	file_virtual_name = actemp;
}
l3_int32 image::load_rand_img(const std::string& file_virtual_name,
	const l3_uint32 width, const l3_uint32 height,
	const l3_int32 rand_seed)
{
	#define SCALE ((1<<8))

	l3_uint32 total = width * height * 3;
	this->vd_.resize(total);

    for(l3_uint32 i=0; i < this->vd_.size(); i += 3)
    {
		l3_f32 rnd = abs((l3_f32)rand());
		l3_f32 rnd_f = rnd / (l3_f32)RAND_MAX;
		l3_uint8 rand_u8 = (l3_uint8)(SCALE * rnd_f);

        this->vd_[i + 0] = rand_u8;
        this->vd_[i + 1] = rand_u8;
        this->vd_[i + 2] = rand_u8;
    }

	this->width_ = width;
	this->height_ = height;
	this->bpp_ = 24;

	this->pixel_format_ = E_PIXEL_RGB;

	this->file_path_ = file_virtual_name;

	return 0;
}

l3_int32 image::gen_normal_img_lum8bit(image& img_dst, l3_f32 scale)
{
	switch(this->pixel_format_)
	{
	case E_PIXEL_LUMINANCE_8BIT:
		scale *= (1.0f/255.0f);
		break;

	default:
		L3_LOG_ENG_DEBUG(("unknown pixel format:%d", this->pixel_format_));
		return -1;
		break;
	}

	l3_f32 vh = this->height_ / (l3_f32)this->width_;
	l3_f32 hh = this->width_ / (l3_f32)this->height_;

	/* 生成一个法线贴图 */
	l3_f32 val = 0;

	vector3 v1;
	vector3 v2;

	vector3 vc1;
	vector3 vc2;

	vector3 normal;

	img_dst.file_path_ = this->file_path_;
	img_dst.file_path_.append(".normal");

	img_dst.width_ = this->width_;
	img_dst.height_ = this->height_;
	img_dst.bpp_ = 32;
	img_dst.pixel_format_ = E_PIXEL_RGBA;
	
	l3_int32 bytes_per_pixel = img_dst.bpp_ / 8;

	img_dst.vd_.resize(bytes_per_pixel * this->width_ * this->height_);

	for(l3_uint32 i = 0; i < this->height_; i ++)
	{
		/* 此处可优化额外的下标乘法运算,但这种函数一般在加载时调用,不存在每帧调用的情况 */
		for(l3_uint32 j = 0; j < this->width_; j ++)
		{
			this->_get_pixel_lum8bit(i - 1, j, scale, val);
			v1.xyz((i - 1) * hh, val, j * vh);
			this->_get_pixel_lum8bit(i + 1, j, scale, val);
			v2.xyz((i + 1) * hh, val, j * vh);
			v2.sub(v1, vc1);

			this->_get_pixel_lum8bit(i, j - 1, scale, val);
			v1.xyz(i * hh, val, (j - 1) * vh);
			this->_get_pixel_lum8bit(i, j + 1, scale, val);
			v2.xyz(i * hh, val, (j + 1) * vh);
			v1.sub(v2, vc2);

			vc1.cross_product(vc2, normal);

			/* 转到0-255区间 */
			normal.normalise();
			normal.mult(0.5f);
			normal.sub(vector3(-0.5f, -0.5f, -0.5f));

			normal.mult(255.0f);
			{
				l3_int32 pos = bytes_per_pixel * i * this->width_ + j * bytes_per_pixel;
				img_dst.vd_[pos + 0] = (l3_uint8)normal.x();//normal.z();
				img_dst.vd_[pos + 1] = (l3_uint8)normal.z();//normal.x();
				img_dst.vd_[pos + 2] = (l3_uint8)normal.y();

				this->_get_pixel_lum8bit(i, j, 1.0f, val);
				img_dst.vd_[pos + 3] = (l3_uint8)val;
			}

		}
	}

	return 0;
}

l3_int32 image::gen_normal_img(image& img_dst, l3_f32 scale)
{
	l3_f32 vh = this->height_ / (l3_f32)this->width_;
	l3_f32 hh = this->width_ / (l3_f32)this->height_;

	vector3 v1;
	vector3 v2;

	vector3 vc1;
	vector3 vc2;

	vector3 normal;

	img_dst.file_path_ = this->file_path_;
	img_dst.file_path_.append(".normal");

	img_dst.width_ = this->width_;
	img_dst.height_ = this->height_;
	img_dst.bpp_ = 32;
	img_dst.pixel_format_ = E_PIXEL_RGBA;

	l3_int32 bytes_per_pixel = img_dst.bpp_ / 8;

	img_dst.vd_.resize(bytes_per_pixel * this->width_ * this->height_);//todo pitch*height here

	color clr;

	for(l3_uint32 i = 0; i < this->height_; i ++)
	{
		/* 此处可优化额外的下标乘法运算,但这种函数一般在加载时调用,不存在每帧调用的情况 */
		for(l3_uint32 j = 0; j < this->width_; j ++)
		{
			this->_get_pixel(i - 1, j, scale, clr);
			v1.xyz((i - 1) * hh, clr.r(), j * vh);
			this->_get_pixel(i + 1, j, scale, clr);
			v2.xyz((i + 1) * hh, clr.r(), j * vh);
			v2.sub(v1, vc1);

			this->_get_pixel(i, j - 1, scale, clr);
			v1.xyz(i * hh, clr.r(), (j - 1) * vh);
			this->_get_pixel(i, j + 1, scale, clr);
			v2.xyz(i * hh, clr.r(), (j + 1) * vh);
			v1.sub(v2, vc2);

			vc1.cross_product(vc2, normal);

			/* 转到0-255区间 */
			normal.normalise();
			normal.mult(0.5f);
			normal.sub(vector3(-0.5f, -0.5f, -0.5f));

			normal.mult(255.0f);
			{
				l3_int32 pos = bytes_per_pixel * i * this->width_ + j * bytes_per_pixel;
				img_dst.vd_[pos + 0] = (l3_uint8)normal.x();//normal.z();
				img_dst.vd_[pos + 1] = (l3_uint8)normal.z();//normal.x();
				img_dst.vd_[pos + 2] = (l3_uint8)normal.y();

				this->_get_pixel(i, j, 1.0f, clr);
				img_dst.vd_[pos + 3] = (l3_uint8)clr.r();
			}

		}
	}

	return 0;
}

static inline float EvalHermite(float pA, float pB, float vA, float vB, float u)
{
	float u2=(u*u), u3=u2*u;
	float B0 = 2*u3 - 3*u2 + 1;
	float B1 = -2*u3 + 3*u2;
	float B2 = u3 - 2*u2 + u;
	float B3 = u3 - u;
	return( B0*pA + B1*pB + B2*vA + B3*vB );
}

l3_int32 image::gen_gussian_img(const std::string& file_virtual_name,
	const l3_uint32 width, const l3_uint32 height,
	const l3_f32 pA, const l3_f32 pB, const l3_f32 vA, const l3_f32 vB)
{
	this->width_ = width;
	this->height_ = height;
	this->bpp_ = 8;

	this->pixel_format_ = E_PIXEL_LUMINANCE_8BIT;

	l3_int32 pitch = this->_cal_bytes_pitch();
	this->vd_.resize(pitch * this->height_);

	l3_f32 inc_x = 2.0f / this->width_;
	l3_f32 inc_y = 2.0f / this->height_;

	l3_f32 y = -1.0f;

	l3_uint8 * pdata = &this->vd_[0];
	for(l3_uint32 i = 0; i < this->height_; i ++, y += inc_y)
	{
		l3_f32 x = -1.0f;
		l3_f32 y2 = y * y;
		for(l3_uint32 j = 0; j < this->width_; j ++, x += inc_x)
		{
			l3_f32 dist = (l3_f32)l3_sqrt(x*x + y2);
			if(dist > 1.0f)
				dist = 1.0f;
			pdata[j] = (l3_uint8)(EvalHermite(pA, pB, vA, vB, dist) * 255);
		}
		pdata += pitch;
	}

	return 0;
}

l3_int32 image::_get_pixel_lum8bit(l3_int32 x, l3_int32 y, l3_f32 scale, l3_f32& val)
{
	if(x < 0)
		x = this->width_ - 1;
	if((l3_uint32)x >= this->width_)
		x = 0;

	if(y < 0)
		y = this->height_ - 1;
	if((l3_uint32)y >= this->height_)
		y = 0;


	switch(this->pixel_format_)
	{
	case E_PIXEL_LUMINANCE_8BIT:
		val = this->vd_[x * this->width_ + y] * scale;
		break;

	default:
		L3_LOG_ENG_DEBUG(("unknown pixel format:%d", this->pixel_format_));
		val = 0;
		return -1;
		break;
	}

	return 0;
}

l3_int32 image::_get_pixel(l3_int32 x, l3_int32 y, l3_f32 scale, color& clr)
{
	if(x < 0)
		x = this->width_ - 1;
	if((l3_uint32)x >= this->width_)
		x = 0;

	if(y < 0)
		y = this->height_ - 1;
	if((l3_uint32)y >= this->height_)
		y = 0;

	l3_uint32 pitch = this->_cal_bytes_pitch();

	l3_int32 idx = x * pitch + y * this->_cal_bytes_per_pix();

	if(idx < 0)
		return -1;
	if(idx >= this->vd_.size())
		return -1;

	switch(this->pixel_format_)
	{
	case E_PIXEL_LUMINANCE_8BIT:
		{
			l3_f32 r = this->vd_[idx] * scale;
			clr.rgb(r, r, r);
		}
		break;

	case E_PIXEL_RGB:
		{
			if((idx + 2) >= this->vd_.size())
				return -1;

			l3_f32 r = this->vd_[idx + 0] * scale;
			l3_f32 g = this->vd_[idx + 1] * scale;
			l3_f32 b = this->vd_[idx + 2] * scale;

			clr.rgb(r, g, b);
		}
		break;

	case E_PIXEL_RGBA:
		{
			if((idx + 3) >= this->vd_.size())
				return -1;

			l3_f32 r = this->vd_[idx + 0] * scale;
			l3_f32 g = this->vd_[idx + 1] * scale;
			l3_f32 b = this->vd_[idx + 2] * scale;
			l3_f32 a = this->vd_[idx + 3] * scale;

			clr.rgba(r, g, b, a);
		}
		break;

	default:
		L3_LOG_ENG_DEBUG(("unknown pixel format:%d", this->pixel_format_));
		return -1;
		break;
	}

	return 0;
}

l3_int32 image::load_round_cake(const std::string& file_virtual_name,
	const l3_uint32 width, const l3_uint32 height,
	const l3_uint32 radius1, const l3_uint32 radius2)
{
	#define SCALE ((1<<8))

	l3_uint32 total = width * height * 3;
	this->vd_.resize(total);

	l3_uint32 center_x = width / 2;
	l3_uint32 center_y = height / 2;

	l3_f32 rad_power1 = l3_f32(radius1 * radius1);
	l3_f32 rad_power2 = l3_f32(radius2 * radius2);
	l3_f32 rad_power_delta = rad_power1 - rad_power2;

    for(l3_uint32 i=0; i < height; i ++)
    {
		for(l3_uint32 j = 0; j < width; j ++)
		{
			l3_f32 len_power = l3_f32((i - center_y) * (i - center_y) + (j - center_x) * (j - center_x));
			
			l3_uint8 color = 0;
			if(len_power > rad_power1)
				color = 0;
			else if(len_power < rad_power2)
				color = 255;
			else
			{
				l3_f32 rate = (rad_power_delta - (len_power - rad_power2)) / rad_power_delta;
				color = (l3_uint8)(255 * rate);
			}

			this->vd_[i * width * 3 + j * 3 + 0] = color;
			this->vd_[i * width * 3 + j * 3 + 1] = color;
			this->vd_[i * width * 3 + j * 3 + 2] = color;
		}
    }

	this->width_ = width;
	this->height_ = height;
	this->bpp_ = 24;

	this->pixel_format_ = E_PIXEL_RGB;

	this->file_path_ = file_virtual_name;

	return 0;
}

void image::get_round_cake_name(std::string& file_virtual_name,
	const l3_uint32 width, const l3_uint32 height,
	const l3_uint32 radius1,
	const l3_uint32 radius2)
{
	char actemp[256] = {0};
	::_snprintf_s(actemp, sizeof(actemp) - 1, "round_cake_%d_%d_%d_%d.bmp", width, height, radius1, radius2);
	file_virtual_name = actemp;
}

l3_int32 image::get_sub_img(image& img_dst,
	const l3_uint32 pos_x, const l3_uint32 pos_y,
	const l3_uint32 width, const l3_uint32 height)
{
	img_dst.bpp_ = this->bpp_;
	img_dst.pixel_format_ = this->pixel_format_;

	img_dst.width_ = width;
	img_dst.height_ = height;

	if(pos_x + width > this->width_)
		img_dst.width_ = this->width_ - pos_x;

	if(pos_y + height > this->height_)
		img_dst.height_ = this->height_ - pos_y;

	l3_uint32 byte_per_line = this->_cal_bytes_pitch();
	l3_uint32 byte_offset = (this->bpp_/8) * pos_y;
	l3_uint32 byte_per_line_dst = (img_dst.bpp_/8) * img_dst.width_;

	img_dst.vd_.resize(this->bpp_ * img_dst.width_ * img_dst.height_);

	for(l3_uint32 i = 0; i < img_dst.height_; i ++)
	{
		l3_uint32 row_start = byte_per_line * (i + pos_x) + byte_offset;
		l3_uint32 row_start_dst = byte_per_line_dst * i;

		memcpy(&img_dst.vd_[row_start_dst], &this->vd_[row_start], byte_per_line_dst);
	}

	return 0;
}

l3_int32 image::conver_to_gray(image& img_dst)
{
	switch(this->pixel_format_)
	{
	case E_PIXEL_RGB:
	case E_PIXEL_RGBA:
		break;

	default:
		return -1;
		break;
	}

	img_dst.pixel_format_ = E_PIXEL_LUMINANCE_8BIT;
	image::_get_bpp_by_format(img_dst.pixel_format_, img_dst.bpp_);
	img_dst.width_ = this->width_;
	img_dst.height_ = this->height_;


	l3_uint32 bytes_per_pix = this->_cal_bytes_per_pix();
	l3_uint32 bytes_per_line = this->_cal_bytes_pitch();
	l3_uint32 bytes_per_pix_dst = img_dst._cal_bytes_per_pix();
	l3_uint32 bytes_per_line_dst = img_dst._cal_bytes_pitch();

	img_dst.vd_.resize(bytes_per_line * img_dst.height_);

	for(l3_uint32 i = 0; i < this->height_; i ++)
	{
		l3_uint32 line_start = i * bytes_per_line;
		l3_uint32 line_start_dst = i * bytes_per_line_dst;

		for(l3_uint32 j = 0; j < this->width_; j ++)
		{
			l3_f32 r = this->vd_[line_start + j * bytes_per_pix + 0];
			l3_f32 g = this->vd_[line_start + j * bytes_per_pix + 0];
			l3_f32 b = this->vd_[line_start + j * bytes_per_pix + 0];

			l3_f32 gray = (0.299f * r) + (0.587f * g) + (0.114f * b);

			img_dst.vd_[line_start_dst + j * bytes_per_pix_dst] = (l3_uint8)gray;
		}
	}

	return 0;
}

l3_int32 image::conver_to_rgba(image& img_dst)
{
	if(E_PIXEL_RGBA == img_dst.pixel_format_)
		return -1;

	img_dst.create(this->width_, this->height_, E_PIXEL_RGBA);

	l3_uint32 bytes_per_line = this->_cal_bytes_pitch();
	l3_uint32 bytes_per_pix = this->_cal_bytes_per_pix();

	l3_uint32 bytes_per_line_dst = img_dst._cal_bytes_pitch();
	l3_uint32 bytes_per_pix_dst = img_dst._cal_bytes_per_pix();

	for(l3_uint32 i = 0; i < this->height_; i ++)
	{
		l3_uint32 line_start = i * bytes_per_line;
		l3_uint32 line_start_dst = i * bytes_per_line_dst;
		for(l3_uint32 j = 0; j < this->width_; j ++)
		{
			switch(this->pixel_format_)
			{
			case E_PIXEL_LUMINANCE_8BIT:
				{
					l3_uint8 clr = this->vd_[line_start + j * bytes_per_pix];
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 0] = clr;//r
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 0] = clr;//g
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 0] = clr;//b
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 0] = 255;//a
				}
				break;

			case E_PIXEL_RGB:
				{
					l3_uint8 r = this->vd_[line_start + j * bytes_per_pix + 0];
					l3_uint8 g = this->vd_[line_start + j * bytes_per_pix + 1];
					l3_uint8 b = this->vd_[line_start + j * bytes_per_pix + 2];
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 0] = b;//b
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 1] = g;//g
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 2] = r;//r
					img_dst.vd_[line_start_dst + j * bytes_per_pix_dst + 3] = 255;//a
				}
				break;
			}
		}
	}

	return 0;
}

l3_int32 image::apply_alpha(image& img_alpha)
{
	if(img_alpha.pixel_format_ != E_PIXEL_LUMINANCE_8BIT
		|| this->pixel_format_ != E_PIXEL_RGBA)
		return -1;

	if(img_alpha.width_ != this->width_ ||
		img_alpha.height_ != this->height_)
		return -1;

	l3_uint32 bytes_per_pix = this->_cal_bytes_per_pix();
	l3_uint32 bytes_per_line = this->_cal_bytes_pitch();

	l3_uint32 bytes_per_pix_alpha = img_alpha._cal_bytes_per_pix();
	l3_uint32 bytes_per_line_alpha = img_alpha._cal_bytes_pitch();

	for(l3_uint32 i = 0; i < img_alpha.height_; i ++)
	{
		l3_uint32 line_start = i * bytes_per_line;
		l3_uint32 line_start_alpha = i * bytes_per_line_alpha;
		for(l3_uint32 j = 0; j < img_alpha.width_; j ++)
		{
			//this->vd_[line_start + j * bytes_per_pix + 0] = img_alpha.vd_[line_start_alpha + j * bytes_per_pix_alpha];
			//this->vd_[line_start + j * bytes_per_pix + 1] = img_alpha.vd_[line_start_alpha + j * bytes_per_pix_alpha];
			//this->vd_[line_start + j * bytes_per_pix + 2] = img_alpha.vd_[line_start_alpha + j * bytes_per_pix_alpha];
			this->vd_[line_start + j * bytes_per_pix + 3] = img_alpha.vd_[line_start_alpha + j * bytes_per_pix_alpha];
		}
	}

	return 0;
}

}




