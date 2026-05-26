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
    string protocolpath;
    bool readonly;
    string domain;
    string admin;
    string domain_token;
    string regiserv;
    string centralPswd;

    TestSettings(const string& protocolpath) : protocolpath(protocolpath) {
        Type(_type_);
    }

    TestSettings() : TestSettings("jserv-album") {
    }
};

inline static void register_testsettingsAst(AstMap & asts) {

    AnsonAst * ast = createAST <TestSettings, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"jserv", {.dataAnclass="string"} },
        {"protocolpath", {.dataAnclass="string"} },
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
        .ctor<const string&>()
        .ctor<>()

        .data<&anson::TestSettings::jserv>("jserv")
        .data<&anson::TestSettings::protocolpath>("protocolpath")
        .data<&anson::TestSettings::readonly>("readonly")
        .data<&anson::TestSettings::domain>("domain")
        .data<&anson::TestSettings::admin>("admin")
        .data<&anson::TestSettings::domain_token>("domain_token")
        .data<&anson::TestSettings::regiserv>("regiserv")
        .data<&anson::TestSettings::centralPswd>("centralPswd")
        ;

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const TestSettings&>(ans);
                if ("jserv" == fieldname)
                    return entt::forward_as_meta(concrete.jserv);
                if ("protocolpath" == fieldname)
                    return entt::forward_as_meta(concrete.protocolpath);
                if ("readonly" == fieldname)
                    return entt::forward_as_meta(concrete.readonly);
                if ("domain" == fieldname)
                    return entt::forward_as_meta(concrete.domain);
                if ("admin" == fieldname)
                    return entt::forward_as_meta(concrete.admin);
                if ("domain_token" == fieldname)
                    return entt::forward_as_meta(concrete.domain_token);
                if ("regiserv" == fieldname)
                    return entt::forward_as_meta(concrete.regiserv);
                if ("centralPswd" == fieldname)
                    return entt::forward_as_meta(concrete.centralPswd);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<TestSettings>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

}
