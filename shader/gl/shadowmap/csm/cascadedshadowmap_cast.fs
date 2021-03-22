/*** cascadedshadowmap_cast.fs ***/

varying vec4 vary_view_pos;  //观察位置
varying vec4 vary_world_pos; //世界位置

void main()
{
	float clr = vary_view_pos.z;
	gl_FragColor = vec4(clr, clr, clr, 1);
	gl_FragDepth = vary_view_pos.z;
}
