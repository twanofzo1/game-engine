#!/usr/bin/env python3
"""
@file SConstruct
@brief SCons build script for the GLFW + GLAD + Dear ImGui (docking) demo.

Builds two configurations of a single executable:

    scons build=debug     -> build/debug/app
    scons build=release   -> build/release/app   (default when omitted)

GLFW is linked as a shared library:
  - Linux:   the system `libglfw.so` (found via pkg-config, falls back to -lglfw).
  - Windows: a prebuilt `glfw3dll.lib` import library + `glfw3.dll` at runtime.

Dear ImGui and GLAD do not ship as shared libraries, so they are compiled
directly from the sources checked out under `external/`.
"""

import glob
import os
import sys

IS_WINDOWS = (sys.platform == 'win32')

BUILD = ARGUMENTS.get('build', 'release')
if BUILD not in ('debug', 'release'):
    print("error: 'build' must be 'debug' or 'release' (got '%s')" % BUILD)
    Exit(1)

ROOT = Dir('.').abspath
IMGUI = os.path.join(ROOT, 'external', 'imgui')
GLAD = os.path.join(ROOT, 'external', 'glad')
GLFW = os.path.join(ROOT, 'external', 'glfw')
STB = os.path.join(ROOT, 'external', 'stb')
DR_LIBS = os.path.join(ROOT, 'external', 'dr_libs')
MINIAUDIO = os.path.join(ROOT, 'external', 'miniaudio')
YAML_CPP = os.path.join(ROOT, 'external', 'yaml-cpp')

env = Environment()

# --------------------------------------------------------------------------
# Include paths
# --------------------------------------------------------------------------
env.Append(CPPPATH=[
    os.path.join(ROOT, 'src'),
    os.path.join(ROOT, 'editor'),
    IMGUI,
    os.path.join(IMGUI, 'backends'),
    os.path.join(GLAD, 'include'),
    STB,
    DR_LIBS,
    MINIAUDIO,
    os.path.join(YAML_CPP, 'include'),
])

env.Append(CPPDEFINES=['YAML_CPP_STATIC_DEFINE'])

# --------------------------------------------------------------------------
# Compiler flags: common, C++-only and C-only
# --------------------------------------------------------------------------
if IS_WINDOWS:
    env.Append(CCFLAGS=['/nologo', '/W3', '/EHsc'])
    if BUILD == 'debug':
        env.Append(CCFLAGS=['/Od', '/Zi'])
        env.Append(CPPDEFINES=['_DEBUG'])
        env.Append(LINKFLAGS=['/DEBUG'])
    else:
        env.Append(CCFLAGS=['/O2'])
        env.Append(CPPDEFINES=['NDEBUG'])
    # Force Dear ImGui's OpenGL3 backend to use GLAD instead of its own loader.
    force_include = ['/FI', os.path.join(ROOT, 'src', 'imgui_gl_loader.h')]
else:
    env.Append(CCFLAGS=['-Wall', '-Wextra', '-Wno-unused-parameter'])
    env.Append(CXXFLAGS=['-std=c++17'])
    env.Append(CFLAGS=['-std=c99'])
    if BUILD == 'debug':
        env.Append(CCFLAGS=['-O0', '-g'])
        env.Append(CPPDEFINES=['_DEBUG'])
    else:
        env.Append(CCFLAGS=['-O2'])
        env.Append(CPPDEFINES=['NDEBUG'])
    force_include = ['-include', os.path.join(ROOT, 'src', 'imgui_gl_loader.h')]

# --------------------------------------------------------------------------
# GLFW shared-library linkage
# --------------------------------------------------------------------------
if IS_WINDOWS:
    # GLFW 3.3 headers are checked out under external/glfw; the import library
    # (glfw3dll.lib) is expected on LIBPATH and glfw3.dll next to the exe.
    env.Append(CPPPATH=[os.path.join(GLFW, 'include')])
    env.Append(LIBS=['glfw3dll', 'opengl32', 'gdi32', 'user32', 'shell32'])
else:
    try:
        env.ParseConfig('pkg-config --cflags --libs glfw3')
    except Exception:
        env.Append(LIBS=['glfw'])
    env.Append(LIBS=['X11', 'dl', 'pthread', 'm'])

# --------------------------------------------------------------------------
# Source files
# --------------------------------------------------------------------------
# Engine sources: everything under src/ except the demo's entry point.
_src_cpps = glob.glob(os.path.join('src', '**', '*.cpp'), recursive=True)
engine_sources = sorted(
    os.path.join(ROOT, f)
    for f in _src_cpps
    if os.path.normpath(f) != os.path.join('src', 'main.cpp')
)
app_main_source = os.path.join(ROOT, 'src', 'main.cpp')

# Editor sources: the (future) YAML editor executable.
editor_sources = sorted(
    os.path.join(ROOT, f)
    for f in glob.glob(os.path.join('editor', '**', '*.cpp'), recursive=True)
)

yaml_cpp_sources = sorted(
    os.path.join(ROOT, f)
    for f in glob.glob(os.path.join('external', 'yaml-cpp', 'src', '*.cpp'))
)

imgui_sources = [
    os.path.join(IMGUI, 'imgui.cpp'),
    os.path.join(IMGUI, 'imgui_demo.cpp'),
    os.path.join(IMGUI, 'imgui_draw.cpp'),
    os.path.join(IMGUI, 'imgui_tables.cpp'),
    os.path.join(IMGUI, 'imgui_widgets.cpp'),
    os.path.join(IMGUI, 'backends', 'imgui_impl_glfw.cpp'),
    os.path.join(IMGUI, 'backends', 'imgui_impl_opengl3.cpp'),
]

glad_sources = [os.path.join(GLAD, 'src', 'glad.c')]

all_sources = (engine_sources + [app_main_source] + editor_sources
               + yaml_cpp_sources + imgui_sources + glad_sources)


def object_path(src):
    """Map a source path to its object file under build/<build>/obj/."""
    rel = os.path.relpath(src, ROOT)
    return os.path.join('build', BUILD, 'obj',
                        os.path.splitext(rel)[0] + env['OBJSUFFIX'])


objects = []
obj_by_src = {}
for src in all_sources:
    extra = []
    if src.endswith('imgui_impl_opengl3.cpp'):
        extra = force_include
    if IS_WINDOWS and src.endswith('glad.c'):
        # GLAD's generated C uses C99 constructs; compile it as C++ on MSVC.
        extra = extra + ['/TP']
    obj = env.Object(object_path(src), src,
                     CCFLAGS=env['CCFLAGS'] + extra)
    objects.append(obj)
    obj_by_src[src] = obj

external_sources = yaml_cpp_sources + imgui_sources + glad_sources

app_objects = [obj_by_src[src]
               for src in engine_sources + [app_main_source] + external_sources]
editor_objects = [obj_by_src[src]
                  for src in engine_sources + editor_sources + external_sources]

app = env.Program(os.path.join('build', BUILD, 'app'), app_objects)
editor = env.Program(os.path.join('build', BUILD, 'editor'), editor_objects)
env.Default(app, editor)

# --------------------------------------------------------------------------
# Tests: a headless executable linking only the pure-logic engine sources
# (no GLFW/GL/window/audio-device), so it runs without a display.
# --------------------------------------------------------------------------
logic_sources = [
    os.path.join(ROOT, 'src', 'core', 'allocator', 'memory_pool.cpp'),
    os.path.join(ROOT, 'src', 'core', 'ecs', 'components.cpp'),
    os.path.join(ROOT, 'src', 'core', 'ecs', 'component_pool.cpp'),
    os.path.join(ROOT, 'src', 'core', 'ecs', 'world.cpp'),
    os.path.join(ROOT, 'src', 'resources', 'resource.cpp'),
    os.path.join(ROOT, 'src', 'resources', 'image.cpp'),
    os.path.join(ROOT, 'src', 'resources', 'audio.cpp'),
    os.path.join(ROOT, 'src', 'core', 'timers', 'timers.cpp'),
    os.path.join(ROOT, 'src', 'graphics', 'rendering', 'camera.cpp'),
    os.path.join(ROOT, 'src', 'serialization', 'yaml.cpp'),
] + yaml_cpp_sources
logic_objects = [obj_by_src[src] for src in logic_sources]

test_sources = sorted(glob.glob(os.path.join(ROOT, 'tests', '*.cpp')))
test_objects = [env.Object(object_path(src), src) for src in test_sources]

test_env = env.Clone()
if IS_WINDOWS:
    test_env['LIBS'] = []
else:
    test_env['LIBS'] = ['m']

test_prog = test_env.Program(os.path.join('build', BUILD, 'tests'),
                             test_objects + logic_objects)


def run_tests(target, source, env):
    """Run the test binary; return its exit status."""
    import subprocess
    return subprocess.call([source[0].abspath])


test_run = env.Command(os.path.join('build', BUILD, 'test.run'),
                       test_prog, run_tests)
env.AlwaysBuild(test_run)
Alias('test', test_run)

# Convenient aliases: `scons debug`, `scons release` and `scons editor`.
Alias('debug', os.path.join('build', 'debug', 'app'))
Alias('release', os.path.join('build', 'release', 'app'))
Alias('editor', os.path.join('build', BUILD, 'editor'))
