/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_3d.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhirabay <mhirabay@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/07 15:16:00 by mhirabay          #+#    #+#             */
/*   Updated: 2022/04/20 17:28:41 by mhirabay         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int	get_tex_type(t_ray *ray)
{
	if (ray->found_vert_wallhit)
	{
		if (ray->is_ray_facing_left)
			return (EA);
		else
			return (WE);
	}
	else
	{
		if (ray->is_ray_facing_up)
			return (SO);
		else
			return (NO);
	}
}

int	get_tex_color(t_game *g, int tex_type, int x, int y)
{
	if (tex_type == NO)
	{
		return (g->texture.no_img.data[to_coord_tex(x, y)]);
	}
	else if (tex_type == SO)
	{
		return (g->texture.so_img.data[to_coord_tex(x, y)]);
	}
	else if (tex_type == EA)
	{
		return (g->texture.ea_img.data[to_coord_tex(x, y)]);
	}
	else if (tex_type == WE)
	{
		return (g->texture.we_img.data[to_coord_tex(x, y)]);
	}
	return (YELLOW);
}

void	render_3dwall(t_game *g, int i, t_wall3d_info w_info)
{
	int	tex_type;
	int	tex_offset_x;
	int	texture_offset_y;
	int	y;
	int	color;

	tex_type = get_tex_type(g->player->ray[i]);
	if (tex_type == NO || tex_type == SO)
		tex_offset_x = calc_offset_x(g->player->ray[i]->horz_wall_hit_x);
	else if (tex_type == EA || tex_type == WE)
		tex_offset_x = calc_offset_x(g->player->ray[i]->vert_wall_hit_y);
	y = w_info.wall_top_pixel;
	while (y < w_info.wall_bottom_pixel)
	{
		texture_offset_y = (y - w_info.wall_top_pixel) \
		* (50 / (double)w_info.projected_wall_height);
		color = get_tex_color(g, tex_type, tex_offset_x, texture_offset_y);
		g->img.data[to_coord(i, y)] = color;
		y++;
	}
}

void	calc_wall_info(t_ray *ray, double p_angle, t_wall3d_info *wall_info)
{
	double	projected_wall_height;
	int		wall_top_pixel;
	int		wall_bottom_pixel;
	double	perp_dist;
	double	dist_project_plane;

	dist_project_plane = ((WIDTH) / 2) / tan(FOV_ANGLE / 2);
	perp_dist = ray->dist * cos(ray->angle - p_angle);
	projected_wall_height = ((TILE_SIZE) / perp_dist) * dist_project_plane;
	wall_top_pixel = (HEIGHT / 2) - (projected_wall_height / 2);
	if (wall_top_pixel < 0)
		wall_top_pixel = 0;
	wall_bottom_pixel = (HEIGHT / 2) + (projected_wall_height / 2);
	if (wall_bottom_pixel > HEIGHT)
		wall_bottom_pixel = HEIGHT;
	wall_info->wall_top_pixel = wall_top_pixel;
	wall_info->wall_bottom_pixel = wall_bottom_pixel;
	wall_info->projected_wall_height = projected_wall_height;
}

void	generate_3d(t_game *g)
{
	int				i;
	t_wall3d_info	w_info;

	i = 0;
	while (i < NUM_RAYS)
	{
		calc_wall_info(g->player->ray[i], g->player->rotate_angle, &w_info);
		render_celling(g, i, w_info.wall_top_pixel);
		render_3dwall(g, i, w_info);
		render_floor(g, i, w_info.wall_bottom_pixel);
		i++;
	}
}
