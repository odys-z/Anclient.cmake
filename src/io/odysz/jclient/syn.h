#pragma once

#include "../clients.h"
#include "io/odysz/gen/doctier.hpp"

namespace anson {

class Semantier {
public:
    std::string mtabl;
};

class Doclientier : public Semantier {

public:
    string sysuri;
    const string synuri;
    // const Port port;
    const string doctbl;
    const OnError & err;

    SessionClient client;

    // Doclientier(const OnError& err) : client(JServUrl{"", {}}), err(err) {}

    /**
     * @brief Doclientier
     * @param doctbl
     * @param sysuri
     * @param synuri
     * @param onheartlink Design Memo: In c++, if this is provide by callback callers of openLink,
     * the onlink call back can be a dangling prointer. So force the constructor requiring it for safety.
     * @param onerr
     */
    Doclientier(const string &doctbl, const string &sysuri, const string &synuri, const OnLink& onheartlink, const OnError& onerr)
        : doctbl(doctbl), sysuri(sysuri), synuri(synuri), err(onerr), client(JServUrl{"", {}}, onheartlink, onerr) {
    }

    DocsResp synQueryPathsPage(const PathsPage &page, Port port) {
        AnsonHeader header = client.Header();
        header.Act("synclient.cpp", "query", "r/states", "query sync");

        DocsReq req {doctbl, {}, synuri};

        req.synuri = synuri;
        req.syncingPage = page;
        req.device = Device{page.device, "synode anclient.cmake test", "Ody@test"};
        req.a = DocsReq::A::selectSyncs;

        AnsonMsg<DocsReq> q{Port{Port::docstier}};
        q.Body(req);
        q.Header(header);

        req.pageInf.size = std::max(1, (int)page.clientPaths.size()); //page.end - page.start;

        DocsResp resp = client.commit<DocsResp>(q, err, true);

        return resp;
    }

    inline Doclientier* loginWithUri(const JServUrl &jserv,
           const string& uid, const string& pswd, const string& device, const OnError& err) {
        this->client.jserv = jserv;
        SessionClient::loginWithUri(this->client, sysuri, uid, pswd, device, err);
        return this;
    }
};

}
