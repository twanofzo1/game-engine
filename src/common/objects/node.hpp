#pragma once

#include "common/core/types.hpp"
#include "common/objects/script.hpp"

using NodeID = u32; ///< Unique identifier for a node.

struct Node {
    ScriptID script; ///< Script to execute for this node.
    std::vector<NodeID> children; ///< Child nodes.
};