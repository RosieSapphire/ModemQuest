#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <cglm/cglm.h>

#include "config.h"
#include "types.h"
#include "render.h"
#include "nuklear_inst.h"

/* constants */
static const u16 tile_indis[6] = {0, 2, 1, 2, 3, 1};

static const char *vertshd_src =
"#version 330 core\n"
"\n"
"layout (location = 0) in vec2 a_pos;\n"
"\n"
"uniform mat4 u_proj;\n"
"\n"
"void main(void)\n"
"{\n"
"        gl_Position = u_proj * vec4(a_pos, 0.0, 1.0);\n"
"}\n";

static const char *fragshd_src =
"#version 330 core\n"
"\n"
"uniform vec4 u_col;\n"
"\n"
"out vec4 frag_col;\n"
"\n"
"void main(void)\n"
"{\n"
"        frag_col = vec4(u_col);\n"
"}\n";

/* variables */
static u32 rect_shader, rect_vao, rect_vbo, rect_ebo;
static mat4 mat_proj;

static void render_shader_compile(void)
{
	u32 vert = glCreateShader(GL_VERTEX_SHADER);
	u32 frag = glCreateShader(GL_FRAGMENT_SHADER);
	int vert_stat, frag_stat, prog_stat;
	char log[512];

	glShaderSource(vert, 1, &vertshd_src, NULL);
	glShaderSource(frag, 1, &fragshd_src, NULL);
	glCompileShader(vert);
	glCompileShader(frag);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_stat);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &frag_stat);
	if (!vert_stat)
	{
		glGetShaderInfoLog(vert, 512, NULL, log);
		fprintf(stderr, "Vertex Shader failed to compile: %s\n", log);
		exit(EXIT_FAILURE);
	}
	if (!frag_stat)
	{
		glGetShaderInfoLog(frag, 512, NULL, log);
		fprintf(stderr, "Fragment Shader failed to compile: %s\n", log);
		exit(EXIT_FAILURE);
	}

	rect_shader = glCreateProgram();
	glAttachShader(rect_shader, vert);
	glAttachShader(rect_shader, frag);
	glLinkProgram(rect_shader);
	glGetProgramiv(rect_shader, GL_LINK_STATUS, &prog_stat);
	if (!prog_stat)
	{
		glGetProgramInfoLog(rect_shader, 512, NULL, log);
		fprintf(stderr, "Shader Program failed to link: %s\n", log);
		exit(EXIT_FAILURE);
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}

static void render_rect_buffers_init(void)
{
	glGenVertexArrays(1, &rect_vao);
	glBindVertexArray(rect_vao);

	glGenBuffers(1, &rect_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			      sizeof(float) * 2, NULL);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &rect_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6, tile_indis, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void render_init(const char *outpath)
{
	glewExperimental = 1;
	glewInit();
	glViewport(0, 0, WIN_WID, WIN_HEI);
	glm_ortho(0, WIN_WID, WIN_HEI, 0, -1, 1, mat_proj);
	render_rect_buffers_init();
	render_shader_compile();

	/* Render Mode */
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	/* Nuklear GUI */
	struct nk_font_atlas *atlas;

	nuklear_inst_init();
}

void render_rect(const int x0, const int y0, const int x1, const int y1,
		 const float r, const float g, const float b,
		 const float a)
{
	const float tile_verts[4][2] = {{x0, y0}, {x1, y0}, {x0, y1}, {x1, y1}};
	const int u_proj_loc = glGetUniformLocation(rect_shader, "u_proj");
	const int u_col_loc = glGetUniformLocation(rect_shader, "u_col");
	const float rgba[4] = {r, g, b, a};

	if (u_proj_loc == -1)
	{
		fprintf(stderr, "Couldn't find 'u_proj' in shader\n");
		exit(EXIT_FAILURE);
	}

	if (u_col_loc == -1)
	{
		fprintf(stderr, "Couldn't find 'u_col' in shader\n");
		exit(EXIT_FAILURE);
	}

	glBindVertexArray(rect_vao);
	glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tile_verts),
		     tile_verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(rect_shader);
	glUniformMatrix4fv(u_proj_loc, 1, GL_FALSE, (float *)mat_proj);
	glUniform4fv(u_col_loc, 1, rgba);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, tile_indis);
	glBindVertexArray(0);
}
