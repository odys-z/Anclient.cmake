#pragma once

// #include <io/odysz/jprotocol.h>
// #include <vector>
// #include <map>

#include "../clients.h"

#include "io/odysz/gen/doctier.hpp"


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

    SessionClient *client;

    Doclientier(const string &doctbl, const JServUrl &jserv, const OnError& onerr)
        : doctbl(doctbl), err(onerr) {
        client = new SessionClient(jserv);
    }

    DocsResp synQueryPathsPage(const PathsPage &page, Port port) {
        AnsonHeader header = client->Header();
        header.Act("synclient.cpp", "query", "r/states", "query sync");

        DocsReq req {doctbl, {}, ""};

        req.uri = sysuri;
        req.synuri = synuri;
        req.syncingPage = page;
        req.device = Device{page.device, "synode anclient.cmake test", "Ody@test"};
        req.a = DocsReq::A::selectSyncs;

        AnsonMsg<DocsReq> q{Port::docstier};
        q.Body(req);
        q.Header(header);

        DocsResp resp = client->commit<DocsResp>(q, err, true);

        return resp;
    }

    inline Doclientier* loginWithUri(const JServUrl &jserv, const string& uri,
                        const string& uid, const string& pswd, const string& device, const OnError& err) {

        this->client = SessionClient::loginWithUri(jserv, uri, uid, pswd, device, err);
        return this;
    }
};

}
