#ifndef MQME_H
#define MQME_H

void mqme_init(const char *argv1, const char *font_path);
void mqme_update(const float dt);
void mqme_render(void);
void mqme_free(void);

#endif /* MQME_H */
