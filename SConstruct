# 2D game engine — recursive SCons build.
#
# Recursively compiles everything under src/** and pulls in the vendored
# third-party sources from external/ (GLFW, GLAD, Dear ImGui, yaml-cpp,
# stb, dr_libs, miniaudio).
#
# Usage:
#   scons                 # release build -> build/release/app
#   scons build=debug     # debug build   -> build/debug/app
#   scons -c              # clean the default (release) build
#   scons build=debug -c  # clean the debug build

import os
import platform
import sys
import glob

# ---------------------------------------------------------------------------
# Command-line options
# ---------------------------------------------------------------------------
build_type = ARGUMENTS.get("build", "release")
if build_type not in ("debug", "release"):
    print("unknown build type '%s' (expected 'debug' or 'release')" % build_type)
    Exit(1)

debug = build_type == "debug"

# ---------------------------------------------------------------------------
# Environment
# ---------------------------------------------------------------------------
env = Environment(tools=["default"], ENV=os.environ)

# Compile flags common to every TU.
env.Append(CPPPATH=[
    "src",
    "external/glad/include",
    "external/glfw/include",
    "external/imgui",
    "external/imgui/backends",
    "external/yaml-cpp/include",
    "external/stb",
    "external/dr_libs",
    "external/miniaudio",
])

env.Append(CPPDEFINES=[
    "YAML_CPP_STATIC_DEFINE",
])

# C and C++ standards used across the codebase and vendored libs.
env.Append(CXXFLAGS=["-std=c++23"])
env.Append(CFLAGS=["-std=c11"])

# Platform-specific flags/libraries.
system = platform.system()
if system == "Linux":
    env.Append(LIBS=["GL", "dl", "pthread", "m"])
    env.Append(LINKFLAGS=["-pthread"])
elif system == "Darwin":
    env.Append(FRAMEWORKS=["Cocoa", "IOKit", "CoreVideo", "OpenGL"])
else:
    # Windows / MinGW fallback.
    env.Append(LIBS=["opengl32", "gdi32", "user32", "shell32"])
    env.Append(LINKFLAGS=["-static"])

if debug:
    env.Append(CCFLAGS=["-g", "-O0", "-DDEBUG"])
    env.Append(LINKFLAGS=["-g"])
else:
    env.Append(CCFLAGS=["-O2", "-DNDEBUG"])

# ---------------------------------------------------------------------------
# Source collection
# ---------------------------------------------------------------------------

# Source collection, split by executable.
# - common/  : engine core shared by BOTH editor and runtime.
# - tools/   : the shader_parsing toolchain (lexer/parser/transpiler); also
#              shared, pulled into whichever binary needs it.
# - editor/  : the windowed YAML editor entrypoint.
# - runtime/ : the game runtime entrypoint (loads YAML, runs the game).
# Python's glob with recursive=True walks every nested subdirectory, unlike
# SCons' Glob() whose `**` only matches a single directory level.
common_sources = sorted(glob.glob("src/common/**/*.cpp", recursive=True))
tool_sources   = sorted(glob.glob("src/tools/**/*.cpp", recursive=True))
editor_main    = sorted(glob.glob("src/editor/**/*.cpp", recursive=True))
runtime_main   = sorted(glob.glob("src/runtime/**/*.cpp", recursive=True))

# Dear ImGui core + the GLFW/OpenGL3 backends used by the engine.
imgui_sources = [
    "external/imgui/imgui.cpp",
    "external/imgui/imgui_draw.cpp",
    "external/imgui/imgui_tables.cpp",
    "external/imgui/imgui_widgets.cpp",
    "external/imgui/imgui_demo.cpp",
    "external/imgui/backends/imgui_impl_glfw.cpp",
    "external/imgui/backends/imgui_impl_opengl3.cpp",
]

# GLFW platform sources for the current OS.
if system == "Linux":
    glfw_sources = [
        "external/glfw/src/context.c",
        "external/glfw/src/init.c",
        "external/glfw/src/input.c",
        "external/glfw/src/monitor.c",
        "external/glfw/src/vulkan.c",
        "external/glfw/src/window.c",
        "external/glfw/src/egl_context.c",
        "external/glfw/src/osmesa_context.c",
        "external/glfw/src/posix_thread.c",
        "external/glfw/src/posix_time.c",
        "external/glfw/src/glx_context.c",
        "external/glfw/src/linux_joystick.c",
        "external/glfw/src/x11_init.c",
        "external/glfw/src/x11_monitor.c",
        "external/glfw/src/x11_window.c",
        "external/glfw/src/xkb_unicode.c",
    ]
    env.Append(CPPDEFINES=["_GLFW_X11"])
    env.Append(LIBS=["X11", "Xrandr", "Xinerama", "Xcursor", "Xi"])
elif system == "Darwin":
    glfw_sources = [
        "external/glfw/src/context.c",
        "external/glfw/src/init.c",
        "external/glfw/src/input.c",
        "external/glfw/src/monitor.c",
        "external/glfw/src/vulkan.c",
        "external/glfw/src/window.c",
        "external/glfw/src/egl_context.c",
        "external/glfw/src/osmesa_context.c",
        "external/glfw/src/posix_thread.c",
        "external/glfw/src/cocoa_time.c",
        "external/glfw/src/cocoa_init.m",
        "external/glfw/src/cocoa_joystick.m",
        "external/glfw/src/cocoa_monitor.m",
        "external/glfw/src/cocoa_window.m",
        "external/glfw/src/nsgl_context.m",
    ]
    env.Append(CPPDEFINES=["_GLFW_COCOA"])
else:
    glfw_sources = [
        "external/glfw/src/context.c",
        "external/glfw/src/init.c",
        "external/glfw/src/input.c",
        "external/glfw/src/monitor.c",
        "external/glfw/src/vulkan.c",
        "external/glfw/src/window.c",
        "external/glfw/src/egl_context.c",
        "external/glfw/src/osmesa_context.c",
        "external/glfw/src/win32_thread.c",
        "external/glfw/src/win32_time.c",
        "external/glfw/src/win32_init.c",
        "external/glfw/src/win32_joystick.c",
        "external/glfw/src/win32_monitor.c",
        "external/glfw/src/win32_window.c",
        "external/glfw/src/wgl_context.c",
    ]
    env.Append(CPPDEFINES=["_GLFW_WIN32"])

# GLAD (OpenGL loader).
glad_sources = ["external/glad/src/glad.c"]

# yaml-cpp (all its .cpp files).
yaml_sources = Glob("external/yaml-cpp/src/*.cpp")

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
build_dir = "build/%s" % build_type    # object files (.o) land here
bin_dir = "bin/%s" % build_type        # the executable lands here

# Duplicate the source tree into build_dir so that every .o file is emitted
# under build/<build_type>/ instead of next to its source file. duplicate=0
# uses symlinks on platforms that support them (no full source copy).
variant_env = env.Clone()
variant_env.VariantDir(build_dir, ".", duplicate=0)

# Sources, re-rooted under the build directory so compile outputs follow suit.
def build_path(path):
    return os.path.join(build_dir, path)

def build_paths(paths):
    return [build_path(p) for p in paths]

build_common = build_paths(common_sources)
build_tools  = build_paths(tool_sources)
build_editor_main = build_paths(editor_main)
build_runtime_main = build_paths(runtime_main)
build_glad = build_paths(glad_sources)
build_imgui = build_paths(imgui_sources)
build_glfw = build_paths(glfw_sources)
# yaml_sources are Node objects from Glob(); map their string paths.
build_yaml = build_paths([str(s) for s in yaml_sources])

# Third-party + shared engine sources, common to every executable.
vendor_sources = build_glad + build_imgui + build_glfw + build_yaml
common_engine_sources = build_common + vendor_sources

# --- Editor executable (windowed YAML editor) -------------------------------
editor = env.Program(
    target=os.path.join(bin_dir, "editor"),
    source=build_editor_main + build_tools + common_engine_sources,
)

# --- Runtime executable (loads YAML, runs the game) -------------------------
runtime = env.Program(
    target=os.path.join(bin_dir, "runtime"),
    source=build_runtime_main + build_tools + common_engine_sources,
)

Default(editor, runtime)


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
# Test binaries link only against the lexer/parser/transpiler/compiler
# translation units (no GL/GLFW/imgui), so they stay small.
# Build and run with:  scons test
# Each test binary builds on the previous stage, so the source sets are
# cumulative (lexer -> parser -> transpiler -> compiler).
lexer_sources      = sorted(glob.glob("src/tools/shader_transpiler/lexer/*.cpp", recursive=True))
parser_sources     = sorted(glob.glob("src/tools/shader_transpiler/parser/*.cpp", recursive=True))
transpiler_sources = sorted(glob.glob("src/tools/shader_transpiler/transpiler/*.cpp", recursive=True))
compiler_sources   = sorted(glob.glob("src/tools/shader_transpiler/*.cpp", recursive=True))

build_lexer      = build_paths(lexer_sources)
build_parser     = build_paths(parser_sources)
build_transpiler = build_paths(transpiler_sources)
build_compiler   = build_paths(compiler_sources)

test_lexer_bin = env.Program(
    target=os.path.join(bin_dir, "test_lexer"),
    source=["tests/lexer_test.cpp"] + build_lexer,
)

test_parser_bin = env.Program(
    target=os.path.join(bin_dir, "test_parser"),
    source=["tests/parser_test.cpp"] + build_lexer + build_parser,
)

test_transpiler_bin = env.Program(
    target=os.path.join(bin_dir, "test_transpiler"),
    source=["tests/transpiler_test.cpp"] + build_lexer + build_parser + build_transpiler,
)

test_compiler_bin = env.Program(
    target=os.path.join(bin_dir, "test_compiler"),
    source=["tests/shader_compiler_test.cpp"] + build_lexer + build_parser + build_transpiler + build_compiler,
)

# Run a test binary after building it. A marker file is written so SCons can
# track whether the run step has been performed; tests are re-run on every
# `scons test` invocation (AlwaysBuild).
def _run_test_bin(target, source, env):
    import subprocess
    result = subprocess.call([str(source[0])])
    if result != 0:
        return result
    with open(str(target[0]), "w") as f:
        f.write("ok\n")
    return 0

run_lexer = env.Command(
    os.path.join(bin_dir, "test_lexer.run"),
    test_lexer_bin,
    _run_test_bin,
)

run_parser = env.Command(
    os.path.join(bin_dir, "test_parser.run"),
    test_parser_bin,
    _run_test_bin,
)

run_transpiler = env.Command(
    os.path.join(bin_dir, "test_transpiler.run"),
    test_transpiler_bin,
    _run_test_bin,
)

run_compiler = env.Command(
    os.path.join(bin_dir, "test_compiler.run"),
    test_compiler_bin,
    _run_test_bin,
)

env.AlwaysBuild(run_lexer)
env.AlwaysBuild(run_parser)
env.AlwaysBuild(run_transpiler)
env.AlwaysBuild(run_compiler)
env.Alias("test", [run_lexer, run_parser, run_transpiler, run_compiler])


# ---------------------------------------------------------------------------
# compile_commands.json (for clang-tidy / clangd)
# ---------------------------------------------------------------------------
# Emit a compilation database so tools like clang-tidy can pick up the exact
# include paths and defines used by the build. Regenerate with:
#   scons compile_commands
import json


def _emit_compile_commands(target, source, env):
    # Reconstruct the include/define flags from this environment's settings.
    inc_flags = env.subst("$CPPPATH", source=source[0]).split()
    def_flags = ["-D" + d for d in env.subst("$CPPDEFINES", source=source[0]).split()]
    cxx_flags = env.subst("$CXXFLAGS", source=source[0]).split()

    entries = []
    for src in source:
        command = (
            [env.subst("$CXX", source=src)]
            + cxx_flags
            + def_flags
            + ["-I" + inc for inc in inc_flags]
            + ["-c", str(src), "-o", str(src) + ".o"]
        )
        entries.append({
            "directory": os.getcwd(),
            "command": " ".join(command),
            "file": str(src),
        })

    with open(compile_commands_db, "w") as f:
        json.dump(entries, f, indent=2)


compile_commands_db = os.path.join("compile_commands.json")

# All sources compiled across the editor + runtime + tests, for the compile
# database. (Includes both entrypoints and every shared translation unit.)
all_compiled_sources = (
    build_editor_main
    + build_runtime_main
    + build_tools
    + build_common
    + build_glad
    + build_imgui
    + build_glfw
    + build_yaml
)

env.Command(
    compile_commands_db,
    all_compiled_sources,
    _emit_compile_commands,
)

env.Alias("compile_commands", compile_commands_db)
