#ifndef TYPES_H
#define TYPES_H

#define Kilobytes(size) (size * 1024)
#define Megabytes(size) (Kilobytes(size) * 1024)
#define Gigabytes(size) (Megabytes(size) * 1024)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

enum StreamOp
{
    Stream_Write,
    Stream_Read,
};

struct Stream
{
    u8 *memory;
    u32 size;
    u32 ptr;
};

inline Stream stream_from_mem(u8 *memory, u32 size)
{
    return Stream { memory, size };
}

struct V3
{
    union {
        struct {
            float x;
            float y;
            float z;
        };

        struct {
            float r;
            float g;
            float b;
        };
    };

    V3 operator+(V3 b);
    V3 operator-(V3 b);
};

inline V3 v3(float v0, float v1, float v2)
{
    return { v0, v1, v2 };
}

inline V3 v3(float v0)
{
    return { v0, v0, v0 };
}

// NOTE: Column major
// 0  4  8  12
// 1  5  9  13
// 2  6  10 14
// 3  7  11 15
struct Mat4
{
    float v[16];

    Mat4 operator*(Mat4 other);
};

#endif
