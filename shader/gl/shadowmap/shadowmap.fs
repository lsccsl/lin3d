#define SHADOWMAP_STAGE_CAST 0
#define SHADOWMAP_STAGE_RECV 1

uniform int uni_shader_stage;

uniform vec2 uni_light_dep_range; //光源的depth range
uniform mat4 uni_light_view_mtx; //光源观察
uniform mat4 uni_light_proj_mtx; //光源投影
uniform vec2 uni_light_dep_pix_offset; //光源深度纹理分辨率

uniform sampler2D uni_tex_light_depth_tex; //光源空间深度
uniform sampler2DShadow uni_tex_light_depth_shadow; //同一个纹理,深度

void shadowmap_cast_fs()
{
	gl_FragColor = vec4(vary_view_pos.z, 0, vary_view_pos.z, 1);
	gl_FragDepth = vary_view_pos.z;
}

void shadowmap_recv_fs()
{
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	gl_FragDepth = tex_dep;	

	vec4 sence_light = texture2D(uni_tex_light, gl_TexCoord[0].st);
	vec4 sence_diff = texture2D(uni_tex_clr, gl_TexCoord[0].st) * 0.1;

//gl_FragColor = texture2D(uni_tex_light_depth_tex, gl_TexCoord[0].st);
//return;

	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos.xy, uni_tan_half_fov, uni_aspect, view_pos);

	vec4 world_pos = uni_view_inv_mtx * vec4(view_pos, 1);

	//变换到光源的空间
	vec4 light_view_pos = uni_light_view_mtx * world_pos;
	vec4 light_proj_pos = uni_light_proj_mtx * light_view_pos;
	light_proj_pos = light_proj_pos / light_proj_pos.w;

#if 1
	if(light_proj_pos.x > 1.0 || light_proj_pos.x < -1.0)
	{
		gl_FragColor = sence_light + sence_diff;//abs(vec4(1, 1, 1, 1));
		return;
	}
	if(light_proj_pos.y > 1.0 || light_proj_pos.y < -1.0)
	{
		gl_FragColor = sence_light + sence_diff;
		return;
	}
#endif

	float light_view_dep = (-light_view_pos.z - uni_light_dep_range.x) / uni_light_dep_range.y;
	vec2 uv = light_proj_pos.st * 0.5 + 0.5;

#if 1
	int loop = 7;
	int loop_half = loop / 2;
	float s = 0.0;
	uv = uv - loop_half * uni_light_dep_pix_offset;
	for(int i = 0; i < loop; i ++){
		for(int j = 0; j < loop; j ++){		
			s += shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
		}
	}
	s = s / 49.0;
#else
	float s0  = shadow2D(uni_tex_light_depth_shadow, vec3(uv, light_view_dep), 0.0).x;
	
	float s1 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2( uni_light_dep_pix_offset.x,  uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
	float s2 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(-uni_light_dep_pix_offset.x,  uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
	float s3 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2( uni_light_dep_pix_offset.x, -uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
	float s4 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(-uni_light_dep_pix_offset.x, -uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
	
	float s5 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2( uni_light_dep_pix_offset.x,  0), light_view_dep), 0.0).x;
	float s6 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(-uni_light_dep_pix_offset.x,  0), light_view_dep), 0.0).x;
	float s7 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(0, -uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
	float s8 = shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(0, -uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;

	float s = (s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8) / 9.0;
#endif
	
	gl_FragColor = vec4(s, s, s, 1) * sence_light + sence_diff;
}

void main()
{
	switch(uni_shader_stage)
	{
	case SHADOWMAP_STAGE_CAST:
		shadowmap_cast_fs();
		break;
		
	case SHADOWMAP_STAGE_RECV:
		shadowmap_recv_fs();
		break;
	}	
}