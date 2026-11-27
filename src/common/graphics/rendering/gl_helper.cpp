#include <cassert>

#include "gl_helper.hpp"

/// @brief Uploads a 2D texture image and applies repeat/linear sampling.
/// @param config The texture name, target, format and pixel data.
void setup_gl_texture(const GLTextureConfig config)
{
    glBindTexture(config.target, config.index);
    glTexImage2D(config.target, config.level, config.internal_format, config.width,
        config.height, config.border, config.format, config.type, config.data);
    glTexParameteri(config.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(config.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(config.target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(config.target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(config.target, 0);
}

/// @brief Sets one texture sampler parameter.
/// @param config The texture target, parameter and value.
void setup_gl_tex_param(const GLTexParamConfig config)
{
    glTexParameteri(config.target, config.param, config.value);
}

/// @brief Uploads one raw 2D texture image without touching sampler state.
/// @param config The texture target, level, format and pixel data.
void setup_gl_texture_image(const GLTexImage2DConfig config)
{
    glTexImage2D(config.target, config.level, config.internal_format, config.width,
        config.height, config.border, config.format, config.type, config.data);
}

/// @brief Uploads data into a GL buffer object.
/// @param config The buffer name, target, size, data and usage hint.
void setup_gl_buffer(const GLBufferConfig config)
{
    glBindBuffer(config.target, config.index);
    glBufferData(config.target, config.size, config.data, config.usage);
}

/// @brief Uploads data into an already-bound GL buffer object.
/// @param config The buffer target, size, data and usage hint.
void gl_buffer_data(const GLBufferDataConfig config)
{
    glBufferData(config.target, config.size, config.data, config.usage);
}

/// @brief Creates one GL buffer object.
/// @param config The buffer target and output array receiving the name.
/// @warn The output pointer must address a live u32 to receive the name.
void gl_gen_buffer(const GLGenBufferConfig config)
{
    assert(config.output != nullptr);
    glGenBuffers(1, config.output);
    glBindBuffer(config.target, *config.output);
}

/// @brief Enables and describes one vertex attribute pointer.
/// @param config The attribute location, component layout and offset.
void setup_gl_vertex_attrib_pointer(const GLVertexAttribPointerConfig config)
{
    glEnableVertexAttribArray(config.index);
    glVertexAttribPointer(config.index, config.size, config.type,
        config.normalized, config.stride, config.pointer);
}

/// @brief Sets the per-instance divisor for one attribute.
/// @param config The attribute location and divisor.
void setup_gl_vertex_attrib_divisor(const GLVertexAttribDivisorConfig config)
{
    glVertexAttribDivisor(config.index, config.divisor);
}

/// @brief Creates one GL vertex array object.
/// @param config The output array receiving the VAO name.
/// @warn The output pointer must address a live u32 to receive the name.
void gl_gen_vertex_array(const GLGenVertexArrayConfig config)
{
    assert(config.output != nullptr);
    glGenVertexArrays(1, config.output);
    glBindVertexArray(*config.output);
}

/// @brief Compiles a single GL shader from source.
/// @param config The shader stage and source text.
/// @return The compiled shader name, or 0 if compilation failed.
static u32 gl_compile_shader(const GLShaderConfig config)
{
    assert(config.source != nullptr);
    const u32 shader = glCreateShader(config.type);
    glShaderSource(shader, 1, &config.source, nullptr);
    glCompileShader(shader);
    return shader;
}

/// @brief Links a GL program from its attached shaders.
/// @param config The program name to link.
static void gl_link_program(const GLLinkProgramConfig config)
{
    glLinkProgram(config.program);
}

/// @brief Sets one float uniform.
/// @param config The uniform location and value.
static void gl_uniform_1f(const GLUniform1fConfig config)
{
    glUniform1f(config.location, config.value);
}

/// @brief Sets two float uniforms.
/// @param config The uniform location and values.
static void gl_uniform_2f(const GLUniform2fConfig config)
{
    glUniform2f(config.location, config.x, config.y);
}

/// @brief Sets three float uniforms.
/// @param config The uniform location and values.
static void gl_uniform_3f(const GLUniform3fConfig config)
{
    glUniform3f(config.location, config.x, config.y, config.z);
}

/// @brief Sets four float uniforms.
/// @param config The uniform location and values.
static void gl_uniform_4f(const GLUniform4fConfig config)
{
    glUniform4f(config.location, config.x, config.y, config.z, config.w);
}

/// @brief Sets one integer uniform.
/// @param config The uniform location and value.
static void gl_uniform_1i(const GLUniform1iConfig config)
{
    glUniform1i(config.location, config.value);
}

/// @brief Sets a 4x4 matrix uniform.
/// @param config The uniform location, transpose flag and matrix data.
/// @warn The value pointer must address 16 live floats.
static void gl_uniform_matrix_4(const GLUniformMatrix4Config config)
{
    assert(config.value != nullptr);
    glUniformMatrix4fv(config.location, 1, config.transpose, config.value);
}

/// @brief Sets the blend source and destination factors.
/// @param config The source and destination blend factors.
static void gl_blend_func(const GLBlendFuncConfig config)
{
    glBlendFunc(config.source_factor, config.destination_factor);
}

/// @brief Sets the GL viewport rectangle.
/// @param config The viewport origin and size.
static void gl_viewport(const GLViewportConfig config)
{
    glViewport(config.x, config.y, config.width, config.height);
}

/// @brief Sets the scissor test rectangle.
/// @param config The scissor origin and size.
static void gl_scissor(const GLScissorConfig config)
{
    glScissor(config.x, config.y, config.width, config.height);
}

/// @brief Sets the depth comparison function.
/// @param config The depth comparison function.
static void gl_depth_func(const GLDepthFuncConfig config)
{
    glDepthFunc(config.function);
}

/// @brief Clears the current buffer to a solid color.
/// @param config The clear color and buffer bit mask.
static void gl_clear(const GLClearConfig config)
{
    glClearColor(config.red, config.green, config.blue, config.alpha);
    glClear(config.mask);
}

/// @brief Draws indexed geometry.
/// @param config The draw mode, element count, index type and index offset.
void gl_draw_elements(const GLDrawElementsConfig config)
{
    glDrawElements(config.mode, config.count, config.type, config.indices);
}

/// @brief Draws non-indexed geometry.
/// @param config The draw mode, first vertex and vertex count.
void gl_draw_arrays(const GLDrawArraysConfig config)
{
    glDrawArrays(config.mode, config.first, config.count);
}