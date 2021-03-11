uniform sampler2D uni_tex1; //����1
uniform sampler2D uni_rand_tex;
uniform float uni_dissolution_threshold; //�۽ֵⷧ
uniform float uni_dissolution_transition; //�۽��Ե����

uniform mat4 uni_proj_mtx; //ͶӰ
uniform mat4 uni_view_mtx; //�۲�
uniform mat4 uni_world_mtx; //����任
uniform vec2 uni_dep_range; //������

varying vec4 vary_view_pos;  //�۲�λ��
varying vec4 vary_world_pos; //����λ��
varying vec3 vary_world_nml; //���編��
varying vec3 vary_view_nml; //�۲취��
varying vec4 vary_proj_pos;

void main()
{
	gl_FragColor = texture2D(uni_tex1, gl_TexCoord[0].st);
	vec4 r = texture2D(uni_rand_tex, gl_TexCoord[0].st);
	
	float diff = r.r - uni_dissolution_threshold;
	float edge_smooth = smoothstep(0, uni_dissolution_transition, diff); 	
	
	gl_FragColor.a = edge_smooth;	
}

/*

�����������
�����뷧ֵ�Ƚ�,����͸����

*/


