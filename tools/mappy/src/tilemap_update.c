#include <stdio.h>

#include "nuklear_inst.h"
#include "endian.h"
#include "tilemap.h"

#define TILEMAP_PANNING_SPEED 512

void tilemap_place_rect(const int mx_tile, const int my_tile)
{
	static int which_point, point_ax, point_ay, point_bx, point_by;

	switch (which_point)
	{
	case 0:
		point_ax = mx_tile;
		point_ay = my_tile;
		break;

	case 1:
		point_bx = mx_tile;
		point_by = my_tile;
		if (point_ax > point_bx)
		{
			int tmp = point_bx;

			point_bx = point_ax;
			point_ax = tmp;
		}
		if (point_ay > point_by)
		{
			int tmp = point_by;

			point_by = point_ay;
			point_ay = tmp;
		}
		for (int y = point_ay; y <= point_by; y++)
		{
			for (int x = point_ax; x <= point_bx; x++)
			{
				tilemap[y][x].type = tile_selected.type;
				tilemap[y][x].col = tile_selected.col;
			}
		}
		break;
	}

	which_point ^= 1;
}

static void tilemap_save(const char *outpath)
{
	FILE *file = fopen(outpath, "wb");

	fwrite_ef16(&tilemap_w, file);
	fwrite_ef16(&tilemap_h, file);
	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			const tile_t *t = tilemap[y] + x;

			fwrite(&t->type, 1, 1, file);
			fwrite_ef16(&t->col, file);
		}
	}

	fclose(file);
	printf("SAVED FILE '%s'\n", outpath);
}

void tilemap_resize(const int w_new, const int h_new)
{
	if (w_new <= 0 || h_new <= 0 ||
	    w_new > TILES_W_MAX || h_new > TILES_H_MAX)
		return;

	tilemap_w = w_new;
	tilemap_h = h_new;
}

void tilemap_update(const glwin_input_t *inp, const int mx_tile,
		    const int my_tile, const char *outpath, const float dt)
{
	/* panning */
	for (int i = 0; i < inp->shift_now + 1; i++)
	{
		tilemap_pan_x += (inp->d - inp->a) * TILEMAP_PANNING_SPEED * dt;
		tilemap_pan_y += (inp->s - inp->w) * TILEMAP_PANNING_SPEED * dt;
	}

	/* zooming */
	tilemap_is_zoomed ^= (inp->z_now && !inp->z_last);

	/* placing tile */
	int mouse_inside_map = tilemap_is_mouse_inside(inp, mx_tile, my_tile);

	if (inp->lmb_now && mouse_inside_map)
		tilemap[my_tile][mx_tile] = tile_selected;

	/* picking tile */
	if (inp->mmb_now && !inp->mmb_last && mouse_inside_map)
	{
		printf("Selected\n");
		const tile_t *t = tilemap[my_tile] + mx_tile;

		tile_selected.type = t->type;
		tile_selected_colf.r =
			(float)((t->col & 0xF800) >> 11) / 31.0f;
		tile_selected_colf.g =
			(float)((t->col & 0x07C0) >> 6) / 31.0f;
		tile_selected_colf.b =
			(float)((t->col & 0x003E) >> 1) / 31.0f;
	}

	/* recting tilemap */
	if (inp->rmb_now && !inp->rmb_last && mouse_inside_map)
		tilemap_place_rect(mx_tile, my_tile);

	/* saving */
	if (inp->enter_now && !inp->enter_last)
		tilemap_save(outpath);
}
