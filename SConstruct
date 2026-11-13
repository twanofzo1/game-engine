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

# Everything the engine writes lives under src/ — gather it recursively.
# Python's glob with recursive=True walks every nested subdirectory, unlike
# SCons' Glob() whose `**` only matches a single directory level.
src_sources = sorted(glob.glob("src/**/*.cpp", recursive=True))

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

build_src = [build_path(s) for s in src_sources]
build_glad = [build_path(s) for s in glad_sources]
build_imgui = [build_path(s) for s in imgui_sources]
build_glfw = [build_path(s) for s in glfw_sources]
# yaml_sources are Node objects from Glob(); map their string paths.
build_yaml = [build_path(str(s)) for s in yaml_sources]

# Compile everything into build_dir, then link into bin_dir.
all_sources = (
    build_src
    + build_glad
    + build_imgui
    + build_glfw
    + build_yaml
)

app = env.Program(
    target=os.path.join(bin_dir, "app"),
    source=all_sources,
)

Default(app)


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

env.Command(
    compile_commands_db,
    all_sources,
    _emit_compile_commands,
)

env.Alias("compile_commands", compile_commands_db)
