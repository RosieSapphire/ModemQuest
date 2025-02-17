#version 330 core

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_uv;

uniform mat4 u_proj;
uniform ivec2 u_offsets[128];
uniform int u_colors[128];
uniform int u_tile_size;
uniform int u_tile_type_count;
uniform int u_texture_indis[128];

out vec2 o_uv;
out vec4 o_col;
flat out ivec2 o_tile_pos;

const int texture_count = 5 + 1;
const int texture_index = 1;

void main(void)
{
	o_tile_pos = u_offsets[gl_InstanceID];
        gl_Position = u_proj * vec4(a_pos + (o_tile_pos * u_tile_size),
				    0.0, 1.0);
	o_uv = vec2((a_uv.x + float(u_texture_indis[gl_InstanceID])) / 
		    (u_tile_type_count + 1), a_uv.y);

	int col16 = u_colors[gl_InstanceID];
	o_col = vec4(float((col16 & 0x003E) >> 1) / 0x1F,
		     float((col16 & 0x07C0) >> 6) / 0x1F,
		     float((col16 & 0xF800) >> 11) / 0x1F,
		     u_colors[gl_InstanceID] & 0x1);
}
