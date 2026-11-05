1. dont use modern cpp function, try to make it a mostly c like codebase
2. classes are not permitted
3. use a unique_ptr/shared_ptr for memory management or better a custom memory pool
4. never use inheritance, use composition instead
5. it should be mostly consistent with an ecs style architecture
6. every file should have a small header comment describing its purpose
7. every file should be smaller than 200 lines preferably around 100 lines
8. every directory should have a README.md describing its purpose per file and the directory as a whole
9. datastructures are permitted
10. optional values are good if not in a hot loop or memory critical path
11. unsafe code is permitted and encouraged if it improves performance, but should be well documented with clear comments and reasoning with an @warning tag
12. comment style should be doxygen style
13. make shure the code is well thought about and not just a quick hack, it should be maintainable and easy to read
14. dont use refferences, use pointers instead. only use refferences for compatability with existing code or libraries.
15. use namespaces to group related functions and types, but avoid putting everything in a single namespace. 
16. Use sub-namespaces for different modules or systems and never use "using namespace"
17. never give namespace extra names when its the same as the struct, for example `Engine::Ecs::create()` and not `Engine::Ecs::create_ecs()` 
18. always name namespaces for the struct it uses the same name as the struct.
19. always use snake_case for function names and variable names, and PascalCase for struct and namespace names.
