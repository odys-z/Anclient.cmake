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
#include "io/odysz/jclient/syn.h"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

/**
 * @brief Requires starting jserv-sample at localhost:8080 before running the test.
 */
TEST(ANCLIENT, PING) {
    aninfo(R"([Note 13 June 2026]
    To run this test,
    start jserv-sample JsampleApp (port 8080).)");

    AstMap enums;
    JsonOpt opts{&enums};
    register_jserv(enums, opts);
    load_echoAst_ext(enums);
    register_semantier(enums, "ast/");

    OnError errctx = [](MsgCode c, const string& e, const vector<string>& a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    JServUrl jserv{"http://127.0.0.1:8080/jserv-sample"};
    ASSERT_EQ("jserv-sample", jserv.jprotocol.protocolpath);

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
    register_semantier(enums, "ast/");

    OnError errctx = [](MsgCode c, const string& e, const vector<string> &a) {
        anerror(std::format("[ERROR code {}], error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    string myuri = "anclient.cmake";
    JProtocol j{"jserv-sample"};
    JServUrl jserv{"http://127.0.0.1:8080", j};

    string plainkey = "123456";
    Doclientier doclient{"h_photos", "sys", "syn", errctx};
    anlog("1 ==================================================");
    doclient.loginWithUri(jserv, "ody", plainkey, "test.device", errctx);

    SessionClient c = doclient.client;
    ASSERT_EQ("ody", c.ssInf.uid);
    anlog("token: "s + c.ssInf.ssToken);
    ASSERT_EQ(2, LangExt::split(c.ssInf.ssToken, ':').size());

    vector<string_view> ss = LangExt::split(c.ssInf.ssToken, ':');
    string knowledge = AESHelper2::decrypt(string{ss[0]}, plainkey, AESHelper2::decode64(string{ss[1]}));
    knowledge = LangExt::split(knowledge, ':')[1]; // Not correct comments? What we got here is not the response of server, it's repacked token. So need decape the prefix
    ASSERT_TRUE(AESHelper2::verifyToken(c.ssInf.ssToken, knowledge, "ody", plainkey));

    // Verify header token handling
    anlog("2 ==================================================");
    ASSERT_FALSE(c.heartbeating);
    c.openLink(myuri, errctx, 2001);
    std::this_thread::sleep_for(3000ms);
    ASSERT_TRUE(c.heartbeating);
    c.stopbeat();
    this_thread::sleep_for(2400ms);
    ASSERT_FALSE(c.heartbeating);
}
