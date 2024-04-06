#define NETWORK_MAGIC_NUMBER 297949

#define Step(expr) if (!(expr)) { return false; }

enum PacketType
{
    Type_Ping,
    Type_ClientInput,
    Type_ServerInput,
    Type_InitGameState,
    Type_AckGameState,
};

struct NetworkHeader
{
    u32 magic_number;
    u32 type;
};

inline NetworkHeader header_of_type(PacketType type) {
    return { NETWORK_MAGIC_NUMBER, (u32) type };
}

struct ServerInput
{
    u32 player_count;
    V3 *player_pos;
};

struct ServerInitState
{
    u32 me;
    u32 player_count;
    V3 player_pos[4];
    V3 player_color[4];
};

bool stream_u32(Stream *stream, u32 *value, StreamOp op)
{
    if (stream->ptr + sizeof(u32) > stream->size) {
        return false;
    }

    u32 *addr = (u32*) (stream->memory + stream->ptr);
    stream->ptr += sizeof(u32);

    if (op == Stream_Write) {
        *addr = platform_htonu(*value);
    } else {
        *value = platform_ntohu(*addr);
    }

    return true;
}

bool stream_f32(Stream *stream, f32 *value, StreamOp op)
{
    if (stream->ptr + sizeof(f32) > stream->size) {
        return false;
    }

    u32 *addr = (u32*) (stream->memory + stream->ptr);
    stream->ptr += sizeof(f32);

    if (op == Stream_Write) {
        *addr = platform_htonf(*value);
    } else {
        *value = platform_ntohf(*addr);
    }

    return true;
}

bool stream_v3(Stream *stream, V3 *value, StreamOp op)
{
    Step(stream_f32(stream, &value->x, op));
    Step(stream_f32(stream, &value->y, op));
    Step(stream_f32(stream, &value->z, op));
    return true;
}

bool stream_header(Stream *stream, NetworkHeader *header, StreamOp op)
{
    Step(stream_u32(stream, &header->magic_number, op));
    Step(stream_u32(stream, &header->type, op));
    return true;
}

bool stream_inputs(Stream *stream, GameInputs *inputs, StreamOp op)
{
    Step(stream_u32(stream, &inputs->down, op));
    return true;
}

bool stream_init_state(Stream *stream, ServerInitState *state, StreamOp op)
{
    Step(stream_u32(stream, &state->me, op));
    Step(stream_u32(stream, &state->player_count, op));

    for (u32 i = 0; i < state->player_count; ++i) {
        Step(stream_v3(stream, state->player_pos + i, op));
        Step(stream_v3(stream, state->player_color + i, op));
    }

    return true;
}
