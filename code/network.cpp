#define NETWORK_MAGIC_NUMBER 297949

enum PacketType
{
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

struct NetworkPacket
{
    NetworkHeader header;
    char buffer[256];
};

bool stream_u32(Stream *stream, u32 *value, StreamOp op)
{
    if (stream->ptr + sizeof(u32) > stream->size) {
        return false;
    }

    u32 *addr = (u32*) (stream->memory + stream->ptr);
    stream->ptr += sizeof(u32);

    if (op == Stream_Write) {
        *addr = platform_hton(*value);
    } else {
        *value = platform_ntoh(*addr);
    }

    return true;
}

bool stream_header(Stream *stream, NetworkHeader *header, StreamOp op)
{
    if (!stream_u32(stream, &header->magic_number, op)) {
        return false;
    }
    if (!stream_u32(stream, &header->type, op)) {
        return false;
    }
    return true;
}

void stream_inputs(Stream *stream, GameInputs *inputs, StreamOp op)
{
    stream_u32(stream, &inputs->down, op);
}
