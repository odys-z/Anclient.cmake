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

/**
 * Requires Synodict Jclient 0.1.9.
 *
 * Start up regist-jserv at port 1900, see synode-7.10-...-instance.json/regiserv
 */
TEST(T06_REGSTRY, Query) {
    // See synode-7.10-template-instance.json for configuration
    string setting_json = "settings/synode-7.10-localhost-instance.json";
    // string setting_json = "settings/synode-7.10-reddish-instance.json";

    AstMap reg_asts;
    JsonOpt reg_opts{&reg_asts};
    register_jserv(&reg_opts);
    register_anclient_cmake(&reg_opts, "ast/");
    register_semantier(&reg_opts, "ast");
    register_centralclientier(&reg_opts, "ast/");

    OnError errctx = [&reg_opts](MsgCode c, const string& e, const vector<string>& a) {
        anerror(std::format("Error code {}, error: {}", AnsonJavaEnumAst::name<MsgCode>(&reg_opts, c), e));
    };

    OnLink onbeat = [](connect_state conn) {
        aninfo("connect_state: "s + conn.registlink);
    };

    AnclientSettings settings;
    Anson::from_file(setting_json, settings, &reg_opts);

    SynodeConfig node_cfg {"test-id", "device"};

    JServUrl jserv{settings.regiserv, &reg_opts};
    ASSERT_EQ("regist-central", jserv.jprotocol.protocolpath);

    Clients::if_verbose = true;
    AnsonResp resp = Clients::pingLess(jserv, "Anson.cmake/test", "TEST Echo...", errctx);

    RegistryClient client{settings, jserv, onbeat, errctx};

    /*
     * Semantic-Network\registration\jserv\src\main\webapp\regist-vol\system.sqlite
     *  c_syndomx
        market|orgid |domid    |domname        |stat|nonsql|optime|oper|
        ------+------+---------+---------------+----+------+------+----+
        alpha |org.my|edu-x    |my edu-x       |    |      |      |    |
        alpha |qqhome|reddish-2|alpha reddish-2|    |      |      |    |
        alpha |pmking|pm-1     |alpha pm-1     |    |      |      |    |
        alpha |pmking|pm-2     |alpha pm-2     |    |      |      |    |

     * c_synodes
        market|orgid |domx |sid      |mode|stat|jserv                                |ip       |port|nonsql|optime             |oper     |
        ------+------+-----+---------+----+----+-------------------------------------+---------+----+------+-------------------+---------+
        my    |org.my|edu-x|edu-x-hub|hub |h   |http://182.--.--.--:----0/jserv-album|127.0.0.1|   0|      |2025-10-25 09:48:38|edu-x-hub|
        my    |org.my|edu-x|edu-x-1  |    |p   |http://192.168.0.201:---1/jserv-album|127.0.0.1|   0|      |2025-10-25 09:42:29|edu-x-1  |
        my    |org.my|edu-x|edu-x-2  |    |p   |http://192.168.0.201:---2/jserv-album|127.0.0.1|   0|      |2025-10-25 09:40:55|edu-x-2  |
     *
     */
    client.market = "alpha";
    client.orgid  = "org.my";
    client.orgname  = "my edu-x";

    ASSERT_FALSE(settings.sysuri.empty());
    ASSERT_FALSE(settings.admin.empty());
    ASSERT_FALSE(settings.centralPswd.empty());
    ASSERT_FALSE(settings.device.empty());
    client.loginWithUri(settings.sysuri, settings.admin, settings.centralPswd, "device/test", errctx);

    ASSERT_FALSE(client.ssInf.ssid.empty());

    aninfo(std::format("[*** *** step 1 *** ***] Login verified, {} of {}", settings.admin, settings.domain));

    string testorg = "test.org";
    std::promise<RegistResp> p;
    auto provide = p.get_future();

    client.asyquery_orgdoms(testorg, [&](AnsonResp& rep) mutable {
        RegistResp& r = static_cast<RegistResp&>(rep);
        p.set_value(r);
    }, errctx);

    ASSERT_EQ(provide.wait_for(std::chrono::seconds(15)), std::future_status::ready)
        << "timed out (1)";
    RegistResp res = provide.get();

    ASSERT_EQ(res.diction.org.orgId, client.orgid);
    ASSERT_EQ(1, res.orgDomains.size());
    ASSERT_EQ("edu-x", res.orgDomains[0]);

    aninfo(std::format("[*** *** step 2 *** ***] Domains list retrieved, {}", res.orgDomains[0]));

    std::promise<RegistResp> q;
    provide = q.get_future();
    client.asyquery_domconfig(testorg, res.orgDomains[0], [&](AnsonResp& rep) mutable {
        RegistResp& r = static_cast<RegistResp&>(rep);
        q.set_value(r);
    }, errctx);

    ASSERT_EQ(provide.wait_for(std::chrono::seconds(15)), std::future_status::ready)
        << "timed out (2)";
    res = provide.get();

    ASSERT_EQ(res.diction.org.orgId, client.orgid);
    // ASSERT_EQ(1, res.orgDomains.size());
    ASSERT_EQ(3, res.diction.peers.size());
    ASSERT_EQ("edu-x-hub", res.diction.peers[0].synid);
    ASSERT_EQ("edu-x-1", res.diction.peers[1].synid);
    ASSERT_EQ("edu-x-2", res.diction.peers[2].synid);

    ASSERT_EQ("edu-x", res.diction.peers[0].domain);
    ASSERT_EQ("edu-x", res.diction.peers[1].domain);
    ASSERT_EQ("edu-x", res.diction.peers[2].domain);

    ASSERT_TRUE(std::regex_search(res.diction.peers[0].jserv, std::regex("/jserv-album")));
    ASSERT_TRUE(std::regex_search(res.diction.peers[1].jserv, std::regex("/jserv-album")));
    ASSERT_TRUE(std::regex_search(res.diction.peers[2].jserv, std::regex("/jserv-album")));

    aninfo(std::format("[*** *** step 3 *** ***] Domain Synodes retrieved, edu-x:\n{}\n{}\n{}",
           res.diction.peers[0].jserv, res.diction.peers[1].jserv, res.diction.peers[2].jserv));
}
