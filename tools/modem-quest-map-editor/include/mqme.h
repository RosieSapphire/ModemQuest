#ifndef _MQME_H_
#define _MQME_H_

void mqme_init(const char *argv1, const char *font_path);
void mqme_update(const float dt);
void mqme_render(void);
void mqme_free(void);

#endif /* _MQME_H_ */
