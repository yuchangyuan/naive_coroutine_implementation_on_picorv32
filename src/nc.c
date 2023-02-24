#include "nc.h"

// context for main corotine
static nc_ctx_t _main_ctx = {
    .next = &_main_ctx,
};

// current corotine
nc_ctx_t *_curr = &_main_ctx;

void __attribute__((naked)) nc_yield()
{
    // save context
    __asm__ volatile(
        // copy sp to t0
        "mv t0, sp\n\t"

        // load *_curr to sp
        "lw sp, _curr\n\t"
        "lw sp, (sp)\n\t"

        // save ra & sp to _curr->ra_old & _curr->sp_old
        "sw ra,  4(sp)\n\t"
        "sw t0,  8(sp)\n\t"

        // save s0~s11 to _curr->s_old
        "sw  s0, 12(sp)\n\t"
        "sw  s1, 16(sp)\n\t"
        "sw  s2, 20(sp)\n\t"
        "sw  s3, 24(sp)\n\t"
        "sw  s4, 28(sp)\n\t"
        "sw  s5, 32(sp)\n\t"
        "sw  s6, 36(sp)\n\t"
        "sw  s7, 40(sp)\n\t"
        "sw  s8, 44(sp)\n\t"
        "sw  s9, 48(sp)\n\t"
        "sw s10, 52(sp)\n\t"
        "sw s11, 56(sp)\n\t"

        // also save ra to _curr->ra_new
        "sw ra, 60(sp)\n\t"
        );

    // find next & update _curr
    __asm__ volatile(
        "lw t0, (sp)\n\t" // load _next to t0
        "lw sp, _curr\n\t" // load _curr addr to sp
        "sw t0, (sp)\n\t"
        "mv sp, t0\n\t"
        ); // update sp to new _curr

    // restore context
    __asm__ volatile(
        // restore s0~s11 from _curr->s_old
        "lw  s0, 12(sp)\n\t"
        "lw  s1, 16(sp)\n\t"
        "lw  s2, 20(sp)\n\t"
        "lw  s3, 24(sp)\n\t"
        "lw  s4, 28(sp)\n\t"
        "lw  s5, 32(sp)\n\t"
        "lw  s6, 36(sp)\n\t"
        "lw  s7, 40(sp)\n\t"
        "lw  s8, 44(sp)\n\t"
        "lw  s9, 48(sp)\n\t"
        "lw s10, 52(sp)\n\t"
        "lw s11, 56(sp)\n\t"
        // restore ra from _curr->ra_new
        "lw ra, 60(sp)\n\t"
        // load _curr->ra_old to t0
        "lw t0,  4(sp)\n\t"
        // restore sp
        "lw sp,  8(sp)\n\t"
        // return, but use ra_old
        "jr t0\n\t"
        );
}

void finalize()
{
    // find prev ctx
    nc_ctx_t *ctx = _curr;

    while (ctx->next != _curr) {
        ctx = ctx->next;
    }

    // skip _curr
    ctx->next = _curr->next;

    // switch to next corotine, bye
    nc_yield();
}

void nc_new(nc_func_t func, nc_ctx_t *ctx, uintptr_t *stack_top)
{
    // init context
    ctx->sp_old = (uintptr_t)stack_top;
    ctx->ra_old = (uintptr_t)func;
    ctx->ra_new = (uintptr_t)finalize;

    // append ctx to _curr
    ctx->next = _curr->next;
    _curr->next = ctx;
}
