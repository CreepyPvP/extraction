#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "types.h"
#include "platform.h"
#include "network.cpp"
#include "game_math.cpp"
#include "arena.cpp"
#include "renderer.cpp"
#include "opengl_renderer.cpp"
#include "game.cpp"

GameInputs inputs;

bool running;
bool show_debug_cursor = false;

i32 sock;

static WindowDimension win32_get_window_dimensions(HWND window);

static void win32_set_key(u32 key, bool is_down);


union Win32Addr
{
    sockaddr addr;
    sockaddr_in6 ipv6;
    sockaddr_in ipv4;
};


struct Win32NetworkState
{
    i32 socket;
    u32 buffer_size;
    u8 *buffer;
};

struct Win32ServerState
{
    Win32NetworkState base;
    u32 client_count;
    Win32Addr clients[4];
};

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

static void win32_init_opengl(HWND window) 
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

    // void (*swap_interval)(GLuint) = (void (*)(GLuint)) get_opengl_proc("wglSwapIntervalEXT");
    // swap_interval(0);
}

static ADDRINFOA* win32_select_addr(ADDRINFOA *addr)
{
    ADDRINFOA *selected = NULL;

    while (addr) {
        // We prefer ipv6
        if (!selected || addr->ai_family == AF_INET6) {
            selected = addr;
        }
        addr = addr->ai_next;
    }

    return selected;
}

static ADDRINFO* win32_get_addr_info(char *name, char *port, bool will_bind)
{
    ADDRINFOA hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (!name && will_bind) {     
        // NOTE: Dont set this when connecting to remote host
        hints.ai_flags |= AI_PASSIVE;
    }

    ADDRINFOA *result;
    if (getaddrinfo(name, port, &hints, &result)) {
        // TODO: Handle this
        assert(0);
    }

    return win32_select_addr(result);
}

bool win32_compare_addr(Win32Addr *a, Win32Addr* b)
{
    if (a->addr.sa_family != b->addr.sa_family) {
        return false;
    }

    if (a->addr.sa_family == AF_INET) {
        return a->ipv4.sin_port == b->ipv4.sin_port &&
                a->ipv4.sin_addr.S_un.S_addr == b->ipv4.sin_addr.S_un.S_addr;
    }
    if (a->addr.sa_family == AF_INET6) {
        if (a->ipv6.sin6_port != b->ipv6.sin6_port) {
            return false;
        }

        for (u32 i = 0; i < 16; ++i) {
            if (a->ipv6.sin6_addr.u.Byte[i] != b->ipv6.sin6_addr.u.Byte[i]) {
                return false;
            }
        }

        return true;
    }

    return false;
}

// NOTE:DLKFJDSFl;kdsjflsdkfjsdflksdjf
static void win32_check_socket_error()
{
    i32 error = WSAGetLastError();
    char buffer[256];
    sprintf(buffer, "Network Error %i\n", error);
    // TOOD: Maybe print error name?
    platform_log(buffer);
}

static Win32NetworkState* win32_init_network(bool is_server, Arena *win32_arena)
{

    if (is_server) {
        platform_log("Running server.\n");
    } else {
        platform_log("Running client.\n");
    }

    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        platform_log("WSAStartup failed\n");
        assert(0);
    }

    if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2) {
        platform_log("Version 2.2 of Winsock is not available\n");
        WSACleanup();
        assert(0);
    }

    ADDRINFOA *server_addr = win32_get_addr_info(NULL, "7000", is_server);

    i32 sock = socket(server_addr->ai_family, server_addr->ai_socktype, server_addr->ai_protocol);
    assert(sock != -1);

    DWORD non_blocking = 1;
    assert(ioctlsocket(sock, FIONBIO, &non_blocking) == 0);

    Win32NetworkState *state;
    if (is_server) {
        Win32ServerState *server = push_struct(win32_arena, Win32ServerState);
        server->client_count = 0;
        state = (Win32NetworkState*) server;
    } else {
        state = push_struct(win32_arena, Win32NetworkState);
    }

    state->socket = sock;
    // The largest safe size of udp packets
    state->buffer_size = 512;
    state->buffer = push_bytes(win32_arena, state->buffer_size);

    if (is_server) {
        if (bind(sock, server_addr->ai_addr, server_addr->ai_addrlen)) {
            assert(0);
        }
    } else {
        if (connect(sock, server_addr->ai_addr, server_addr->ai_addrlen)) {
            win32_check_socket_error();
            assert(0);
        }
    }

    return state;
}

static void win32_destroy_socket()
{
    closesocket(sock);
    WSACleanup();
}

void* win32_alloc(u64 size) 
{
    void* memory = VirtualAlloc( NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!memory) {
        platform_log("Failed to allocate memory\n");
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
        platform_log(buffer);
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
    // OutputDebugStringA(message);
}

inline u32 platform_ntoh(u32 value)
{
    return ntohl(value);
}

inline u32 platform_hton(u32 value)
{
    return htonl(value);
}

inline u32 platform_ntoh(f32 value)
{
    return ntohf(value);
}

inline u32 platform_hton(f32 value)
{
    return htonf(value);
}

int WINAPI WinMain(HINSTANCE instance, 
                   HINSTANCE prev_instance, 
                   PSTR cmd_line, 
                   int window_show) {
    bool is_server = *cmd_line;

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
        // Not the kind of fullscreen we want
        // ShowWindow(window, SW_MAXIMIZE);
        platform_log("Created Window\n");
        win32_init_opengl(window);
        running = true;

    } else {
        platform_log("Failed to create Window\n");
        return 1;
    }

    u64 platform_memory_size = Megabytes(64);
    void* platform_memory = win32_alloc(platform_memory_size);
    Arena win32_arena;
    init_arena(&win32_arena, platform_memory, platform_memory_size);

    Win32NetworkState *network = win32_init_network(is_server, &win32_arena);

    u64 game_memory_size = Megabytes(64);
    void* game_memory = win32_alloc(game_memory_size);

    opengl_init_renderer(&win32_arena);

    CommandBuffer render_commands = alloc_command_buffer(&win32_arena);
    GameState *game_state = NULL;
    if (is_server) {
        game_state = create_game_state(game_memory, game_memory_size, NULL);
    }

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

        // TODO: Make sure all player inputs get polled here
        {
            // TODO: Handle stream op failures
            if (is_server) {
                Win32ServerState *server = (Win32ServerState*) network;
                while (true) {
                    Win32Addr from;
                    i32 from_len = sizeof(from);
                    i32 bytes_read = recvfrom(network->socket, (char*) network->buffer, network->buffer_size, 0, (SOCKADDR*) &from, &from_len);
                    if (bytes_read == -1) {
                        break;
                    }
                    if (bytes_read > 0) {
                        Stream stream = stream_from_mem(network->buffer, network->buffer_size);
                        NetworkHeader header; 
                        bool valid_header = stream_header(&stream, &header, Stream_Read);

                        if (!valid_header || header.magic_number != NETWORK_MAGIC_NUMBER) {
                            continue;
                        }

                        i32 client_id = -1;
                        for (u32 i = 0; server->client_count; ++i) {
                            if (win32_compare_addr(&from, server->clients + i)) {
                                client_id = i;
                                break;
                            }
                        }

                        if (client_id == -1 && header.type == Type_Ping && server->client_count < 4) {
                            server->clients[server->client_count] = from;
                            server->client_count++;

                            Stream stream = stream_from_mem(network->buffer, network->buffer_size);
                            NetworkHeader header = header_of_type(Type_InitGameState);
                            stream_header(&stream, &header, Stream_Write);
                            ServerInitState state;
                            to_init_state(game_state, &state);
                            stream_init_state(&stream, &state, Stream_Write);
                            sendto(network->socket, (char*) stream.memory, stream.size, 0, &from.addr, from_len);
                            continue;
                        }
                    } else {
                        break;
                    }
                }
            } else {
                while (true) {
                    i32 bytes_read = recv(network->socket, (char*) network->buffer, network->buffer_size, 0);
                    Stream stream = stream_from_mem(network->buffer, bytes_read);

                    if (bytes_read == -1) {
                        win32_check_socket_error();
                        break;
                    }
                    if (bytes_read > 0) {
                        // Check if message starts with magic number
                        NetworkHeader header;
                        bool valid_header = stream_header(&stream, &header, Stream_Read);

                        if (!valid_header || header.magic_number != NETWORK_MAGIC_NUMBER) {
                            continue;
                        }

                        if (header.type == Type_InitGameState) {
                            if (!game_state) {
                                ServerInitState init_state;
                                bool valid = stream_init_state(&stream, &init_state, Stream_Read); 
                                if (valid) {
                                    game_state = create_game_state(game_memory, game_memory_size, &init_state);
                                }
                            }
                            Stream stream = stream_from_mem(network->buffer, network->buffer_size);
                            NetworkHeader ack_header = header_of_type(Type_AckGameState);
                            stream_header(&stream, &ack_header, Stream_Write);
                            send(network->socket, (char*) stream.memory, stream.ptr, 0);
                        }

                        if (header.type == Type_ServerInput && game_state) {
                            // TODO: Parse and apply server input
                            // ServerInput *server_input = (ServerInput*) buffer.buffer;
                            // handle_server_input(game_state, server_input);
                        }
                    } else {
                        break;
                    }
                }

                Stream stream = stream_from_mem(network->buffer, network->buffer_size);
                if (game_state) {
                    NetworkHeader header = header_of_type(Type_ClientInput);
                    stream_header(&stream, &header, Stream_Write);
                    stream_inputs(&stream, &inputs, Stream_Write);
                } else {
                    NetworkHeader header = header_of_type(Type_Ping);
                    stream_header(&stream, &header, Stream_Write);
                }
                send(network->socket, (char*) stream.memory, stream.ptr, 0);
            }
        }

        HDC device_context = GetDC(window);
        WindowDimension dimension = win32_get_window_dimensions(window);

        begin_command_buffer(&render_commands);
        if (game_state) {
            update_and_render(game_state, &render_commands, inputs, 1.0f / 60.0f);
        } else {
            render_loading_screen(&render_commands);
        }

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
        platform_log(buffer);
    }

    return 0;
}
