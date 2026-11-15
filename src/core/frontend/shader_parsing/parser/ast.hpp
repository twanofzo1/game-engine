/**
 * @file ast.hpp
 * @brief Abstract syntax tree for the shader language.
 *
 * Nodes reference one another by index into parallel vectors held on the
 * owning Shader. This avoids pointers-to-base, polymorphism, and std::variant
 * (none of which are permitted by the project conventions) while still
 * sharing storage between nodes of the same kind.
 */
#pragma once

#include "core/typedefs.hpp"

#include <string>
#include <vector>


/** @brief Kind of an expression node. */
enum class ExprKind : u8 {
    Int,        ///< Integer literal.
    Float,      ///< Floating-point literal.
    Bool,       ///< `true` / `false`.
    Variable,   ///< Reference to a named variable.
    Call,       ///< Function/constructor call.
    Member,     ///< Field access (`a.b`).
    Unary,      ///< Prefix unary operator applied to one operand.
    Binary,     ///< Infix binary operator applied to two operands.
    Assign,     ///< Assignment (plain or compound).
    Ternary,    ///< `cond ? a : b`.
};

/** @brief Kind of a statement node. */
enum class StmtKind : u8 {
    Empty,      ///< A lone semicolon.
    Expression, ///< An expression evaluated for effect.
    Variable,   ///< A local variable declaration with optional initialiser.
    Return,     ///< `return [expr];`.
    Block,      ///< A `{ ... }` compound statement.
    If,         ///< `if / else if / else`.
    For,        ///< `for (init; cond; step) body`.
    While,      ///< `while (cond) body`.
};

/** @brief Kind of a top-level declaration. */
enum class DeclKind : u8 {
    Uniform,   ///< A global `uniform Type name;` declaration.
    Function,  ///< A stage or helper function definition.
    Include,   ///< A `#include "..."` directive.
};

/** @brief Which entry-point stage a function represents. */
enum class Stage : u8 {
    None,      ///< A plain helper function.
    Vertex,    ///< The `Vertex(...)` entry point.
    Fragment,  ///< The `Fragment(...)` entry point.
};


/** @brief Storage for a single expression node. All children are indices. */
struct Expr {
    ExprKind kind;       ///< Node kind.
    std::string text;    ///< Literal text / variable name / operator symbol.
    u32 a;               ///< First child index (var name/id, lhs, cond...).
    u32 b;               ///< Second child index (rhs, then/else...).
    u32 c;               ///< Third child index (ternary else).
};

/** @brief Storage for a single statement node. */
struct Stmt {
    StmtKind kind;       ///< Node kind.
    u32 a;               ///< First child (expression, var decl, or stmt).
    u32 b;               ///< Second child (block, else, init...).
    u32 c;               ///< Third child (step, body...).
    u32 d;               ///< Fourth child (else-if chain).
};

/** @brief Storage for a single top-level declaration. */
struct Decl {
    DeclKind kind;       ///< Declaration kind.
    std::string name;    ///< Uniform/function/include name.
    std::string type;    ///< Type name for uniforms and parameters.
    Stage stage;         ///< Entry-point stage (or None).
    u32 a;               ///< First child (body statement / nothing).
    u32 b;               ///< Second child (return type expr index).
};

/** @brief A single function parameter. */
struct Param {
    std::string type;    ///< Parameter type name.
    std::string name;    ///< Parameter variable name.
};

/**
 * @brief The parsed shader: a collection of declarations plus the storage
 * vectors they index into.
 */
struct Shader {
    std::vector<Decl>  decls;    ///< Top-level declarations in source order.
    std::vector<Expr>  exprs;    ///< Expression nodes.
    std::vector<Stmt>  stmts;    ///< Statement nodes.
    std::vector<Param> params;   ///< Function parameter lists (grouped per fn).

    // Call arguments: flatten list of expression indices, grouped per call.
    std::vector<u32> expr_args;  ///< Call/constructor argument expression indices.

    // Block children: flatten list of statement indices, grouped per block.
    std::vector<u32> block_children; ///< Statement indices inside each block.

    // Local variable declarations (parallel arrays, keyed by Stmt index a).
    std::vector<std::string> var_names;  ///< Declared variable names.
    std::vector<std::string> var_types;  ///< Declared variable type names.

    // Per-function metadata (parallel to decls of kind Function).
    std::vector<std::string> fn_names;      ///< Function name.
    std::vector<std::string> fn_types;      ///< Function return type name.
    std::vector<Stage>       fn_stages;     ///< Function stage.
    std::vector<u32>         fn_param_start;///< Index into params[].
    std::vector<u32>         fn_param_count;///< Number of parameters.
    std::vector<u32>         fn_body;       ///< Index into stmts[] (body block).
};
