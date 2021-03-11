/*** vol.vs ***/

attribute vec2 attr_screen_quad_pos;
attribute vec2 attr_screen_quad_texcoord;

varying vec2 vary_proj_pos;

void main()
{
	screen_quad_vs(attr_screen_quad_pos, attr_screen_quad_texcoord, vary_proj_pos);
}