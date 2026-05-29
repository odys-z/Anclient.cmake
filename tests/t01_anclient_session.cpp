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

#include "io/odysz/gen/semantier.hpp"
#include "io/odysz/semantier.h"
#include "io/odysz/clients.h"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

/**
 * @brief Requires starting jserv-sample at localhost:8080 before running the test.
 */
TEST(ANCLIENT, PING) {
    AstMap enums;
    JsonOpt opts{&enums};
    register_jserv(enums, opts);
    load_echoAst_ext(enums);
    register_semantier(enums, "./");

    OnError errctx = [](MsgCode c, string_view e, vector<string_view> &a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    JProtocol j{"jserv-sample"};
    JServUrl jserv{"http://127.0.0.1:8080", j};

    Clients::if_verbose = true;
    AnsonResp resp = Clients::pingLess(jserv, "Anson.cmake/test", "TEST Echo...", errctx);

    ASSERT_EQ("TEST Echo...", resp.m);
}

/**
 * @brief Requires starting jserv-sample at localhost:8080 before running the test.
 */
TEST(ANCLIENT, AnSESSION) {
    AstMap enums;
    JsonOpt opts{&enums};
    register_jserv(enums, opts);
    load_echoAst_ext(enums);
    register_semantier(enums, "./");

    OnError errctx = [](MsgCode c, string_view e, vector<string_view> &a) {
        anerror(std::format("[ERROR code {}], error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    JProtocol j{"jserv-sample"};
    JServUrl jserv{"http://127.0.0.1:8080", j};

    SessionClient *c = SessionClient::loginWithUri(jserv, "anclient.cmake", "ody", "123456", "cpp", errctx);

    ASSERT_EQ("ody", c->ssInf.uid);
    anlog("token: "s + c->ssInf.ssToken);
    ASSERT_EQ(2, LangExt::split(c->ssInf.ssToken, ':').size());

    vector<string_view> ss = LangExt::split(c->ssInf.ssToken, ':');
    string knowledge = AESHelper2::decrypt(string{ss[0]}, "123456", AESHelper2::decode64(string{ss[1]}));
    knowledge = LangExt::split(knowledge, ':')[1]; // What we got here is not the response of server, it's repacked token. So need decape the prefix

    ASSERT_TRUE(AESHelper2::verifyToken(c->ssInf.ssToken, knowledge, "ody", "123456"));

}
