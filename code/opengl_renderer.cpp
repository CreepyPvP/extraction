#include "opengl_renderer.h"


struct OpenGLProgram
{
    u32 id;
    u32 proj;
};

struct OpenGL
{
    OpenGLProgram prim_shader;

    // NOTE: Used for gl multidraw
    u32 max_multidraw_prim;
    i32* first_buffer;
    i32* count_buffer;
};

OpenGL opengl;

static OpenGLProgram load_program(const char* vertex_file, const char* frag_file, Arena* arena)
{
    TempMemoryRegion temp_memory = arena->start_temp();

    char info_log[512];
    i32 status;

    char *vert_src = platform_load_file(vertex_file, arena, NULL);
    u32 vertex_prog = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_prog, 1, &vert_src, NULL);
    glCompileShader(vertex_prog);
    glGetShaderiv(vertex_prog, GL_COMPILE_STATUS, &status);
    if (!status) {
        glGetShaderInfoLog(vertex_prog, 512, NULL, info_log);
        printf("Error compiling vertex shader: %s\n", info_log);
        assert(0);
    }

    char *frag_src = platform_load_file(frag_file, arena, NULL);
    u32 frag_prog = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_prog, 1, &frag_src, NULL);
    glCompileShader(frag_prog);
    glGetShaderiv(frag_prog, GL_COMPILE_STATUS, &status);
    if(!status) {
        glGetShaderInfoLog(frag_prog, 512, NULL, info_log);
        printf("Error compiling fragment shader: %s\n", info_log);
        assert(0);
    }

    OpenGLProgram shader;
    shader.id = glCreateProgram();
    glAttachShader(shader.id, vertex_prog);
    glAttachShader(shader.id, frag_prog);
    glLinkProgram(shader.id);
    glGetProgramiv(shader.id, GL_LINK_STATUS, &status);
    if(!status) {
        glGetProgramInfoLog(shader.id, 512, NULL, info_log);
        printf("Error linking shader: %s\n", info_log);
        assert(0);
    }

    glDeleteShader(vertex_prog);
    glDeleteShader(frag_prog);

    shader.proj = glGetUniformLocation(shader.id, "proj");
    
    temp_memory.forget();

    return shader;
}

static void set_mat4(u32 id, Mat4* mat, u32 count)
{
    glUniformMatrix4fv(id, count, GL_FALSE, (GLfloat*) mat);
}

void opengl_init_renderer(Arena *arena)
{
    opengl.max_multidraw_prim = 10000;
    opengl.first_buffer = push_arr(arena, i32, opengl.max_multidraw_prim);
    opengl.count_buffer = push_arr(arena, i32, opengl.max_multidraw_prim);

    u32 prim_arr;
    glGenVertexArrays(1, &prim_arr);
    glBindVertexArray(prim_arr);

    u32 prim_buf;
    glGenBuffers(1, &prim_buf);
    glBindBuffer(GL_ARRAY_BUFFER, prim_buf);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

    opengl.prim_shader = load_program("shader/prim.vert", "shader/prim.frag", arena);
    glUseProgram(opengl.prim_shader.id);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void opengl_process_commands(CommandBuffer *commands, WindowDimension dimension)
{
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * commands->vert_count, commands->vertices, 
                 GL_STREAM_DRAW);
    set_mat4(opengl.prim_shader.proj, &commands->basis.transform, 1);
    glViewport(0, 0, dimension.width, dimension.height);

    u32 offset = 0;
    while (offset < commands->curr) {
        CommandEntry *type = (CommandEntry*) (commands->ptr + offset);
        switch (*type) {

            case Entry_Clear: {
                CmdClear* clear = (CmdClear*) type;
                offset += sizeof(*clear);
                glClearColor(clear->color.r, clear->color.g, clear->color.b, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            } break;

            case Entry_DrawQuads: {
                CmdDrawQuads* draw = (CmdDrawQuads*) type;
                offset += sizeof(*draw);

                assert(draw->quad_count <= opengl.max_multidraw_prim);
                for (u32 i = 0; i < draw->quad_count; ++i) {
                    opengl.first_buffer[i] = draw->vert_offset + 4 * i;
                    opengl.count_buffer[i] = 4;
                }

                glMultiDrawArrays(GL_TRIANGLE_STRIP, opengl.first_buffer, opengl.count_buffer, draw->quad_count);
            } break;

            default: {
                assert(0 && "Unknown type");
            }
        }
    }
}
