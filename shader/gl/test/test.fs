#if 0

	void main()
	{
		gl_FragColor = vec4(1, 1, 1, 1);
	}

#else

uniform sampler2D uni_tex1; //����1

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
	vec4 tex1 = texture2D(uni_tex1, gl_TexCoord[0].st);
	
	vec4 clr = tex1 * 0.5 + gl_Color * 0.5;

	gl_FragColor = vec4(clr.x, 0, clr.y, 1);
	
//gl_FragColor = abs(vec4(tex1.a, tex1.a, tex1.a, 1));	
	//gl_FragDepth = vary_proj_pos.z / vary_proj_pos.w;
}

#endif



