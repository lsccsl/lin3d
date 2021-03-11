#define DEFER_RENDER_GBUFFER 0
#define DEFER_RENDER_LIGHTING 1

uniform int uni_shader_stage;

void defer_render_gbuffer_vs()
{
	vary_world_pos = uni_world_mtx * gl_Vertex;
	vary_view_pos  = uni_view_mtx * vary_world_pos;
	gl_Position = gl_ProjectionMatrix * vary_view_pos;

	vary_view_pos.z = (-vary_view_pos.z - uni_dep_range.x) / uni_dep_range.y;
	vary_world_nml = (uni_world_mtx * vec4(gl_Normal, 0.0)).xyz;
	vary_view_nml = (uni_view_mtx * uni_world_mtx * vec4(gl_Normal, 0.0)).xyz;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_FrontColor = gl_Color;
}

void defer_render_light_vs()
{	
	gl_Position.x = attr_screen_quad_pos.x;
	gl_Position.y = attr_screen_quad_pos.y;

	gl_Position.z = 0.0;
	gl_Position.w = 1.0;

	vary_proj_pos = gl_Position;

	gl_TexCoord[0].xy = attr_screen_quad_texcoord;
}

void main()
{
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	switch(uni_shader_stage)
	{
	case DEFER_RENDER_GBUFFER:
		defer_render_gbuffer_vs();
		break;
		
	case DEFER_RENDER_LIGHTING:
		screen_quad_vs();
		break;
	}
}
