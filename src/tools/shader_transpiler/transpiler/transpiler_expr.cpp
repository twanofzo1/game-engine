/**
 * @file transpiler_expr.cpp
 * @brief Emits expression nodes as GLSL text.
 */
#include "tools/shader_transpiler/transpiler/transpiler_internal.hpp"


/**
 * @brief Emit an expression node to GLSL text (fully infix).
 */
std::string transpile_expr(const Shader* shader, u32 idx)
{
    if (idx >= shader->exprs.size()) {
        return "";
    }
    const Expr* e = &shader->exprs[idx];

    switch (e->kind) {
    case ExprKind::Int:
    case ExprKind::Float:
    case ExprKind::Bool:
        return e->text;

    case ExprKind::Variable:
        // Variable references also carry type names when used as constructor
        // callees (e.g. `Vec4(...)`); lowercase names pass through unchanged.
        return transpile_type(e->text);

    case ExprKind::Unary:
        return e->text + "(" + transpile_expr(shader, e->a) + ")";

    case ExprKind::Binary:
        return "(" + transpile_expr(shader, e->a)
             + " " + e->text + " "
             + transpile_expr(shader, e->b) + ")";

    case ExprKind::Assign: {
        std::string lhs = transpile_expr(shader, e->a);
        std::string rhs = transpile_expr(shader, e->b);
        return "(" + lhs + " " + e->text + " " + rhs + ")";
    }

    case ExprKind::Ternary:
        return "(" + transpile_expr(shader, e->a)
             + " ? " + transpile_expr(shader, e->b)
             + " : " + transpile_expr(shader, e->c) + ")";

    case ExprKind::Member:
        return transpile_expr(shader, e->a) + "." + e->text;

    case ExprKind::Call: {
        // Callee in `a`; arguments in expr_args[b .. b+count].
        std::string callee = transpile_expr(shader, e->a);
        u32 arg_count = static_cast<u32>(std::stoul(e->text));
        std::string args;
        for (u32 i = 0; i < arg_count; i++) {
            u32 arg_idx = shader->expr_args[e->b + i];
            if (i > 0) {
                args += ", ";
            }
            args += transpile_expr(shader, arg_idx);
        }
        return callee + "(" + args + ")";
    }

    default:
        return "";
    }
}
