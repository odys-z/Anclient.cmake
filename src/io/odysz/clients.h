#pragma once

#include <functional>
#include <string>

#include "semantier.h"
// #include "io/odysz/semantic/tier/docs.h"

using namespace std;

namespace anson {

class SessionClient {

public:
    string jservrt;

    SessionClient(string jserv) : jservrt(jserv) {
    }

    template<typename R>
    AnsonResp commit(AnsonMsg<R> req, OnError err) {
        AnsonResp resp;
        return resp;
    }
};

class InsecureClient : public SessionClient {
public:
    InsecureClient(string jserv) : SessionClient(jserv) {}
};

/**
 * TODO move to a stand alone protocol tier?
 * JProtocol.OnProgress
 */
using OnProgress = std::function<void(const string& path, std::string status)>;

class Clients {
public:

    inline static AnsonResp pingLess(JProtocol& protocol, string uri, string msg, OnError err) {
        EchoReq req;
        req.echo = msg;
        #ifdef ANCLIENT_BUILD_TESTS
        cout << "[Clinets.pingLess.body] " << req.toString();
        #endif

        AnsonMsg<EchoReq> anmsg(Port(Port::echo), req);
        // string reqs = anmsg.toBlock<AnsonMsg<EchoReq>>();
        #ifdef ANCLIENT_BUILD_TESTS
        cout << "[Clinet.pingLess Msg] " << reqs.c_str();
        #endif

        InsecureClient client{protocol.prtocolpath};
        return client.commit(anmsg, err);
    }
};

// class WSClient {

// };

// class Doclientier : public WSClient {

//     string device;
// public:
//     Doclientier(string device) : device(device) {}

//     void push_files(const map<string, vector<string>>& paths, OnProgress onprc) {
//         DocsReq reqbd{device, DocsReq::A::requestSyn};
//         for (auto& [pth, stas] : paths) {
//             reqbd.syncingPage.append(pth, {ShareFlag::publish});
//             onprc(pth, ShareFlag::pushing);
//         }

//         AnsonMsg<DocsReq> req{Port::docstier};
//         req.Body(reqbd);
//     }
// };
}
