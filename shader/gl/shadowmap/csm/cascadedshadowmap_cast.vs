/*** cascadedshadowmap_cast.vs ***/

uniform vec2 uni_light_dep_range; //��Դ��depth range
uniform mat4 uni_light_view_mtx; //��Դ�۲�
uniform mat4 uni_light_proj_mtx; //��ԴͶӰ

uniform mat4 uni_world_mtx; //����任

varying vec4 vary_view_pos;  //�۲�λ��
varying vec4 vary_world_pos;  //����λ��

void main()
{
	vary_world_pos = uni_world_mtx * gl_Vertex;
	vary_view_pos  = uni_light_view_mtx * vary_world_pos;
	gl_Position = uni_light_proj_mtx * vary_view_pos;

	vary_view_pos.z = (-vary_view_pos.z - uni_light_dep_range.x) / uni_light_dep_range.y;
}