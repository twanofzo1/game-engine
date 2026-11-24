# C++ code rules

Apply these rules to all new and modified first-party C++ code, including
private helpers and test helpers. Review changes against these rules.

## Readability

- Prefer direct control flow, descriptive names, and small functions. Get the
  required behavior working in the simplest form a reviewer can follow.
- Keep each first-party C++ source and header file under 200 lines, including
  comments and blank lines. Aim for under 150. Split larger files by
  responsibility.

## C-like style and const

- Prefer simple structs, ordinary functions, and straightforward loops and
  conditionals. Keep additional C++ features to a minimum. Use the standard
  types and resource-management conventions in this guide. Introduce other
  features only when they solve a concrete problem and keep the code easy to
  read. Avoid unnecessary custom templates, operator overloading, and
  inheritance hierarchies.
- Never use `auto`, including `decltype(auto)`, or lambda expressions. Spell
  out types and use a named function when a callable is needed.
- Iterators are allowed. Spell out the iterator type and prefer
  `const_iterator` when its elements are read-only.
- Declare local variables and by-value parameters `const` when their values
  are never modified. In function definitions, apply this to config parameters
  as well. Mark member functions `const` when they only read object state.
- Use `const T*` when the pointed-to object is read-only through the pointer.
  Add top-level `const` to the pointer variable itself when it is never
  reassigned.

## Standard library types

Use the following types whenever their semantics fit. Keep existing framework
types at API boundaries that require them.

| Type | Use it for |
| --- | --- |
| `std::string` | Owned text. Use `std::string_view` for borrowed, read-only text whose lifetime is clear. |
| `std::optional<T>` | A value whose absence is a valid outcome, such as a search with no match. |
| `std::expected<T, E>` | An operation that can succeed or return a recoverable error. Use `std::expected<void, E>` when success has no payload. |
| `std::vector<T>` | A dynamically sized sequence. |
| `std::unordered_map<K, V>` | Key-based lookup when iteration order is irrelevant. |

Check `std::optional` and `std::expected` before accessing their values. Use a
meaningful error type for `std::expected`. The examples assume C++23 and a
standard library that supports `std::expected`.

## Function parameters

- Every function with two or more explicit inputs must take one aggregate config
  struct. This applies to free functions, member functions, static factories,
  and private helpers. The implicit object of a member function is excluded.
- Do not nest config structs inside a class. Define them at namespace scope,
  outside the class, using a function-specific name such as `ScaleConfig` or
  `Vec2Config`. Use public, descriptive fields and sensible defaults where
  applicable. Forward-declare the struct before the class when a member
  signature references it before its definition.
- Pass the config by value. When constructing a config at a call site, use
  designated initializers in the fields' declaration order. Reuse an existing
  config variable when it already represents the intended inputs.
- Zero- and one-input functions may use ordinary parameters. Use the required
  signature at language or external API boundaries, such as `main`, overrides,
  and framework callbacks. Keep boundary adapters small.

## Class creation and side effects

- Leave constructors and destructors implicit in project classes. Use
  RAII members to own and release resources.
- Provide a static `create` factory for pure construction. A pure factory
  depends only on its inputs and has no externally observable side effects.
  Call the factory when creating instances of the class.
- Name an impure factory for the effect or external dependency, such as
  `Image::loadFromFile`, `Socket::connect`, or `Session::createAndRegister`.
  Use the same convention for other functions with effects, such as
  `saveToFile` or `writeToSocket`. A recoverable failure alone does not make
  a function impure.

## Pointers and null checks

- Do not declare C++ reference parameters, return types, fields, or local
  bindings in project code. Pass or return values for value semantics. Use
  pointers when borrowing and smart pointers when transferring or sharing
  ownership. Use `const T*` for read-only access through a borrowed pointer.
- Assert `pointer != nullptr` for every required pointer input, including
  pointer fields in configs, before its first use. Apply the same check to
  pointers obtained inside a function before their first dereference.
- Use `std::optional` to express intentional absence. Check for absence before
  accessing the value, and assert that an engaged pointer is non-null.
- Use a runtime check for any null that can arise from external input or a
  recoverable failure. Return the error or handle absence before proceeding.
  Standard `assert` is disabled when `NDEBUG` is defined. Assertions must have
  no side effects.

## Doxygen

- Document every explicitly written function, including private helpers,
  with a Doxygen block on its definition in the implementation file. Put the
  block on the declaration in the header only when there is no separate
  definition (for example, functions defined inline in the header).
- Do not duplicate the Doxygen block in the header when a definition exists in
  a source file. The definition carries the documentation.
- Keep it short. Use `@brief` for what the function does, `@param` for each
  parameter, and `@return` for every non-void function. Include meaningful
  absence or failure cases in the return description. For a config parameter,
  explain its purpose and document any fields whose requirements need context.
- Use `@warn` for unsafe operations or caller obligations. Name the precise
  operation or argument, the precondition, and what can go wrong. Add
  `ALIASES += "warn=\warning"` to the Doxyfile so Doxygen renders `@warn`
  as its built-in warning command.

## Example

```cpp
#include <cassert>

/// @brief Configuration used to construct a vector from its coordinates.
struct Vec2Config {
    float x = 0.0f;
    float y = 0.0f;
};

/// @brief Configuration used to scale both coordinates.
struct ScaleConfig {
    const Vec2* vector = nullptr;
    float factor = 1.0f;
};

class Vec2 {
publstatic Vec2 create(const Vec2Config config);
ut the call.
    /// Reading through a dangling pointer has undefined behavior.
    static Vec2 scale(const ScaleConfig config);

private:
    float x_ = 0.0f;
    float y_ = 0.0f;
};

/// @brief Creates a vector from its coordinates.
/// @param config The x and y coordinates.
/// @return The initialized vector.
Vec2 Vec2::create(const Vec2Config config) {
    Vec2 vector;
    vector.x_ = config.x;
    vector.y_ = config.y;
    return vector;
}

/// @brief Scales both coordinates.
/// @param config The source vector and multiplier.
/// @return A new vector holding the scaled coordinates.
/// @warn The source pointer must address a live Vec2 throughout the call.
/// Reading through a dangling pointer has undefined behavior.
Vec2 Vec2::scale(const ScaleConfig config) {
    assert(config.vector != nullptr);
    return create({
        .x = config.vector->x_ * config.factor,
        .y = config.vector->y_ * config.factor,
    });
}

/// @brief Creates and scales an example vector.
/// @return Zero on success.
int main() {
    const Vec2 vector = Vec2::create({.x = 1.0f, .y = 2.0f});
    const Vec2 scaled = Vec2::scale({.vector = &vector, .factor = 2.0f});
    return 0;
}
```
