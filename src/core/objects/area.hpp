/**
 * @file area.hpp
 * @brief Area type: a named trigger region (rectangle or circle).
 */
#pragma once
#include "core/types.hpp"

/** @brief A named region used to trigger overlaps, as either a rect or a circle. */
struct Area {
    std::string name; ///< Unique identifier.
    Shape shape; ///< Shape of the area.
    bool continuous; ///< True if the area triggers every frame while overlapping, false if only on the first frame of overlap.
};