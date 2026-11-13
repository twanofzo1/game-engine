# Doxygen
- Every file gets a file-level block at the top, right before/after the include guard:
/**
* @file name.ext
* @brief One-line purpose of the file.
*/
- Every function gets a @brief one-liner placed at its definition site:
- Member functions of a struct → documented in the .cpp.
- Header-defined static inline/static free functions → documented inline in the .hpp (there is no .cpp definition to attach to).
- Every struct gets a @brief placed in the .hpp (structs have no implementation file, so the header is their only home).
- Every struct member gets an inline trailing comment ///< describing it.
- Parameters are documented with @param and return values with @return when they aren't obvious from the @brief.
- Dangerous/unsafe behavior uses @warning, matching rule 11 in rules.md (e.g. normalize on a zero-length vector).
- Style: /** ... */ blocks, @-style commands, @brief single sentence; trailing member comments use ///<.


# Code
- Use snake_case for function names and variable names, and PascalCase for struct and namespace names.

- Use consistent indentation (4 spaces) and avoid tabs.

- Prefer const and constexpr for compile-time constants.

- Use meaningful variable names that clearly indicate their purpose.

- Avoid global variables when possible.

- Keep functions small and focused on a single task.

- Use comments to explain complex logic or non-obvious decisions.

- Dont use modern cpp functionality except for the following:
    * std::vector
    * std::string 
    * std::optional
    * std::expected 
    * std::unordered_map

- use an optional value for functions that can fail instead of returning a boolean or throwing an exception. for example, instead of `bool foo(int x)`, use `std::optional<int> foo(int x)`.

- use an expected value for functions that can fail and return an error code instead of throwing an exception. for example, instead of `int foo(int x)`, use `std::expected<int, ErrorCode> foo(int x)` hereby try to make shure the enum is of the smallest type, almost always u8.

- never use classes only use structs, and avoid inheritance and polymorphism.

- never use member functions for structs

- use static functions for structs to provide functionality related to the struct, for example, instead of having a member function like `vec2.length()`, have a static function like `Vec2::length(vec2 v)`.

- when using structs, use a Config struct to pass in parameters to the constructor function. for example, instead of having a constructor like `vec2(float x, float y)`, have a constructor like `vec2(Vec2Config config)`. config structs should have default values for all their members, so that the user can choose to only set the parameters they care about. for example, instead of having a constructor like `vec2(float x, float y)`, have a constructor like `vec2(Vec2Config config = {})`. this way, the user can create a vec2 with default values by calling `vec2 v = vec2();`, or they can create a vec2 with specific values by calling `vec2 v = vec2({.x = 1.0f, .y = 2.0f});`.

- when instantiating a struct use the config like this: `Vec2 v = Vec2::new({.x = 1.0f, .y = 2.0f});` instead of `Vec2 v = Vec2::new(1.0f, 2.0f);`.

- keep files small and focused on a single purpose. If a file grows too large, consider splitting it into multiple files.

- keep the codebase modular and organized, with clear separation of concerns between different components.

- try to make code readable and maintainable instead of just functional.

- dont ever use refferences, except for interconnecting with existing code/libraries, always use pointers or values. for example, instead of `void foo(int& x)`, use `void foo(int* x)` or `void foo(int x)`.

