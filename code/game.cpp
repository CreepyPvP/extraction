struct GameState
{
    Arena arena;
    Mat4 projection;

    V3 pos;
};


GameState* create_game_state(void *memory, u64 size)
{
    assert(size >= sizeof(GameState));

    Arena *arena = (Arena*) memory;
    init_arena(arena, memory, size);
    GameState *state = push_struct(arena, GameState);

    state->projection = perspective(45, 16.0f / 9.0f, 0.1, 1000);
    state->pos = v3(0);

    return state;
}

void update_and_render(GameState* state, CommandBuffer *commands, GameInputs inputs, float delta)
{
    Mat4 view = look_at(v3(2, 0, 7), v3(0), v3(0, 0, 1)); 

    float speed = 4;

    if (inputs.down & KEY_W) {
        state->pos.x -= speed * delta;
    }
    if (inputs.down & KEY_S) {
        state->pos.x += speed * delta;
    }
    if (inputs.down & KEY_A) {
        state->pos.y -= speed * delta;
    }
    if (inputs.down & KEY_D) {
        state->pos.y += speed * delta;
    }

    commands->basis = basis(state->projection, view);
    commands->push_clear(v3(0.1, 0.1, 0.2));

    commands->draw_cube(state->pos, v3(0.5), v3(1, 0, 0));
    commands->draw_cube(v3(0, 0, -1), v3(10, 10, 0.5), v3(1));
}
