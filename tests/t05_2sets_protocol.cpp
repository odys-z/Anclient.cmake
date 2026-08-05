#include "io/odysz/gen/anclient_settings.hpp"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN     // excludes OLE/COM/RPC stuff → no rpcndr.h
#define NOMINMAX                // prevents min/max macro clashes
#endif

#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>
#include <io/odysz/gen/doctier.hpp>
#include <io/odysz/gen/registry.hpp>

#include "io/odysz/semantier.h"
#include "io/odysz/clients.h"
#include "io/odysz/jclient/syn.h"
#include "../src/io/odysz/gen/anclient_settings.hpp"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

TEST(PROTOCOL_SETS, Double_PING) {
    // See synode-7.10-template-instance.json for configuration
    string setting_json = "settings/synode-7.10-localhost-instance.json";
    // string setting_json = "settings/synode-7.10-reddish-instance.json";

    AstMap syn_asts;
    JsonOpt syn_opts{&syn_asts};
    register_jserv(syn_asts, syn_opts);
    register_anclient_cmake(syn_asts, "ast/");
    register_semantier(syn_asts, "ast/");

    AstMap reg_asts;
    JsonOpt reg_opts{&reg_asts};
    register_jserv(reg_asts, reg_opts);
    register_anclient_cmake(reg_asts, "ast/");
    register_centralclientier(reg_asts, "ast/");

    OnError errctx = [](MsgCode c, const string& e, const vector<string>& a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    AnclientSettings settings;
    Anson::from_file(setting_json, settings);

    JServUrl jserv{settings.regiserv};
    ASSERT_EQ("regist-central", jserv.jprotocol.protocolpath);

    Clients::if_verbose = true;
    AnsonResp resp = Clients::pingLess(jserv, "Anson.cmake/test", "TEST Echo...", errctx);

    ASSERT_EQ("TEST Echo...", resp.m);
}
