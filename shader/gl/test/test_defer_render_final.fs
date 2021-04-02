/*** defer_render_final.fs ***/

uniform sampler2D uni_tex_light_shadow; //光照阴影
uniform sampler2D uni_tex_light_vol; //体积光
uniform sampler2D uni_tex_clr; //颜色
uniform sampler2D uni_tex_view_depth; //摄影机空间线性深度纹理
uniform sampler2D uni_tex_view_proj_depth; //摄影机空间深度纹理
uniform sampler2D uni_tex_ssao;
uniform sampler2D uni_tex_atmospheric;

uniform int uni_has_ssao; //是否开启ssao
uniform int uni_has_light_vol; //是否开启体积光
uniform int uni_has_atmospheric; //是否开启大气散射

uniform float uni_diff_scale;
uniform float uni_atmospheric_scale;

varying vec4 vary_proj_pos;

void main()
{
	gl_FragDepth = texture2D(uni_tex_view_proj_depth, gl_TexCoord[0].st).x;
	
	vec4 light_shadow = texture2D(uni_tex_light_shadow, gl_TexCoord[0].st);	

	//float dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	//float diff_scale = (dep >= 1.0 ? 0 : uni_diff_scale);

	vec4 diff = texture2D(uni_tex_clr, gl_TexCoord[0].st);	
	float diff_scale = ceil(1.0 - gl_FragDepth) * uni_diff_scale;
	vec4 diff_final = diff * diff_scale;
	
	vec4 light_final = light_shadow;
	if(1 == uni_has_ssao) // 编译器有时可以对分支进行一定的优化 uniform数据分支：If判断语句仅仅包含常数或uniform参数；---- 希望这句话是真的
	{
		vec4 ssao = texture2D(uni_tex_ssao, gl_TexCoord[0].st);
		light_final = ssao * light_shadow;
	}
	
	gl_FragColor = light_final + diff_final;
	
	//添加体积光分量
	if(1 == uni_has_light_vol)
	{
		vec4 light_vol = texture2D(uni_tex_light_vol, gl_TexCoord[0].st);
		gl_FragColor += light_vol;
	}
	
	//添加大气散射分量
	if(1 == uni_has_atmospheric)
	{
		vec4 atmospheric = texture2D(uni_tex_atmospheric, gl_TexCoord[0].st);
		gl_FragColor += uni_atmospheric_scale * atmospheric;
	}
}
