/**
 * @file transpiler_stmt.cpp
 * @brief Emits statement nodes as GLSL text.
 */
#include "core/frontend/shader_parsing/transpiler/transpiler_internal.hpp"


/**
 * @brief Emit a single statement's body (without its own block braces).
 */
static void transpile_statement_inner(const Shader* shader, Emitter* em,
                                      u32 idx, StageKind stage);

/**
 * @brief Emit a call whose callee is a builtin stage output, or false.
 *
 * `vertex_output(x)` sets gl_Position and forwards varyings; `fragment_output(x)`
 * sets the fragment color. Returns true if `e` was such a call and was emitted.
 */
static bool transpile_builtin_call(const Shader* shader, Emitter* em,
                                   const Expr* e, StageKind stage)
{
    if (e->kind != ExprKind::Call) {
        return false;
    }
    if (e->a >= shader->exprs.size()) {
        return false;
    }
    const Expr* callee = &shader->exprs[e->a];
    if (callee->kind != ExprKind::Variable) {
        return false;
    }

    u32 arg_count = static_cast<u32>(std::stoul(e->text));
    std::string arg;
    if (arg_count >= 1) {
        arg = transpile_expr(shader, shader->expr_args[e->b]);
    }

    if (callee->text == "vertex_output") {
        // Position may be a Vec2 or Vec3; pad to vec4 in NDC space.
        emit_line(em, "gl_Position = uViewProj * vec4(" + arg + ", 0.0, 1.0);");
        return true;
    }
    if (callee->text == "fragment_output") {
        emit_line(em, "FragColor = " + arg + ";");
        return true;
    }
    return false;
}

/**
 * @brief Emit an expression statement, handling builtin output calls.
 */
static void transpile_expression_stmt(const Shader* shader, Emitter* em,
                                      u32 expr, StageKind stage)
{
    const Expr* e = &shader->exprs[expr];
    if (transpile_builtin_call(shader, em, e, stage)) {
        return;
    }
    emit_line(em, transpile_expr(shader, expr) + ";");
}

/**
 * @brief Emit a statement node (recursively for blocks and control flow).
 */
static void transpile_statement_inner(const Shader* shader, Emitter* em,
                                      u32 idx, StageKind stage)
{
    if (idx >= shader->stmts.size()) {
        return;
    }
    const Stmt* s = &shader->stmts[idx];

    switch (s->kind) {
    case StmtKind::Empty:
        emit_line(em, ";");
        break;

    case StmtKind::Expression:
        transpile_expression_stmt(shader, em, s->a, stage);
        break;

    case StmtKind::Variable: {
        std::string type = shader->var_types[s->a];
        std::string name = shader->var_names[s->a];
        std::string decl = transpile_type(type) + " " + name;
        if (s->b != 0) {
            decl += " = " + transpile_expr(shader, s->b);
        }
        emit_line(em, decl + ";");
        break;
    }

    case StmtKind::Return:
        if (s->a != 0) {
            emit_line(em, "return " + transpile_expr(shader, s->a) + ";");
        } else {
            emit_line(em, "return;");
        }
        break;

    case StmtKind::Block: {
        emit_line(em, "{");
        emit_indent(em);
        u32 start = s->a;
        u32 count = s->b;
        for (u32 i = 0; i < count; i++) {
            transpile_statement_inner(shader, em, shader->block_children[start + i], stage);
        }
        emit_dedent(em);
        emit_line(em, "}");
        break;
    }

    case StmtKind::If: {
        std::string head = "if (" + transpile_expr(shader, s->a) + ")";
        emit_line(em, head);
        transpile_statement_inner(shader, em, s->b, stage);
        if (s->c != 0) {
            emit_line(em, "else");
            transpile_statement_inner(shader, em, s->c, stage);
        }
        break;
    }

    case StmtKind::For: {
        // A C-style for loop. The init is a full statement (e.g. `int i = 0;`),
        // so it is emitted on its own line, followed by `for (; cond; step)`.
        if (s->a != 0) {
            transpile_statement_inner(shader, em, s->a, stage);
        }
        std::string cond = (s->b != 0) ? transpile_expr(shader, s->b) : "true";
        std::string step = (s->c != 0) ? transpile_expr(shader, s->c) : "";
        emit_line(em, "for (; " + cond + "; " + step + ")");
        transpile_statement_inner(shader, em, s->d, stage);
        break;
    }

    case StmtKind::While: {
        std::string cond = transpile_expr(shader, s->a);
        emit_line(em, "while (" + cond + ")");
        transpile_statement_inner(shader, em, s->b, stage);
        break;
    }

    default:
        break;
    }
}

/**
 * @brief Emit a statement (and its nested statements) to GLSL.
 */
void transpile_statement(const Shader* shader, Emitter* em, u32 stmt, StageKind stage)
{
    transpile_statement_inner(shader, em, stmt, stage);
}
