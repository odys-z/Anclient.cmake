#pragma once

// #include <io/odysz/jprotocol.h>
// #include <vector>
// #include <map>

#include "../clients.h"

#include "gen/doctier.hpp"


namespace anson {

class Semantier {
public:
    string mtabl;

};


class Doclientier : public Semantier {

public:
    const string sysuri;
    const string synuri;
    const Port port;
    const string doctbl;
    const OnError & err;

    SessionClient client;

    Doclientier(const string &doctbl, const JServUrl &jserv, const OnError& onerr)
        : doctbl(doctbl), client(jserv), err(onerr) {}

    DocsResp synQueryPathsPage(const PathsPage &page, Port port) {
        AnsonHeader header{client.ssInf.ssid, client.ssInf.uid, client.ssInf.ssToken};
        header.Act("synclient.java", "query", "r/states", "query sync");

        DocsReq req {doctbl, {}, ""};

        req.uri = sysuri;
        req.synuri = synuri;
        req.syncingPage = page;
        req.device = Device{page.device, "anclient.cmake test", "Ody@test"};
        req.a = DocsReq::A::selectSyncs;

        AnsonMsg<DocsReq> q;
        q.Body(req);
        q.Header(header);

        DocsResp resp = client.commit<DocsResp>(q, err);

        return resp;
    }
};

}
