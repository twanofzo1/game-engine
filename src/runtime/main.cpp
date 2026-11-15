/**
 * @file main.cpp
 * @brief Game runtime entrypoint.
 *
 * Loads the game world from YAML (produced by the editor) and runs the main
 * loop. This is a minimal scaffold: it reads the YAML scene file and reports
 * success/failure, ready to be extended into the full runtime.
 */
#include "common/serialisation/yaml.hpp"

#include <optional>
#include <stdio.h>


/**
 * @brief Runtime entrypoint: load the YAML world and run the loop.
 */
int main(int argc, char** argv)
{
    const char* scene_path = "src/test.yaml";
    if (argc > 1) {
        scene_path = argv[1];
    }

    std::optional<YamlNode> scene = Yaml::load(scene_path);
    if (!scene.has_value()) {
        fprintf(stderr, "runtime: failed to parse scene file '%s'\n", scene_path);
        return 1;
    }

    printf("runtime: loaded scene '%s'\n", scene_path);
    return 0;
}
