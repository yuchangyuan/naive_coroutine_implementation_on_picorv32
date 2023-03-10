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
    while (x != 0) {
        buf[i++] = (x % 10) + '0';
        x /= 10;
    }


    while (i > 0) {
        out_char(buf[--i]);
    }

    out_char('\n');
}

int main()
{
    int i;

    for (i = 0; i < 10; i++) {
        put_uint_ln(i);
        nc_yield();
    }

    return 0;
}
