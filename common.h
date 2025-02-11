#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Get the n-th bit of the number a
#define BIT(a, n) ((a & (1 << n)) ? 1 : 0)

#endif