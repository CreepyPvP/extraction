#include <windows.h>
#include <gl/gl.h>

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "platform.h"
#include "game_math.cpp"
#include "arena.cpp"
#include "renderer.cpp"
#include "opengl_renderer.cpp"
#include "game.cpp"

GameInputs inputs;

bool running;
bool show_debug_cursor = false;

static WindowDimension win32_get_window_dimensions(HWND window);

static void win32_set_key(u32 key, bool is_down);

LRESULT WINAPI 
win32_callback(HWND window,
               UINT message,
               WPARAM w_param,
               LPARAM l_param) {

    LRESULT result = 0;

    switch (message) {
        case WM_SIZE: {
            break;
        }
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
            break;
        }
        case WM_SETCURSOR: {
            if(show_debug_cursor)
            {
                result = DefWindowProcA(window, message, w_param, l_param);
            }
            else
            {
                SetCursor(0);
            }
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT paint;
            BeginPaint(window, &paint);
            EndPaint(window, &paint);
            break;
        }
        case WM_SYSKEYDOWN: 
        case WM_SYSKEYUP: 
        case WM_KEYDOWN: 
        case WM_KEYUP: {
            u32 code = w_param;
            bool was_down = ((l_param & (1 << 30)) != 0);
            bool is_down = ((l_param & (1 << 31)) == 0);
            
            if (was_down == is_down) {
                break;
            }

            if (code == 'W') {
                win32_set_key(KEY_W, is_down);
            } else if (code == 'S') {
                win32_set_key(KEY_S, is_down);
            } else if (code == 'A') {
                win32_set_key(KEY_A, is_down);
            } else if (code == 'D') {
                win32_set_key(KEY_D, is_down);
            } else if (code == VK_ESCAPE) {
                running = false;
            }

            break;
        }
        case WM_ACTIVATEAPP: {
            break;
        }
        default: {
            result = DefWindowProc(window, message, w_param, l_param);
        }
    };

    return result;
}

static void win32_set_key(u32 key, bool is_down)
{
    if (is_down) {
        inputs.down |= key;
    } else {
        inputs.down &= ~key;
    }
}

static WindowDimension win32_get_window_dimensions(HWND window)
{
    WindowDimension result;
    
    RECT rect;
    GetClientRect(window, &rect);
    result.width = rect.right - rect.left;
    result.height = rect.bottom - rect.top;

    return result;
}

static void* get_opengl_proc(const char* name) 
{
    void* proc = wglGetProcAddress(name);
    // TODO: Logging
    assert(proc && "Failed to load opengl proc");
    return proc;
}

static void init_opengl(HWND window) 
{
    HDC dc = GetDC(window);

    PIXELFORMATDESCRIPTOR format = {};
    format.nSize = sizeof(format);
    format.nVersion = 1;
    format.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    format.cColorBits = 32;
    format.cAlphaBits = 8;
    format.iLayerType = PFD_MAIN_PLANE;

    i32 index = ChoosePixelFormat(dc, &format);
    PIXELFORMATDESCRIPTOR actual_format;
    DescribePixelFormat(dc, index, sizeof(actual_format), &actual_format);

    SetPixelFormat(dc, index, &actual_format);

    HGLRC glrc = wglCreateContext(dc);
    if (!wglMakeCurrent(dc, glrc)) {
        // TODO: Handle this
    }
    ReleaseDC(window, dc);

    glCreateShader = (GL_CREATE_SHADER) get_opengl_proc("glCreateShader");
    glShaderSource = (GL_SHADER_SOURCE) get_opengl_proc("glShaderSource");
    glCompileShader = (GL_COMPILE_SHADER) get_opengl_proc("glCompileShader");
    glGetShaderiv = (GL_GET_SHADERIV) get_opengl_proc("glGetShaderiv");
    glGetShaderInfoLog = (GL_GET_SHADER_INFO_LOG) get_opengl_proc("glGetShaderInfoLog");
    glCreateProgram = (GL_CREATE_PROGRAM) get_opengl_proc("glCreateProgram");
    glAttachShader = (GL_ATTACH_SHADER) get_opengl_proc("glAttachShader");
    glLinkProgram = (GL_LINK_PROGRAM) get_opengl_proc("glLinkProgram");
    glGetProgramiv = (GL_GET_PROGRAMIV) get_opengl_proc("glGetProgramiv");
    glGetProgramInfoLog = (GL_GET_PROGRAM_INFO_LOG) get_opengl_proc("glGetProgramInfoLog");
    glDeleteShader = (GL_DELETE_SHADER) get_opengl_proc("glDeleteShader");
    glGetUniformLocation = (GL_GET_UNIFORM_LOCATION) get_opengl_proc("glGetUniformLocation");
    glUniformMatrix4fv = (GL_UNIFORM_MATRIX4FV) get_opengl_proc("glUniformMatrix4fv");
    glGenVertexArrays = (GL_GEN_VERTEX_ARRAYS) get_opengl_proc("glGenVertexArrays");
    glBindVertexArray = (GL_BIND_VERTEX_ARRAY) get_opengl_proc("glBindVertexArray");
    glGenBuffers = (GL_GEN_BUFFERS) get_opengl_proc("glGenBuffers");
    glBindBuffer = (GL_BIND_BUFFER) get_opengl_proc("glBindBuffer");
    glEnableVertexAttribArray = (GL_ENABLE_VERTEX_ATTRIB_ARRAY) get_opengl_proc("glEnableVertexAttribArray");
    glVertexAttribPointer = (GL_VERTEX_ATTRIB_POINTER) get_opengl_proc("glVertexAttribPointer");
    glUseProgram = (GL_USE_PROGRAM) get_opengl_proc("glUseProgram");
    glBufferData = (GL_BUFFER_DATA) get_opengl_proc("glBufferData");
    glMultiDrawArrays = (GL_MULTI_DRAW_ARRAYS) get_opengl_proc("glMultiDrawArrays");
}

void* win32_alloc(u64 size) 
{
    void* memory = VirtualAlloc( NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!memory) {
        OutputDebugStringA("Failed to allocate memory\n");
        assert(0);
        // TODO: Handle this
    }

    return memory;
}

char* platform_load_file(const char *name, Arena *arena, u64* length)
{
    FILE *fptr = fopen(name, "rb");
    if (fptr == NULL) {
        char buffer[256];
        sprintf(buffer, "Failed to read file: %s\n", name);
        OutputDebugStringA(buffer);
        assert(0);
    }

    fseek(fptr, 0, SEEK_END);
    u64 len = ftell(fptr);
    u8 *buffer = push_bytes(arena, len + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buffer, len, 1, fptr);
    buffer[len] = 0;

    if (length) {
        *length = len;
    }
    fclose(fptr);
    return (char*) buffer;
}

inline void platform_log(char *message)
{
    OutputDebugStringA(message);
}

int WINAPI WinMain(HINSTANCE instance, 
                   HINSTANCE prev_instance, 
                   PSTR cmd_line, 
                   int cmd_show) {
    inputs = {};

    LARGE_INTEGER perf_count_freq_res;
    QueryPerformanceFrequency(&perf_count_freq_res);
    f64 perf_count_freq = (f64) perf_count_freq_res.QuadPart;

    WNDCLASSA window_class = {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = win32_callback;
    window_class.hInstance = instance;
    window_class.lpszClassName = "ExtractionShooter";
    window_class.hCursor = LoadCursor(0, IDC_ARROW);

    HWND window = {};

    if (RegisterClass(&window_class)) {
         window = CreateWindowEx(
            0,
            window_class.lpszClassName,
            "Extraction",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            instance,
            0
        );
    }

    if (window) {
        OutputDebugStringA("Created Window\n");
        init_opengl(window);
        running = true;
    } else {
        OutputDebugStringA("Failed to create Window\n");
    }

    u64 platform_memory_size = Megabytes(64);
    void* platform_memory = win32_alloc(platform_memory_size);
    Arena win32_arena;
    init_arena(&win32_arena, platform_memory, platform_memory_size);

    u64 game_memory_size = Megabytes(64);
    void* game_memory = win32_alloc(game_memory_size);

    opengl_init_renderer(&win32_arena);

    CommandBuffer render_commands = alloc_command_buffer(&win32_arena);
    GameState *game_state = create_game_state(game_memory, game_memory_size);

    while (running) {
        MSG message;
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                running = false;
            }

            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        LARGE_INTEGER start_counter;
        QueryPerformanceCounter(&start_counter);
        u64 start_cycles = __rdtsc();

        HDC device_context = GetDC(window);
        WindowDimension dimension = win32_get_window_dimensions(window);

        begin_command_buffer(&render_commands);
        update_and_render(game_state, &render_commands, inputs, 1.0f / 60.0f);

        opengl_process_commands(&render_commands, dimension);

        SwapBuffers(device_context);
        ReleaseDC(window, device_context);

        LARGE_INTEGER end_counter;
        QueryPerformanceCounter(&end_counter);
        u64 end_cycles = __rdtsc();
        u64 cycles_elapsed = end_cycles - start_cycles;
        i64 counter_elapsed = end_counter.QuadPart - start_counter.QuadPart;
        f64 ms_per_frame = (1000.0f * counter_elapsed) / perf_count_freq;
        f64 fps = perf_count_freq / (f64) counter_elapsed;
        f64 mcpf = ((f64) cycles_elapsed) / (1000.0f * 1000.0f);

        char buffer[256];
        sprintf(buffer, "%.02fms/f,  %.02ff/s,  %.02fmc/f\n", ms_per_frame, fps, mcpf);
        OutputDebugStringA(buffer);

    }

    return 0;
}
