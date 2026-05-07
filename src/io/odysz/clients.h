#pragma once

#include <functional>
#include <string>
#include <cpr/cpr.h>

#include <io/odysz/reflect.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/semantier.h>

// #include "io/odysz/semantic/tier/docs.h"

using namespace cpr;

namespace anson {

class SessionClient {

public:
    JServUrl jserv;

    SessionClient(const JServUrl &jserv) : jserv(jserv) { }

    template<typename R>
    AnsonResp& commit(AnsonResp &rep, AnsonMsg<R> &req, OnError err) {
        std::stringstream ss;
        req.toBlock(ss, *IJsonable::contxt_ptr);

        stringstream ssview = std::move(ss);

        string url{std::format("{}/{}", jserv.jserv(), req.port.url())};
        anlog(url);
        andebug(ss.view());

        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Proxies{{"https", ""}, {"http", ""}},
            cpr::Verbose{true},
            cpr::Body{std::move(ssview.str())},
            cpr::Header{{"Content-Type", "application/json"},
            {"User-Agent", "Mozilla/5.0 (Anclient.cmake)"}}
        );

        // AnsonResp resp;
        AnsonMsg<AnsonResp> resp;
        resp.Body(rep);

        if (r.status_code == 201 || r.status_code == 200) { // 201 is 'Created'
            std::cout << "Success!" << std::endl;
            std::cout << "Response from server: " << r.text << std::endl;

            // EnTTSaxParser<AnsonResp> handler(resp);
            // bool result = nlohmann::json::sax_parse(r.text, &handler);
            bool result = Anson::from_json(r.text, resp);

            if (!result) {
                resp.Code(MsgCode::Code::exGeneral);
                // resp.body[0]->msg("Parsing response failed: " + r.text);
                AnsonResp *bd = new AnsonResp{};
                bd->msg("Parsing response failed: " + r.text);
                resp.Body(*bd);
                vector<string_view> args;
                err(MsgCode::Code::exGeneral, r.error.message, args);
            }
        } else {
            std::cerr << "Error - Clients::commit(): " << r.status_code << " - " << r.error.message << std::endl;
            resp.Code(MsgCode::Code::exIo);
            // resp.body[0]->msg(r.error.message);
            AnsonResp *bd = new AnsonResp{};
            bd->msg(r.error.message);
            resp.Body(*bd);
            vector<string_view> args;
            err(MsgCode::Code::exIo, string_view(r.error.message), args);
        }

        // return resp.Body();
        return rep;
    }
};

class InsecureClient : public SessionClient {
public:
    InsecureClient(const JServUrl &jserv) : SessionClient(jserv) {}
};

class Clients {
    inline static JProtocol protocol;

public:
    inline static OnError err = [] (MsgCode c, string_view m, vector<string_view> args) {
        anerror(format("code: {}, msg:\n{}", AnsonJavaEnumAst::name<MsgCode>(c), m));
        anerror(args);
    };

    // inline static void setup(JProtocol p) {
    //     protocol = p;
    // }

    /**
     * @brief pingLess Ping without session.
     * @param jsert jserv-root, this is required since java 1.5.17, for instance-wise referencing.
     * @param uri
     * @param msg
     * @param err
     * @return response
     */
    inline static AnsonResp pingLess(AnsonResp resp, const JServUrl &jserv,
            const string &uri, const string &msg, OnError err=Clients::err) {

        EchoReq req;
        req.echo = msg;
        anlog("[Clinet.pingLess.body] "s + req.toBlock());

        AnsonMsg<EchoReq> anmsg(Port(Port::echo), req);

        // string reqs = anmsg.toBlock<AnsonMsg<EchoReq>>();
        anlog(std::format("[Clinet.pingLess Msg] port {} [{}]", anmsg.port.valof(), anmsg.port.url()));

        InsecureClient client{jserv};
        return client.commit(resp, anmsg, err);
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
