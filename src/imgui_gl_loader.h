/**
 * @file imgui_gl_loader.h
 * @brief Points Dear ImGui's OpenGL3 backend at the GLAD loader.
 *
 * Dear ImGui normally ships its own minimal GL loader. To keep a single GL
 * loader (GLAD) for the whole program, this header is force-included when
 * compiling `imgui_impl_opengl3.cpp` (see SConstruct) and defines the
 * custom-loader macro so the backend uses the GLAD entry points instead.
 */

#pragma once

#include <glad/glad.h>

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
