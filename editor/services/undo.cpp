/**
 * @file undo.cpp
 * @brief Undo/redo stack implementation.
 */
#include "services/undo.hpp"

#include <stdlib.h>
#include <string.h>

namespace Editor {

void undo_init(UndoState* u)
{
    u->undo_stack = (Editor*)malloc(sizeof(Editor) * EDITOR_UNDO_MAX);
    u->redo_stack = (Editor*)malloc(sizeof(Editor) * EDITOR_UNDO_MAX);
    u->undo_count = 0;
    u->redo_count = 0;
}

void undo_free(UndoState* u)
{
    free(u->undo_stack);
    free(u->redo_stack);
    u->undo_stack = NULL;
    u->redo_stack = NULL;
    u->undo_count = 0;
    u->redo_count = 0;
}

/** @brief Copy `src` into `dst`, preserving live GPU/preview and transient UI. */
static void copy_state(Editor* dst, const Editor* src)
{
    Editor live = *dst;
    *dst = *src;

    /* Never restore the GPU texture or undo-stack pointer from a snapshot. */
    dst->preview.gpu = live.preview.gpu;
    dst->preview.preview_loaded = live.preview.preview_loaded;
    memcpy(dst->preview.preview_path, live.preview.preview_path, EDITOR_MAX_PATH);
    dst->undo = live.undo;

    /* Clear transient UI state after a restore. */
    dst->errors.error_active = false;
    dst->guard.pending_action = PENDING_NONE;
    dst->rename.rename_open = false;
    dst->viewport.view_edit_drag = 0;
    dst->viewport.view_edit_drag_sub = 0;
}

void undo_push(Editor* editor)
{
    UndoState* u = editor->undo;
    if (!u)
        return;

    if (u->undo_count == EDITOR_UNDO_MAX)
    {
        /* Drop the oldest snapshot to make room. */
        memmove(&u->undo_stack[0], &u->undo_stack[1],
                sizeof(Editor) * (EDITOR_UNDO_MAX - 1));
        u->undo_count--;
    }

    memcpy(&u->undo_stack[u->undo_count], editor, sizeof(Editor));
    u->undo_count++;
    u->redo_count = 0; /* a new edit invalidates the redo history */
}

void undo_do(Editor* editor)
{
    UndoState* u = editor->undo;
    if (!u || u->undo_count == 0)
        return;

    if (u->redo_count < EDITOR_UNDO_MAX)
    {
        memcpy(&u->redo_stack[u->redo_count], editor, sizeof(Editor));
        u->redo_count++;
    }

    copy_state(editor, &u->undo_stack[--u->undo_count]);
}

void redo_do(Editor* editor)
{
    UndoState* u = editor->undo;
    if (!u || u->redo_count == 0)
        return;

    if (u->undo_count < EDITOR_UNDO_MAX)
    {
        memcpy(&u->undo_stack[u->undo_count], editor, sizeof(Editor));
        u->undo_count++;
    }

    copy_state(editor, &u->redo_stack[--u->redo_count]);
}

bool undo_can_undo(const Editor* editor)
{
    return editor->undo && editor->undo->undo_count > 0;
}

bool undo_can_redo(const Editor* editor)
{
    return editor->undo && editor->undo->redo_count > 0;
}

} // namespace Editor
