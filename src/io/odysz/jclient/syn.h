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

    Doclientier(const string &doctbl, const string &sysuri, const string &synuri, const OnError& onerr)
        : doctbl(doctbl), sysuri(sysuri), synuri(synuri), err(onerr), client(JServUrl{"", {}}) {
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

    // void push_files(map<string, vector<string>> paths, OnProgress onprc) {
    //     // // PathsPage sync_page(device);
    //     // DocsReq reqbd{device, DocsReq::A::requestSyn};
    //     for (auto kv : paths) {
    //         anlog(kv.first);
    //     }
    // }

};

}
