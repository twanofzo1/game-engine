/**
 * @file transpiler.cpp
 * @brief Transpiler entry point and per-stage source generation.
 */
#include "core/frontend/shader_parsing/transpiler/transpiler_internal.hpp"


/** @brief The GLSL version line shared by both stages. */
static const char* GLSL_VERSION = "#version 330 core\n";


/**
 * @brief Emit the default vertex-shader header (version, attributes,
 * view-projection uniform, varyings). Custom uniforms are appended by the
 * caller, and the entry-point body replaces the default `main` below.
 */
static void emit_vertex_header(Emitter* em)
{
    emit_raw(em, GLSL_VERSION);
    emit_raw(em, "layout(location = 0) in vec3 aPos;");
    emit_raw(em, "layout(location = 1) in vec2 aUV;");
    emit_raw(em, "layout(location = 2) in vec4 aColor;");
    emit_raw(em, "uniform mat4 uViewProj;");
    emit_raw(em, "out vec2 vUV;");
    emit_raw(em, "out vec4 vColor;");
}

/**
 * @brief Emit the default fragment-shader header (version, varyings, texture
 * sampler, output). Custom uniforms are appended by the caller.
 */
static void emit_fragment_header(Emitter* em)
{
    emit_raw(em, GLSL_VERSION);
    emit_raw(em, "in vec2 vUV;");
    emit_raw(em, "in vec4 vColor;");
    emit_raw(em, "uniform sampler2D uTexture;");
    emit_raw(em, "out vec4 FragColor;");
}

/**
 * @brief Find the index of the entry point with the given stage.
 * @return The function index, or nullopt if none exists.
 */
static std::optional<u32> find_entry(const Shader* shader, Stage stage)
{
    for (u32 i = 0; i < shader->fn_stages.size(); i++) {
        if (shader->fn_stages[i] == stage) {
            return i;
        }
    }
    return std::nullopt;
}

/**
 * @brief Emit the uniforms into the shader's global scope.
 */
void transpile_uniforms(const Shader* shader, Emitter* em)
{
    for (const Decl* d = &shader->decls[0];
         d < &shader->decls[0] + shader->decls.size(); ++d) {
        if (d->kind == DeclKind::Uniform) {
            emit_raw(em, "uniform " + transpile_type(d->type) + " " + d->name + ";");
        }
    }
    emit_raw(em, "");
}

/**
 * @brief Emit the complete vertex shader.
 */
std::string transpile_vertex(const Shader* shader)
{
    Emitter em;
    emit_vertex_header(&em);

    std::string u;
    {
        Emitter ue;
        transpile_uniforms(shader, &ue);
        u = ue.out;
    }
    em.out += u;

    std::optional<u32> entry = find_entry(shader, Stage::Vertex);
    if (!entry.has_value()) {
        // Default main when no Vertex entry point is supplied.
        em.out +=
            "void main() {\n"
            "    vUV = aUV;\n"
            "    vColor = aColor;\n"
            "    gl_Position = uViewProj * vec4(aPos, 1.0);\n"
            "}\n";
        return em.out;
    }

    // Entry point signature: map parameters onto the vertex attributes.
    em.out += "void main() {\n";
    emit_indent(&em);
    u32 param_start = shader->fn_param_start[*entry];
    u32 param_count = shader->fn_param_count[*entry];
    for (u32 i = 0; i < param_count; i++) {
        const Param* p = &shader->params[param_start + i];
        std::string glsl_type = transpile_type(p->type);
        // First parameter binds to the 3D position attribute; 2D/3D target
        // types swizzle accordingly. Subsequent parameters bind to the 2D UV
        // attribute.
        if (i == 0) {
            std::string init = (glsl_type == "vec2") ? "aPos.xy"
                             : (glsl_type == "vec3") ? "aPos"
                             : "aPos";
            emit_raw(&em, std::string("    ") + glsl_type + " " + p->name + " = " + init + ";\n");
        } else {
            emit_raw(&em, std::string("    ") + glsl_type + " " + p->name + " = aUV;\n");
        }
    }
    emit_dedent(&em);

    // Forward the varyings by default so the fragment shader receives real
    // interpolated UVs and vertex colours. The user body can override these.
    emit_raw(&em, "    vUV = aUV;\n");
    emit_raw(&em, "    vColor = aColor;\n");

    // Emit the user body (usually a block).
    Emitter body;
    transpile_statement(shader, &body, shader->fn_body[*entry], StageKind::Vertex);
    em.out += body.out;

    em.out += "}\n";
    return em.out;
}

/**
 * @brief Emit the complete fragment shader.
 */
std::string transpile_fragment(const Shader* shader)
{
    Emitter em;
    emit_fragment_header(&em);

    std::string u;
    {
        Emitter ue;
        transpile_uniforms(shader, &ue);
        u = ue.out;
    }
    em.out += u;

    std::optional<u32> entry = find_entry(shader, Stage::Fragment);
    if (!entry.has_value()) {
        em.out +=
            "void main() {\n"
            "    FragColor = vColor * texture(uTexture, vUV);\n"
            "}\n";
        return em.out;
    }

    em.out += "void main() {\n";
    emit_indent(&em);
    u32 param_start = shader->fn_param_start[*entry];
    u32 param_count = shader->fn_param_count[*entry];
    for (u32 i = 0; i < param_count; i++) {
        const Param* p = &shader->params[param_start + i];
        emit_raw(&em, std::string("    ") + transpile_type(p->type) + " " + p->name + " = vUV;\n");
    }
    emit_dedent(&em);

    Emitter body;
    transpile_statement(shader, &body, shader->fn_body[*entry], StageKind::Fragment);
    em.out += body.out;

    em.out += "}\n";
    return em.out;
}

/** @brief Transpile the parsed shader into GLSL vertex/fragment sources. */
std::expected<TranspileResult, TranspileError> Transpiler::transpile(const Shader* shader)
{
    if (shader == nullptr) {
        return std::unexpected(TranspileError::InternalError);
    }

    TranspileResult result;
    result.vertex_source = transpile_vertex(shader);
    result.fragment_source = transpile_fragment(shader);
    return result;
}
