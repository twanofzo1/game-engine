/**
 * @file undo.hpp
 * @brief Undo/redo stack for the editor (snapshots of the editing state).
 */
#pragma once

#include "core/editor.hpp"

namespace Editor {

/** Maximum snapshots kept on each side of the undo/redo stack. */
#define EDITOR_UNDO_MAX 16

/** A two-stack undo/redo history of full editor snapshots. */
struct UndoState {
    Editor* undo_stack; ///< States to restore on undo (heap).
    Editor* redo_stack; ///< States to restore on redo (heap).
    i32     undo_count;
    i32     redo_count;
};

/** @brief Allocate the undo stacks. */
void undo_init(UndoState* u);

/** @brief Free the undo stacks. */
void undo_free(UndoState* u);

/** @brief Snapshot the current state before a mutation (uses `editor->undo`). */
void undo_push(Editor* editor);

/** @brief True when an undo step is available. */
bool undo_can_undo(const Editor* editor);

/** @brief True when a redo step is available. */
bool undo_can_redo(const Editor* editor);

/** @brief Undo the last edit. */
void undo_do(Editor* editor);

/** @brief Redo the last undone edit. */
void redo_do(Editor* editor);

} // namespace Editor
