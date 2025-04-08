#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define clz32(x) clz2(x, 0)
static const int mask[] = {0, 8, 12, 14};
static const int magic[] = {1, 1, 0, 0};

unsigned clz2(uint32_t x, int c)
{
    if (!x && !c)
        return 32;

    uint32_t upper = (x >> (16 >> c));
    uint32_t lower = (x & (0xFFFF >> mask[c]));
    if (c == 3)
        return upper ? magic[upper] : 2 + magic[lower];
    return upper ? clz2(upper, c + 1) : (16 >> (c)) + clz2(lower, c + 1);
}

static inline int clz64(uint64_t x)
{
    /* If the high 32 bits are nonzero, count within them.
     * Otherwise, count in the low 32 bits and add 32.
     */
    return (x >> 32) ? clz32((uint32_t) (x >> 32)) : clz32((uint32_t) x) + 32;
}

uint64_t sqrti(uint64_t x, bool if_ceil)
{
    uint64_t m, y = 0;
    if (x <= 1)
        return x;

    int total_bits = 64;

    /* clz64(x) returns the count of leading zeros in x.
     * (total_bits - 1 - clz64(x)) gives the index of the highest set bit.
     * Rounding that index down to an even number ensures our starting m is a
     * power of 4.
     */
    int shift = (total_bits - 1 - clz64(x)) & ~1UL;
    m = 1ULL << shift;

    while (m) {
        uint64_t b = y + m;
        y >>= 1;
        if (x >= b) {
            x -= b;
            y += m;
        }
        m >>= 2;
    }
    
    if (if_ceil)
        return x != 0? y+1: y;
    return y;
}

int main(){
    uint64_t result = sqrti(1023, true);
    printf("%ld\n", result);
    
    return 0;
}
