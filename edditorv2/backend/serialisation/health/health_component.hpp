#pragma once
#include "../../../typedefs.hpp"
#include <vector>

struct HealthComponentConfig {
    u16 health;
    u16 maxHealth;
};

struct HealthComponentSignals {
    std::vector<Subscriber> on_damage_subscribers;
    std::vector<Subscriber> on_heal_subscribers;
    std::vector<Subscriber> on_death_subscribers;
    std::vector<void*> subscriber_payloads;
};

struct HealthComponent{
    u16 health;
    u16 maxHealth;

    static HealthComponent make(const HealthComponentConfig config) {
        HealthComponent health_component;
        health_component.health = config.health;
        health_component.maxHealth = config.maxHealth;
        return health_component;
    }

    static HealthComponent damage(HealthComponent health_component, u16 damage) {
        if (damage >= health_component.health) {
            health_component.health = 0;
        } else {
            health_component.health -= damage;
        }
        return health_component;
    }

    static HealthComponent heal(HealthComponent health_component, u16 heal) {
        if (health_component.health + heal >= health_component.maxHealth) {
            health_component.health = health_component.maxHealth;
        } else {
            health_component.health += heal;
        }
        return health_component;
    }

    static Yaml::Node serialize(const HealthComponent& health_component) {
        Yaml::Node node;
        node["UID"] = Yaml::UID::generate();
        node["health"] = health_component.health;
        node["maxHealth"] = health_component.maxHealth;
        return node;
    }

    static HealthComponent deserialize(const Yaml::Node& node) {
        HealthComponent health_component;
        health_component.health = node["health"].as<u16>();
        health_component.maxHealth = node["maxHealth"].as<u16>();
        return health_component;
    }
};


std::vector<Subscriber> on_damage_subscribers;
std::vector<Subscriber> on_heal_subscribers;
std::vector<Subscriber> on_death_subscribers;
std::vector<void*> subscriber_payloads;