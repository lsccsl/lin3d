uniform mat4 uni_proj_mtx; //投影
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_world_mtx; //世界变换
uniform vec2 uni_dep_range; //最大深度

varying vec4 vary_view_pos;  //观察位置
varying vec4 vary_world_pos; //世界位置
varying vec3 vary_world_nml; //世界法线
varying vec3 vary_view_nml; //观察法线
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