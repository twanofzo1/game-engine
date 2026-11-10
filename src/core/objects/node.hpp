#pragma once

#include "core/types.hpp"
#include <vector>

struct Node {
    ScriptID script; ///< Script to execute for this node.
    std::vector<NodeID> children; ///< Child nodes.
};