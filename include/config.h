#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_RESOLUTION RESOLUTION_320x240
#define DISPLAY_BITDEPTH DEPTH_16_BPP
#define DISPLAY_NUM_BUFFERS 2
#define DISPLAY_GAMMA GAMMA_NONE
#define DISPLAY_FILTERS FILTERS_RESAMPLE

#define VIEWPORT_FOV 90.0f
#define VIEWPORT_NEAR 10.0f
#define VIEWPORT_FAR 100.0f

#define TICKRATE 60
#define DELTA_TIME (1.0f / (float)TICKRATE)

#endif /* _CONFIG_H_ */
