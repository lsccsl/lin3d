/*** vol_lightdir_vertex_extrude.vs ***/

uniform mat4 uni_world_mtx; //����任
uniform mat4 uni_view_mtx; //�۲�
uniform mat4 uni_proj_mtx; //ͶӰ����

uniform vec4 uni_light_dir;
uniform float uni_vol_max;
uniform float uni_dis_from_light;

varying float vary_dis_attenuation;
varying vec4 vary_proj_pos;
varying vec4 vary_world_pos;
varying float vary_ver_dis_from_light;
varying vec3 test_vary_world_nml; //���編��

varying float test_vary_pos_light_dir_scal;


void main()
{
    vec4 real_world_pos = uni_world_mtx * gl_Vertex;
    vec3 light_dir = uni_light_dir.w * real_world_pos.xyz - uni_light_dir.xyz;
    //vary_ver_dis_from_light = length(light_dir);
    light_dir = normalize(light_dir);


    //�ж϶��㷨��,
    //�����ⷽ����ͬ,���Źⷽ���ƶ�ָ������
    float nml_proj_on_light_dir = dot(gl_Normal.xyz, light_dir);

#if 1
    float pos_light_dir_scal = clamp(-nml_proj_on_light_dir, 0, 1); //���nml_proj_on_light_dir>0,��pos_light_dir_scalΪ0
    pos_light_dir_scal = ceil(pos_light_dir_scal);//Ҫô0,Ҫô1
#else
	float pos_light_dir_scal = 0;
	if(nml_proj_on_light_dir < 0)
		pos_light_dir_scal = 1;
#endif

test_vary_pos_light_dir_scal = 1-pos_light_dir_scal;

    vary_dis_attenuation = 1 - pos_light_dir_scal;

    vec4 new_vertex_world_pos = real_world_pos + vec4(light_dir, 0) * pos_light_dir_scal * uni_vol_max;
    new_vertex_world_pos.w = real_world_pos.w;

    vary_ver_dis_from_light = length(new_vertex_world_pos.xyz - uni_light_dir.xyz);


//new_vertex_world_pos = gl_Vertex;
//new_vertex_world_pos.y = new_vertex_world_pos.y - 35;
//new_vertex_world_pos.x = new_vertex_world_pos.x + 80;


	vec4 view_pos  = uni_view_mtx * new_vertex_world_pos;
	gl_Position = uni_proj_mtx * view_pos;
	
	test_vary_world_nml = (uni_world_mtx * vec4(gl_Normal, 0.0)).xyz;	

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color; 
}
