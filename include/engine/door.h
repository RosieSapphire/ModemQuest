#ifndef _ENGINE_DOOR_H_
#define _ENGINE_DOOR_H_

struct door {
	u8 map_index;
#ifndef IS_USING_MQME
	vec2s pos;
#else /* IS_USING_MQME */
	ivec2 pos;
#endif /* IS_USING_MQME */
};

#endif /* _ENGINE_DOOR_H_ */
