/*** defer_render_light_vol_mix.fs ***/

uniform sampler2D uni_tex_light_vol; //�����
uniform sampler2D uni_tex_light_mix; //��ɫ

varying vec2 vary_proj_pos;

void main()
{	
	vec4 light_vol = texture2D(uni_tex_light_vol, gl_TexCoord[0].st);
	vec4 light_mix = texture2D(uni_tex_light_mix, gl_TexCoord[0].st);

	gl_FragColor = (light_vol + light_mix);
}