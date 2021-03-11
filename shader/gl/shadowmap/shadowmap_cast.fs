/*** shadowmap_cast.fs ***/

varying vec4 vary_view_pos;  //观察位置
varying vec4 vary_world_pos; //世界位置

void main()
{
	gl_FragColor = vec4(vary_view_pos.z, 0, vary_view_pos.z, 1);
	gl_FragDepth = vary_view_pos.z;
}