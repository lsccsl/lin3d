/*** common_fun.fs ***/

/**
* @brief ��������,�õ�����ӿռ�����
* @brief dep:����
* @param xy_proj:ȡֵΪ0,1(ͶӰ���x y)
* @brief tan_half_fov:��Ӱ���ӽ�һ���tanֵ
* @brief aspect:�ݺ��
*/
void get_pos_by_dep(float dep, vec2 xy_proj, float tan_half_fov, float aspect, out vec3 pos)
{
	/* �������ֵ,��ԭ�����ӿռ�λ����Ϣ */
	pos = vec3(tan_half_fov * xy_proj.x * aspect * dep, tan_half_fov * xy_proj.y * dep, -dep);
}

void tex_dep_2_view_dep(float tex_dep, vec2 dep_range, out float view_dep)
{
	view_dep = (dep_range.y * tex_dep + dep_range.x);
}
