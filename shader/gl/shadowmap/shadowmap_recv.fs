/*** shadowmap_recv.fs ***/

uniform sampler2D uni_tex_light; //����
//uniform sampler2D uni_tex_clr; //��ɫ
uniform sampler2D uni_tex_view_depth; //��Ӱ���ռ��������

//uniform sampler2D uni_tex_light_depth_tex; //��Դ�ռ����
uniform sampler2DShadow uni_tex_light_depth_shadow; //ͬһ������,���

uniform float uni_tan_half_fov; //�ӽ�һ���tanֵ
uniform float uni_aspect; //��Ļ��߱�
uniform vec2 uni_dep_range; //������

uniform mat4 uni_view_inv_mtx; //�۲������

uniform vec2 uni_light_dep_range; //��Դ��depth range
uniform mat4 uni_light_view_mtx; //��Դ�۲�
uniform mat4 uni_light_proj_mtx; //��ԴͶӰ
uniform vec2 uni_light_dep_pix_offset; //��Դ�������ֱ���
uniform int uni_loop;


varying vec2 vary_proj_pos;


//==============================================================================

/**
* @brief ��������,�õ�����ӿռ�����
* @param xy_proj:ȡֵΪ0,1(ͶӰ���x y)
*/
void get_pos_by_dep(float dep, vec2 xy_proj, float tan_half_fov, float aspect, out vec3 pos)
{
	/* �������ֵ,��ԭ�����ӿռ�λ����Ϣ */
	pos = vec3(tan_half_fov * xy_proj.x * aspect * dep, tan_half_fov * xy_proj.y * dep, -dep);
}

void tex_dep_2_view_dep(float tex_dep, vec2 dep_range, out float view_dep)
{
	view_dep = (dep_range.y * tex_dep + dep_range.x);
}

//==============================================================================

void shadowmap_recv_fs()
{
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	//gl_FragDepth = tex_dep;	

	vec4 sence_light = texture2D(uni_tex_light, gl_TexCoord[0].st);
	//vec4 sence_diff = texture2D(uni_tex_clr, gl_TexCoord[0].st) * 0.1;

	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos.xy, uni_tan_half_fov, uni_aspect, view_pos);

	vec4 world_pos = uni_view_inv_mtx * vec4(view_pos, 1);

	//�任����Դ�Ŀռ�
	vec4 light_view_pos = uni_light_view_mtx * world_pos;
	vec4 light_proj_pos = uni_light_proj_mtx * light_view_pos;
	light_proj_pos = light_proj_pos / light_proj_pos.w;

#if 1
	if(light_proj_pos.x > 1.0 || light_proj_pos.x < -1.0)
	{
		gl_FragColor = sence_light;// + sence_diff;//abs(vec4(1, 1, 1, 1));
		return;
	}
	if(light_proj_pos.y > 1.0 || light_proj_pos.y < -1.0)
	{
		gl_FragColor = sence_light;
		return;
	}
	
#endif

	float light_view_dep = (-light_view_pos.z - uni_light_dep_range.x) / uni_light_dep_range.y;
	
	if(light_view_dep > 1.0 || light_view_dep < 0.0)
	{
		gl_FragColor = sence_light;
		return;
	}
	
	vec2 uv = light_proj_pos.st * 0.5 + 0.5;

	int loop = (uni_loop > 0) ? uni_loop : 1;
	int loop_half = loop / 2;
	float s = 0.0;
	uv = uv - loop_half * uni_light_dep_pix_offset;
	for(int i = 0; i < loop; i ++){
		for(int j = 0; j < loop; j ++){		
			s += shadow2D(uni_tex_light_depth_shadow, vec3(uv + vec2(j * uni_light_dep_pix_offset.x, i * uni_light_dep_pix_offset.y), light_view_dep), 0.0).x;
		}
	}
	s = s / (loop * loop);
	
	gl_FragColor = vec4(s, s, s, 1) * sence_light;// + sence_diff;
	
	
//gl_FragColor = vec4(s, s, s, 1);
}

void main()
{
	shadowmap_recv_fs();
}