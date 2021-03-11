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
	vary_world_pos = uni_world_mtx * gl_Vertex;
	vary_view_pos  = uni_view_mtx * vary_world_pos;
	vary_proj_pos = uni_proj_mtx * vary_view_pos;
	gl_Position = vary_proj_pos;

	vary_world_nml = (uni_world_mtx * vec4(gl_Normal, 0.0)).xyz;
	vary_view_nml = (uni_view_mtx * uni_world_mtx * vec4(gl_Normal, 0.0)).xyz;

	gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_FrontColor = gl_Color;
}