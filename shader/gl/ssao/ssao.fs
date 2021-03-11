/*** ssao.fs ***/

uniform sampler2D uni_tex_view_depth; //视空间深度纹理
uniform sampler2D uni_tex_nml; //视空间法线

uniform vec2 uni_tex_dep_pix_offset; //视深度纹理分辨率

uniform float uni_tan_half_fov; //视角一半的tan值
uniform vec2 uni_dep_range; //最大深度
uniform float uni_aspect; //屏幕宽高比

varying vec2 vary_proj_pos;

vec2 dir_array[8] = vec2[]
(
	vec2( 1,  0),
	vec2(-1,  0),
	vec2( 0, -1),
	vec2( 0,  1),
	vec2( 0.707,  0.707),
	vec2( 0.707, -0.707),
	vec2(-0.707,  0.707),
	vec2(-0.707, -0.707)
);

#define SSAO_STEP_COUNT 6

void main()
{
	float view_tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	vec3 view_nml = texture2D(uni_tex_nml, gl_TexCoord[0].st).xyz * 2.0 - 1.0;
	
	//取出当前采样点周围的点的位置(根据深度算出),法线,根据法线与距离,算出ao权值

	float view_dep;
	tex_dep_2_view_dep(view_tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos, uni_tan_half_fov, uni_aspect, view_pos);
	
	/*
	#if 0
	float z_scale = clamp(view_dep - 40.0, 1.0, 10000);
	float r = 1.0 / (z_scale / 20.0);
	#else
	float r = 3.0 / (clamp(view_dep/5.0, 1.0, 1000.0));
	#endif
	*/
	float r = 1.0;

	float ao = 0.0;
	float count = 0.0;
	for(int i = 0; i < 8; i ++)
	{
		vec2 ray_dir = dir_array[i];
		ray_dir = normalize(ray_dir);

		for(int j = 1; j <= SSAO_STEP_COUNT; j ++)
		{
			count = count + 1.0;
			vec2 v_offset = uni_tex_dep_pix_offset.xy * (ray_dir * r * j);

			vec2 uv = gl_TexCoord[0].st + v_offset;


			float sample_tex_dep = texture2D(uni_tex_view_depth, uv).x;

			float sample_view_dep;
			tex_dep_2_view_dep(sample_tex_dep, uni_dep_range, sample_view_dep);

			vec3 sample_view_pos;
			get_pos_by_dep(sample_view_dep, vary_proj_pos + v_offset * 2, uni_tan_half_fov, uni_aspect, sample_view_pos);


			vec3 v_dis = sample_view_pos - view_pos;
			float dis = length(v_dis);

			{
				v_dis = normalize(v_dis);

				float normal_contrib = max(0, (dot(v_dis, view_nml)));

				float dis_x = clamp(dis - 0.0, 0.0, 1000.0);
				float dis_contrib = 1.0 / (1.0 + dis_x);

				float ao_contrib = 1.0 - normal_contrib * dis_contrib;
				//float ao_contrib = normal_contrib;
				//float ao_contrib = dis_contrib;

				ao += (ao_contrib);
			}
		}
	}

	ao = ao / count;

	gl_FragColor = vec4(ao, ao, ao, 1);
}
