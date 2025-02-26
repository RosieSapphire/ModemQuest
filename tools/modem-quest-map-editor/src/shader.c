#include <malloc.h>
#include <GL/glew.h>

#include "util.h"
#include "shader.h"

u32 shader_init_from_file(const char *vpath, const char *fpath)
{
	u32 vert = glCreateShader(GL_VERTEX_SHADER);
	u32 frag = glCreateShader(GL_FRAGMENT_SHADER);
	u32 prog;
	int vert_stat, frag_stat, prog_stat;
	char *vert_src = file_read_data(vpath);
	char *frag_src = file_read_data(fpath);
	char log[512];

	glShaderSource(vert, 1, (const char *const *)&vert_src, NULL);
	glShaderSource(frag, 1, (const char *const *)&frag_src, NULL);
	free(vert_src);
	free(frag_src);
	glCompileShader(vert);
	glCompileShader(frag);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vert_stat);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &frag_stat);
	if (!vert_stat) {
		glGetShaderInfoLog(vert, 512, NULL, log);
		assertf(0, "Vertex Shader failed to compile: %s\n", log);
	}

	if (!frag_stat) {
		glGetShaderInfoLog(frag, 512, NULL, log);
		assertf(0, "Fragment Shader failed to compile: %s\n", log);
	}

	prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &prog_stat);
	if (!prog_stat) {
		glGetProgramInfoLog(prog, 512, NULL, log);
		assertf(0, "Shader Program failed to link: %s\n", log);
	}

	glDeleteShader(vert);
	glDeleteShader(frag);

	return prog;
}

void shader_free(const u32 id)
{
	glDeleteProgram(id);
}
