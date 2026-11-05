/**
 * @file event.cpp
 * @brief Minimal publish/subscribe event bus implementation.
 */
#include "core/event/event.hpp"

#include <stdlib.h>
#include <string.h>

namespace Engine {
namespace Event {

/* Process-lifetime intern table: canonical event names -> ids.
 * Ids are 1-based; 0 is reserved for "no event / match all". Never freed. */
static const char** intern_names    = NULL;
static u32          intern_count    = 0;
static u32          intern_capacity = 0;

void init(Bus* bus)
{
    bus->subs     = NULL;
    bus->count    = 0;
    bus->capacity = 0;
}

void destroy(Bus* bus)
{
    free(bus->subs);
    bus->subs     = NULL;
    bus->count    = 0;
    bus->capacity = 0;
}

void subscribe(Bus* bus, const char* event, Handler handler, void* user)
{
    if (bus->count == bus->capacity)
    {
        u32 next = bus->capacity == 0 ? 8 : bus->capacity * 2;
        bus->subs = (Subscription*)realloc(bus->subs, sizeof(Subscription) * next);
        bus->capacity = next;
    }

    Subscription* sub = &bus->subs[bus->count++];
    sub->event_id = intern(event);
    sub->handler  = handler;
    sub->user     = user;
}

EventId intern(const char* name)
{
    if (name == NULL || name[0] == '\0')
        return 0;

    for (u32 i = 0; i < intern_count; ++i)
    {
        if (strcmp(intern_names[i], name) == 0)
            return i + 1;
    }

    if (intern_count == intern_capacity)
    {
        u32 next = intern_capacity == 0 ? 16 : intern_capacity * 2;
        intern_names = (const char**)realloc(intern_names, sizeof(const char*) * next);
        intern_capacity = next;
    }

    size_t len = strlen(name) + 1;
    char* copy = (char*)malloc(len);
    memcpy(copy, name, len);
    intern_names[intern_count] = copy;
    return ++intern_count;
}

const char* name_of(EventId id)
{
    if (id == 0 || id > intern_count)
        return NULL;
    return intern_names[id - 1];
}

static void dispatch(const Bus* bus, EventId event_id, const char* name, void* payload)
{
    for (u32 i = 0; i < bus->count; ++i)
    {
        const Subscription* sub = &bus->subs[i];
        if (sub->event_id == 0 || sub->event_id == event_id)
            sub->handler(sub->user, name, payload);
    }
}

void emit(const Bus* bus, const char* event, void* payload)
{
    dispatch(bus, intern(event), event, payload);
}

void emit_id(const Bus* bus, EventId event_id, void* payload)
{
    dispatch(bus, event_id, name_of(event_id), payload);
}

Handler lookup(const Binding* bindings, u32 count, const char* name)
{
    if (!bindings || !name)
        return NULL;
    for (u32 i = 0; i < count; ++i)
    {
        if (bindings[i].name != NULL && strcmp(bindings[i].name, name) == 0)
            return bindings[i].handler;
    }
    return NULL;
}

} // namespace Event
} // namespace Engine
