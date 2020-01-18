#define BIT_GET(value, bit) (((value) >> (bit)) & 1)
#define BIT_SET(value, bit) (value) |= 1 << (bit)
#define BIT_CLEAR(value, bit) (value) &= ~(1 << (bit))