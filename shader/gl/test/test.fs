#if 0

	void main()
	{
		gl_FragColor = vec4(1, 1, 1, 1);
	}

#else

uniform sampler2D uni_tex1; //纹理1

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
	vec4 tex1 = texture2D(uni_tex1, gl_TexCoord[0].st);
	
	vec4 clr = tex1 * 0.5 + gl_Color * 0.5;

	gl_FragColor = vec4(clr.x, 0, clr.y, 1);
	
//gl_FragColor = abs(vec4(tex1.a, tex1.a, tex1.a, 1));	
	//gl_FragDepth = vary_proj_pos.z / vary_proj_pos.w;
}

#endif



