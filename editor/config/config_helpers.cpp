/**
 * @file config_helpers.cpp
 * @brief Reusable YAML config sequence helpers shared by the editor modules.
 */
#include "config/config_helpers.hpp"

#include <string>

namespace Editor
{

    Yaml::Node config_open(const char *config_path)
    {
        Yaml::Node root;
        if (!Yaml::load(&root, config_path))
            root = Yaml::Node(YAML::NodeType::Map);
        return root;
    }

    Yaml::Node config_sequence(Yaml::Node *root, const char *key)
    {
        if (!(*root)[key].IsSequence())
            (*root)[key] = Yaml::Node(YAML::NodeType::Sequence);
        return (*root)[key];
    }

    i32 config_find_by_name(const Yaml::Node &seq, const char *name)
    {
        for (usize i = 0; i < seq.size(); ++i)
        {
            std::string n = Yaml::as_string(seq[i]["name"], "");
            if (n == name)
                return (i32)i;
        }
        return -1;
    }

    Yaml::Node config_remove_index(const Yaml::Node &seq, i32 index)
    {
        Yaml::Node out(YAML::NodeType::Sequence);
        for (usize i = 0; i < seq.size(); ++i)
        {
            if ((i32)i == index)
                continue;
            out.push_back(seq[i]);
        }
        return out;
    }

    bool config_upsert_by_name(Yaml::Node *root, const char *key,
                               const Yaml::Node &entry, const char *name,
                               i32 match_sprite_id)
    {
        Yaml::Node seq = config_sequence(root, key);

        Yaml::Node out(YAML::NodeType::Sequence);
        bool replaced = false;
        for (usize i = 0; i < seq.size(); ++i)
        {
            std::string n = Yaml::as_string(seq[i]["name"], "");
            bool match = (n == name);
            if (match && match_sprite_id >= 0)
                match = (Yaml::as_int(seq[i]["sprite_id"], -1) == match_sprite_id);

            if (!replaced && match)
            {
                out.push_back(entry);
                replaced = true;
            }
            else
            {
                out.push_back(seq[i]);
            }
        }
        if (!replaced)
            out.push_back(entry);
        (*root)[key] = out;
        return replaced;
    }

    void config_rename_at(Yaml::Node *root, const char *key, i32 index,
                          const char *new_name)
    {
        Yaml::Node seq = (*root)[key];
        if (seq.IsSequence() && index >= 0 && (usize)index < seq.size())
            seq[index]["name"] = std::string(new_name);
    }

} // namespace Editor
