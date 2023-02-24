#include "nc.h"

#include <stdint.h>
#include <math.h>

static void out_char(char c)
{
    volatile char *out = (volatile char *)(512 * 1024);
    *out = c;
}

void put_uint_ln(uint32_t x)
{
    char buf[20];
    int i;

    i = 0;

    if (x == 0) buf[i++] = '0';

    while (x != 0) {
        buf[i++] = (x % 10) + '0';
        x /= 10;
    }


    while (i > 0) {
        out_char(buf[--i]);
    }

    out_char('\n');
}

void put_str(char *str)
{
    int i;

    while (*str) {
        out_char(*str++);
    }
}

void put_str_ln(char *str)
{
    put_str(str);
    out_char('\n');
}


volatile int _tick = 0;

uint8_t c1_stack[1024] __attribute__((aligned (16)));
nc_ctx_t c1_ctx;

void c1_func()
{
    while (1) {
        if ((_tick % 5) == 0) {
            put_str("\tt1: ");
            put_uint_ln(_tick);
        }

        nc_yield();
    }
}

uint8_t c2_stack[1024] __attribute__((aligned (16)));
nc_ctx_t c2_ctx;

void c2_func()
{
    while (_tick < 22) {
        if ((_tick % 7) == 0) {
            put_str("\t\tt2: ");
            put_uint_ln(_tick);
        }

        nc_yield();
    }
}

int main()
{
    int i;

    nc_new(c1_func, &c1_ctx, (uintptr_t*)(c1_stack + 1024));
    nc_new(c2_func, &c2_ctx, (uintptr_t*)(c2_stack + 1024));

    while (1) {
        _tick++;
        put_str("main: ");
        put_uint_ln(_tick);

        nc_yield();

        if (_tick >= 40) break;
    }

    return 0;
}
