#pragma once
#include <glad/glad.h>

#include "common/core/typedefs.hpp"

/// @brief Configuration for creating a 2D GL texture image.
struct GLTextureConfig {
    u32         index;
    GLenum      target;
    i32         level;
    i32         internal_format;
    i32         width;
    i32         height;
    i32         border;
    GLenum      format;
    GLenum      type;
    const void* data;
};

/// @brief Configuration for one texture sampler parameter.
struct GLTexParamConfig {
    GLenum target;
    GLenum param;
    GLint  value;
};

/// @brief Configuration for uploading one raw 2D texture image.
struct GLTexImage2DConfig {
    GLenum      target;
    i32         level;
    i32         internal_format;
    i32         width;
    i32         height;
    i32         border;
    GLenum      format;
    GLenum      type;
    const void* data;
};

/// @brief Configuration for uploading data into a GL buffer object.
struct GLBufferConfig {
    u32        index;
    GLenum     target;
    GLsizeiptr size;
    const void* data;
    GLenum     usage;
};

/// @brief Configuration for uploading data into an already-bound GL buffer.
struct GLBufferDataConfig {
    GLenum     target;
    GLsizeiptr size;
    const void* data;
    GLenum     usage;
};

/// @brief Configuration for creating a GL buffer object.
struct GLGenBufferConfig {
    GLenum target;
    u32*   output;
};

/// @brief Configuration for one vertex attribute pointer.
struct GLVertexAttribPointerConfig {
    u32         index;
    i32         size;
    GLenum      type;
    GLboolean   normalized;
    GLsizei     stride;
    const void* pointer;
};

/// @brief Configuration for the per-instance attribute divisor.
struct GLVertexAttribDivisorConfig {
    u32  index;
    u32  divisor;
};

/// @brief Configuration for creating a GL vertex array object.
struct GLGenVertexArrayConfig {
    u32* output;
};

/// @brief Configuration for creating a GL shader source.
struct GLShaderConfig {
    GLenum      type;
    const char* source;
};

/// @brief Configuration for linking a GL program.
struct GLLinkProgramConfig {
    u32 program;
};

/// @brief Configuration for setting one float uniform.
struct GLUniform1fConfig {
    i32 location;
    f32 value;
};

/// @brief Configuration for setting two float uniforms.
struct GLUniform2fConfig {
    i32 location;
    f32 x;
    f32 y;
};

/// @brief Configuration for setting three float uniforms.
struct GLUniform3fConfig {
    i32 location;
    f32 x;
    f32 y;
    f32 z;
};

/// @brief Configuration for setting four float uniforms.
struct GLUniform4fConfig {
    i32 location;
    f32 x;
    f32 y;
    f32 z;
    f32 w;
};

/// @brief Configuration for setting one integer uniform.
struct GLUniform1iConfig {
    i32 location;
    i32 value;
};

/// @brief Configuration for setting a 4x4 matrix uniform.
struct GLUniformMatrix4Config {
    i32          location;
    GLboolean    transpose;
    const f32*   value;
};

/// @brief Configuration for setting the blend factors.
struct GLBlendFuncConfig {
    GLenum source_factor;
    GLenum destination_factor;
};

/// @brief Configuration for setting the GL viewport rectangle.
struct GLViewportConfig {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
};

/// @brief Configuration for setting the scissor rectangle.
struct GLScissorConfig {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
};

/// @brief Configuration for setting the depth function.
struct GLDepthFuncConfig {
    GLenum function;
};

/// @brief Configuration for clearing color + depth/stencil bits.
struct GLClearConfig {
    f32        red;
    f32        green;
    f32        blue;
    f32        alpha;
    GLbitfield mask;
};

/// @brief Configuration for a per-element draw call.
struct GLDrawElementsConfig {
    GLenum      mode;
    GLsizei     count;
    GLenum      type;
    const void* indices;
};

/// @brief Configuration for a per-vertex draw call.
struct GLDrawArraysConfig {
    GLenum  mode;
    GLint   first;
    GLsizei count;
};

// ---------------------------------------------------------------------------
// Wrapper declarations (definitions live in gl_helper.cpp).
// ---------------------------------------------------------------------------

void setup_gl_texture(const GLTextureConfig config);
void setup_gl_tex_param(const GLTexParamConfig config);
void setup_gl_texture_image(const GLTexImage2DConfig config);
void setup_gl_buffer(const GLBufferConfig config);
void gl_buffer_data(const GLBufferDataConfig config);
void gl_gen_buffer(const GLGenBufferConfig config);
void setup_gl_vertex_attrib_pointer(const GLVertexAttribPointerConfig config);
void setup_gl_vertex_attrib_divisor(const GLVertexAttribDivisorConfig config);
void gl_gen_vertex_array(const GLGenVertexArrayConfig config);
void gl_draw_elements(const GLDrawElementsConfig config);
void gl_draw_arrays(const GLDrawArraysConfig config);