#ifndef _ENGINE_CONFIG_H_
#define _ENGINE_CONFIG_H_

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_RESOLUTION RESOLUTION_320x240
#define DISPLAY_BITDEPTH DEPTH_16_BPP
#define DISPLAY_BUFFER_CNT 3
#define DISPLAY_GAMMA GAMMA_NONE
#define DISPLAY_FILTER FILTERS_RESAMPLE

#define BASE_FONT_ID 1

#define TICKRATE 60
#define DELTATIME (1.0f / TICKRATE)
#define DELTATICKS (DELTATIME * TICKS_PER_SECOND)

#define STICK_DEADZONE 40

#endif /* _ENGINE_CONFIG_H_ */