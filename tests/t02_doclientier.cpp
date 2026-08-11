#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN     // excludes OLE/COM/RPC stuff → no rpcndr.h
#define NOMINMAX                // prevents min/max macro clashes
#endif

#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/entt_jserv.h>

#include "io/odysz/jclient/syn.h"
#include "io/odysz/gen/doctier.hpp"
#include "io/odysz/semantic/meta.h"
#include "../src/io/odysz/gen/anclient_settings.hpp"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

namespace anson {
void DocsReq::format(const IFileDescriptor& p) {
    device = Device({}, {}, p.device);
    doc.device = p.device;
    doc.clientpath = p.fullpath();
}

static void verifyPathsPage(Doclientier &clientier, const string &entityName, vector<string> paths) {
    PathsPage pths(clientier.client.ssInf.device, 0, paths.size() + 1);
    set<string> pathpool;
    for (const string& pth : paths) {
        pths.clientPaths.insert({pth, {}}); // what's the querying flag?
        pathpool.insert(pth);
    }

    DocsResp rep = clientier.synQueryPathsPage(pths);

    PathsPage pthpage = rep.syncingPage;

    ASSERT_EQ(clientier.client.ssInf.device, rep.device.id);
    ASSERT_EQ(paths.size(), pthpage.clientPaths.size());

    for (const string& pth : paths)
        pathpool.erase(pth);

    ASSERT_EQ(0, pathpool.size());
}

void ExpSyncDoc::format(const AnResultset & rs) {}

void DocsReq::format(const IFileDescriptor &, const string) {}
}

/** See synode-7.10-template-instance.json for configuration */
string setting_json = "settings/synode-7.10-reddish-instance.json";

TEST(T02_Syncpage, Query) {
    aninfo(R"([Note 13 June 2026]
    To run this test,
    start synode at port 8957,
    setup )" + setting_json);

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(&opts);
    register_semantier(&opts, "ast/");

    filesystem::path folder_path{"ast/"};
    aninfo((folder_path/"docsreq.ast.json").string());
    filesystem::path docreq_ast{folder_path/"docsreq.ast.json"};
    ASSERT_EQ(".json", docreq_ast.extension());
    ASSERT_TRUE(filesystem::exists(docreq_ast));

    register_doctier(&opts, "ast/");
    register_anclientsettingsAst(&opts);

    AnclientSettings settings;
    bool result = Anson::from_file(setting_json, settings, &opts);
    ASSERT_TRUE(result);
    aninfo(std::format("synode {} : {} : {}", settings.jserv, settings.admin, settings.domain_token));

    PhotoMeta phm{"opaque to client"};
    JProtocol jprotocol{"jserv-album", &opts};
    // jprotocol.setup(settings.jprotocolpath, Port::docstier);
    JServUrl jserv{settings.jserv, jprotocol};

    OnError onErr = [&opts](MsgCode c, const string &e, const vector<string> &a) {
        anerror(std::format("[ERROR code {}], error: {}", AnsonJavaEnumAst::name<MsgCode>(&opts, c), e));
    };

    OnLink onlink = [](const connect_state& newstate) {};
    Doclientier doclient{phm.tbl, "/sys", "/syn", jserv, onlink, onErr};
    doclient.loginWithUri(settings.admin, settings.domain_token, "cpp-test", onErr);

    SessionInf ssinf = doclient.client.ssInf;
    ASSERT_EQ(settings.admin, ssinf.uid);
    ASSERT_EQ(8, ssinf.ssid.length());
    ASSERT_EQ(string{"eV+Rb6ZZSEDn4dl6KX81yf8Gp0XEsGf5+WbsdC5+aD8=:qtPxqG61FzMweHX1fDD0ew=="}.length(),
              ssinf.ssToken.length());

    verifyPathsPage(doclient, phm.tbl, {});

    /*
     * INSERT INTO h_photos
     * (pid,family,folder,docname,uri,pdate,device,clientpath,shareby,sharedate,tags,geox,geoy,exif,mime,filesize,css,shareflag,oper,opertime,syncstamp,io_oz_synuid)
     * VALUES
     * ('should be died','inforise','folder.cmake','infor-17','',NULL,'cpp-test','path/a','ody','now()',NULL,0.0,0.0,NULL,NULL,0,NULL,'prv','ody','now()','2026-06-01 03:23:02','should-died-0001');
     */
    verifyPathsPage(doclient, phm.tbl, {"path/a"});
}
