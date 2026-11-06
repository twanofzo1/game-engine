/**
 * @file area.hpp
 * @brief Area type: a named trigger region (rectangle or circle).
 */
#pragma once
#include "core/types.hpp"

/** @brief A named region used to trigger overlaps, as either a rect or a circle. */
struct Area {
    std::string name; ///< Unique identifier.
    union {
        struct {
            Rect rect; ///< Rectangular shape.
        };
        struct {
            Circle circle; ///< Circular shape.
        };
    };
};