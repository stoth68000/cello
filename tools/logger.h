#ifndef CELLO_LOGGER_H
#define CELLO_LOGGER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

void *cello_logger_alloc(char *filename);
void  cello_logger_free(void *ctx);
void  cello_logger_printf(void *handle, const char *format, ...);

#ifdef __cplusplus
};
#endif

#endif /* CELLO_LOGGER_H */
