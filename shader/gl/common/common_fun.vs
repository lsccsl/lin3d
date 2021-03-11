/*** common_fun.vs ***/

void screen_quad_vs(vec2 quad_pos, vec2 quad_texcoord, out vec2 proj_pos)
{
	gl_Position.x = quad_pos.x;
	gl_Position.y = quad_pos.y;

	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	proj_pos = gl_Position.xy;

	gl_TexCoord[0].xy = quad_texcoord;
}
