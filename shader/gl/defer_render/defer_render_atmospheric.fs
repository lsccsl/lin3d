/*** defer_render_atmospheric.fs ***/

uniform float uni_sea_level;
uniform float uni_rayleigh_scale_height;
uniform float uni_scale;
uniform	vec3 uni_vlight_dir;
uniform float uni_g;
uniform float uni_g_sky;
uniform float uni_esun;
uniform float uni_kr;
uniform float uni_kr4PI;
uniform float uni_km;
uniform float uni_km4PI;
uniform	vec3 uni_inv_array_fWavelength4;
uniform float uni_attenuation_final; //最终计算结果衰减

uniform vec3 uni_cam_pos;
uniform float uni_skydome_radius; //skydome的半径


uniform sampler2D uni_tex_view_depth; //摄影机空间深度纹理
uniform sampler2D uni_tex_clr; //颜色

uniform float uni_tan_half_fov; //视角一半的tan值
uniform vec2 uni_dep_range; //最大深度
uniform float uni_aspect; //屏幕宽高比	
uniform mat4 uni_view_inv_mtx; //观察逆矩阵


varying vec2 vary_proj_pos;


int sample_count = 10;

float atmospheric_cal_scale(float fCos)
{
	float x = 1.0 - fCos;

	return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main()
{	
	float tex_dep = texture2D(uni_tex_view_depth, gl_TexCoord[0].st).x;

	if(tex_dep > 1.0)
		return;

	float view_dep;
	tex_dep_2_view_dep(tex_dep, uni_dep_range, view_dep);

	vec3 view_pos;
	get_pos_by_dep(view_dep, vary_proj_pos.xy, uni_tan_half_fov, uni_aspect, view_pos);

	vec4 world_pos = uni_view_inv_mtx * vec4(view_pos, 1);

	/* 开始计算大气散射 */
	vec3 v3Start = uni_cam_pos;
	//vec3 v3Start_dir = normalize(uni_cam_pos - vec3(0, -200, 0));

	vec3 v3Ray = world_pos.xyz - uni_cam_pos;
	float fFar = length(v3Ray);
	v3Ray = v3Ray / fFar;

	/* 认为垂直 */
	float fStartAngle = abs(dot(-v3Ray, vec3(0.0, 1.0, 0.0)));

	float scale = uni_scale;

#if 0
float len_tmp = length(world_pos.xyz - uni_cam_pos);
if(len_tmp >= 6000.0)
{
	float dec = min( max(0.0, (len_tmp - 6000.0)/3000.0), 1.0 );
	scale = 0.0025 + (0.004 - 0.0013) * dec;
	//gl_FragColor = vec4((scale - 0.004)/0.004);
	//return;
	//scale = 0.004;
}
else{
	//if(len_tmp <= 4000)
		scale = 0.0025;
}
//scale = 0;
#endif

	float fStartDepth = exp( scale * (1.0 / uni_rayleigh_scale_height) * (-(v3Start.y - uni_sea_level)) );

	float fStartOffset = fStartDepth * atmospheric_cal_scale(fStartAngle);

	float fSampleLength = fFar / (sample_count);
	float fScaledLength = fSampleLength * scale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate = vec3(0.0, 0.0, 0.0);

	float fCos = dot(-v3Ray, uni_vlight_dir);
	for(int i = 0; i < sample_count; i++)
	{
		float fHeight = v3SamplePoint.y - uni_sea_level;
		float fDepth = exp(scale * (1.0 / uni_rayleigh_scale_height) * (-fHeight));

		float fScatter = 0.0;

		{
			float fLightAngle_tmp = dot(uni_vlight_dir, vec3(0.0, 1.0, 0.0));
			float fCameraAngle_tmp = 0.0;

			bool bCamAbove = (uni_cam_pos.y >= v3SamplePoint.y);

			if(bCamAbove)
			{
				fCameraAngle_tmp = dot(-v3Ray, vec3(0.0, 1.0, 0.0));
				fScatter = (-fStartOffset + fDepth*(atmospheric_cal_scale(fLightAngle_tmp) + atmospheric_cal_scale(fCameraAngle_tmp)));
			}
			else
			{
				fCameraAngle_tmp = dot(v3Ray, vec3(0.0, 1.0, 0.0));
				fScatter = (fStartOffset + fDepth*(atmospheric_cal_scale(fLightAngle_tmp) - atmospheric_cal_scale(fCameraAngle_tmp)));
			}
		}

		v3Attenuate = exp(-fScatter * (uni_inv_array_fWavelength4 * uni_kr4PI + uni_km4PI));

		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	float g = uni_g + (fFar / uni_skydome_radius) * (uni_g_sky - uni_g);
	
	vec3 clr_final;
	{
		float fKrESun = uni_esun * uni_kr;
		float fKmESun = uni_esun * uni_km;

		float g2 = g * g;
		float fRayPhase = 0.75 * (1.0 + fCos * fCos);
		float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);

		vec3 clr_m = v3FrontColor * fKmESun * fMiePhase;
		vec3 clr_r;
		{
			clr_r = v3FrontColor * fKrESun * fRayPhase;
			
			clr_r = uni_inv_array_fWavelength4 * clr_r;
		}

		clr_final = (clr_r + clr_m);
	}

	clr_final = clr_final * uni_attenuation_final;
	
	gl_FragColor = vec4(clr_final, 1);	

	return;
}