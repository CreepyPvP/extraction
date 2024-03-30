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

void push_clear(CommandBuffer *commands, V3 color)
{
    CmdClear *clear = push_cmd(commands, CmdClear);
    clear->type = Entry_Clear;
    clear->color = color;
}

void push_quad(CommandBuffer* commands,
               V3 p0, V3 c0, 
               V3 p1, V3 c1,
               V3 p2, V3 c2,
               V3 p3, V3 c3)
{
    if (!commands->active_draw) {
        commands->active_draw = push_cmd(commands, CmdDrawQuads);
        commands->active_draw->type = Entry_DrawQuads;
        commands->active_draw->quad_count = 0;
        commands->active_draw->vert_offset = commands->vert_count;
    }

    assert(commands->vert_count + 4 <= commands->vert_cap);

    u32 vert = commands->vert_count;
    commands->vertices[vert + 0].pos = p0;
    commands->vertices[vert + 0].color = c0;
    commands->vertices[vert + 1].pos = p1;
    commands->vertices[vert + 1].color = c1;
    commands->vertices[vert + 2].pos = p2;
    commands->vertices[vert + 2].color = c2;
    commands->vertices[vert + 3].pos = p3;
    commands->vertices[vert + 3].color = c3;

    commands->vert_count += 4;
    commands->active_draw->quad_count++;
}

void draw_axis_widget(CommandBuffer* commands, float size)
{
        push_quad(commands, v3(0, size, 0), v3(0, 1, 0),
                            v3(0, 0, 0), v3(0),
                            v3(size, size, 0), v3(1, 1, 0),
                            v3(size, 0, 0), v3(1, 0, 0));

        push_quad(commands, v3(0, 0, size), v3(0, 0, 1),
                            v3(0, 0, 0), v3(0),
                            v3(size, 0, size), v3(1, 0, 1),
                            v3(size, 0, 0), v3(1, 0, 0));
}
