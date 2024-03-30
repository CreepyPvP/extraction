#ifndef RENDERER_H
#define RENDERER_H

struct Vertex
{
    V3 pos;
    V3 color;
};

enum CommandEntry
{
    Entry_Clear,
    Entry_DrawQuads
};

struct CmdDrawQuads
{
    CommandEntry type;
    u32 vert_offset;
    u32 quad_count;
};

struct CmdClear
{
    CommandEntry type;
    V3 color;
};

struct Basis
{
    // NOTE: proj and view matrix combined
    Mat4 transform;
    // TODO: up, right, ... (billboards)
};

struct CommandBuffer
{
    u32 curr;
    u32 cap;
    u8 *ptr;

    u32 vert_count;
    u32 vert_cap;
    Vertex* vertices;

    CmdDrawQuads *active_draw;

    Basis basis;
};

inline Basis basis(Mat4 proj, Mat4 view)
{
    return { proj * view };
}

void push_clear(CommandBuffer *commands, V3 color);

void push_quad(CommandBuffer *commands,
               V3 p0, V3 c0, 
               V3 p1, V3 c1,
               V3 p2, V3 c2,
               V3 p3, V3 c3);

void draw_axis_widget(CommandBuffer *commands, float size);

#endif
