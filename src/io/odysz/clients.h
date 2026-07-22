#pragma once

#include <thread>
#include <string>
#include <cpr/cpr.h>

#include <io/odysz/common.h>
#include <io/odysz/reflect.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/semantier.h>
#include <io/odysz/semantic/x.h>
#include <io/odysz/transact/x.h>

#include "io/odysz/gen/semantier.hpp"

using namespace cpr;

namespace anson {

class SessionClient {

	bool stoplink;
	string syncFlag;
	AnsonMsg<HeartBeat> beatReq;
	int msInterval;

public:
    bool heartbeating = true;

    JServUrl jserv;

    AnsonHeader header;

    SessionInf ssInf;

    SessionClient(const JServUrl &jserv) : jserv(jserv) { }

    AnsonHeader Header() {
        if (LangExt::isblank(header.ssid)) {
            header = AnsonHeader{ssInf.uid, ssInf.ssid, ssInf.ssToken};
        }
        return header;
    }

    template<typename Rp, typename R>
    static Rp& commit(const JServUrl &jserv, AnsonMsg<R> &req, const OnError &err, bool verbose = false) {

        std::stringstream ss;
        req.toBlock(ss, *IJsonable::contxt_ptr); // FIXME performance problem

        stringstream ssview = std::move(ss);

        string url{std::format("{}/{}", jserv.jserv(), req.port.url())};
        anlog(url);
        // if(verbose)
        //     anlog(ssview.view());
        // else
        //     andebug(ssview.view());

        cpr::Response r = cpr::Post(
            cpr::Url{url},
            cpr::Proxies{{"https", ""}, {"http", ""}},
            cpr::Verbose{verbose},
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

            if (!result) {
                resp->Code(MsgCode::Code::exGeneral);
                if(resp->body_size() == 0)
                    resp->Body(Rp()); // managed by shared_ptr
                resp->body[0]->msg("Parsing response failed: " + r.text);
                err(MsgCode::Code::exGeneral, r.error.message + "\n" + r.text, {});
                throw SemanticException(resp->Body().m);
            }
            else if (resp->code != MsgCode::Code::ok) {
                err(resp->code, {resp->Body().m}, {});
                throw SemanticException(resp->Body().m);
            }
            return resp->Body();
        } else {
            std::cerr << "Error - Clients::commit(): " << r.status_code << " - " << r.error.message << std::endl;
            resp->Code(MsgCode::Code::exIo);
            if(resp->body_size() == 0)
                resp->Body(Rp());
            resp->body[0]->msg(r.error.message);
            err(MsgCode::Code::exIo, r.error.message, {});

            // throw std::system_error(std::make_error_code(std::errc::host_unreachable), resp->Body().m);
            throw AnsonException(resp->Body().m);
        }
    }

    template<typename Rp, typename R>
    Rp& commit(AnsonMsg<R> &req, const OnError &err, bool if_verbose = false) {
        return SessionClient::commit<Rp, R>(jserv, req, err, if_verbose);
    }

    inline static void format_sessionReq(AnSessionReq &req, const string uid, const string & pswd, const string &device);

    inline static SessionClient loginWithUri(const JServUrl &jserv, const string uri,
            const string uid, const string pswd, const string device, OnError err);

    template<typename T> // T extends AnsonBody
    inline static AnsonMsg<T> userReq(const AnsonHeader& header, const string& uri,
                                      const JavaEnum& port, T& bodyItem, const vector<string>& act = {}) {
        bodyItem.uri = uri;

        AnsonMsg<T>msg{port};
        msg.Header(header).Body(bodyItem);
        msg.header.Act(act);
        return msg;
    }

    template<typename T> // T extends AnsonBody
    AnsonMsg<T> userReq(const string& uri,
        const JavaEnum& port, T& bodyItem, const vector<string>& act = {}) {
        return SessionClient::userReq<T>(header, uri, port, bodyItem, act);
    }

    void stopbeat() { stoplink = true; }

    SessionClient* openLink(const string& clientUri, const OnError& beat_err, int msInterv = 60000) {
		// link
		syncFlag = "link";
		stoplink = false;

        HeartBeat beat{clientUri, ssInf.ssid, ssInf.uid};
        beatReq = AnsonMsg<HeartBeat>{Port{Port::heartbeat}};
        beatReq .Header(ssInf)
				.Body(beat);
		
		// msInterval = msInterv == null || msInterv.length < 1 ? 60000 : msInterv[0];
        std::thread beat_thread([this, beat_err, msInterv]() {
			int failed = 0;
            heartbeating = true;
            while (!stoplink) {
                commit<AnsonResp>(beatReq, beat_err);
                std::this_thread::sleep_for(std::chrono::milliseconds(std::min(2000, msInterv)));
            }
            heartbeating = false;
		});
        beat_thread.detach();
		
        return this;
    }
};

class InsecureClient : public SessionClient {
public:
    InsecureClient(const JServUrl &jserv) : SessionClient(jserv) {}
};

class Clients {
    inline static JProtocol protocol;

public:
    inline static bool if_verbose;

    inline static OnError err = [] (MsgCode c, const string& m, const vector<string>& args) {
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
        return client.commit<AnsonResp>(anmsg, err, if_verbose);
    }
};

inline SessionClient SessionClient::loginWithUri(const JServUrl &jserv, const string uri,
            const string uid, const string pswd, const string device, OnError err) {

    AnSessionReq req{};
    req.uri = uri;

    format_sessionReq(req, uid, pswd, device);

    AnsonMsg<AnSessionReq> msg{Port{Port::session}};
    msg.Body(req);

    AnSessionResp &rply = SessionClient::commit<AnSessionResp>(jserv, msg, err);

    SessionClient client{jserv};
    client.ssInf = rply.ssInf;
    andebug(std::format("{}: {}", client.ssInf.ssid, client.ssInf.ssToken));

    string ssToken = AESHelper2::repackSessionToken(client.ssInf.ssToken, pswd, uid);
    client.ssInf.ssToken = ssToken;
    // Notes: A permanent ssToken is slightly different to the java implementation
    // client.header = AnsonHeader(uid, rply.ssInf.ssid, ssToken);

    return client;
}

inline void SessionClient::format_sessionReq(AnSessionReq &req, const string uid,
                    const string & pswd, const string &device) {
    req.a = AnSessionReq::A::login;
    req.uid = uid;

    vector<unsigned char>iv = AESHelper2::getRandom();
    req.token = AESHelper2::encrypt(uid, pswd, iv);
    req.iv = AESHelper2::encode64(iv);
    req.deviceId = device;
}
}
