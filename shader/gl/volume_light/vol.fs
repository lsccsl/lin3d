/*** vol.fs ***/

uniform sampler2D uni_tex_view_depth; //��Ӱ���ռ��������

uniform sampler2D uni_tex_clr; //��ɫ
uniform sampler2D uni_tex_light_depth_tex; //��Դ�ռ����
uniform sampler2DShadow uni_tex_light_depth_shadow; //ͬһ������,���

uniform float uni_tan_half_fov; //�ӽ�һ���tanֵ
uniform float uni_aspect; //��Ļ��߱�
uniform vec2 uni_dep_range; //������

uniform mat4 uni_view_inv_mtx; //�۲������

uniform int uni_light_type; //�������
uniform vec2 uni_light_dep_range; //��Դ��depth range
uniform mat4 uni_light_view_mtx; //��Դ�۲�
uniform mat4 uni_light_proj_mtx; //��ԴͶӰ
uniform vec3 uni_light_world_spot_dir; //Spotlight����
uniform float uni_light_cutoff_cos; //�۹��,��׶�������Ե�нǵ�cosֵ
uniform vec3 uni_light_clr; //�ƹ���ɫ

uniform vec3 uni_eye_world_pos;
uniform vec3 uni_light_world_pos;

varying vec2 vary_proj_pos;

int max_step = 500;
int min_step = 1;
float sample_dis = 0.15;


// todo : ����ⲿ��������Ȼ�Ϊ 1, ���·��ӳ���Ⱦ�����Ѳ������򸲸�


void cal_coordinates(out vec4 coordinates, vec4 pos)
{
	vec4 pos_light_space = uni_light_view_mtx * pos;
	coordinates = uni_light_proj_mtx * pos_light_space;
	
	coordinates.z = (-pos_light_space.z - uni_light_dep_range.x)/(uni_light_dep_range.y);
}

void main()
{
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
//gl_FragDepth = tex_dep;return;
	
	float light_dep = texture2D(uni_tex_light_depth_tex, gl_TexCoord[0].st).x;
	//vec3 clr = texture2D(uni_tex_clr, gl_TexCoord[0].st).x;


	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos, uni_tan_half_fov, uni_aspect, view_pos);

	vec4 world_pos = uni_view_inv_mtx * vec4(view_pos, 1);

	vec3 view_dir = normalize(world_pos.xyz - uni_eye_world_pos);
	
	vec4 pos_start;
	pos_start.xyz = uni_eye_world_pos + view_dir; //todo : and rand here
	pos_start.w = 1.0;

	float trace_dis = length(world_pos.xyz - pos_start.xyz);
	int step_count = int(trace_dis / sample_dis);
	step_count = min(step_count, max_step);
	step_count = max(step_count, min_step);

	vec4 pos_delt;
	pos_delt.xyz = sample_dis * view_dir;
	pos_delt.w = 1.0;

	vec4 coordinates;
	/* �����ʼ��,��ʼ����ӳ��,�ڹ�Դ����ϵ���zֵ */
	cal_coordinates(coordinates, pos_start);

	/* ��һ������λ�� */
	vec4 coordinates_next;
	vec4 pos_next;
	pos_next.xyz = pos_start.xyz + sample_dis * view_dir;
	pos_next.w = 1.0;
	cal_coordinates(coordinates_next, pos_next);

	vec4 coord_delta;
	coord_delta = (coordinates_next - coordinates);


	vec4 tmp_coord;
	float scale = 0.0;

	float light_clr = 0.0;
	float shadow_depth = 0.0;
	vec3 cur_pos_world = pos_start.xyz;
	
	float vol_scale = 1.0;
	if(uni_light_type == E_LIGHT_DIR)
		vol_scale = 0.45;

	for(int i = 0; i < step_count; i ++)
	{
		coordinates = coordinates + coord_delta;
		cur_pos_world = cur_pos_world + pos_delt.xyz;

		tmp_coord = coordinates;
		tmp_coord.xy = tmp_coord.xy / tmp_coord.w;
		
		if(tmp_coord.x > 1.0 || tmp_coord.x < -1.0)
			continue;
		if(tmp_coord.y > 1.0 || tmp_coord.y < -1.0)
			continue;
		if(tmp_coord.z < 0)
			continue; 
		if(tmp_coord.z > 1.0)
			continue; 
		
		tmp_coord.xy = tmp_coord.xy * 0.5 + 0.5;

		float attenuation = 1.0;
#if 0
		if(uni_light_type == E_LIGHT_DIR)
		{
			vec3 light_dir = (cur_pos_world - uni_light_world_pos);
			float dis = length(light_dir);
			light_dir = normalize(light_dir);
			float cos_spot = dot(light_dir, uni_light_world_spot_dir);
			float delta_cos = clamp(cos_spot - uni_light_cutoff_cos, 0.0, 1.0);	
			float tmp = cos_spot * ceil(delta_cos);

			float attenuation1 = 1.0 - smoothstep (30.0, 100.0, dis);
			float attenuation2 = 1.0 - smoothstep (30.0, 1000.0, dis);
			attenuation = (attenuation1 + attenuation2 * 0.1) * tmp;//attenuation1 * tmp;
		}
#endif

#if 1
		scale = shadow2D(uni_tex_light_depth_shadow, vec3(tmp_coord.xy, tmp_coord.z), 0.0).x;
#else
		//shadow_depth = texture2D(uni_tex_light_depth_tex, tmp_coord.xy).x;
		//if(tmp_coord.z < shadow_depth)
		//	scale = 1.0;
		//else
		//	scale = 0.0;
#endif		

		light_clr += scale * attenuation;
	}

	light_clr = vol_scale * light_clr / step_count;

	gl_FragColor = vec4(uni_light_clr * light_clr, 1);
}


/*
{
vec4 tmpcoordinates;
cal_coordinates(tmpcoordinates, world_pos);
tmpcoordinates.xy = tmpcoordinates.xy / tmpcoordinates.w;
tmpcoordinates.xy = tmpcoordinates.xy * 0.5 + 0.5;

float tmp_shadow = shadow2D(uni_tex_light_depth_shadow, vec3(tmpcoordinates.xy, tmpcoordinates.z), 0.0).x;

gl_FragColor.xyz = vec3(tmp_shadow, tmp_shadow, tmp_shadow) + clr * 0.2;

//float light_dep = texture2D(uni_tex_light_depth_tex, tmpcoordinates.xy).x;
//if(tmpcoordinates.z < light_dep)
//	gl_FragColor.xyz = vec3(0.8, 0.8, 0.8) + clr * 0.2;
//else
//	gl_FragColor.xyz = vec3(0.0, 0.0, 0.0) + clr * 0.2;
return;
}
*/