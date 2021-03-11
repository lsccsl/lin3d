/*** defer_render_gbuffer.fs ***/

uniform sampler2D uni_tex1; //����1
uniform sampler2D uni_tex2; //����2

uniform vec2 uni_dep_range; //������
uniform mat4 uni_world_mtx; //����任
uniform mat4 uni_view_mtx; //�۲�
uniform mat4 uni_proj_mtx; //ͶӰ����

uniform float uni_shininess;
uniform int uni_enable_reflect; //�Ƿ�������


varying vec4 vary_world_pos; //����λ��
varying vec4 vary_proj_pos; //ͶӰ
varying vec3 vary_world_nml; //���編��
varying vec3 vary_view_nml; //�۲취��

varying float vary_dep_line_z; //��������

void main()
{
	vec4 tex1 = texture2D(uni_tex1, gl_TexCoord[0].st);

	vec4 vdep = vec4(vary_dep_line_z, vary_dep_line_z, vary_dep_line_z, 1);

	vec3 world_nml = normalize(vary_world_nml);
	world_nml = (world_nml + 1.0) * 0.5;
	vec3 view_nml = normalize(vary_view_nml);
	view_nml = (view_nml + 1.0) * 0.5;

	gl_FragData[0] = tex1 * gl_Color;
	gl_FragData[0].a = uni_enable_reflect;
	gl_FragData[1] = vec4(view_nml, uni_shininess);
	gl_FragData[2] = vdep;
	
	//�������͸�ӳ��� �ɷǿɱ����Ⱦ���߲����������˴�������
	//gl_FragDepth = vary_proj_pos.z / vary_proj_pos.w;
}
