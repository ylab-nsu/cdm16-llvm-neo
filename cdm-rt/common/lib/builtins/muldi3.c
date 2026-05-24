typedef unsigned long long uint64;
typedef long long int64;

uint64 __muldi3(uint64 a, uint64 b) {
    uint64 result = 0;
    while (b) {
        if (b & 1) {
            result += a;
        }
        a <<= 1;
        b >>= 1;
    }
    return result;
}