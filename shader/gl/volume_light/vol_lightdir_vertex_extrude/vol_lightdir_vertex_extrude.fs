uniform sampler2D uni_tex1; //纹理1

uniform mat4 uni_world_mtx; //世界变换
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_proj_mtx; //投影矩阵

uniform vec4 uni_light_dir;
uniform float uni_vol_max;
uniform float uni_dis_from_light;

varying float vary_dis_attenuation;
varying float vary_ver_dis_from_light;

varying float test_vary_pos_light_dir_scal;
varying vec3 test_vary_world_nml; //世界法线

void main()
{
    float att1 = (vary_ver_dis_from_light - (uni_dis_from_light - uni_dis_from_light * 0.1)) / uni_vol_max;
    att1 = clamp(att1, 0, 1);

    float att2 = (uni_vol_max - 50 + uni_dis_from_light - vary_ver_dis_from_light) / (uni_vol_max - 50);
    att2 = clamp(att2, 0, 1);
    att2 = att2 * att2;

    float att = att1 * att2;

	vec4 tex = texture2D(uni_tex1, gl_TexCoord[0].st);

	vec3 world_nml = normalize(test_vary_world_nml);
	world_nml = (world_nml + 1.0) * 0.5;


	//gl_FragColor = tex;//

	//gl_FragColor.w = att* 4;
	gl_FragColor = vec4(att* 4,att* 4,att* 4,att* 4);//tex;//

}