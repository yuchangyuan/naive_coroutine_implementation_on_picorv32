#ifndef NC_H
#define NC_H

#include <stdint.h>

typedef struct _nc_ctx_t {
    struct _nc_ctx_t *next;
    uintptr_t ra_old;
    uintptr_t sp_old;
    uintptr_t s_old[12];

    uintptr_t ra_new;
} nc_ctx_t;

typedef void (*nc_func_t)(void *data);

void nc_yield();
void nc_new(nc_func_t func, nc_ctx_t *ctx, uintptr_t *stack_top);

#endif
