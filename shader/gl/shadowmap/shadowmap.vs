#define SHADOWMAP_STAGE_CAST 0
#define SHADOWMAP_STAGE_RECV 1

uniform int uni_shader_stage;

uniform vec2 uni_light_dep_range; //光源的depth range
uniform mat4 uni_light_view_mtx; //光源观察
uniform mat4 uni_light_proj_mtx; //光源投影
uniform vec2 uni_light_dep_pix_offset; //光源深度纹理分辨率

void shadowmap_cast_vs()
{
	vary_world_pos = uni_world_mtx * gl_Vertex;
	vary_view_pos  = uni_light_view_mtx * vary_world_pos;
	gl_Position = uni_light_proj_mtx * vary_view_pos;

	vary_view_pos.z = (-vary_view_pos.z - uni_light_dep_range.x) / uni_light_dep_range.y;
}

void main()
{
	switch(uni_shader_stage)
	{
	case SHADOWMAP_STAGE_CAST:
		shadowmap_cast_vs();
		break;

	case SHADOWMAP_STAGE_RECV:
		screen_quad_vs();
		break;
	}
}
