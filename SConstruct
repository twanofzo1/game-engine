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
env.Append(CXXFLAGS=["-std=c++17"])
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

glfw_gen = env.Object(glfw_sources)

# GLAD (OpenGL loader).
glad_sources = ["external/glad/src/glad.c"]

# yaml-cpp (all its .cpp files).
yaml_sources = Glob("external/yaml-cpp/src/*.cpp")

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------
variant_dir = "build/%s" % build_type

app = env.Program(
    target=os.path.join(variant_dir, "app"),
    source=src_sources
    + glad_sources
    + imgui_sources
    + glfw_gen
    + yaml_sources,
)

Default(app)
