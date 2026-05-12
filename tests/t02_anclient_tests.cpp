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
#include <gen/jserv.hpp>

#include "io/odysz/semantier.h"
#include "io/odysz/clients.h"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

// TEST(ANCLIENT, PING) {
//     AstMap enums;
//     JsonOpt opts{&enums};
//     register_jserv(enums, opts);
//     load_echoAst_ext(enums);
//     // load_ansessionreqAst(enums, "ast-cpy/session-req.ast.json");
//     // load_ansessionrespAst(enums, "ast-cpy/session-resp.ast.json");


//     OnError errctx = [](MsgCode c, string_view e, vector<string_view> &a) {
//         anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
//     };

//     JProtocol j{"jserv-sample"};
//     JServUrl jserv{"http://127.0.0.1:8080", j};

//     Clients::if_verbose = true;
//     AnsonResp resp = Clients::pingLess(jserv, "Anson.cmake/test", "TEST Echo...", errctx);

//     ASSERT_EQ("TEST Echo...", resp.m);
// }

TEST(ANCLIENT, AnSESSION) {
    AstMap enums;
    JsonOpt opts{&enums};
    register_jserv(enums, opts);
    load_echoAst_ext(enums);
    load_ansessionreqAst(enums, "ast-cpy/session-req.ast.json");
    load_ansessionrespAst(enums, "ast-cpy/session-resp.ast.json");


    OnError errctx = [](MsgCode c, string_view e, vector<string_view> &a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    JProtocol j{"jserv-sample"};
    JServUrl jserv{"http://127.0.0.1:8080", j};

    SessionClient *c = SessionClient::loginWithUri(jserv, "anclient.cmake", "ody", "123456", "cpp", errctx);

    ASSERT_EQ("ody", c->ssInf.uid);
}
