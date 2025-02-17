#version 330 core

in vec2 o_uv;
in vec4 o_col;
flat in ivec2 o_tile_pos;

uniform sampler2D u_texture;
uniform ivec2 u_dimensions;

out vec4 frag_col;

void main(void)
{
	if (o_tile_pos.x >= u_dimensions.x || o_tile_pos.y >= u_dimensions.y) {
		discard;
	}

	vec2 uv = o_uv;
	frag_col = texture(u_texture, uv) * o_col;
}
