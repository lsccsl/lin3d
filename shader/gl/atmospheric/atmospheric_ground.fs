/*** atmospheric_ground.fs ***/

uniform float uni_sea_level;
uniform float uni_rayleigh_scale_height;
uniform float uni_scale;
uniform	vec3 uni_vlight_dir;
uniform float uni_g;
uniform float uni_esun;
uniform float uni_kr;
uniform float uni_kr4PI;
uniform float uni_km;
uniform float uni_km4PI;
uniform	vec3 uni_inv_array_fWavelength4;

uniform mat4 uni_proj_mtx; //投影
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_world_mtx; //世界变换

int sample_count = 4;

uniform vec3 uni_cam_pos;

uniform sampler2D tex1;
varying vec4 vary_world_pos;

float atmospheric_cal_scale(float fCos)
{
	float x = 1.0 - fCos;

	return 0.25 * exp(-0.00287 + x*(0.459 + x*(3.83 + x*(-6.80 + x*5.25))));
}

void main()
{
	vec4 clr = texture2D(tex1,gl_TexCoord[0].st);
	gl_FragColor.xyz = gl_Color.xyz;

#if 1
	vec3 v3Start = uni_cam_pos;

	vec3 v3Ray = vary_world_pos.xyz - uni_cam_pos;
	float fFar = length(v3Ray);
	v3Ray = v3Ray / fFar;


	/* 认为垂直 */
	float fStartAngle = abs(dot(-v3Ray, vec3(0.0f, 1.0, 0.0)));

	float fStartDepth = exp( uni_scale * (1.0 / uni_rayleigh_scale_height) * (-(v3Start.y - uni_sea_level)) );

	float fStartOffset = fStartDepth * atmospheric_cal_scale(fStartAngle);

	float fSampleLength = fFar / (sample_count);
	float fScaledLength = fSampleLength * uni_scale;
	vec3 v3SampleRay = v3Ray * fSampleLength;
	vec3 v3SamplePoint = v3Start + v3SampleRay * 0.5;

	// Now loop through the sample rays
	vec3 v3FrontColor = vec3(0.0, 0.0, 0.0);
	vec3 v3Attenuate = vec3(0.0, 0.0, 0.0);

	for(int i = 0; i < sample_count; i++)
	{
		float fHeight = v3SamplePoint.y - uni_sea_level;
		float fDepth = exp(uni_scale * (1.0 / uni_rayleigh_scale_height) * (-fHeight));

		float fScatter = 0.0;

		{
			float fLightAngle_tmp = dot(uni_vlight_dir, vec3(0.0f, 1.0f, 0.0f));
			float fCameraAngle_tmp = 0.0;


			bool bCamAbove = (uni_cam_pos.y >= v3SamplePoint.y);

			if(bCamAbove)
			{
				fCameraAngle_tmp = dot(-v3Ray, vec3(0.0f, 1.0f, 0.0f));
				fScatter = (-fStartOffset + fDepth*(atmospheric_cal_scale(fLightAngle_tmp) + atmospheric_cal_scale(fCameraAngle_tmp)));
			}
			else
			{
				fCameraAngle_tmp = dot(v3Ray, vec3(0.0f, 1.0f, 0.0f));
				fScatter = (fStartOffset + fDepth*(atmospheric_cal_scale(fLightAngle_tmp) - atmospheric_cal_scale(fCameraAngle_tmp)));
			}
		}

		v3Attenuate = exp(-fScatter * (uni_inv_array_fWavelength4 * uni_kr4PI + uni_km4PI));
		//v3Attenuate.x( exp(-fScatter * (this->inv_array_fWavelength4_.x() * this->Kr4PI_ + this->Km4PI_)) );
		//v3Attenuate.y( exp(-fScatter * (this->inv_array_fWavelength4_.y() * this->Kr4PI_ + this->Km4PI_)) );
		//v3Attenuate.z( exp(-fScatter * (this->inv_array_fWavelength4_.z() * this->Kr4PI_ + this->Km4PI_)) );

		v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
		v3SamplePoint += v3SampleRay;
	}

	vec3 clr_final;
	{
		float fKrESun = uni_esun * uni_kr;
		float fKmESun = uni_esun * uni_km;

		float fCos = dot(-v3Ray, uni_vlight_dir);
		float g2 = uni_g * uni_g;
		float fRayPhase = 0.75 * (1.0 + fCos * fCos);
		float fMiePhase = 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos*fCos) / pow(1.0 + g2 - 2.0 * uni_g * fCos, 1.5);

		vec3 clr2 = v3FrontColor * fKmESun * fMiePhase;
		vec3 clr1;
		{
			clr1 = v3FrontColor * fKrESun * fRayPhase;
			
			clr1 = uni_inv_array_fWavelength4 * clr1;
		}

		clr_final = (clr1 + clr2);

	}

	gl_FragColor.xyz = clr_final * 0.1;
#endif
}