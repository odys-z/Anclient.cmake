#pragma once

#include <functional>
#include <string>
#include <cpr/cpr.h>

#include <io/odysz/reflect.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/semantier.h>

#include <gen/jserv.hpp>

using namespace cpr;

namespace anson {

class SessionClient {

public:
    JServUrl jserv;

    SessionInf ssInf;

    SessionClient(const JServUrl &jserv) : jserv(jserv) { }

    template<typename R, typename Rp>
    static Rp& commit(const JServUrl &jserv, AnsonMsg<R> &req, OnError err, bool if_verbose = false) {
        std::stringstream ss;
        req.toBlock(ss, *IJsonable::contxt_ptr); // FIXME performance problem

        stringstream ssview = std::move(ss);

        string url{std::format("{}/{}", jserv.jserv(), req.port.url())};
        anlog(url);
        andebug(ss.view());

        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Proxies{{"https", ""}, {"http", ""}},
            cpr::Verbose{if_verbose},
            cpr::Body{std::move(ssview.str())},
            cpr::Header{
                        {"Content-Type", "html/text"},
                        {"Content-Type", "application/json"},
            {"User-Agent", "Mozilla/5.0 (Anclient.cmake)"}}
        );

        AnsonMsg<Rp> *resp = new AnsonMsg<Rp>{};

        if (r.status_code == 201 || r.status_code == 200) { // 201 is 'Created'
            std::cout << "Success!" << std::endl;
            std::cout << "Response from server: " << r.text << std::endl;

            bool result = Anson::from_json(r.text, *resp);

            vector<string_view> err_args;
            if (!result) {
                resp->Code(MsgCode::Code::exGeneral);
                if(resp->body_size() == 0)
                    resp->Body(Rp()); // managed by shared_ptr
                resp->body[0]->msg("Parsing response failed: " + r.text);
                err(MsgCode::Code::exGeneral, r.error.message, err_args);
            }
            else if (resp->code != MsgCode::Code::ok) {
                err(resp->code, {resp->body.at(0)->m}, err_args);
            }
        } else {
            std::cerr << "Error - Clients::commit(): " << r.status_code << " - " << r.error.message << std::endl;
            resp->Code(MsgCode::Code::exIo);
            if(resp->body_size() == 0)
                resp->Body(Rp());
            resp->body[0]->msg(r.error.message);
            vector<string_view> args;
            err(MsgCode::Code::exIo, string_view(r.error.message), args);
        }

        return resp->Body();
    }

    template<typename R, typename Rp>
    Rp& commit(AnsonMsg<R> &req, OnError err, bool if_verbose = false) {
        return SessionClient::commit<R, Rp>(jserv, req, err, if_verbose);
    }

    static void format_sessionReq(AnSessionReq &req, const string uid, const string & pswd, const string &device);

    static SessionClient* loginWithUri(const JServUrl &jserv, const string uri,
            const string uid, const string pswd, const string device, OnError err);
};

class InsecureClient : public SessionClient {
public:
    InsecureClient(const JServUrl &jserv) : SessionClient(jserv) {}
};

class Clients {
    inline static JProtocol protocol;

public:
    inline static bool if_verbose;

    inline static OnError err = [] (MsgCode c, string_view m, vector<string_view> args) {
        anerror(format("code: {}, msg:\n{}", AnsonJavaEnumAst::name<MsgCode>(c), m));
        anerror(args);
    };

    /**
     * @brief pingLess Ping without session.
     * @param jsert jserv-root, this is required since java 1.5.17, for instance-wise referencing.
     * @param uri
     * @param msg
     * @param err
     * @return response
     * @issue The server siede, echo-tier, cannot return resp.body.data of type map<string, list<string/VarType.
     */
    inline static AnsonResp pingLess(const JServUrl &jserv,
            const string &uri, const string &msg, OnError err=Clients::err) {

        EchoReq req;
        req.echo = msg;
        req.a = EchoReq::A::inet;
        anlog("[Clinet.pingLess.body] "s + req.toBlock());

        AnsonMsg<EchoReq> anmsg(Port(Port::echo), req);

        anlog(std::format("[Clinet.pingLess Msg] port {} [{}]", anmsg.port.valof(), anmsg.port.url()));

        InsecureClient client{jserv};
        return client.commit<EchoReq, AnsonResp>(anmsg, err, if_verbose);
    }
};

SessionClient* SessionClient::loginWithUri(const JServUrl &jserv, const string uri,
            const string uid, const string pswd, const string device, OnError err) {

    AnSessionReq req{};
    format_sessionReq(req, uid, pswd, device);

    AnsonMsg<AnSessionReq> msg{Port::session};
    msg.Body(req);

    AnSessionResp &rply = SessionClient::commit<AnSessionReq, AnSessionResp>(jserv, msg, err);

    SessionClient* client = new SessionClient{jserv};
    client->ssInf = rply.ssInf;

    return client;
}

void SessionClient::format_sessionReq(AnSessionReq &req, const string uid,
                    const string & pswd, const string &device) {
    req.a = AnSessionReq::A::login;
    req.uid = uid;

    vector<unsigned char>iv = AESHelper2::getRandom();
    req.token = AESHelper2::encrypt(uid, pswd, iv);
    req.iv = AESHelper2::encode64(iv);
    req.deviceId = device;
}

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
