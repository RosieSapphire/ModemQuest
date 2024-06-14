#ifndef _MAPPY_GLWIN_H_
#define _MAPPY_GLWIN_H_

#include <GLFW/glfw3.h>

#include "types.h"

#define WIN_WID_DEFAULT 1024
#define WIN_HEI_DEFAULT 768

typedef struct
{
	double mx_last, mx_now,
	       my_last, my_now;
	u8 w : 1, s : 1, a : 1, d : 1,
	   z_last : 1, z_now : 1, shift_last : 1, shift_now : 1;
	   
	u8 lmb_last   : 1, lmb_now   : 1,
	   rmb_last   : 1, rmb_now   : 1,
	   mmb_last   : 1, mmb_now   : 1,
	   enter_last : 1, enter_now : 1;
} glwin_input_t;

extern GLFWwindow *glwin;
extern int glwin_w, glwin_h;

void glwin_init(const char *outpath);
void glwin_input_poll(glwin_input_t *inp);

#endif /* _MAPPY_GLWIN_H_ */
