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
#include "io/odysz/semantier.h"
#include "io/odysz/clients.h"
#include "expect/jserv.hpp"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

TEST(ANCLIENT, PING) {
    AstMap enums;
    JsonOpt opts{&enums};
    register_jserv(enums, opts);
    load_echoAst_ext(enums);
    load_ansessionreqAst(enums, "ast-cpy/session-req.ast.json");
    load_ansessionrespAst(enums, "ast-cpy/session-resp.ast.json");


    OnError errctx = [](MsgCode c, string_view e, vector<string_view> &a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    // Clients::setup(JProtocol{"jserv-album"});
    JProtocol j{"jserv-album"};
    JServUrl jserv{"http://127.0.0.1:8961", j};
    AnsonResp resp;
    Clients::pingLess(resp, jserv, "Anson.cmake/test", "TEST(ANCLIENT) ping...", errctx);

    ASSERT_EQ("", resp.m);
}
