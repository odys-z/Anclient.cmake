#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>
#include <io/odysz/module/rs.h>



namespace anson {

class TestSettings : public anson::Anson {
public:
    inline static const std::string _type_ = "io.oz.syn.test.TestSettings";
    string jserv;
    bool readonly;
    string domain;
    string admin;
    string domain_token;
    string regiserv;
    string centralPswd;

    TestSettings() {
        Type(_type_);
    }
};

inline static void register_testsettingsAst(AstMap & asts) {

    AnsonAst * ast = createAST <TestSettings, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"jserv", {.dataAnclass="string"} },
        {"readonly", {.dataAnclass="bool"} },
        {"domain", {.dataAnclass="string"} },
        {"admin", {.dataAnclass="string"} },
        {"domain_token", {.dataAnclass="string"} },
        {"regiserv", {.dataAnclass="string"} },
        {"centralPswd", {.dataAnclass="string"} },
       });

    entt::meta_factory <anson::TestSettings> ()
        .type(ast->enttypeid)
        .base<Anson>()
        .ctor<>()

        .data<&anson::TestSettings::jserv>("jserv")
        .data<&anson::TestSettings::readonly>("readonly")
        .data<&anson::TestSettings::domain>("domain")
        .data<&anson::TestSettings::admin>("admin")
        .data<&anson::TestSettings::domain_token>("domain_token")
        .data<&anson::TestSettings::regiserv>("regiserv")
        .data<&anson::TestSettings::centralPswd>("centralPswd")
        ;
}

}
