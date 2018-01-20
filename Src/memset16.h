#ifndef MEMSET16_H
#define MEMSET16_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

void memset16(void *dst, uint16_t val, size_t len);

#ifdef __cplusplus
}
#endif

#endif // MEMSET16_H
