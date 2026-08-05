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

/** See synode-7.10-template-instance.json for configuration */
// string setting_json = "settings/synode-7.10-localhost-instance.json";

/**
 * @brief Requires starting jserv-sample at localhost:8080 before running the test.
 */
TEST(Registry, PING_Local) {
    string setting_json = "settings/synode-7.10-localhost-instance.json";

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_anclient_cmake(asts, "ast/");
    register_centralclientier(asts, "ast/");

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

TEST(Registry, PING_Central) {
    string setting_json = "settings/synode-7.10-reddish-instance.json";

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_anclient_cmake(asts, "ast/");
    register_centralclientier(asts, "ast/");

    OnError errctx = [](MsgCode c, const string& e, const vector<string>& a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    AnclientSettings settings;
    Anson::from_file(setting_json, settings);

    JServUrl jserv{settings.regiserv};
    ASSERT_EQ("regist-alpha", jserv.jprotocol.protocolpath);

    Clients::if_verbose = true;
    AnsonResp resp = Clients::pingLess(jserv, "Anson.cmake/test", "TEST Echo...", errctx, EchoReq::A::echo);

    ASSERT_EQ("echo", resp.m);
}
