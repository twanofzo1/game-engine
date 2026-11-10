#pragma once
#include "core/types.hpp"
#include "core/objects/script.hpp"


struct Signal {
    std::string name;              ///< Unique identifier.
    std::vector<ScriptID> scripts; ///< Scripts to execute when the signal is emitted.
};