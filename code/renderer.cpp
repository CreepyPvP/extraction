#include "renderer.h"

CommandBuffer alloc_command_buffer(Arena *arena)
{
    CommandBuffer commands = {};
    commands.cap = Kilobytes(5);
    commands.ptr = push_bytes(arena, commands.cap);
    commands.vert_cap = 1000;
    commands.vertices = push_arr(arena, Vertex, commands.vert_cap);
    return commands;
}

void begin_command_buffer(CommandBuffer* commands)
{
    commands->curr = 0;
    commands->vert_count = 0;
    commands->active_draw = NULL;
}

#define push_cmd(commands, type) (type*) push_cmd_(commands, sizeof(type))

void* push_cmd_(CommandBuffer *commands, u32 size)
{
    assert(commands->curr + size <= commands->cap);
    void *ptr = commands->ptr + commands->curr;
    commands->curr += size;
    return ptr;
}

void CommandBuffer::push_clear(V3 color)
{
    CmdClear *clear = push_cmd(this, CmdClear);
    clear->type = Entry_Clear;
    clear->color = color;
}

void CommandBuffer::push_quad(V3 p0, V3 p1, V3 p2, V3 p3, V3 norm, V3 color)
{
    if (!active_draw) {
        active_draw = push_cmd(this, CmdDrawQuads);
        active_draw->type = Entry_DrawQuads;
        active_draw->quad_count = 0;
        active_draw->vert_offset = vert_count;
    }

    assert(vert_count + 4 <= vert_cap);

    vertices[vert_count + 0].pos = p0;
    vertices[vert_count + 0].norm = norm;
    vertices[vert_count + 0].color = color;
    vertices[vert_count + 1].pos = p1;
    vertices[vert_count + 1].norm = norm;
    vertices[vert_count + 1].color = color;
    vertices[vert_count + 2].pos = p2;
    vertices[vert_count + 2].norm = norm;
    vertices[vert_count + 2].color = color;
    vertices[vert_count + 3].pos = p3;
    vertices[vert_count + 3].norm = norm;
    vertices[vert_count + 3].color = color;

    vert_count += 4;
    active_draw->quad_count++;
}

void CommandBuffer::draw_axis_widget(float size)
{
    // TODO: Implement
}

void CommandBuffer::draw_cube(V3 pos, V3 radius, V3 color)
{
    V3 p1 = v3(pos.x - radius.x, pos.y - radius.y, pos.z + radius.z);
    V3 p2 = v3(pos.x - radius.x, pos.y + radius.y, pos.z + radius.z);
    V3 p3 = v3(pos.x + radius.x, pos.y + radius.y, pos.z + radius.z);
    V3 p4 = v3(pos.x + radius.x, pos.y - radius.y, pos.z + radius.z);
    V3 p5 = v3(pos.x - radius.x, pos.y - radius.y, pos.z - radius.z);
    V3 p6 = v3(pos.x - radius.x, pos.y + radius.y, pos.z - radius.z);
    V3 p7 = v3(pos.x + radius.x, pos.y + radius.y, pos.z - radius.z);
    V3 p8 = v3(pos.x + radius.x, pos.y - radius.y, pos.z - radius.z);

    push_quad(p1, p2, p4, p3, v3(0, 0, 1), color);
    push_quad(p8, p7, p5, p6, v3(0, 0, -1), color);
    push_quad(p8, p4, p7, p3, v3(1, 0, 0), color);
    push_quad(p6, p2, p5, p1, v3(-1, 0, 0), color);
    push_quad(p7, p3, p6, p2, v3(0, 1, 0), color);
    push_quad(p5, p1, p8, p4, v3(0, -1, 0), color);
}
