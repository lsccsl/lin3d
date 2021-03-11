#define DEFER_RENDER_GBUFFER 0
#define DEFER_RENDER_LIGHTING 1

uniform int uni_shader_stage;


void defer_render_gbuffer_fs()
{
	vec4 tex1 = texture2D(uni_tex1, gl_TexCoord[0].st);

	vec4 vdep = vec4(vary_view_pos.z, vary_view_pos.z, vary_view_pos.z, 1);

	vec3 world_nml = normalize(vary_world_nml);
	world_nml = (world_nml + 1.0) * 0.5;
	vec3 view_nml = normalize(vary_view_nml);
	view_nml = (view_nml + 1.0) * 0.5;

	gl_FragData[0] = tex1 * gl_Color;//vdep;
	gl_FragData[1] = vec4(view_nml, 1);
	gl_FragDepth = vary_view_pos.z;
}

void main()
{
	switch(uni_shader_stage)
	{
	case DEFER_RENDER_GBUFFER:
		defer_render_gbuffer_fs();
		break;

	case DEFER_RENDER_LIGHTING:
		defer_render_lighting_fs();
		break;
	}
}