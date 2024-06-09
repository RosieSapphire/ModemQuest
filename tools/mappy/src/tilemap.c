#include <stdarg.h>
#include <GL/glew.h>

#include "config.h"
#include "render.h"
#include "endian.h"
#include "nuklear_inst.h"
#include "tilemap.h"

#define TILEMAP_W_DEFAULT 10
#define TILEMAP_H_DEFAULT 10

#undef TILE_SIZE
#define TILE_SIZE (tilemap_is_zoomed ? 64 : 32)

tile_t tilemap[TILES_H_MAX][TILES_W_MAX];
tile_t tile_selected = {TILE_TYPE_FLOOR, 0xFFFF};
u16 tilemap_w;
u16 tilemap_h;
int tilemap_pan_x, tilemap_pan_y, tilemap_is_zoomed;

void tilemap_load(const char *path)
{
	tilemap_pan_x = 0;
	tilemap_pan_y = -128;
	tilemap_is_zoomed = 0;

	FILE *file = fopen(path, "rb");

	/* If no file is found, we just initalize with the default */
	if (!file)
	{
		tilemap_w = TILEMAP_W_DEFAULT;
		tilemap_h = TILEMAP_H_DEFAULT;
		for (int y = 0; y < TILES_H_MAX; y++)
		{
			for (int x = 0; x < TILES_W_MAX; x++)
			{
				tilemap[y][x].type = TILE_TYPE_FLOOR;
				tilemap[y][x].col = 0xFFFF;
			}
		}
	}

	fread_ef16(&tilemap_w, file);
	fread_ef16(&tilemap_h, file);
	for (int y = 0; y < tilemap_h; y++)
	{
		for (int x = 0; x < tilemap_w; x++)
		{
			tile_t *t = tilemap[y] + x;

			fread(&t->type, 1, 1, file);
			fread_ef16(&t->col, file);
		}
	}
	fclose(file);
}

static void tilemap_tile_render(const int x, const int y, int is_selected)
{
	if (x < 0 || x >= tilemap_w || y < 0 || y >= tilemap_h)
		return;

	const int pad = 3;
	const int x0 = (x * TILE_SIZE) - tilemap_pan_x;
	const int y0 = (y * TILE_SIZE) - tilemap_pan_y;
	const int x1 = x0 + TILE_SIZE;
	const int y1 = y0 + TILE_SIZE;
	float r, g, b, a;

	if (is_selected)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		r = tile_selected_colf.r;
		g = tile_selected_colf.g;
		b = tile_selected_colf.b;
		a = 0.5f;
		glDisable(GL_BLEND);
	}
	else
	{
		const tile_t *tile = tilemap[y] + x;

		r = ((tile->col & 0xF800) >> 11) / 31.0f;
		g = ((tile->col & 0x07C0) >> 6) / 31.0f;
		b = ((tile->col & 0x003E) >> 1) / 31.0f;
		a = ((tile->col & 0x0001) >> 0);
	}

	render_rect(x0, y0, x1, y1, r / 2, g / 2, b / 2, a);
	render_rect(x0 + pad, y0 + pad, x1 - pad, y1 - pad, r, g, b, a);
}

void tilemap_render(const int mx_tile, const int my_tile)
{
	glDisable(GL_BLEND);
	for (int y = 0; y < tilemap_h; y++)
		for (int x = 0; x < tilemap_w; x++)
			tilemap_tile_render(x, y, x == mx_tile && y == my_tile);
}

void tilemap_get_mouse_tile_pos(const glwin_input_t *inp,
				int *mx_tile, int *my_tile)
{
	*mx_tile = (inp->mx_now + tilemap_pan_x) / TILE_SIZE;
	*my_tile = (inp->my_now + tilemap_pan_y) / TILE_SIZE;
}

int tilemap_is_mouse_inside(const glwin_input_t *inp, const int mx_tile,
			    const int my_tile)
{
	return (mx_tile >= 0 && mx_tile < tilemap_w &&
		my_tile >= 0 && my_tile < tilemap_h &&
		inp->mx_now < WIN_WID - 200 && inp->my_now > 128);
}
