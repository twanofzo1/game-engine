#pragma once
#include "common/core/types.hpp"

using ScriptID = u32; ///< Unique identifier for a script.

struct Script {
    ScriptID id; ///< Unique identifier.
    std::string name; ///< Name of the script.
    std::string path; ///< Path to the script file.
};