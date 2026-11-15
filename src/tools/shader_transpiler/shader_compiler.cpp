/**
 * @file shader_compiler.cpp
 * @brief Binds the lexer, parser and transpiler into a single compile step.
 */
#include "tools/shader_transpiler/shader_compiler.hpp"

#include "tools/shader_transpiler/lexer/lexer.hpp"
#include "tools/shader_transpiler/parser/parser.hpp"
#include "tools/shader_transpiler/transpiler/transpiler.hpp"


/**
 * @brief Compile `.shader` source text into GLSL vertex/fragment strings.
 */
std::expected<CompiledShader, CompileFailure> ShaderCompiler::compile(const char* source)
{
    if (source == nullptr || source[0] == '\0') {
        return std::unexpected(CompileFailure{
            .error = CompileError::NoSource,
            .message = "source is null or empty",
            .line = 0,
            .column = 0,
        });
    }

    // 1. Lex.
    std::expected<std::vector<Token>, LexerError> lexed =
        Lexer::tokenise({.source = source});
    if (!lexed.has_value()) {
        LexerError e = lexed.error();
        const char* what = (e == LexerError::UnexpectedChar)
            ? "unexpected character"
            : (e == LexerError::UnterminatedComment)
                ? "unterminated block comment"
                : "unterminated string literal";
        return std::unexpected(CompileFailure{
            .error = CompileError::LexError,
            .message = what,
            .line = 1,
            .column = 1,
        });
    }

    // 2. Parse.
    std::expected<Shader, ParseFailure> parsed =
        Parser::parse({.tokens = &*lexed});
    if (!parsed.has_value()) {
        return std::unexpected(CompileFailure{
            .error = CompileError::ParseError,
            .message = parsed.error().message,
            .line = parsed.error().line,
            .column = parsed.error().column,
        });
    }

    // 3. Transpile.
    std::expected<TranspileResult, TranspileError> transpiled =
        Transpiler::transpile(&*parsed);
    if (!transpiled.has_value()) {
        return std::unexpected(CompileFailure{
            .error = CompileError::TranspileError,
            .message = "transpilation failed",
            .line = 0,
            .column = 0,
        });
    }

    CompiledShader result;
    result.vertex_source = transpiled->vertex_source;
    result.fragment_source = transpiled->fragment_source;
    return result;
}
