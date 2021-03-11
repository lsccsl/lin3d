/*** debug_show_rtt.fs ***/

uniform sampler2D uni_tex;
uniform float uni_dep;

varying vec2 vary_proj_pos;

void main()
{
	gl_FragColor.xyz = texture2D(uni_tex, gl_TexCoord[0].st).xyz;
	gl_FragColor.w = 1.0;

	gl_FragDepth = uni_dep;
}