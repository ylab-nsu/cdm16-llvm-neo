typedef unsigned long long uint64;
typedef long long int64;

#ifdef __COCAS__
#define TRAP() __asm__("dw 0")
#else
#define TRAP() __asm__("zero")
#endif

uint64 __udivmoddi4(uint64 a, uint64 b, uint64 *rem) {
    if (b == 0) {
        TRAP();
    }

    uint64 quotient = 0;
    uint64 remainder = 0;

    for (int i = 0; i < 64; i++) {
        remainder <<= 1;
        if (a & 0x8000000000000000ULL) {
            remainder |= 1;
        }
        a <<= 1;

        quotient <<= 1;
        if (remainder >= b) {
            remainder -= b;
            quotient |= 1;
        }
    }

    if (rem) {
        *rem = remainder;
    }

    return quotient;
}

uint64 __udivdi3(uint64 a, uint64 b) {
    return __udivmoddi4(a, b, 0);
}

uint64 __umoddi3(uint64 a, uint64 b) {
    uint64 rem;
    __udivmoddi4(a, b, &rem);
    return rem;
}

int64 __divdi3(int64 a, int64 b) {
    int neg = 0;
    uint64 ua, ub;

    if (a < 0) {
        neg = 1;
        ua = -(uint64)a;
    } else {
        ua = (uint64)a; 
    }

    if (b < 0) {
        neg ^= 1;
        ub = -(uint64)b;
    } else {
        ub = (uint64)b;
    }

    uint64 q = __udivmoddi4(ua, ub, 0);
    
    if (neg) {
        q = -q;
    }

    return (int64)q;
}

int64 __moddi3(int64 a, int64 b) {
    int neg = 0;
    uint64 ua, ub, rem;

    if (a < 0) {
        neg = 1;
        ua = -(uint64)a;
    } else {
        ua = (uint64)a;
    }

    if (b < 0) {
        ub = -(uint64)b;
    } else {
        ub = (uint64)b;
    }

    __udivmoddi4(ua, ub, &rem);

    if (neg) {
        rem = -rem;
    }

    return (int64)rem;
}
