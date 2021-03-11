/*** defer_render_final.fs ***/

uniform sampler2D uni_tex_light_shadow; //光照阴影
uniform sampler2D uni_tex_light_vol; //体积光
uniform sampler2D uni_tex_light_mix; //颜色

varying vec4 vary_proj_pos;

void main()
{	
	vec4 light_shadow = texture2D(uni_tex_light_shadow, gl_TexCoord[0].st);
	//vec4 light_vol = texture2D(uni_tex_light_vol, gl_TexCoord[0].st);
	vec4 light_mix = texture2D(uni_tex_light_mix, gl_TexCoord[0].st);
	
	//gl_FragColor = (light_shadow + light_vol + light_mix);
	gl_FragColor = (light_shadow + light_mix);
}