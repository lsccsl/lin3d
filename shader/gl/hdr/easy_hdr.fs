/*** easy_hdr.fs ***/

uniform sampler2D uni_tex_final;
varying vec2 vary_proj_pos;

void main()
{
	vec4 clr = texture2D(uni_tex_final, gl_TexCoord[0].st);
	//float lum = clr.r + clr.g + clr.b;
	//float lum = 0.2125 * clr.r + 0.7154 * clr.g + 0.0721 * clr.b;
	float lum = 0.27 * clr.r + 0.67 * clr.g + 0.06 * clr.b;
	
	//gl_FragColor = (lum / (lum + 1.0)) * clr;
	gl_FragColor = (clr / (clr + 1.0));	
}
