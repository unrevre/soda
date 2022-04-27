#include "utils.h"

#include <stddef.h>
#include <stdint.h>

char* split(char* buffer, char token) {
    int32_t i;

    for (i = 0; buffer[i] != '\0' && buffer[i] != token; ++i);

    return (buffer[i] == '\0') ? NULL : &buffer[++i];
}
