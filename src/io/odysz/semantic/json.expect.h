#pragma once

#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>

#include "io/oz/anclient/socketier.h"
#include "io/odysz/semantic/tier/docs.h"
#include "io/odysz/semantier.h"
#include "io/odysz/semanitcs.h"

namespace anson {

using namespace entt;
using namespace entt::literals;

inline void register_semantier(map<string, map<string, int>*>& enum_vals) {
    using namespace entt::literals;

    entt::meta_factory<anson::SessionInf>()
        .type("SessionInf"_hs)
        .base<anson::Anson>()
        .ctor<>()
        .ctor<const std::string&, const std::string&>()
        .ctor<const std::string&, const std::string&, const std::string>()
        .data<&anson::SessionInf::ssid>("ssid"_hs, "ssid")
        .data<&anson::SessionInf::uid>("uid"_hs, "uid")
        .data<&anson::SessionInf::roleId>("roleId"_hs, "roleId")
        .data<&anson::SessionInf::userName>("userName"_hs, "userName")
        .data<&anson::SessionInf::roleName>("roleName"_hs, "roleName")
        .data<&anson::SessionInf::ssToken>("ssToken"_hs, "ssToken")
        .data<&anson::SessionInf::seq>("seq"_hs, "seq")
        .data<&anson::SessionInf::device>("device"_hs, "device")
        ;

    entt::meta_factory<anson::WSEchoReq>()
        .type("WSEchoReq"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::WSEchoReq::echo>("echo"_hs, "echo")
        ;

    entt::meta_factory<anson::AnsonMsg<anson::WSEchoReq>>()
        .type("AnsonMsgWSEchoReq"_hs)
        .base<anson::Anson>()
        .ctor<std::string>()
        .data<&anson::AnsonMsg<anson::WSEchoReq>::port>("port"_hs, "port")
        .data<&anson::AnsonMsg<anson::WSEchoReq>::body>("body"_hs, "body")
        ;

    entt::meta_factory<anson::PathsPage>()
        .type("PathsPage"_hs)
        .base<anson::Anson>()
        .ctor<std::string>()
        ;

    entt::meta_factory<anson::DocsReq>()
        .type("DocsReq"_hs)
        .ctor<>()
        .ctor<const std::string&, const std::string&>()
        .base<anson::UserReq>()
        .data<&anson::DocsReq::synuri>("synuri"_hs, "synuri")
        .data<&anson::DocsReq::docTabl>("docTabl"_hs, "docTabl")
        .data<&anson::DocsReq::syncingPage>("syncingPage"_hs, "syncingPage")
        ;

    entt::meta_factory<anson::AnsonMsg<anson::DocsReq>>()
        .type("AnsonMsgDocsReq"_hs)
        .base<anson::Anson>()
        .ctor<std::string>()
        .data<&anson::AnsonMsg<anson::DocsReq>::port>("port"_hs, "port")
        .data<&anson::AnsonMsg<anson::DocsReq>::body>("body"_hs, "body")
        ;

    entt::meta_factory<anson::ShareFlag>()
        .type("ShareFlag"_hs, "ShareFlag")
        .base<anson::JavaEnum>()
        .ctor<std::string>() // TODO: Force to check
        ;

    {
        // ShareFlag::decode["prv"] = R"(🔒)";
        // ShareFlag::decode["pushing"] = R"(⇈)";
        // ShareFlag::decode["publish"] = R"(🌎)";
        // ShareFlag::decode["device"] = R"(📱)";
        // ShareFlag::decode["loc_remove"] = R"(Ⓛ)";
        // ShareFlag::decode["deny"] = R"(⛔)";
        // ShareFlag::decode["unknown"] = R"(⚠)";

        // ShareFlag::encode[R"(🔒)"] = "prv";
        // ShareFlag::encode[R"(⇈)"] = "pushing";
        // ShareFlag::encode[R"(🌎)"] = "publish";
        // ShareFlag::encode[R"(📱)"] = "device";
        // ShareFlag::encode[R"(Ⓛ)"] = "loc_remove";
        // ShareFlag::encode[R"(⛔)"] = "deny";
        // ShareFlag::encode[R"(⚠)"] = "unknown";
    }
}
}
