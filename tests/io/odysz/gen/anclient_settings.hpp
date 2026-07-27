// #pragma once

// #include <entt/meta/factory.hpp>
// #include <entt/meta/meta.hpp>

// #include <io/odysz/anson.h>
// #include <io/odysz/jprotocol.h>
// #include <io/odysz/entt_jserv.h>
// #include <io/odysz/module/rs.h>



// namespace anson {

// class AnclientSettings : public anson::Anson {
// public:
//     inline static const std::string _type_ = "io.odysz.jclient.AnclientSettings";
//     string sysuri;
//     string synuri;
//     string jserv;
//     string jprotocolpath;
//     bool readonly_test;
//     string domain;
//     string admin;
//     string domain_token;
//     string regiserv;
//     string regiprotopath;
//     string centralPswd;

//     AnclientSettings(const string& protocolpath) : jprotocolpath(protocolpath) {
//         Type(_type_);
//     }

//     AnclientSettings() : AnclientSettings("jserv-album") {
//     }
// };

// inline static void register_anclientsettingsAst(AstMap & asts) {

//     AnsonAst * ast = createAST <AnclientSettings, AnsonAst> (
//         asts, Anson::_type_, map <string, AnsonField> {
//         {"sysuri", {.dataAnclass="string"} },
//         {"synuri", {.dataAnclass="string"} },
//         {"jserv", {.dataAnclass="string"} },
//         {"jprotocolpath", {.dataAnclass="string"} },
//         {"readonly_test", {.dataAnclass="bool"} },
//         {"domain", {.dataAnclass="string"} },
//         {"admin", {.dataAnclass="string"} },
//         {"domain_token", {.dataAnclass="string"} },
//         {"regiserv", {.dataAnclass="string"} },
//         {"regiprotopath", {.dataAnclass="string"} },
//         {"centralPswd", {.dataAnclass="string"} },
//        });

//     entt::meta_factory <anson::AnclientSettings> ()
//         .type(ast->enttypeid)
//         .base<Anson>()
//         .ctor<const string&>()
//         .ctor<>()

//         .data<&anson::AnclientSettings::sysuri>("sysuri")
//         .data<&anson::AnclientSettings::synuri>("synuri")
//         .data<&anson::AnclientSettings::jserv>("jserv")
//         .data<&anson::AnclientSettings::jprotocolpath>("jprotocolpath")
//         .data<&anson::AnclientSettings::readonly_test>("readonly_test")
//         .data<&anson::AnclientSettings::domain>("domain")
//         .data<&anson::AnclientSettings::admin>("admin")
//         .data<&anson::AnclientSettings::domain_token>("domain_token")
//         .data<&anson::AnclientSettings::regiserv>("regiserv")
//         .data<&anson::AnclientSettings::regiprotopath>("regiprotopath")
//         .data<&anson::AnclientSettings::centralPswd>("centralPswd")
//         ;

//         //
//         ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
//             if (ast->fields.contains(fieldname)) {
//                 auto& concrete = static_cast<const AnclientSettings&>(ans);
//                 if ("sysuri" == fieldname)
//                     return entt::forward_as_meta(concrete.sysuri);
//                 if ("synuri" == fieldname)
//                     return entt::forward_as_meta(concrete.synuri);
//                 if ("jserv" == fieldname)
//                     return entt::forward_as_meta(concrete.jserv);
//                 if ("jprotocolpath" == fieldname)
//                     return entt::forward_as_meta(concrete.jprotocolpath);
//                 if ("readonly_test" == fieldname)
//                     return entt::forward_as_meta(concrete.readonly_test);
//                 if ("domain" == fieldname)
//                     return entt::forward_as_meta(concrete.domain);
//                 if ("admin" == fieldname)
//                     return entt::forward_as_meta(concrete.admin);
//                 if ("domain_token" == fieldname)
//                     return entt::forward_as_meta(concrete.domain_token);
//                 if ("regiserv" == fieldname)
//                     return entt::forward_as_meta(concrete.regiserv);
//                 if ("regiprotopath" == fieldname)
//                     return entt::forward_as_meta(concrete.regiprotopath);
//                 if ("centralPswd" == fieldname)
//                     return entt::forward_as_meta(concrete.centralPswd);
//             }

//             if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
//                 AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
//                 return bast->get_field_instance(ans, fieldname);
//             }

//             anerror("get_field_instance<AnclientSettings>(): Failed to get entt instance (meta_any)");
//             return { };
//         };
// }

// }
