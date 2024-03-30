struct GameState
{
    Arena arena;
    Mat4 projection;
};


GameState* create_game_state(void *memory, u64 size)
{
    assert(size >= sizeof(GameState));

    Arena *arena = (Arena*) memory;
    init_arena(arena, memory, size);
    GameState *state = push_struct(arena, GameState);

    state->projection = perspective(45, 16.0f / 9.0f, 0.1, 1000);

    return state;
}

void update_and_render(GameState* state, CommandBuffer *commands)
{
    Mat4 view = look_at(v3(3), v3(0), v3(0, 0, 1)); 

    commands->basis = basis(state->projection, view);

    commands->push_clear(v3(0.1, 0.1, 0.2));
    // commands->draw_axis_widget(0.5);

    commands->draw_cube(v3(0), v3(0.5), v3(1, 0, 0));
    commands->draw_cube(v3(0, 0, -1), v3(10, 10, 0.5), v3(1));
}
