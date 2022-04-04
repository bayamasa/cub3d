/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mhirabay <mhirabay@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 10:17:16 by mhirabay          #+#    #+#             */
/*   Updated: 2022/03/31 16:12:12 by mhirabay         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	game_init(t_game *game)
{	
	int map[ROWS][COLS] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};
	memcpy(game->map, map, sizeof(int) * ROWS * COLS);
}

void	draw_line(t_game *game, double x1, double y1, double x2, double y2)
{
	double	deltaX;
	double	deltaY;
	double	step;

	deltaX = x2 - x1;
	deltaY = y2 - y1;
	// https://ichi.pro/ddasen-byoga-arugorizumu-209590219776463
	// 傾きが |m| が 1より大きいかそれ以下かを示している
	step = (fabs(deltaX) > fabs(deltaY)) ? fabs(deltaX) : fabs(deltaY);
	// どちらかが1に限りなく近い数値になる。
	deltaX /= step;
	deltaY /= step;
	while (fabs(x2 - x1) > 0.01 || fabs(y2 - y1) > 0.01)
	{
		game->img.data[TO_COORD(x1, y1)] = 0xB3B3B3;
		x1 += deltaX;
		y1 += deltaY;
	}
}

void	find_player_coord(t_game *g)
{
	int	i;
	int	j;

	i = 0;
	while (i < ROWS)
	{	
		j = 0;
		while (j < COLS)
		{
			if (g->map[i][j] == 2)
			{
				g->player->x = j * TILE_SIZE - TILE_SIZE / 2;
				g->player->y = i * TILE_SIZE - TILE_SIZE / 2;
				g->player->x_draw_point = g->player->x - PLAYER_SIZE / 2;
				g->player->y_draw_point = g->player->y - PLAYER_SIZE / 2;
				g->player->x_draw_end = g->player->x_draw_point + PLAYER_SIZE;
				g->player->y_draw_end = g->player->y_draw_point + PLAYER_SIZE;
				g->player->fov_min = FOV_MIN_DEGREE;
				g->player->fov_max = FOV_MAX_DEGREE;
				return ;
			}
			j++;
		}
		i++;
	}
}

void	draw_lines(t_game *game)
{
	int	i;
	int	j;

	i = 0;
	while (i < COLS)
	{
		draw_line(game, i * TILE_SIZE, 0, i * TILE_SIZE, HEIGHT);
		i++;
	}
	draw_line(game, COLS * TILE_SIZE - 1, 0, COLS * TILE_SIZE - 1, HEIGHT);
	j = 0;
	while (j < ROWS)
	{
		draw_line(game, 0, j * TILE_SIZE, WIDTH, j * TILE_SIZE);
		j++;
	}
	draw_line(game, 0, ROWS * TILE_SIZE - 1, WIDTH, ROWS * TILE_SIZE - 1);
}

void	window_init(t_game *game)
{
	game->mlx = mlx_init();
	game->win = mlx_new_window(game->mlx, WIDTH, HEIGHT, "mlx_42");
}

void	img_init(t_game *game)
{
	game->img.img = mlx_new_image(game->mlx, WIDTH, HEIGHT);
	game->img.data = (int *)mlx_get_data_addr(game->img.img, &game->img.bpp, &game->img.size_l, &game->img.endian);
}

void	draw_rectangle(t_game *game, int x, int y)
{
	int	i;
	int	j;

	x *= TILE_SIZE;
	y *= TILE_SIZE;
	i = 0; 
	while (i < TILE_SIZE)
	{
		j = 0;
		while (j < TILE_SIZE)
		{
			// TILEサイズの左上から全部1pixelずつなぞっていく
			game->img.data[(y + i) * WIDTH + x + j] = 0xFFFFFF;
			// printf("(y + i) * WIDTH + x + j = %d\n",(y + i) * WIDTH + x + j );
			j++;
		}
		i++;
	}
}

void	draw_player(t_game *game)
{
	int	i;
	int	j;

	game->player = (t_player *)malloc(sizeof(t_player));
	game->player->is_collide = (bool *)malloc(sizeof(bool) * (FOV_MAX_DEGREE - FOV_MIN_DEGREE + 1));
	i = 0;
	while (i < FOV_MAX_DEGREE - FOV_MIN_DEGREE + 1)
	{
		game->player->is_collide[i] = false;
		i++;
	}
	find_player_coord(game);
	i = 0; 
	while (i < PLAYER_SIZE)
	{
		j = 0;
		while (j < PLAYER_SIZE)
		{
			// TILEサイズの左上から全部1pixelずつなぞっていく
			game->img.data[(game->player->y_draw_point + i) * WIDTH + game->player->x_draw_point + j] = 0xFFFF00;
			j++;
		}
		i++;
	}
}

void	draw_rectangles(t_game *game)
{
	int	i;
	int	j;

	i = 0;
	while (i < ROWS)
	{	
		j = 0;
		while (j < COLS)
		{
			if (game->map[i][j] == 1)
				draw_rectangle(game, j, i);
			j++;
		}
		i++;
	}
}

int	deal_key(int key_code, t_game *game)
{
	printf("key_code = %d\n", key_code);
	if (key_code == KEY_ESC)
		exit(0);
	if (key_code == KEY_W)
		move_forward(game);
	else if (key_code == KEY_A)
		move_left(game);
	else if (key_code == KEY_D)
		move_right(game);
	else if (key_code == KEY_S)
		move_back(game);
	if (key_code == KEY_LEFT_ARROW)
		look_left(game);
	else if (key_code == KEY_RIGHT_ARROW)
		look_right(game);
	return (0);
}

int	close(t_game *game)
{
	(void)game;
	exit(0);
}

int	main_loop(t_game *game)
{
	draw_rectangles(game);
	draw_lines(game);
	draw_vision(game);
	mlx_put_image_to_window(game->mlx, game->win, game->img.img, 0, 0);
	return (0);
}

int	deal_mouse(int mouse_code, t_game *game)
{
	(void)game;
	printf("mouse_code = %d\n", mouse_code);
	return (0);
}

int	main()
{
	t_game	game;

	game_init(&game);
	window_init(&game);
	img_init(&game);
	draw_player(&game);
	mlx_key_hook(game.win, &deal_key, &game);
	// mlx_mouse_hook(game.win, &deal_mouse, &game);
	mlx_hook(game.win, X_EVENT_KEY_EXIT, 0, &close, &game);
	mlx_loop_hook(game.mlx, &main_loop, &game);
	mlx_loop(game.mlx);
	return (0);
}
