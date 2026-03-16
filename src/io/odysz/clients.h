#pragma once

#include <functional>
#include <string>
#include <cpr/cpr.h>

#include <io/odysz/jprotocol.h>

// #include "io/odysz/semantic/tier/docs.h"

using namespace cpr;

namespace anson {

class SessionClient {

public:
    JServUrl jserv;

    SessionClient(string jserv) : jserv(jserv) { }

    template<typename R>
    AnsonResp commit(AnsonMsg<R> req, OnError err) {
        std::stringstream ss;
        JsonOpt opt;
        req.toBlock(ss, opt);

        stringstream ssview = std::move(ss);

        andebug(jserv.jserv());
        andebug(ss.view());

        cpr::Response r = cpr::Post(
            cpr::Url{jserv.jserv()},
            cpr::Body{std::move(ssview.str())},
            cpr::Header{{"Content-Type", "application/json"}}
        );

        AnsonResp resp;

        if (r.status_code == 201) { // 201 is 'Created'
            std::cout << "Success!" << std::endl;
            std::cout << "Response from server: " << r.text << std::endl;

            EnTTSaxParser<AnsonResp> handler(resp);
            bool result = nlohmann::json::sax_parse(r.text, &handler);

            if (!result) {
                resp.Code(MsgCode::exGeneral)
                    .msg("Parsing response failed: " + r.text);
                vector<string_view> args;
                err(MsgCode::exGeneral, r.error.message, args);
            }
        } else {
            std::cerr << "Error: " << r.status_code << " - " << r.error.message << std::endl;
            resp.Code(MsgCode::exIo)
                .msg(r.error.message);
            vector<string_view> args;
            err(MsgCode::exIo, string_view(r.error.message), args);
        }

        return resp;
    }
};

class InsecureClient : public SessionClient {
public:
    InsecureClient(string jserv) : SessionClient(jserv) {}
};

class Clients {
    static JProtocol protocol;

public:
    inline static OnError err = [] (MsgCode c, string_view m, vector<string_view> args) {
        anerror(format("code: {}, msg:\n{}", int(c), m));
        anerror(args);
    };

    inline static void setup(JProtocol p) {
        protocol = p;
    }

    inline static AnsonResp pingLess(string uri, string msg, OnError err=Clients::err) {
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

        InsecureClient client{protocol.protocolpath};
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
