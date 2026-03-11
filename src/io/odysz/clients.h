#pragma once

#include <functional>
#include <string>

#include "semantier.h"
#include "io/odysz/semantic/tier/docs.h"

using namespace std;

namespace anson {


/**
 * TODO move to a stand alone protocol tier?
 * JProtocol.OnProgress
 */
using OnProgress = std::function<void(const string& path, std::string status)>;

class Clients {
public:

    inline static AnsonResp pingLess(string uri, string msg, OnError) {
        EchoReq req;
        req.echo = msg;

        qDebug() << "[Qt Clinet Ping].body" << req.toBlock().c_str();

        AnsonMsg<WSEchoReq> anmsg(Port(Port::echo), req);
        // anmsg.body.push_back(req);
        string reqs = anmsg.toBlock<AnsonMsg<WSEchoReq>>();
        qDebug() << "[Qt Clinet Ping]" << reqs.c_str();
        skt.sendTextMessage(reqs.c_str());
    }
};

class WSClient {

};

class Doclientier : public WSClient {

    string device;
public:
    Doclientier(string device) : device(device) {}

    void push_files(const map<string, vector<string>>& paths, OnProgress onprc) {
        DocsReq reqbd{device, DocsReq::A::requestSyn};
        for (auto& [pth, stas] : paths) {
            reqbd.syncingPage.append(pth, {ShareFlag::publish});
            onprc(pth, ShareFlag::pushing);
        }

        AnsonMsg<DocsReq> req{Port::docstier};
        req.Body(reqbd);
    }
};
}
