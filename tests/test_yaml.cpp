/**
 * @file test_yaml.cpp
 * @brief Tests for the YAML facade (yaml-cpp backend).
 */
#include "framework.hpp"

#include "serialization/yaml.hpp"
#include "core/types/vec2.hpp"
#include "core/types/color.hpp"

#include <string>

void test_yaml(void)
{
    const char* text =
        "name: engine\n"
        "window:\n"
        "  width: 1280\n"
        "  height: 720\n"
        "  vsync: true\n"
        "items:\n"
        "  - a\n"
        "  - b\n";

    Yaml::Node root;
    TEST_CHECK(Yaml::load_text(&root, text));

    TEST_CHECK(Yaml::as_string(root["name"], "") == "engine");
    TEST_CHECK(Yaml::as_int(root["window"]["width"], 0) == 1280);
    TEST_CHECK(Yaml::as_int(root["window"]["height"], 0) == 720);
    TEST_CHECK(Yaml::as_bool(root["window"]["vsync"], false) == true);

    TEST_CHECK(root["items"].IsSequence());
    TEST_CHECK(root["items"].size() == 2u);
    TEST_CHECK(Yaml::as_string(root["items"][0], "") == "a");
    TEST_CHECK(Yaml::as_string(root["items"][1], "") == "b");

    /* Round-trip: serialize then re-parse. */
    Yaml::Node copy;
    std::string serialized = YAML::Dump(root);
    TEST_CHECK(Yaml::load_text(&copy, serialized.c_str()));
    TEST_CHECK(Yaml::as_int(copy["window"]["width"], 0) == 1280);

    /* Flow sequences and nested flow collections must parse. */
    const char* flow =
        "size: [256, 128]\n"
        "movement:\n"
        "  - [0.0, [0, 0]]\n"
        "  - [0.2, [1, 0]]\n";

    Yaml::Node flow_root;
    TEST_CHECK(Yaml::load_text(&flow_root, flow));
    TEST_CHECK(flow_root["size"].IsSequence());
    TEST_CHECK(Yaml::as_int(flow_root["size"][0], 0) == 256);
    TEST_CHECK(Yaml::as_int(flow_root["size"][1], 0) == 128);
    TEST_CHECK(flow_root["movement"].IsSequence());
    TEST_CHECK(flow_root["movement"][1][1][0].as<int>() == 1);

    /* Vector2 parse/write helpers. */
    Engine::Vec2::Vec2 v = Yaml::as_vec2(flow_root["size"], Engine::Vec2::make(0.0f, 0.0f));
    TEST_CHECK_FEQ(v.x, 256.0f, 1e-6);
    TEST_CHECK_FEQ(v.y, 128.0f, 1e-6);

    i32 got_int = 0;
    TEST_CHECK(Yaml::get_int(flow_root["size"][0], &got_int));
    TEST_CHECK(got_int == 256);
    i32 failed_int = 7;
    TEST_CHECK(!Yaml::get_int(root["missing"], &failed_int));
    TEST_CHECK(failed_int == 0);

    Yaml::Node made = Yaml::make_vec2(Engine::Vec2::make(3.0f, 4.0f));
    TEST_CHECK(made.IsSequence() && made.size() == 2u);
    TEST_CHECK_FEQ(Yaml::as_float(made[0], 0.0f), 3.0f, 1e-6);
    TEST_CHECK_FEQ(Yaml::as_float(made[1], 0.0f), 4.0f, 1e-6);

    /* Vec2 and Color serialize as flow sequences. */
    std::string vec_dump = YAML::Dump(made);
    TEST_CHECK(!vec_dump.empty() && vec_dump[0] == '[');

    Yaml::Node made_color = Yaml::make_color(Engine::Color::make(0.1f, 0.2f, 0.3f, 0.4f));
    TEST_CHECK(made_color.IsSequence() && made_color.size() == 4u);
    std::string color_dump = YAML::Dump(made_color);
    TEST_CHECK(!color_dump.empty() && color_dump[0] == '[');

    Engine::Color::Color c = Yaml::as_color(made_color, Engine::Color::make(0.0f, 0.0f, 0.0f, 0.0f));
    TEST_CHECK_FEQ(c.r, 0.1f, 1e-6);
    TEST_CHECK_FEQ(c.g, 0.2f, 1e-6);
    TEST_CHECK_FEQ(c.b, 0.3f, 1e-6);
    TEST_CHECK_FEQ(c.a, 0.4f, 1e-6);
}
