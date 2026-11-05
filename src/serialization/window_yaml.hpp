/**
 * @file window.hpp
 * @brief YAML serialization for Window::Options.
 */
#pragma once

#include "platform/window/window.hpp"
#include "serialization/yaml.hpp"

namespace Yaml {

/**
 * @brief Read window options from a YAML mapping node.
 * @param node YAML mapping to read from.
 * @param out  Destination options (initialized with defaults first).
 * @return True when `node` is a mapping.
 */
bool load_window(const Node* node, Engine::Window::Options* out);

/**
 * @brief Build a YAML mapping node describing the window options.
 */
Node make_window(const Engine::Window::Options* options);

} // namespace Yaml
