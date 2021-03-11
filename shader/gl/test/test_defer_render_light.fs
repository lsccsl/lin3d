/*** defer_render_light.fs ***/

//#define E_LIGHT_DIR 0
//#define E_LIGHT_SPOT 1
//#define E_LIGHT_POINT 2

uniform float uni_tan_half_fov; //�ӽ�һ���tanֵ
uniform vec2 uni_dep_range; //������
uniform float uni_aspect; //��Ļ��߱�	

uniform sampler2D uni_tex_nml; //����
uniform sampler2D uni_tex_clr; //��ɫ
uniform sampler2D uni_tex_view_depth; //��Ӱ���ռ��������

uniform int uni_light_type; //�������
uniform vec3 uni_light_view_spot_dir; //Spotlight����
uniform vec4 uni_light_view_pos; //�ƹ�λ��(w=0ƽ�йⷽ�� w=1���Դλ��)
uniform vec3 uni_light_clr;
uniform float uni_light_attenuation; //������ڴ�ֵ��ʼ˥��
uniform float uni_light_max_len; //������ڴ�ֵ����˥��Ϊ0
uniform float uni_light_cutoff_cos; //�۹��,��׶�������Ե�нǵ�cosֵ
uniform float uni_light_exponent; //�۹�����������Ե˥��


varying vec2 vary_proj_pos;


//========================================================================

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

//========================================================================

vec4 defer_render_light_dir(vec3 view_pos, vec3 view_nml, float shininess)
{
	float light_dot = clamp(dot(-uni_light_view_pos.xyz, view_nml), 0.0, 1.0);

	vec4 color = vec4(0, 0, 0, 1);
	color.xyz = light_dot * uni_light_clr * shininess;

	return color;
}

vec4 defer_render_light_spot(vec3 view_pos, vec3 view_nml)
{
	vec3 light_dir = normalize(uni_light_view_pos.xyz - view_pos);
	float light_dot = clamp(dot(light_dir, view_nml), 0.0, 1.0);

	float dis = length(view_pos - uni_light_view_pos.xyz);
	float attenuation = 1.0 - smoothstep (uni_light_attenuation, uni_light_max_len, dis);

	float cos_spot = dot(-light_dir, uni_light_view_spot_dir);
	float delta_cos = clamp(cos_spot - uni_light_cutoff_cos, 0.0, 1.0);
	float tmp = cos_spot * ceil(delta_cos);

	float effect_spot = pow(clamp(cos_spot, 0.0, 1.0), uni_light_exponent) * tmp;

	vec4 color = vec4(0, 0, 0, 1);
	float light_final = light_dot * attenuation * effect_spot;
	color.xyz = uni_light_clr * light_final;

	return color;
}

vec4 defer_render_light_point(vec3 view_pos, vec3 view_nml)
{
	vec3 light_dir = normalize(uni_light_view_pos.xyz - view_pos);
	float light_dot = clamp(dot(light_dir, view_nml), 0.0, 1.0);

	float dis = length(view_pos - uni_light_view_pos.xyz);
	float attenuation = 1.0 - smoothstep (uni_light_attenuation, uni_light_max_len, dis);

	vec4 color = vec4(0, 0, 0, 1);
	color.xyz = light_dot * uni_light_clr * attenuation;

	return color;
}

//========================================================================

void defer_render_lighting_fs()
{
	//������Ȼ���,��ԭ������,�������

	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	vec4 tex_nml = texture2D(uni_tex_nml, gl_TexCoord[0].st);
	vec3 view_nml = tex_nml.xyz * 2.0 - 1.0;
	vec4 diff = texture2D(uni_tex_clr, gl_TexCoord[0].st);

	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos.xy, uni_tan_half_fov, uni_aspect, view_pos);
	
	vec4 color;
	switch(uni_light_type)
	{
	case E_LIGHT_DIR:
		color = defer_render_light_dir(view_pos, view_nml, tex_nml.a);
		break;

	case E_LIGHT_SPOT:
		color = defer_render_light_spot(view_pos, view_nml);
		break;

	case E_LIGHT_POINT:
		color = defer_render_light_point(view_pos, view_nml);
		break;
	}
	gl_FragColor = color;

//gl_FragColor = diff + color;
//gl_FragDepth = tex_dep;
//gl_FragColor.xyz = abs(view_pos / 1000.0);
//gl_FragColor.rgb = vec3(1,1,1);
}

void main()
{
	defer_render_lighting_fs();
}