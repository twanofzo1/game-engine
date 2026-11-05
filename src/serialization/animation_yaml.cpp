/**
 * @file animation_yaml.cpp
 * @brief YAML serialization for Animation, built on the Yaml helpers.
 */
#include "serialization/animation_yaml.hpp"

#include <string.h>

namespace Yaml {

bool load_animation(const Node* node, Engine::Animation::Animation* out)
{
    if (out == NULL)
        return false;
    if (node == NULL || !node->IsMap())
        return false;

    out->sprite_id = as_int((*node)["sprite_id"], 0);

    std::string name = as_string((*node)["name"], "");
    strncpy(out->name, name.c_str(), sizeof(out->name) - 1);
    out->name[sizeof(out->name) - 1] = '\0';

    Node frames = (*node)["frames"];
    if (frames.IsSequence())
    {
        for (u32 i = 0; i < frames.size(); ++i)
        {
            Node kf = frames[i];
            if (!kf.IsSequence() || kf.size() < 2)
                continue;
            Engine::Animation::add_frame(out, as_float(kf[0], 0.0f), as_int(kf[1], 0));
        }
    }

    return true;
}

Node make_animation(const Engine::Animation::Animation* animation)
{
    Node node;

    node["sprite_id"] = animation->sprite_id;
    node["name"]      = std::string(animation->name);

    Node frames;
    for (u32 i = 0; i < animation->frame_count; ++i)
    {
        Node kf;
        kf.SetStyle(YAML::EmitterStyle::Flow);
        kf.push_back(animation->frames[i].time);
        kf.push_back(animation->frames[i].frame);
        frames.push_back(kf);
    }
    node["frames"] = frames;

    return node;
}

} // namespace Yaml
