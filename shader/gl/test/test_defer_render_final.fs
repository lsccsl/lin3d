/*** defer_render_final.fs ***/

uniform sampler2D uni_tex_light_shadow; //������Ӱ
uniform sampler2D uni_tex_light_vol; //�����
uniform sampler2D uni_tex_clr; //��ɫ
uniform sampler2D uni_tex_view_depth; //��Ӱ���ռ������������
uniform sampler2D uni_tex_view_proj_depth; //��Ӱ���ռ��������
uniform sampler2D uni_tex_ssao;
uniform sampler2D uni_tex_atmospheric;

uniform int uni_has_ssao; //�Ƿ���ssao
uniform int uni_has_light_vol; //�Ƿ��������
uniform int uni_has_atmospheric; //�Ƿ�������ɢ��

uniform float uni_diff_scale;
uniform float uni_atmospheric_scale;

varying vec4 vary_proj_pos;

void main()
{
	gl_FragDepth = texture2D(uni_tex_view_proj_depth, gl_TexCoord[0].st).x;
	
	vec4 light_shadow = texture2D(uni_tex_light_shadow, gl_TexCoord[0].st);	

	//float dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;
	//float diff_scale = (dep >= 1.0 ? 0 : uni_diff_scale);

	vec4 diff = texture2D(uni_tex_clr, gl_TexCoord[0].st);	
	float diff_scale = ceil(1.0 - gl_FragDepth) * uni_diff_scale;
	vec4 diff_final = diff * diff_scale;
	
	vec4 light_final = light_shadow;
	if(1 == uni_has_ssao) // ��������ʱ���ԶԷ�֧����һ�����Ż� uniform���ݷ�֧��If�ж�����������������uniform������---- ϣ����仰�����
	{
		vec4 ssao = texture2D(uni_tex_ssao, gl_TexCoord[0].st);
		light_final = ssao * light_shadow;
	}
	
	gl_FragColor = light_final + diff_final;
	
	//�����������
	if(1 == uni_has_light_vol)
	{
		vec4 light_vol = texture2D(uni_tex_light_vol, gl_TexCoord[0].st);
		gl_FragColor += light_vol;
	}
	
	//��Ӵ���ɢ�����
	if(1 == uni_has_atmospheric)
	{
		vec4 atmospheric = texture2D(uni_tex_atmospheric, gl_TexCoord[0].st);
		gl_FragColor += uni_atmospheric_scale * atmospheric;
	}
}
