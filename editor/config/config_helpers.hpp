/**
 * @file config_helpers.hpp
 * @brief Reusable YAML config sequence helpers shared by the editor modules.
 */
#pragma once

#include "core/editor.hpp"
#include "serialization/yaml.hpp"

namespace Editor
{

    /** @brief Load the config document, returning an empty map on failure. */
    Yaml::Node config_open(const char *config_path);

    /** @brief Ensure `root[key]` is a sequence; create one if missing. */
    Yaml::Node config_sequence(Yaml::Node *root, const char *key);

    /** @brief Index of the sequence entry whose `name` equals `name`, or -1. */
    i32 config_find_by_name(const Yaml::Node &seq, const char *name);

    /** @brief Rebuild a sequence without the entry at `index`. */
    Yaml::Node config_remove_index(const Yaml::Node &seq, i32 index);

    /** @brief Replace the entry matching `name` (and `sprite_id` when >= 0), or append. */
    bool config_upsert_by_name(Yaml::Node *root, const char *key,
                               const Yaml::Node &entry, const char *name,
                               i32 match_sprite_id);

    /** @brief Rename the entry at `index` (its `name` field). */
    void config_rename_at(Yaml::Node *root, const char *key, i32 index,
                          const char *new_name);

} // namespace Editor
