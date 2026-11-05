/**
 * @file attack_yaml.cpp
 * @brief YAML serialization for Attack, built on the Yaml helpers.
 */
#include "serialization/attack_yaml.hpp"

#include <string.h>

namespace Yaml {

bool load_attack(const Node* node, Engine::Attack::Attack* out)
{
    if (out == NULL)
        return false;
    if (node == NULL || !node->IsMap())
        return false;

    out->animation_id = as_int((*node)["animation_id"], 0);
    out->windup       = as_int((*node)["windup"], 0);
    out->active       = as_int((*node)["active"], 0);
    out->recovery     = as_int((*node)["recovery"], 0);

    std::string name = as_string((*node)["name"], "");
    strncpy(out->name, name.c_str(), sizeof(out->name) - 1);
    out->name[sizeof(out->name) - 1] = '\0';

    Node events = (*node)["events"];
    if (events.IsSequence())
    {
        for (u32 i = 0; i < events.size(); ++i)
        {
            Node kf = events[i];
            if (!kf.IsSequence() || kf.size() < 2)
                continue;
            std::string event = as_string(kf[1], "");
            Engine::Attack::add_event(out, as_int(kf[0], 0), event.c_str());
        }
    }

    Node hitboxes = (*node)["hitboxes"];
    if (hitboxes.IsSequence())
    {
        for (u32 i = 0; i < hitboxes.size(); ++i)
        {
            Node kf = hitboxes[i];
            if (!kf.IsSequence() || kf.size() < 2)
                continue;
            Engine::Attack::add_hitbox(out, as_int(kf[0], 0), as_int(kf[1], 0));
        }
    }

    Node movements = (*node)["movement"];
    if (movements.IsSequence())
    {
        for (u32 i = 0; i < movements.size(); ++i)
        {
            Node kf = movements[i];
            if (!kf.IsSequence() || kf.size() < 2)
                continue;
            Engine::Attack::add_movement(out, as_int(kf[0], 0),
                                         as_vec2(kf[1], Engine::Vec2::make(0.0f, 0.0f)));
        }
    }

    return true;
}

Node make_attack(const Engine::Attack::Attack* attack)
{
    Node node;

    node["animation_id"] = attack->animation_id;
    node["name"]         = std::string(attack->name);
    node["windup"]       = attack->windup;
    node["active"]       = attack->active;
    node["recovery"]     = attack->recovery;

    Node events;
    for (u32 i = 0; i < attack->event_count; ++i)
    {
        Node kf;
        kf.SetStyle(YAML::EmitterStyle::Flow);
        kf.push_back(attack->events[i].frame);
        kf.push_back(std::string(attack->events[i].name));
        events.push_back(kf);
    }
    node["events"] = events;

    Node hitboxes;
    for (u32 i = 0; i < attack->hitbox_count; ++i)
    {
        Node kf;
        kf.SetStyle(YAML::EmitterStyle::Flow);
        kf.push_back(attack->hitboxes[i].frame);
        kf.push_back(attack->hitboxes[i].shape_id);
        hitboxes.push_back(kf);
    }
    node["hitboxes"] = hitboxes;

    Node movements;
    for (u32 i = 0; i < attack->movement_count; ++i)
    {
        Node kf;
        kf.SetStyle(YAML::EmitterStyle::Flow);
        kf.push_back(attack->movements[i].frame);
        kf.push_back(make_vec2(attack->movements[i].vector));
        movements.push_back(kf);
    }
    node["movement"] = movements;

    return node;
}

} // namespace Yaml
