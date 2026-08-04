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

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

/**
 * @brief Requires starting jserv-sample at localhost:8080 before running the test.
 */
TEST(Registry, PING) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    load_echoAst_ext(asts);
    register_centralclientier(asts, "ast/");

    OnError errctx = [](MsgCode c, const string& e, const vector<string>& a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    JServUrl jserv{"http://127.0.0.1:8080/jserv-sample"};
    ASSERT_EQ("jserv-sample", jserv.jprotocol.protocolpath);

    Clients::if_verbose = true;
    AnsonResp resp = Clients::pingLess(jserv, "Anson.cmake/test", "TEST Echo...", errctx);

    ASSERT_EQ("TEST Echo...", resp.m);
}
