#ifndef PLATFORM_H
#define PLATFORM_H

#include "arena.h"

struct WindowDimension
{
    u32 width;
    u32 height;
};

char* platform_load_file(const char *name, Arena *arena, u64* bytes = NULL);

#endif

