#ifndef ENGINE_DOOR_H
#define ENGINE_DOOR_H

struct door {
	u8 map_index;
#ifndef IS_USING_MQME
	vec2s pos;
#else /* IS_USING_MQME */
	u8 padding0;
	u16 padding1;
	ivec2 pos;
#endif /* IS_USING_MQME */
};

#endif /* ENGINE_DOOR_H */
