uniform sampler2D uni_tex_light_depth_tex; //光源空间深度
uniform sampler2DShadow uni_tex_light_depth_shadow; //同一个纹理,深度


void shadowmap_recv_fs()
{
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	gl_FragDepth = tex_dep;	

	vec4 sence_light = texture2D(uni_tex_light, gl_TexCoord[0].st);
	vec4 sence_diff = texture2D(uni_tex_clr, gl_TexCoord[0].st) * 0.1;

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

//==============================================================
#if 0
if(1)
{
	vec3 v_tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).xyz;
	vec3 view_nml = texture2D(uni_tex_nml, gl_TexCoord[0].st).xyz;
	vec3 view_clr = texture2D(uni_tex_clr, gl_TexCoord[0].st).xyz;
	//vec3 light_shadow = texture2D(uni_tex_light_depth_shadow, gl_TexCoord[0].st).xyz;
	vec3 light_dep = texture2D(uni_tex_light_depth_tex, gl_TexCoord[0].st).xyz;

	gl_FragColor = abs(vec4(v_tex_dep/1.0 + view_nml/1000.0 + view_clr/1000.0 + light_dep/1000.0, 1));
	return;
}
#endif


#if 0
	float light_tex_dep = texture2D(uni_tex_light_depth_tex, uv).x;

	if(light_view_dep < light_tex_dep)
		gl_FragColor = vec4(1, 1, 1, 1);
	else
		gl_FragColor = vec4(0, 0, 0, 0);

	gl_FragColor = (vec4(light_tex_dep, 0, 0, 1));
return;
#endif