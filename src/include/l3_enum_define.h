/**
* @file l3_enum_define.h
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
#ifndef __L3ENG_ENUM_DEFINE_H__
#define __L3ENG_ENUM_DEFINE_H__

namespace l3eng{

enum L3_RENDER_GROUP
{
	L3_RENDER_GROUP_ALL     = 0,
	L3_RENDER_GROUP_DEFAULT = 0x1,
	L3_RENDER_GROUP_WATER   = 0x2,
};

enum EN_SHADER_PARAM
{
	//大气散射参数
	EN_SHADER_ATMOSPHERIC_PARAM_sea_level, // l3_f32 sea_level_;
	EN_SHADER_ATMOSPHERIC_PARAM_rayleigh_scale_height, // l3_f32 rayleigh_scale_height_;
	EN_SHADER_ATMOSPHERIC_PARAM_scale, // l3_f32 scale_;
	EN_SHADER_ATMOSPHERIC_PARAM_vLightDirection, // vector3 vLightDirection_;
	EN_SHADER_ATMOSPHERIC_PARAM_g, // l3_f32 g_;
	EN_SHADER_ATMOSPHERIC_PARAM_g_sky, // l3_f32 g_sky_;
	EN_SHADER_ATMOSPHERIC_PARAM_esun, // l3_f32 esun_;
	EN_SHADER_ATMOSPHERIC_PARAM_Kr, // l3_f32 Kr_, 
	EN_SHADER_ATMOSPHERIC_PARAM_Kr4PI, // Kr4PI_;
	EN_SHADER_ATMOSPHERIC_PARAM_Km, // l3_f32 Km_
	EN_SHADER_ATMOSPHERIC_PARAM_Km4PI, // Km4PI_;
	EN_SHADER_ATMOSPHERIC_PARAM_inv_array_fWavelength4, // vector3 inv_array_fWavelength4_;
	EN_SHADER_ATMOSPHERIC_PARAM_attenuation_final,

	//反射参数
	EN_SHADER_PARAM_RELECT_wave_offset,
	EN_SHADER_PARAM_RELECT_wave_scale,
	EN_SHADER_PARAM_RELECT_refl_distort,

	//延迟渲染参数
	EN_SHADER_PARAM_FINAL_DIFF_SCALE,

	//体积光,光方向挤出参数
	EN_SHADER_PARAM_VOL_LIGHTDIR_EXTRUDE_LIGHT_POS_DIR,
	EN_SHADER_PARAM_VOL_LIGHTDIR_EXTRUDE_LIGHT_IS_DIR,
	EN_SHADER_PARAM_VOL_LIGHTDIR_EXTRUDE_VOL_MAX,

	//pbr渲染参数
	EN_SHADER_PARAM_PBR_ROUGHNESS,
	EN_SHADER_PARAM_PBR_METALIC,

	//dissolution参数
	EN_SHADER_PARAM_DISSOLUTION_RAND_TEX,
	EN_SHADER_PARAM_DISSOLUTION_THRESHOLD,
	EN_SHADER_PARAM_DISSOLUTION_TRANSITION,
};

}

#endif