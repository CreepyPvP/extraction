struct Player
{
    V3 pos;
    V3 color;
};

struct GameState
{
    Arena arena;
    Mat4 projection;

    u32 player_count;
    Player player[4];
    u32 me;
};

void to_init_state(GameState *state, ServerInitState *server_init)
{
    u32 new_player = state->player_count;
    server_init->me = new_player;

    state->player[new_player].pos = v3(0);
    state->player[new_player].color = v3(0, 0, 1);
    state->player_count++;

    server_init->player_count = state->player_count;

    for (u32 i = 0; i < state->player_count; ++i) {
        server_init->player_pos[i] = state->player[i].pos;
        server_init->player_color[i] = state->player[i].color;
    }
}

GameState* create_game_state(void *memory, u64 size, ServerInitState *server_init)
{
    assert(size >= sizeof(GameState));

    Arena *arena = (Arena*) memory;
    init_arena(arena, memory, size);
    GameState *state = push_struct(arena, GameState);

    state->projection = perspective(45, 16.0f / 9.0f, 0.1, 1000);

    if (!server_init) {
        // This means we are the server
        u32 me = 0;
        state->player_count = 1;
        state->player[me].pos = v3(0);
        state->player[me].color = v3(1, 0, 0);
        state->me = me;
    } else {
        state->me = server_init->me;
        state->player_count = server_init->player_count;
        for (u32 i = 0; i < server_init->player_count; ++i) {
            state->player[i].pos = server_init->player_pos[i];
            state->player[i].color = server_init->player_color[i];
        }
    }

    return state;
}

void handle_server_input(GameState* state, ServerInput *input)
{
    for (u32 i = 0; i < input->player_count; ++i) {
        state->player[i].pos = input->player_pos[i];
    }
}

void update_and_render(GameState *state, CommandBuffer *commands, GameInputs inputs, float delta)
{
    Mat4 view = look_at(v3(2, 0, 7), v3(0), v3(0, 0, 1)); 

    float speed = 4;

    u32 me = state->me;
    if (inputs.down & KEY_W) {
        state->player[me].pos.x -= speed * delta;
    }
    if (inputs.down & KEY_S) {
        state->player[me].pos.x += speed * delta;
    }
    if (inputs.down & KEY_A) {
        state->player[me].pos.y -= speed * delta;
    }
    if (inputs.down & KEY_D) {
        state->player[me].pos.y += speed * delta;
    }

    commands->basis = basis(state->projection, view);
    commands->push_clear(v3(0.1, 0.1, 0.2));

    commands->draw_cube(v3(0, 0, -1), v3(10, 10, 0.5), v3(1));
    for (u32 i = 0; i < state->player_count; ++i) {
        commands->draw_cube(state->player[i].pos, v3(0.5), state->player[i].color);
    }
}

void render_loading_screen(CommandBuffer *commands)
{
    commands->push_clear(v3(0.1, 0.2, 0.1));
}
