
uniform mat4 uni_proj_mtx; //投影
uniform mat4 uni_view_mtx; //观察
uniform mat4 uni_world_mtx; //世界变换
uniform vec4 uni_wave_offset;
uniform vec4 uni_wave_scale;
uniform vec3 uni_cam_world_pos; // 摄像机世界坐标

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
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;		
	//vary_proj_pos = gl_Position;

	vary_world_pos = uni_world_mtx * gl_Vertex;
	vary_view_pos  = uni_view_mtx * vary_world_pos;
	vary_proj_pos = uni_proj_mtx * vary_view_pos;
	gl_Position = vary_proj_pos;
	
	vary_world_view_dir = uni_cam_world_pos - vary_world_pos.xyz;
	
	vary_nonstereo_pos = vary_proj_pos * 0.5;
	vary_nonstereo_pos.xy = vec2(vary_nonstereo_pos.x, vary_nonstereo_pos.y) + vary_nonstereo_pos.w;
	vary_nonstereo_pos.zw = vary_proj_pos.zw;
	
	vec4 vtmp = vary_world_pos.xzxz * uni_wave_scale + uni_wave_offset;
	vary_bump1 = vtmp.xy;
	vary_bump2 = vtmp.zw;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
}
