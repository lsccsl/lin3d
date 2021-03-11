/*** vol_radial_blur.fs ***/

#define VOL_RADIAL_BLUR_STAGE_get_high_light 1
#define VOL_RADIAL_BLUR_STAGE_radial_blur    2
#define VOL_RADIAL_BLUR_STAGE_test_final     3

uniform sampler2D uni_tex_src;
uniform sampler2D uni_tex_src1;

uniform int uni_vol_radial_blur_stage; //径向模糊处理过程

uniform float uni_high_light_threshold; //高亮阀值

uniform vec2 uni_light_pos;
uniform float uni_offset;

// 接收变量:当前像素点在标准视口空间中的2d位置
varying vec2 vary_proj_pos;

void vol_rb_get_hight_light()
{
	vec4 clr = texture2D(uni_tex_src, gl_TexCoord[0].st);
	float lum = 0.27 * clr.r + 0.67 * clr.g + 0.06 * clr.b;
	
	float final_clr = max((lum - uni_high_light_threshold), 0);
	
	gl_FragColor = vec4(final_clr, final_clr, final_clr, 1);
}

const int SAMPLE_COUNT = 6;

void vol_rb_radical_blur()
{
	//顺着光源方向进行偏移
	//投影变换至纹理映射
	vec2 offset = uni_light_pos - vary_proj_pos.xy;
	//offset = normalize(offset);
	vec2 texcoord_offset = offset * uni_offset * 1;


	vec4 clr = vec4(0,0,0,1);
	for(int i = 0; i < SAMPLE_COUNT; i ++)
	{
		clr += texture2D(uni_tex_src, gl_TexCoord[0].st + i * texcoord_offset);
	}
	
	clr = clr / SAMPLE_COUNT;
	clr = 1.2 * clr;
	//clr.y = 0;
	//clr += texture2D(uni_tex_src, gl_TexCoord[0].st) * 0.4;

	gl_FragColor = clr;
	gl_FragColor.a = 1;


	//clr = texture2D(uni_tex_src, gl_TexCoord[0].st + texcoord_offset) + texture2D(uni_tex_src, gl_TexCoord[0].st);
	//gl_FragColor.xyz = (texcoord_offset).xxx;
	//gl_FragColor.z = 0;
	//gl_FragColor.a = 1;
	//gl_FragColor = texture2D(uni_tex_src, gl_TexCoord[0].st);
}

void vol_rb_test_final()
{
	vec4 clr = texture2D(uni_tex_src, gl_TexCoord[0].st);
	vec4 clr1 = texture2D(uni_tex_src1, gl_TexCoord[0].st);
	
	gl_FragColor = 0.35 * clr + 2 * clr1 * clr;
	
	//gl_FragColor = clr1;
}

void main()
{
	switch(uni_vol_radial_blur_stage)
	{
	case VOL_RADIAL_BLUR_STAGE_get_high_light:
		vol_rb_get_hight_light();
		break;
		
	case VOL_RADIAL_BLUR_STAGE_radial_blur:
		vol_rb_radical_blur();
		break;
		
	case VOL_RADIAL_BLUR_STAGE_test_final:
		vol_rb_test_final();
		break;
	}
}
