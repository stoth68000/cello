#include "logger.h"

#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdarg.h>

struct logger_ctx_s
{
    char filename[256];
    FILE *fh;
    pthread_mutex_t mutex;
};

int _getTimestamp(char *buf, int buflen, time_t *when)
{
        if (buflen < 16)
                return -1;

        struct tm tm;
        time_t now;

        if (when)
                now = *when;
        else
                time(&now);

        localtime_r(&now, &tm);

        sprintf(buf, "%04d%02d%02d-%02d%02d%02d",
                tm.tm_year + 1900,
                tm.tm_mon  + 1,
                tm.tm_mday,
                tm.tm_hour,
                tm.tm_min,
                tm.tm_sec);

        return 0;
}

void *cello_logger_alloc(char *filename)
{
    struct logger_ctx_s *ctx = (struct logger_ctx_s *)calloc(1, sizeof(*ctx));

    if (filename == NULL) {
        sprintf(ctx->filename, "/tmp/");
        _getTimestamp(ctx->filename + strlen(ctx->filename), sizeof(ctx->filename) - strlen(ctx->filename), NULL);
        sprintf(ctx->filename + strlen(ctx->filename), "-cello.log");
    }

    ctx->fh = fopen(ctx->filename, "w");
    if (ctx->fh == NULL) {
        return NULL;
    }

    return ctx;
}

void cello_logger_free(void *handle)
{
    struct logger_ctx_s *ctx = (struct logger_ctx_s *)handle;
    if (ctx && ctx->fh) {
        fclose(ctx->fh);
        ctx->fh = NULL;
    }
    free(ctx);
}

void cello_logger_printf(void *handle, const char *format, ...)
{
    struct logger_ctx_s *ctx = (struct logger_ctx_s *)handle;
    if (!ctx->fh)
        return;

    char buf[2048];

    _getTimestamp(&buf[0], sizeof(buf), NULL);

    sprintf(buf + strlen(buf), ": ");

    va_list vl;
    va_start(vl, format);
    vsprintf(&buf[strlen(buf)], format, vl);
    va_end(vl);

    sprintf(buf + strlen(buf), "\n");

    printf("%s", buf);

    fprintf(ctx->fh, buf);
    fflush(ctx->fh);
}
