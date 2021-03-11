/*** ssr.fs ***/
#extension GL_EXT_gpu_shader4 : enable

#define SSR_SAMPLE_TYPE_ADAPT 0
#define SSR_SAMPLE_TYPE_LINE  1

uniform int uni_sample_type;

uniform sampler2D uni_tex_reflect_src; //反射源
uniform sampler2D uni_tex_final; //场景颜色
uniform sampler2D uni_tex_nml; //法线
uniform sampler2D uni_tex_clr; //颜色
uniform sampler2D uni_tex_view_depth; //摄影机空间深度纹理

uniform float uni_tan_half_fov; //视角一半的tan值
uniform vec2 uni_dep_range; //最大深度
uniform float uni_aspect; //屏幕宽高比	

uniform mat4 uni_proj_mtx; //投影矩阵

uniform int uni_screen_x_pix;
uniform int uni_screen_y_pix;

uniform int uni_sample_num;        //采样次数

uniform float uni_trace_start; //搜索的起始
uniform float uni_trace_step;  //搜索步长
uniform int uni_trace_count; //搜索次数
uniform int uni_trace_sec_count; // 每个步长细分搜索次数


varying vec2 vary_proj_pos;

//===========================================
#define PI 3.14159265358979323846

void ScrambleTEA(uint v[2], int IterationCount, out uint out_v[2])
{
	// Start with some random data (numbers can be arbitrary but those have been used by others and seem to work well)
	uint k[4];//{ 0xA341316Cu , 0xC8013EA4u , 0xAD90777Du , 0x7E95761Eu };
	k[0] = 0xA341316Cu;
	k[1] = 0xC8013EA4u;
	k[2] = 0xAD90777Du;
	k[3] = 0x7E95761Eu;
	
	uint y = v[0];
	uint z = v[1];
	uint total = 0x0u;
	
	for(int i = 0; i < IterationCount; ++i)
	{
		total += 0x9e3779b9u;
		y += (z << 4u) + k[0] ^ z + total ^ (z >> 5u) + k[1];
		z += (y << 4u) + k[2] ^ y + total ^ (y >> 5u) + k[3];
	}

	out_v[0] = y;
	out_v[1] = z;
}

uint ReverseBits32( uint bits )
{
	bits = ( bits << 16) | ( bits >> 16);
	bits = ( (bits & 0x00ff00ffu) << 8 ) | ( (bits & 0xff00ff00u) >> 8 );
	bits = ( (bits & 0x0f0f0f0fu) << 4 ) | ( (bits & 0xf0f0f0f0u) >> 4 );
	bits = ( (bits & 0x33333333u) << 2 ) | ( (bits & 0xccccccccu) >> 2 );
	bits = ( (bits & 0x55555555u) << 1 ) | ( (bits & 0xaaaaaaaau) >> 1 );
	return bits;
}

void Hammersley( uint Index, uint NumSamples, uint Random[2], out float out_f[2] )
{
	float E1 = float(Index) / float(NumSamples);
	E1 += float( Random[0] & 0xffffu ) / float(1<<16);
	
	E1 = fract(E1);
	float E2 = float( ReverseBits32(Index) ^ Random[1] ) * 2.3283064365386963e-10;
	
	out_f[0] = E1;
	out_f[1] = E2;
}

float ClampedPow(float X, float Y)
{
	return pow(max(abs(X),0.000001f),Y);
}

void ImportanceSampleBlinn( float E[2], float Roughness, out vec4 v_out)
{
	float m = Roughness * Roughness;
	float n = 2.0 / (m*m) - 2.0;

	float Phi = 2 * PI * E[0];
	float CosTheta = ClampedPow( E[1], 1.0 / (n + 1.0) );
	float SinTheta = sqrt( 1 - CosTheta * CosTheta );

	float H_x = SinTheta * cos( Phi );
	float H_y = SinTheta * sin( Phi );
	float H_z = CosTheta;

	float D = (n+2.0)/ (2.0*PI) * ClampedPow( CosTheta, n );
	float PDF = D * CosTheta;

	v_out.x = H_x;
	v_out.y = H_y;
	v_out.z = H_z;
	v_out.w = PDF;
}

vec3 TangentToWorld( vec3 Vec, vec3 TangentZ )
{
	vec3 UpVector = abs(TangentZ.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 TangentX = normalize( cross( UpVector, TangentZ ) );
	vec3 TangentY = cross( TangentZ, TangentX );

	return TangentX * Vec.x + TangentY * Vec.y + TangentZ * Vec.z;
}

//===========================================

void main()
{
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	vec4 tex_nml = texture2D(uni_tex_nml, gl_TexCoord[0].st);
	vec3 view_nml = tex_nml.xyz * 2.0 - 1.0;
	vec4 clr = texture2D(uni_tex_clr, gl_TexCoord[0].st);

	vec3 org_clr = texture2D(uni_tex_final, gl_TexCoord[0].st).xyz;

	if(0 == clr.a)
	{
		gl_FragColor.xyz = org_clr;
		return;
	}

	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos.xy, uni_tan_half_fov, uni_aspect, view_pos);
	
	vec3 view_dir = normalize(view_pos);
	view_nml = normalize(view_nml);

	//============== 开始处理 ================

/*
	uint pix_pos[2];
	uint rand_v[2];

	pix_pos[0] = uint(gl_TexCoord[0].s * uni_screen_x_pix) ^ 1551u;
	pix_pos[1] = uint(gl_TexCoord[0].t * uni_screen_y_pix) ^ 1551u;
	ScrambleTEA(pix_pos, 3, rand_v); 

	float Roughness = 0.10;
*/

	vec3 final_clr = vec3(0,0,0);
	
	for(int ir = 0; ir < uni_sample_num; ir ++) {

#if 0
		//随机扰动法线
		float E[2];
		Hammersley( uint(ir), uint(sample_num), rand_v, E );
		
		vec4 H;
		ImportanceSampleBlinn( E, Roughness, H);
		vec3 h_t = normalize(H.xyz);                      

		vec3 normal_tmp = TangentToWorld(h_t, view_nml);
		normal_tmp = normalize(normal_tmp);
		vec3 trace_dir = reflect(view_dir, normal_tmp);
#else
		vec3 trace_dir = reflect(view_dir, view_nml);
#endif

		trace_dir = normalize(trace_dir);

		//float sec_trace_step = uni_trace_step / uni_trace_sec_count;
		
		vec3 trace_step        = uni_trace_step * trace_dir;
		vec3 second_trace_step = trace_step / uni_trace_sec_count; //sec_trace_step * trace_dir;

		vec3 trace_pos_start = view_pos + uni_trace_start * trace_dir;
		vec4 find_start_view = vec4(trace_pos_start, 1);
		vec4 find_start_proj = uni_proj_mtx * find_start_view;
		find_start_proj.z = find_start_view.z;

		vec4 find_end_view = vec4(trace_pos_start + trace_step, 1);
		vec4 find_end_proj = uni_proj_mtx * find_end_view;
		find_end_proj.z = find_end_view.z;
		vec4 find_step = find_end_proj - find_start_proj;

		vec4 second_find_end_view = vec4(trace_pos_start + second_trace_step, 1);
		vec4 second_find_end_proj = uni_proj_mtx * second_find_end_view;
		second_find_end_proj.z = second_find_end_view.z;			
		vec4 second_find_step = second_find_end_proj - find_start_proj;

	#if 0
		float screen_pix_offset_x = 1. / uni_screen_x_pix;
		float screen_pix_offset_y = 1. / uni_screen_y_pix;
//gl_FragColor.xyz = abs(vec3(1));
		//保证至少步进一个像素
		float dec_y = abs(find_start_proj.y/find_start_proj.w - second_find_end_proj.y/second_find_end_proj.w);
		float dec_x = abs(find_start_proj.x/find_start_proj.w - second_find_end_proj.x/second_find_end_proj.w);
		if(dec_y >= dec_x) {
//gl_FragColor.xyz = abs(vec3(1,0,1));		
			if(dec_y < screen_pix_offset_y) {
				float scale = max((screen_pix_offset_y / dec_y), 1.0);

				//float sec_trace_step = scale * uni_trace_step / uni_trace_sec_count;

				trace_step        = scale * trace_step;
				second_trace_step = trace_step / uni_trace_sec_count;

				trace_pos_start = view_pos + uni_trace_start * trace_dir;
				find_start_view = vec4(trace_pos_start, 1);
				find_start_proj = uni_proj_mtx * find_start_view;
				find_start_proj.z = find_start_view.z;

				find_end_view = vec4(trace_pos_start + trace_step, 1);
				find_end_proj = uni_proj_mtx * find_end_view;
				find_end_proj.z = find_end_view.z;
				find_step = find_end_proj - find_start_proj;

				second_find_end_view = vec4(trace_pos_start + second_trace_step, 1);
				second_find_end_proj = uni_proj_mtx * second_find_end_view;
				second_find_end_proj.z = second_find_end_view.z;			
				second_find_step = second_find_end_proj - find_start_proj;
			}
		}
		else {
//gl_FragColor.xyz = abs(vec3(0));
			if(dec_x < screen_pix_offset_x) {
				float scale = max((screen_pix_offset_x / dec_x), 1.0);

				//float sec_trace_step = scale * uni_trace_step / uni_trace_sec_count;
				
				trace_step        = scale * trace_step;
				second_trace_step = trace_step / uni_trace_sec_count;

				trace_pos_start = view_pos + uni_trace_start * trace_dir;
				find_start_view = vec4(trace_pos_start, 1);
				find_start_proj = uni_proj_mtx * find_start_view;
				find_start_proj.z = find_start_view.z;

				find_end_view = vec4(trace_pos_start + trace_step, 1);
				find_end_proj = uni_proj_mtx * find_end_view;
				find_end_proj.z = find_end_view.z;
				find_step = find_end_proj - find_start_proj;

				second_find_end_view = vec4(trace_pos_start + second_trace_step, 1);
				second_find_end_proj = uni_proj_mtx * second_find_end_view;
				second_find_end_proj.z = second_find_end_view.z;			
				second_find_step = second_find_end_proj - find_start_proj;
			}
		}
//return;
#endif	


		float cur_diff = 0;
		float last_diff = 0;
		float iloop = 0;

		vec4 current_step = find_start_proj;
		vec4 last_step    = find_start_proj;
		
		vec3 second_trace_pos_start = view_pos + uni_trace_start * trace_dir - trace_step;

		for(int i = 0; i <= uni_trace_count; i ++){
//gl_FragColor.xyz = (vec3(i-90));
			vec2 tmp_proj = current_step.xy / current_step.w;			
			vec2 tmp_uv = (tmp_proj) * 0.5 + 0.5;

			float dep_tmp = texture2D(uni_tex_view_depth, tmp_uv).x;
			float dep_tmp_real;
			tex_dep_2_view_dep(dep_tmp, uni_dep_range, dep_tmp_real);
			dep_tmp_real = -dep_tmp_real;

			last_diff = cur_diff;
			cur_diff = current_step.z - dep_tmp_real;
			
			if(cur_diff <= 0.0000) {

				vec4 second_find_start_proj = last_step;

				float second_cur_diff = 0;
				float second_last_diff = 0;

				vec4 second_current_step = second_find_start_proj;
				vec4 second_last_step    = second_find_start_proj;

				for(int j = 0; j <= uni_trace_sec_count + 1; j ++){
				
					vec2 second_tmp_proj = second_current_step.xy / second_current_step.w;			
					vec2 second_tmp_uv = (second_tmp_proj) * 0.5 + 0.5;

					float second_dep_tmp = texture2D(uni_tex_view_depth, second_tmp_uv).x;
					float second_dep_tmp_real;
					tex_dep_2_view_dep(second_dep_tmp, uni_dep_range, second_dep_tmp_real);
					second_dep_tmp_real = -second_dep_tmp_real;

					second_last_diff = second_cur_diff;
					second_cur_diff = second_current_step.z - second_dep_tmp_real;
					
					if(second_cur_diff <= 0.0000) {
						if(second_last_diff >= 0.0000) {
							if(abs(second_cur_diff) < 6 && abs(second_last_diff) < 6)
							{
								float rate = abs(second_cur_diff)/(abs(second_cur_diff) + abs(second_last_diff));

								vec4 intersection_proj = (second_current_step - second_find_step * rate);
								intersection_proj = intersection_proj/intersection_proj.w;
								vec2 intersection_uv = intersection_proj.xy * 0.5 + 0.5;

								float fade_edge = 0;
								if(abs(intersection_proj.y) < 1.0 && abs(intersection_proj.x) < 1.0)
									fade_edge = 1;

								float fade = fade_edge;

								final_clr += texture2D(uni_tex_reflect_src, intersection_uv).xyz * fade;
								break;
							}
							else {
							}
						}
						else {
						}
					}

					second_last_step = second_current_step;
					second_current_step += second_find_step;
				}
				
				break;
			}
			else{
			#if 1
				last_step = current_step;
				current_step += find_step;
				
				second_trace_pos_start += trace_step;
			#else
				last_step = current_step;
				second_trace_pos_start += trace_step;

				int scale = max((i - 20) * 2, 1);

				step_first = step_first * scale;
				trace_step = trace_step * scale;
				find_step = find_step * scale;
				second_step_count = second_step_count * scale;

				current_step += find_step;
				
			#endif
			}
		}	
	}	
//return;
	gl_FragColor.xyz = final_clr;//(final_clr / uni_sample_num) * 1;
	//gl_FragColor.y = 0;
	//gl_FragColor.xyz = (final_clr) * 0.8 + org_clr;
}

