#pragma once

#include "io/oz/syn/test.h"
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
#include <gen/jserv.hpp>

#include "io/odysz/jclient/syn.h"
#include "gen/doctier.hpp"
#include "io/odysz/semantic/meta.h"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

namespace anson {
void DocsReq::format(IFileDescriptor p) {
    device = Device({}, {}, p.device);
    doc.device = p.device;
    doc.clientpath = p.fullpath();
}

static void verifyPathsPage(Doclientier &clientier, const string &entityName, vector<string> paths) {
    PathsPage pths(clientier.client.ssInf.device, 0, 1);
    set<string> pathpool;
    for (string pth : paths) {
        pths.clientPaths.insert({pth, {}}); // what's the querying flag?
        pathpool.insert(pth);
    }

    DocsResp rep = clientier.synQueryPathsPage(pths, Port::docstier);

    PathsPage pthpage = rep.syncingPage;

    ASSERT_EQ(clientier.client.ssInf.device, pthpage.device);
    ASSERT_EQ(paths.size(), pthpage.clientPaths.size());

    for (string pth : paths)
        pathpool.erase(pth);

    ASSERT_EQ(0, pathpool.size());
}
}

TEST(Syncpage, Query) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    load_ansessionreqAst(asts, "ast-cpy/session-req.ast.json");
    load_ansessionrespAst(asts, "ast-cpy/session-resp.ast.json");

    TestSettings settings;
    bool result = Anson::from_file("settings/synode-7.10-reddish-instance.json", settings);

    PhotoMeta phm{"opaque to client"};
    JProtocol jprotocol;
    JServUrl jserv{settings.jserv, jprotocol};

    OnError onErr = [](MsgCode c, string_view e, vector<string_view> &a) {
        anerror(std::format("[ERROR code {}], error: {}", AnsonJavaEnumAst::name<MsgCode>(c), e));
    };

    Doclientier doclient{phm.tbl, jserv, onErr};

    verifyPathsPage(doclient, phm.tbl, {});
}
