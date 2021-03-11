/*** common_screen_quad.vs ***/

attribute vec2 attr_screen_quad_pos;
attribute vec2 attr_screen_quad_texcoord;

varying vec2 vary_proj_pos;

void screen_quad_vs()
{
	gl_Position.x = attr_screen_quad_pos.x;
	gl_Position.y = attr_screen_quad_pos.y;

	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	vary_proj_pos = gl_Position.xy;

	gl_TexCoord[0].xy = attr_screen_quad_texcoord;
}

void main()
{		
	screen_quad_vs();
}
