/*** hdr.fs ***/

#define HDR_STAGE_downsample4x       0
#define HDR_STAGE_downsample         1
#define HDR_STAGE_extract_high_lihgt 2
#define HDR_STAGE_blur_high_light    3
#define HDR_STAGE_glare              4
#define HDR_STAGE_compose            5
#define HDR_STAGE_tone_mapping       6


uniform int uni_hdr_stage; //hdr处理流程


uniform sampler2D uni_tex_src; //输入纹理

uniform sampler2D uni_tex_blur1; //高亮纹理(256-128-64-32)
uniform sampler2D uni_tex_blur2;
uniform sampler2D uni_tex_blur3;
uniform sampler2D uni_tex_blur4;
uniform sampler2D uni_tex_streak1; //横向光晕纹理 
uniform sampler2D uni_tex_streak2;
uniform sampler2D uni_tex_streak3;
uniform sampler2D uni_tex_streak4;

uniform sampler2D uni_tex_final; //场景纹理


//拷贝高亮区域参数
uniform float uni_high_light_threshold; //高亮阀值
uniform vec2 uni_src_pix_offset;        //输入像素点偏移

uniform float uni_ave_lum;//原始场景像素lum均值

//生成光晕时,4个采样的系数
uniform vec4 uni_coeff1;
uniform vec4 uni_coeff2;
uniform vec4 uni_coeff3;
uniform vec4 uni_coeff4;

//每个采样的偏移
uniform vec2 uni_step_offset;


varying vec2 vary_proj_pos;


//======================================================

const float pix_count = 2.0;

void hdr_downsample4x()
{
	vec2 uv = gl_TexCoord[0].st - pix_count * 0.5 * uni_src_pix_offset;
	vec4 clr1 = texture2D(uni_tex_src, uv);
	vec4 clr2 = texture2D(uni_tex_src, uv + pix_count * vec2(uni_src_pix_offset.x, 0));
	vec4 clr3 = texture2D(uni_tex_src, uv + pix_count * vec2(uni_src_pix_offset.x, uni_src_pix_offset.y));
	vec4 clr4 = texture2D(uni_tex_src, uv + pix_count * vec2(0,                    uni_src_pix_offset.y));
	
	gl_FragColor = (clr1 + clr2 + clr3 + clr4) * 0.25;
	gl_FragColor.a = 1.0;
}

void hdr_downsample()
{
	gl_FragColor = texture2D(uni_tex_src, gl_TexCoord[0].st);
	gl_FragColor.a = 1.0;
}

void hdr_extract_high()
{
	vec4 clr = texture2D(uni_tex_src, gl_TexCoord[0].st);
	
	//vec4 v_threshold = vec4(uni_high_light_threshold, uni_high_light_threshold, uni_high_light_threshold, uni_high_light_threshold);
	//vec4 v= greaterThanEqual(clr, v_threshold);
	gl_FragColor = max((clr - uni_high_light_threshold) * 1, vec4(0,0,0,0));
}

void hdr_blur_high_light()
{
	vec2 uv = gl_TexCoord[0].st - pix_count * 0.5 * uni_src_pix_offset;
	vec4 clr1 = texture2D(uni_tex_src, uv);
	vec4 clr2 = texture2D(uni_tex_src, uv + pix_count * vec2(uni_src_pix_offset.x, 0));
	vec4 clr3 = texture2D(uni_tex_src, uv + pix_count * vec2(uni_src_pix_offset.x, uni_src_pix_offset.y));
	vec4 clr4 = texture2D(uni_tex_src, uv + pix_count * vec2(0,                    uni_src_pix_offset.y));
	
	gl_FragColor = (clr1 + clr2 + clr3 + clr4) * 0.25;
	gl_FragColor.a = 1.0;
}

void hdr_glare()
{
	vec2 uv = gl_TexCoord[0].st;
	vec4 clr1 = texture2D(uni_tex_src, uv);
	vec4 clr2 = texture2D(uni_tex_src, uv + uni_step_offset);
	vec4 clr3 = texture2D(uni_tex_src, uv + 2.0 * uni_step_offset);
	vec4 clr4 = texture2D(uni_tex_src, uv + 3.0 * uni_step_offset);

	gl_FragColor = clr1 * uni_coeff1 + clr2 * uni_coeff2 + clr3 * uni_coeff3 + clr4 * uni_coeff4;

	gl_FragColor.a = 1.0;
}

void hdr_compose()
{
	vec4 clr1 = texture2D(uni_tex_blur1, gl_TexCoord[0].st);
	vec4 clr2 = texture2D(uni_tex_blur2, gl_TexCoord[0].st);
	vec4 clr3 = texture2D(uni_tex_blur3, gl_TexCoord[0].st);
	vec4 clr4 = texture2D(uni_tex_blur4, gl_TexCoord[0].st);
	vec4 clr_streak1 = texture2D(uni_tex_streak1, gl_TexCoord[0].st);
	vec4 clr_streak2 = texture2D(uni_tex_streak2, gl_TexCoord[0].st);
	vec4 clr_streak3 = texture2D(uni_tex_streak3, gl_TexCoord[0].st);
	vec4 clr_streak4 = texture2D(uni_tex_streak4, gl_TexCoord[0].st);
	
	vec4 clr_hl = clr1 * 0.3 + clr2 * 0.3 + clr3 * 0.25 + clr4 * 0.20;
	vec4 clr_streak = clr_streak1 + clr_streak2 + clr_streak3 + clr_streak4;
	gl_FragColor = clr_hl * 1.0 + clr_streak * 1.0;

	gl_FragColor.a = 1.0;
}

void hdr_tone_mapping()
{
	vec4 clr_sence = texture2D(uni_tex_final, gl_TexCoord[0].st);
	vec4 clr_glare = texture2D(uni_tex_src, gl_TexCoord[0].st);	
	
	vec4 clr = clr_sence + clr_glare;
	
	float lum = 0.27 * clr.r + 0.67 * clr.g + 0.06 * clr.b;
	
	clr = 1.0 * clr / uni_ave_lum;

	gl_FragColor = clr / (1.0 + clr);
	gl_FragColor.a = 1.0;

//clr = clr * (lum / uni_ave_lum);
//gl_FragColor = clr * (lum / (1 + lum));
}

void main()
{
	switch(uni_hdr_stage)
	{
	case HDR_STAGE_downsample4x:
		hdr_downsample4x();
		break;

	case HDR_STAGE_downsample:
		hdr_downsample();
		break;

	case HDR_STAGE_extract_high_lihgt:
		hdr_extract_high();
		break;
		
	case HDR_STAGE_blur_high_light:
		hdr_blur_high_light();
		break;
		
	case HDR_STAGE_glare:
		hdr_glare();
		break;
		
	case HDR_STAGE_compose:
		hdr_compose();
		break;
		
	case HDR_STAGE_tone_mapping:
		hdr_tone_mapping();
		break;
	}
}
