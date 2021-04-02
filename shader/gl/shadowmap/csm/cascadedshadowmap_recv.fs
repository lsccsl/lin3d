/*** cascadedshadowmap_recv.fs ***/

#define CSM_SEG_MAX 4

uniform sampler2D uni_tex_view_depth; //摄影机空间深度纹理
uniform sampler2D uni_tex_light; //光照

uniform mat4 uni_view_inv_mtx; //观察逆矩阵
uniform float uni_tan_half_fov; //视角一半的tan值
uniform float uni_aspect; //屏幕宽高比
uniform vec2 uni_dep_range; //最大深度



uniform sampler2D uni_tex_light_depth_tex[CSM_SEG_MAX];// 光源空间深度
uniform sampler2DShadow uni_tex_light_depth_shadow[CSM_SEG_MAX];
uniform float uni_far_thredhold;
uniform vec2 uni_light_dep_range[CSM_SEG_MAX]; //光源的depth range
uniform mat4 uni_light_view_mtx[CSM_SEG_MAX]; //光源观察
uniform mat4 uni_light_proj_mtx[CSM_SEG_MAX]; //光源投影
uniform vec2 uni_light_dep_pix_offset; //光源深度纹理分辨率
uniform int uni_loop;

uniform int uni_seg_count;

varying vec2 vary_proj_pos;


void main()
{
	vec4 sence_light = texture2D(uni_tex_light, gl_TexCoord[0].st);
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;

	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);
	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos.xy, uni_tan_half_fov, uni_aspect, view_pos);

	vec4 world_pos = uni_view_inv_mtx * vec4(view_pos, 1);

#if 0
	//变换到光源1的空间
	vec4 light_view_pos_1 = uni_light_view_mtx[0] * world_pos;
	vec4 light_proj_pos_1 = uni_light_proj_mtx[0] * light_view_pos_1;
	light_proj_pos_1 = light_proj_pos_1 / light_proj_pos_1.w;
	float light_view_dep_1 = (-light_view_pos_1.z - uni_light_dep_range[0].x) / uni_light_dep_range[0].y;

	float s1 = 0.0;
	if(light_proj_pos_1.x <= 1.0 && light_proj_pos_1.x >= -1.0)
	{
		if(light_proj_pos_1.y <= 1.0 && light_proj_pos_1.y >= -1.0)
		{
			vec2 uv_center = light_proj_pos_1.st * 0.5 + 0.5;
			int loop = (uni_loop > 0) ? uni_loop : 1;
			int loop_half = loop / 2;
			vec2 uv = uv_center - loop_half * uni_light_dep_pix_offset;
			for(int i = 0; i < loop; i ++){
				for(int j = 0; j < loop; j ++){		
					s1 += shadow2D(uni_tex_light_depth_shadow1, vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep_1), 0.0).x;
				}
			}
			s1 = s1 / (loop * loop);	
			//s1 = shadow2D(uni_tex_light_depth_shadow1, vec3(uv_center, light_view_dep_1), 0.0).x;
		}
	}

	//变换到光源2的空间
	vec4 light_view_pos_2 = uni_light_view_mtx[1] * world_pos;
	vec4 light_proj_pos_2 = uni_light_proj_mtx[1] * light_view_pos_2;
	light_proj_pos_2 = light_proj_pos_2 / light_proj_pos_2.w;
	float light_view_dep_2 = (-light_view_pos_2.z - uni_light_dep_range[1].x) / uni_light_dep_range[1].y;

	float s2 = 0.0;
	if(light_proj_pos_2.x <= 1.0 && light_proj_pos_2.x >= -1.0)
	{
		if(light_proj_pos_2.y <= 1.0 && light_proj_pos_2.y >= -1.0)
		{
			vec2 uv_center = light_proj_pos_2.st * 0.5 + 0.5;
			int loop = (uni_loop > 0) ? uni_loop : 1;
			int loop_half = loop / 2;
			vec2 uv = uv_center - loop_half * uni_light_dep_pix_offset;
			for(int i = 0; i < loop; i ++){
				for(int j = 0; j < loop; j ++){		
					s2 += shadow2D(uni_tex_light_depth_shadow2, vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep_2), 0.0).x;
				}
			}
			s2 = s2 / (loop * loop);
			//s2 = shadow2D(uni_tex_light_depth_shadow2, vec3(uv_center, light_view_dep_2), 0.0).x;
		}
	}

	float s = (s1 + s2) / 2.0f;
	gl_FragColor = vec4(s,s,s,1) * sence_light;
#else

	float fmix = 0.0;
	float s1 = 0.0;
	float s2 = 0.0;
	bool find_seg = false;
	int cas_count = 0;
	int cas_seg_in = 0;
	for(int cas = 0; cas < uni_seg_count; cas ++)
	{
		//变换到光源1的空间
		vec4 light_view_pos = uni_light_view_mtx[cas] * world_pos;
		vec4 light_proj_pos = uni_light_proj_mtx[cas] * light_view_pos;
		light_proj_pos = light_proj_pos / light_proj_pos.w;
		float light_view_dep = (-light_view_pos.z - uni_light_dep_range[cas].x) / uni_light_dep_range[cas].y;

		if(light_view_dep < 1.0 &&
			light_proj_pos.x <= 1.0 &&
			light_proj_pos.x >= -1.0 &&
			light_proj_pos.y <= 1.0 &&
			light_proj_pos.y >= -1.0
			)
		{
			float s = 0.0;
			vec2 uv_center = light_proj_pos.st * 0.5 + 0.5;
			int loop = uni_loop - cas;
			loop = (loop > 0) ? loop : 1;
			int loop_half = loop / 2;
			vec2 uv = uv_center - loop_half * uni_light_dep_pix_offset;
			for(int i = 0; i < loop; i ++){
				for(int j = 0; j < loop; j ++){		
					s += shadow2D(uni_tex_light_depth_shadow[cas], vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
				}
			}
			s = s / (loop * loop);

			cas_count ++;

			if(!find_seg)
			{
				s1 = s;
				float m1 = min(uv_center.s, uv_center.t);
				float m2 = min(1.0 - uv_center.s, 1.0 - uv_center.t);
				fmix = min(min(m1, m2)/0.25, 1.0);
				cas_seg_in = cas;
			}
			else
			{
				s2 = s;
				break;
			}

			find_seg = true;
		}
	}
	
#if 0
	float final_1 = s1;
	float final_2 = mix(s2, s1, fmix);
	float final_s = mix(final_1, final_2, cas_count-1);//其实是 final_1/final_2中二选1
#else
	float final_s = 0.0;
	if(1 == cas_count)
		final_s = s1;
	else
		final_s = s1 * fmix + s2 * (1.0 - fmix); //mix(s2, s1, fmix);// 
#endif

#if 1
	gl_FragColor = vec4(final_s, final_s, final_s, 1) * sence_light;
	//gl_FragColor = sence_light;
#else
	if(0 == cas_seg_in)
		gl_FragColor = vec4(final_s/2, 0,       final_s/2, 1) * sence_light;
	else if(1 == cas_seg_in)
		gl_FragColor = vec4(final_s/2, 0,       0,         1) * sence_light;
	else if(2 == cas_seg_in)
		gl_FragColor = vec4(0,       final_s/2, 0,         1) * sence_light;
	else if(3 == cas_seg_in)
		gl_FragColor = vec4(0,       0,       final_s/2,   1) * sence_light;
#endif
	return;

#endif
}

