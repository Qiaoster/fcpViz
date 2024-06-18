#pragma once

#define bool int
#define true 1
#define false 0

#define float64 double
#define KILO (1024)
#define MEGA (1024 * KILO)
#define GIGA (1024 * MEGA)

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#include <stdint.h>

#define int8 int8_t
#define int16 int16_t
#define int32 int32_t
#define int64 int64_t

#define uint8 uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t

#define Pi (3.141592653589f)
#define Rad(deg) ((deg)*Pi/180.0f)
#define Deg(rad) ((rad)*180.0f/Pi)
