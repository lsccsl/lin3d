uniform sampler2D uni_bump; //凹凸纹理
uniform sampler2D uni_tex1; //反射纹理
uniform sampler2D uni_tex2; //拆射纹理
uniform sampler2D uni_tex_fresnel; //菲涅尔效应纹理
// 低头看向水里，近的地方非常清澈见底（反射较少），而看远的地方却倒映着天空（反射较多）。这就是菲尼尔效应 
// 参考公式:fresnel基础值 + fresnel缩放量*pow( 1 - dot( N, V ), 5 )  N为水面法线,V为摄影机与像素点世界坐标向量
// 此处简化成纹理查表

uniform mat4 uni_proj_mtx; //投影
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_world_mtx; //世界变换
uniform vec4 uni_wave_offset;
uniform vec4 uni_wave_scale;
uniform float uni_refl_distort;

uniform int uni_need_refract;

uniform float uni_reflect_scale;
uniform float uni_refract_scale;
uniform float uni_color_scale;

varying vec4 vary_view_pos;  //观察位置
varying vec4 vary_world_pos; //世界位置
varying vec3 vary_world_nml; //世界法线
varying vec3 vary_view_nml; //观察法线
varying vec4 vary_proj_pos;
varying vec3 vary_world_view_dir;//用于计算菲涅尔效应

varying vec4 vary_nonstereo_pos;
varying vec2 vary_bump1;
varying vec2 vary_bump2;

void main()
{

	vec3 tex_bump1 = texture2D(uni_bump, vary_bump1).xyz;
	tex_bump1 = (tex_bump1 - 0.5) * 2.0;/* 转换至 (-1,1)区间 */
	tex_bump1 = normalize(tex_bump1);

	vec3 tex_bump2 = texture2D(uni_bump, vary_bump2).xyz;
	tex_bump2 = (tex_bump2 - 0.5) * 2.0;/* 转换至 (-1,1)区间 */
	tex_bump2 = normalize(tex_bump2);
	
	vec3 tex_bump = (tex_bump1 + tex_bump2) * 0.5;
	
	vec4 uv1 = vary_nonstereo_pos;
	uv1.xy = uv1.xy - tex_bump.xy * uni_refl_distort;
	uv1.xy = uv1.xy / uv1.w;
	
	vec4 tex_refl = texture2D(uni_tex1, uv1.xy);

	vec4 clr = tex_refl * uni_reflect_scale + gl_Color * uni_color_scale;
	
	if(1 == uni_need_refract)
	{
		vec4 tex_refr = texture2D(uni_tex2, uv1.xy);

		tex_refr = tex_refr * uni_refract_scale * vec4(.34, .85, .92, 1);

#if 0
		clr += tex_refr;
#else
		vec3 world_view_dir = normalize(vary_world_view_dir);
		float fresnel_fact = dot(world_view_dir.xzy, tex_bump);
		float fresnel = texture2D(uni_tex_fresnel, vec2(fresnel_fact)).w;
		
		clr = mix(tex_refr, tex_refl * uni_reflect_scale, fresnel) + gl_Color * uni_color_scale;
#endif		
	}	

	gl_FragColor = vec4(clr.x, clr.y, clr.z, 1);

#if 0
/// test
	vec4 uv = vary_proj_pos;
	uv.xy = (uv.xy/uv.w) * 0.5 + 0.5; 
	
tex_refl = texture2D(uni_tex1, uv.xy);
vec4 tex_refr = texture2D(uni_tex2, uv.xy);
	
vec3 world_view_dir1 = normalize(vary_world_view_dir);
float fresnel_fact1 = dot(world_view_dir1.xzy, vec3(0,0,1));
float fresnel1 = texture2D(uni_tex_fresnel, vec2(fresnel_fact1)).w;
clr = mix(tex_refr, tex_refl, fresnel1);
//clr.xyz = vec3(fresnel1);
gl_FragColor = vec4(clr.x, clr.y, clr.z, 1);
	
//	gl_FragColor = texture2D(uni_tex1, uv.xy) * 0.8 + gl_Color * 0.2;

//gl_FragColor = texture2D(uni_tex2, uv.xy);
//gl_FragColor.g = 0;

//gl_FragColor = tex2;
//gl_FragColor.r = 0;
//gl_FragColor=tex1;

#endif
}
