/*** defer_render_gbuffer.fs ***/

uniform sampler2D uni_tex1; //纹理1
uniform sampler2D uni_tex2; //纹理2

uniform vec2 uni_dep_range; //最大深度
uniform mat4 uni_world_mtx; //世界变换
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_proj_mtx; //投影矩阵

uniform float uni_shininess;
uniform int uni_enable_reflect; //是否开启反射


varying vec4 vary_world_pos; //世界位置
varying vec4 vary_proj_pos; //投影
varying vec3 vary_world_nml; //世界法线
varying vec3 vary_view_nml; //观察法线

varying float vary_dep_line_z; //线性视深

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
	
	//齐次坐标透视除法 由非可编程渲染管线部分完成这个此处不处理
	//gl_FragDepth = vary_proj_pos.z / vary_proj_pos.w;
}
