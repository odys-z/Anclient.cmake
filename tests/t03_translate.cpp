#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/anson.h"
#include "../src/io/odysz/gen/anclient_settings.hpp"

using namespace anson;
static AstMap asts;
static JsonOpt contxt{&asts};

/**
 * StringValues is actually moved back to anson.cmake as LangSting.
 *
 * But the code generated in tests works fine. Leave it here for a while.
 * @brief langs
 */
StringValues my_langs;
std::string operator""_ans_(const char* str, std::size_t len) {
    if (my_langs.en.size() == 0 && my_langs.local.size() == 0)
        anerror("String values of langs is not loaded yet.");
    std::string key(str, len);
    if (my_langs.local.contains(key)) {
        auto it = my_langs.local.find(key);
        return it->second;
    }

    auto it = my_langs.en.find(key);
    return it != my_langs.en.end() ? it->second : key;
}


TEST(Translate, Deprecated_by_LangString) {
    register_asts(asts);
    register_stringvaluesAst(&contxt);

    // StringValues langs;
    Anson::from_file("settings/strings.json", my_langs, &contxt);
    ASSERT_EQ("English", my_langs.en["eng"]);
    ASSERT_EQ("English", "eng"_ans_);
    ASSERT_EQ("Chinese", my_langs.en["chn"]);
    ASSERT_EQ("Chinese", "chn"_ans_);
    ASSERT_EQ("jp", "jp"_ans_);

    Anson::from_file("settings/strings-zh.json", my_langs, &contxt);
    ASSERT_EQ("English", my_langs.en["eng"]);
    ASSERT_EQ("英语", my_langs.local["eng"]);
    ASSERT_EQ("英语", "eng"_ans_);

    ASSERT_EQ("Chinese", my_langs.en["chn"]);
    ASSERT_EQ("中文", my_langs.local["chn"]);
    ASSERT_EQ("中文", "chn"_ans_);
    ASSERT_EQ("jp", "jp"_ans_);
}
