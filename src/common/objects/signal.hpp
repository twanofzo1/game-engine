#pragma once
#include "common/core/types.hpp"
#include "common/objects/script.hpp"


struct Signal {
    std::string name;              ///< Unique identifier.
    std::vector<ScriptID> scripts; ///< Scripts to execute when the signal is emitted.
};