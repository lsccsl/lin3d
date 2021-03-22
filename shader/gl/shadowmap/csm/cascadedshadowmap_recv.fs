/*** cascadedshadowmap_recv.fs ***/

uniform sampler2D uni_tex_view_depth; //摄影机空间深度纹理
uniform sampler2D uni_tex_light; //光照

uniform mat4 uni_view_inv_mtx; //观察逆矩阵
uniform float uni_tan_half_fov; //视角一半的tan值
uniform float uni_aspect; //屏幕宽高比
uniform vec2 uni_dep_range; //最大深度

//uniform sampler2D uni_tex_light_depth_tex; //光源空间深度
uniform sampler2D uni_tex_light_depth_tex1;
uniform sampler2D uni_tex_light_depth_tex2;
uniform sampler2DShadow uni_tex_light_depth_shadow1; //同一个纹理,深度
uniform sampler2DShadow uni_tex_light_depth_shadow2; //同一个纹理,深度
uniform float uni_far_thredhold;
uniform vec2 uni_light_dep_range[2]; //光源的depth range
uniform mat4 uni_light_view_mtx[2]; //光源观察
uniform mat4 uni_light_proj_mtx[2]; //光源投影
uniform vec2 uni_light_dep_pix_offset; //光源深度纹理分辨率
uniform int uni_loop;


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
		}
	}

	//变换到光源1的空间
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
			float s = 0.0;
			vec2 uv = uv_center - loop_half * uni_light_dep_pix_offset;
			for(int i = 0; i < loop; i ++){
				for(int j = 0; j < loop; j ++){		
					s2 += shadow2D(uni_tex_light_depth_shadow2, vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep_2), 0.0).x;
				}
			}
			s2 = s2 / (loop * loop);
		}
	}

	float s = (s1 + s2) / 2.0f;
	gl_FragColor = vec4(s,s,s,1) * sence_light;

	//vec2 uv_center = light_proj_pos_1.st * 0.5 + 0.5;
	//int loop = (uni_loop > 0) ? uni_loop : 1;
	//int loop_half = loop / 2;
	//float s = 0.0;
	//vec2 uv = uv_center - loop_half * uni_light_dep_pix_offset;
	//for(int i = 0; i < loop; i ++){
	//	for(int j = 0; j < loop; j ++){		
	//		s += shadow2D(uni_tex_light_depth_shadow1, vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep_1), 0.0).x;
	//	}
	//}
	//s = s / (loop * loop);



	//float s = shadow2D(uni_tex_light_depth_shadow1, vec3(uv, light_view_dep_1), 0.0).x;
	
	//gl_FragColor = vec4(s,s,s,1);// * sence_light;//vec4(s);//vec4(abs(uni_light_dep_range[0].y));//vec4(light_view_dep_1/10, light_view_dep_1/10, light_view_dep_1/10, 1);

	//gl_FragColor = texture2D(uni_tex_light_depth_tex1, uv_center) - light_view_dep_1;

    //根据深度,查出对应的采样纺理
    //比较深度
	//gl_FragColor = abs(vec4(-light_view_pos_1.z, -light_view_pos_1.z, -light_view_pos_1.z, 1))/1000;
	//gl_FragColor = texture2D(uni_tex_light_depth_tex1, gl_TexCoord[0].st);// vec4(tex_dep, tex_dep, tex_dep, 1);
}
