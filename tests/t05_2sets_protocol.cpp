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

/** Start up regist-jserv at port 1900 (synode-7.10-...-instance.json/regiserv */
TEST(PROTOCOL_SETS, Double_PING) {
    // See synode-7.10-template-instance.json for configuration
    string sets_local = "settings/synode-7.10-localhost-instance.json";
    string sets_centr = "settings/synode-7.10-reddish-instance.json";

    AstMap local_asts;
    JsonOpt local_opts{&local_asts};
    register_jserv(&local_opts);
    register_anclient_cmake(&local_opts, "ast/");
    register_semantier(&local_opts, "ast/");

    AstMap centr_asts;
    JsonOpt centr_opts{&centr_asts};
    register_jserv(&centr_opts);
    register_anclient_cmake(&centr_opts, "ast/");
    register_centralclientier(&centr_opts, "ast/");

    OnError errctx = [&local_opts](MsgCode c, const string& e, const vector<string>& a) {
        anerror(std::format("Error code {}, error: {}", MsgCode::to_string(c.valeur), e));
    };

    AnclientSettings settings;
    Anson::from_file(sets_local, settings, &local_opts);

    JServUrl local_jserv{settings.regiserv, &local_opts};
    ASSERT_EQ("regist-central", local_jserv.jprotocol.protocolpath);

    Clients::if_verbose = true;
    AnsonResp resp = Clients::pingLess(local_jserv, "Anson.cmake/test", "TEST Echo...", errctx);

    ASSERT_EQ("TEST Echo...", resp.m);
}
