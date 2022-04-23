#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdint.h>

struct soda_t;

void handle_message(struct soda_t* soda, const char* buffer, int32_t len);

#endif /* MESSAGES_H */
