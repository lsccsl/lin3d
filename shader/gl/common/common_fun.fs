/*** common_fun.fs ***/

/**
* @brief 根据视深,得到点的视空间坐标
* @brief dep:视深
* @param xy_proj:取值为0,1(投影后的x y)
* @brief tan_half_fov:摄影机视角一半的tan值
* @brief aspect:纵横比
*/
void get_pos_by_dep(float dep, vec2 xy_proj, float tan_half_fov, float aspect, out vec3 pos)
{
	/* 根据深度值,还原顶点视空间位置信息 */
	pos = vec3(tan_half_fov * xy_proj.x * aspect * dep, tan_half_fov * xy_proj.y * dep, -dep);
}

void tex_dep_2_view_dep(float tex_dep, vec2 dep_range, out float view_dep)
{
	view_dep = (dep_range.y * tex_dep + dep_range.x);
}
