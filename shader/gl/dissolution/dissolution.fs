uniform sampler2D uni_tex1; //纹理1
uniform sampler2D uni_rand_tex;
uniform float uni_dissolution_threshold; //熔解阀值
uniform float uni_dissolution_transition; //熔解边缘过渡

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
	gl_FragColor = texture2D(uni_tex1, gl_TexCoord[0].st);
	vec4 r = texture2D(uni_rand_tex, gl_TexCoord[0].st);
	
	float diff = r.r - uni_dissolution_threshold;
	float edge_smooth = smoothstep(0, uni_dissolution_transition, diff); 	
	
	gl_FragColor.a = edge_smooth;	
}

/*

采样随机纹理
纹理与阀值比较,决定透明度

*/


