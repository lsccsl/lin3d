/*** common_def.shdr ***/

#define E_LIGHT_DIR 0
#define E_LIGHT_SPOT 1
#define E_LIGHT_POINT 2

uniform sampler2D uni_tex1; //纹理1

uniform mat4 uni_proj_mtx; //投影
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_world_mtx; //世界变换
uniform vec2 uni_dep_range; //最大深度


uniform int uni_light_type; //光的类型
uniform vec3 uni_light_view_spot_dir; //Spotlight方向
uniform vec4 uni_light_view_pos; //灯光位置(w=0平行光方向 w=1点光源位置)
uniform vec3 uni_light_clr;
uniform float uni_light_attenuation; //距离大于此值开始衰减
uniform float uni_light_max_len; //距离大于此值光线衰减为0

uniform float uni_light_cutoff_inner_cos; //聚光灯,光锥轴线与边缘夹角的cos值最小(小于此值不衰减)
uniform float uni_light_cutoff_outer_cos; //聚光灯,光锥轴线与边缘夹角的cos值最大(大于此值衰减为零)

uniform float uni_light_exponent; //聚光灯由中心向边缘衰减

uniform float uni_roughness;
uniform float uni_metalic;


varying vec4 vary_view_pos;  //观察位置
varying vec4 vary_world_pos; //世界位置
varying vec3 vary_world_nml; //世界法线
varying vec3 vary_view_nml; //观察法线
varying vec4 vary_proj_pos;


const float const_m = 32;
const float const_pi = 3.1415926;

//const vec3 au_albedo = vec3(1.0, 0.71, 0.29);
const float par1_scale = 4;
const float par2_scale = 4;



float gs_schlick_ggx(float roughness, float costheta)
{
	float k = ((roughness + 1) * (roughness + 1)) / 8;
	float ggx = costheta / (k + (1-k) * costheta);
	
	return ggx;
}

float v_schlick(float roughness, float cos_light, float cos_view)
{
	float gs_l = gs_schlick_ggx(roughness, cos_light);//入射光遮挡
	float gs_v = gs_schlick_ggx(roughness, cos_view); //反射光遮挡
	float v = (gs_l * gs_v)/max(4 * cos_light * cos_view, 0.000001);
	
	return v;
}



float d_trowbridge_reitz(float rougnness, float cos_half)
{
	float roughness_sqrt = rougnness * rougnness;
	float cos_half_sqrt = cos_half * cos_half;
	float d_denominator = const_pi * pow(cos_half_sqrt * (roughness_sqrt - 1) + 1, 2);
	float d_tr = roughness_sqrt / d_denominator;
	
	return d_tr;
}



vec3 fresnel_schlick(vec3 f0, float cos_view)
{
	// F 菲涅尔 F = F0 + (1 - F0) * pow((1 - cos_view), 5)
	vec3 fresnel = f0 + (1 - f0) * pow((1 - cos_view), 5);
	
	return fresnel;
}

vec3 fresnel_f0(float metalic, vec3 albedo)
{
	// F0 = Metalic * Albedo + (1 - Metalic) * vec3(0.04, 0.04, 0.04)  Albedo为金属颜色  Metalic表示金属度 0-1
	return metalic * albedo + (1 - metalic) * vec3(0.04, 0.04, 0.04);
}



void pbr_light_dir()
{
	vec3 view_nml = normalize(vary_view_nml);
	vec3 v_view = normalize(-vary_view_pos.xyz);
	vec3 v_light = normalize(-uni_light_view_pos.xyz);
	vec3 v_half = normalize(v_view + v_light);
	
	vec3 albedo = texture2D(uni_tex1, gl_TexCoord[0].st).rgb;

	float cos_light = max(dot(v_light, view_nml), 0.0);
	float cos_half = max(dot(v_half, view_nml), 0);
	float cos_half_m = pow(cos_half, const_m);
	float cos_view = max(dot(v_view, view_nml), 0);

	vec4 color = vec4(0, 0, 0, 1);

	//blinn-phong
	//vec3 clr_part1 = clr_diff * (1 / const_pi);
	//vec3 clr_part2 = cos_half_m * clr_spec * ((const_m + 8) / (8 + const_pi));

	// D 法线分布
#if 1
	float d_tr = d_trowbridge_reitz(uni_roughness, cos_half);
#else
	//float roughness_sqrt = uni_roughness * uni_roughness;
	//float cos_half_sqrt = cos_half * cos_half;
	//float d_denominator = const_pi * pow(cos_half_sqrt * (roughness_sqrt - 1) + 1, 2);
	//float d_tr = roughness_sqrt / d_denominator;
#endif

#if 1
	vec3 f0	= fresnel_f0(uni_metalic, albedo);
	vec3 fresnel = fresnel_schlick(f0, cos_view);
#else	
	// F 菲涅尔 F = F0 + (1 - F0) * pow((1 - cos_view), 5)
	// F0 = Metalic * Albedo + (1 - Metalic) * vec3(0.04, 0.04, 0.04)  Albedo为金属颜色  Metalic表示金属度 0-1
	// another easy fresnel  pow(1 - dot(world_nml, view_dir), 2)
	vec3 f0 = uni_metalic * albedo + (1 - uni_metalic) * vec3(0.04, 0.04, 0.04);
	vec3 fresnel = f0 + (1 - f0) * pow((1 - cos_view), 5);  //vec3 fresnel = f0 + (1 - f0) * pow((1 - cos_half), 5);
#endif

#if 1
	float v = v_schlick(uni_roughness, cos_light, cos_view);
#else
	// V 可见性
	float ggx_l = gs_schlick_ggx(uni_roughness, cos_light);//入射光遮挡
	float ggx_v = gs_schlick_ggx(uni_roughness, cos_view); //反射光遮挡
	float v = (ggx_l * ggx_v)/max(4 * cos_light * cos_view, 0.000001);
#endif
	
	// 能量守恒
	vec3 k_diff = (1 - f0) * (1 - uni_metalic);
	vec3 k_spec = f0;

	vec3 clr_part1 = k_diff * albedo * (1 / const_pi);
	
	vec3 clr_part2 = k_spec * fresnel * d_tr * v;
	
	vec4 clr_ambient = vec4(0.15 * albedo, 1);

	color.xyz = (clr_part1 * par1_scale + clr_part2 * par2_scale) * cos_light * uni_light_clr;

	gl_FragColor = color + clr_ambient;
}

void pbr_light_spot()
{
	vec3 view_pos = vary_view_pos.xyz;
	vec3 view_nml = normalize(vary_view_nml);

	vec3 albedo = texture2D(uni_tex1, gl_TexCoord[0].st).rgb;

	vec3 light_dir = uni_light_view_pos.xyz - view_pos;
	float light_dis = length(light_dir);
	light_dir = light_dir / light_dis;

	vec3 v_view = normalize(-vary_view_pos.xyz);
	vec3 v_half = normalize(v_view + light_dir);

	float cos_light = clamp(dot(light_dir, view_nml), 0.0, 1.0);
	float cos_half = max(dot(v_half, view_nml), 0);
	float cos_half_m = pow(cos_half, const_m);
	float cos_view = max(dot(v_view, view_nml), 0);

	vec4 color = vec4(0, 0, 0, 1);

	float d_tr = d_trowbridge_reitz(uni_roughness, cos_half);

	vec3 f0	= fresnel_f0(uni_metalic, albedo);
	vec3 fresnel = fresnel_schlick(f0, cos_view);

	float v = v_schlick(uni_roughness, cos_light, cos_view);
	
	// 能量守恒
	vec3 k_diff = (1 - f0) * (1 - uni_metalic);
	vec3 k_spec = f0;

	vec3 clr_part1 = k_diff * albedo * (1 / const_pi);
	
	vec3 clr_part2 = k_spec * fresnel * d_tr * v;
	
	vec4 clr_ambient = vec4(0.15 * albedo, 1);


	float cos_spot = max(dot(-light_dir, uni_light_view_spot_dir), 0);
	
	//内外环聚光灯因子 = clamp((外环的聚光灯角度cos值 – 当前顶点的聚光灯角度cos值)/(外环的聚光灯角度cos值 – 内环聚光灯角度cos值), 0, 1)  smoothstep(cosOuterCone, cosInnerCone, cosDirection)
	float attenuation_cutoff = smoothstep(uni_light_cutoff_outer_cos, uni_light_cutoff_inner_cos, cos_spot);

	//距离衰减因子 = 1.0/(距离衰减常量 + 线性衰减常量 × 距离 + 二次衰减常量 × 距离的平方)
	float attenuation_distance = 1.0 - smoothstep (uni_light_attenuation, uni_light_max_len, light_dis);
	
	color.xyz = (clr_part1 * par1_scale + clr_part2 * par2_scale) * cos_light * uni_light_clr;
	gl_FragColor = color * attenuation_distance * attenuation_cutoff + clr_ambient;

	//聚光灯夹角cos值 = power(max(0, dot(单位光源向量, 单位光线向量)), 聚光灯指数)
}

void pbr_light_point()
{
	vec3 view_pos = vary_view_pos.xyz;
	vec3 view_nml = normalize(vary_view_nml);

	vec3 albedo = texture2D(uni_tex1, gl_TexCoord[0].st).rgb;

	vec3 light_dir = uni_light_view_pos.xyz - view_pos;
	float light_dis = length(light_dir);
	light_dir = light_dir / light_dis;

	vec3 v_view = normalize(-vary_view_pos.xyz);
	vec3 v_half = normalize(v_view + light_dir);

	float cos_light = clamp(dot(light_dir, view_nml), 0.0, 1.0);
	float cos_half = max(dot(v_half, view_nml), 0);
	float cos_half_m = pow(cos_half, const_m);
	float cos_view = max(dot(v_view, view_nml), 0);

	vec4 color = vec4(0, 0, 0, 1);

	float d_tr = d_trowbridge_reitz(uni_roughness, cos_half);

	vec3 f0	= fresnel_f0(uni_metalic, albedo);
	vec3 fresnel = fresnel_schlick(f0, cos_view);

	float v = v_schlick(uni_roughness, cos_light, cos_view);
	
	// 能量守恒
	vec3 k_diff = (1 - f0) * (1 - uni_metalic);
	vec3 k_spec = f0;

	vec3 clr_part1 = k_diff * albedo * (1 / const_pi);
	
	vec3 clr_part2 = k_spec * fresnel * d_tr * v;
	
	vec4 clr_ambient = vec4(0.15 * albedo, 1);


	//距离衰减因子 = 1.0/(距离衰减常量 + 线性衰减常量 × 距离 + 二次衰减常量 × 距离的平方)
	float attenuation_distance = 1.0 - smoothstep (uni_light_attenuation, uni_light_max_len, light_dis);
	
	color.xyz = (clr_part1 * par1_scale + clr_part2 * par2_scale) * cos_light * uni_light_clr;
	gl_FragColor = color * attenuation_distance + clr_ambient;
}

void main()
{
	switch(uni_light_type)
	{
	case E_LIGHT_DIR:
		pbr_light_dir();
		break;

	case E_LIGHT_SPOT:
		pbr_light_spot();
		break;

	case E_LIGHT_POINT:
		pbr_light_point();
		break;
	}
}

// 贴图: 1:albedo 2:normal 3:metalic 4:rougnness 5:ao 6:reflect(菲涅尔处理) gmma校正