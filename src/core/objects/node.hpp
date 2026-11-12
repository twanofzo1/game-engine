#pragma once

#include "core/types.hpp"
#include "core/objects/script.hpp"

using NodeID = u32; ///< Unique identifier for a node.

struct Node {
    ScriptID script; ///< Script to execute for this node.
    std::vector<NodeID> children; ///< Child nodes.
};