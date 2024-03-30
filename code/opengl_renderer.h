#ifndef OPENGL_RENDERER_H
#define OPENGL_RENDERER_H

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_ARRAY_BUFFER 0x8892
#define GL_STREAM_DRAW 0x88E0

typedef char GLchar;
typedef i64 GLsizeiptr;

typedef GLuint (*GL_CREATE_SHADER)(GLenum);
typedef void (*GL_SHADER_SOURCE)(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void (*GL_COMPILE_SHADER)(GLuint shader);
typedef void (*GL_GET_SHADERIV)(GLuint shader, GLenum pname, GLint *params);
typedef void (*GL_GET_SHADER_INFO_LOG)(GLuint shader, GLsizei size, GLsizei *length, GLchar *info);
typedef GLuint (*GL_CREATE_PROGRAM)(void);
typedef void (*GL_ATTACH_SHADER)(GLuint program, GLuint shader);
typedef void (*GL_LINK_PROGRAM)(GLuint program);
typedef void (*GL_GET_PROGRAMIV)(GLuint program, GLenum pname, GLint *params);
typedef void (*GL_GET_PROGRAM_INFO_LOG)(GLuint program, GLsizei size, GLsizei *length, GLchar *info);
typedef void (*GL_DELETE_SHADER)(GLuint shader);
typedef GLint (*GL_GET_UNIFORM_LOCATION)(GLuint program, const GLchar *name);
typedef void (*GL_UNIFORM_MATRIX4FV)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (*GL_GEN_VERTEX_ARRAYS)(GLsizei n, GLuint *arrays);
typedef void (*GL_BIND_VERTEX_ARRAY)(GLuint array);
typedef void (*GL_GEN_BUFFERS)(GLsizei n, GLuint *buffers);
typedef void (*GL_BIND_BUFFER)(GLenum target, GLuint buffer);
typedef void (*GL_ENABLE_VERTEX_ATTRIB_ARRAY)(GLuint index);
typedef void (*GL_VERTEX_ATTRIB_POINTER)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void (*GL_USE_PROGRAM)(GLuint program);
typedef void (*GL_BUFFER_DATA)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (*GL_MULTI_DRAW_ARRAYS)(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);

GL_CREATE_SHADER glCreateShader;
GL_SHADER_SOURCE glShaderSource;
GL_COMPILE_SHADER glCompileShader;
GL_GET_SHADERIV glGetShaderiv;
GL_GET_SHADER_INFO_LOG glGetShaderInfoLog;
GL_CREATE_PROGRAM glCreateProgram;
GL_ATTACH_SHADER glAttachShader;
GL_LINK_PROGRAM glLinkProgram;
GL_GET_PROGRAMIV glGetProgramiv;
GL_GET_SHADER_INFO_LOG glGetProgramInfoLog;
GL_DELETE_SHADER glDeleteShader;
GL_GET_UNIFORM_LOCATION glGetUniformLocation;
GL_UNIFORM_MATRIX4FV glUniformMatrix4fv;
GL_GEN_VERTEX_ARRAYS glGenVertexArrays;
GL_BIND_VERTEX_ARRAY glBindVertexArray;
GL_GEN_BUFFERS glGenBuffers;
GL_BIND_BUFFER glBindBuffer;
GL_ENABLE_VERTEX_ATTRIB_ARRAY glEnableVertexAttribArray;
GL_VERTEX_ATTRIB_POINTER glVertexAttribPointer;
GL_USE_PROGRAM glUseProgram;
GL_BUFFER_DATA glBufferData;
GL_MULTI_DRAW_ARRAYS glMultiDrawArrays;

#endif
