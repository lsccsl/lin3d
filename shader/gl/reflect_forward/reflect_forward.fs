uniform sampler2D uni_bump; //��͹����
uniform sampler2D uni_tex1; //��������
uniform sampler2D uni_tex2; //��������
uniform sampler2D uni_tex_fresnel; //������ЧӦ����
// ��ͷ����ˮ����ĵط��ǳ��峺���ף�������٣�������Զ�ĵط�ȴ��ӳ����գ�����϶ࣩ������Ƿ����ЧӦ 
// �ο���ʽ:fresnel����ֵ + fresnel������*pow( 1 - dot( N, V ), 5 )  NΪˮ�淨��,VΪ��Ӱ�������ص�������������
// �˴��򻯳�������

uniform mat4 uni_proj_mtx; //ͶӰ
uniform mat4 uni_view_mtx; //�۲�
uniform mat4 uni_world_mtx; //����任
uniform vec4 uni_wave_offset;
uniform vec4 uni_wave_scale;
uniform float uni_refl_distort;

uniform int uni_need_refract;

uniform float uni_reflect_scale;
uniform float uni_refract_scale;
uniform float uni_color_scale;

varying vec4 vary_view_pos;  //�۲�λ��
varying vec4 vary_world_pos; //����λ��
varying vec3 vary_world_nml; //���編��
varying vec3 vary_view_nml; //�۲취��
varying vec4 vary_proj_pos;
varying vec3 vary_world_view_dir;//���ڼ��������ЧӦ

varying vec4 vary_nonstereo_pos;
varying vec2 vary_bump1;
varying vec2 vary_bump2;

void main()
{

	vec3 tex_bump1 = texture2D(uni_bump, vary_bump1).xyz;
	tex_bump1 = (tex_bump1 - 0.5) * 2.0;/* ת���� (-1,1)���� */
	tex_bump1 = normalize(tex_bump1);

	vec3 tex_bump2 = texture2D(uni_bump, vary_bump2).xyz;
	tex_bump2 = (tex_bump2 - 0.5) * 2.0;/* ת���� (-1,1)���� */
	tex_bump2 = normalize(tex_bump2);
	
	vec3 tex_bump = (tex_bump1 + tex_bump2) * 0.5;
	
	vec4 uv1 = vary_nonstereo_pos;
	uv1.xy = uv1.xy - tex_bump.xy * uni_refl_distort;
	uv1.xy = uv1.xy / uv1.w;
	
	vec4 tex_refl = texture2D(uni_tex1, uv1.xy);

	vec4 clr = tex_refl * uni_reflect_scale + gl_Color * uni_color_scale;
	
	if(1 == uni_need_refract)
	{
		vec4 tex_refr = texture2D(uni_tex2, uv1.xy);

		tex_refr = tex_refr * uni_refract_scale * vec4(.34, .85, .92, 1);

#if 0
		clr += tex_refr;
#else
		vec3 world_view_dir = normalize(vary_world_view_dir);
		float fresnel_fact = dot(world_view_dir.xzy, tex_bump);
		float fresnel = texture2D(uni_tex_fresnel, vec2(fresnel_fact)).w;
		
		clr = mix(tex_refr, tex_refl * uni_reflect_scale, fresnel) + gl_Color * uni_color_scale;
#endif		
	}	

	gl_FragColor = vec4(clr.x, clr.y, clr.z, 1);

#if 0
/// test
	vec4 uv = vary_proj_pos;
	uv.xy = (uv.xy/uv.w) * 0.5 + 0.5; 
	
tex_refl = texture2D(uni_tex1, uv.xy);
vec4 tex_refr = texture2D(uni_tex2, uv.xy);
	
vec3 world_view_dir1 = normalize(vary_world_view_dir);
float fresnel_fact1 = dot(world_view_dir1.xzy, vec3(0,0,1));
float fresnel1 = texture2D(uni_tex_fresnel, vec2(fresnel_fact1)).w;
clr = mix(tex_refr, tex_refl, fresnel1);
//clr.xyz = vec3(fresnel1);
gl_FragColor = vec4(clr.x, clr.y, clr.z, 1);
	
//	gl_FragColor = texture2D(uni_tex1, uv.xy) * 0.8 + gl_Color * 0.2;

//gl_FragColor = texture2D(uni_tex2, uv.xy);
//gl_FragColor.g = 0;

//gl_FragColor = tex2;
//gl_FragColor.r = 0;
//gl_FragColor=tex1;

#endif
}
