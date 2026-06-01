#pragma once

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
#include <io/odysz/semantier.h>

#include "io/odysz/jclient/syn.h"
#include "io/odysz/gen/doctier.hpp"
#include "io/odysz/semantic/meta.h"
#include "io/oz/syn/test_gen.hpp"

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
    PathsPage pths(clientier.client->ssInf.device, 0, paths.size());
    set<string> pathpool;
    for (string pth : paths) {
        pths.clientPaths.insert({pth, {}}); // what's the querying flag?
        pathpool.insert(pth);
    }

    DocsResp rep = clientier.synQueryPathsPage(pths, Port::docstier);

    PathsPage pthpage = rep.syncingPage;

    ASSERT_EQ(clientier.client->ssInf.device, rep.device);
    ASSERT_EQ(paths.size(), pthpage.clientPaths.size());

    for (string pth : paths)
        pathpool.erase(pth);

    ASSERT_EQ(0, pathpool.size());
}

void ExpSyncDoc::format(const AnResultset & rs) {}

void DocsReq::format(const IFileDescriptor &, const string) {}
}

TEST(Syncpage, Query) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_semantier(asts, "./");
    register_doctier(asts, "ast/");
    register_testsettingsAst(asts);

    TestSettings settings;
    bool result = Anson::from_file("settings/synode-7.10-reddish-instance.json", settings);

    PhotoMeta phm{"opaque to client"};
    JProtocol jprotocol;
    jprotocol.setup(settings.protocolpath, Port::docstier);
    JServUrl jserv{settings.jserv, jprotocol};

    OnError onErr = [](MsgCode c, string_view e, vector<string_view> &a) {
        anerror(std::format("[ERROR code {}], error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    Doclientier doclient{phm.tbl, "/sys", "/syn", onErr};
    doclient.loginWithUri(jserv, settings.admin, settings.domain_token, "cpp-test", onErr);

    SessionInf ssinf = doclient.client->ssInf;
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
