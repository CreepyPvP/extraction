#ifndef PLATFORM_H
#define PLATFORM_H

#include "arena.h"

#define KEY_W (1 << 0)
#define KEY_A (1 << 1)
#define KEY_S (1 << 2)
#define KEY_D (1 << 3)

struct WindowDimension
{
    u32 width;
    u32 height;
};

struct GameInputs
{
    u32 down;
};

char* platform_load_file(const char *name, Arena *arena, u64* bytes = NULL);

inline void platform_log(char *message);

#endif

