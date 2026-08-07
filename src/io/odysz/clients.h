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

struct connect_state {
    inline static const string online = "online";
    inline static const string offline = "offline";
    inline static const string login_failed = "x-login";
    string synlink;

    /** Not maintainced for now (0.1.0) */
    string ipclink;

    /** Not used for now (0.1.0) */
    string registlink;

    /** Synodes in domain. A future desing */
    vector<string> synlinks;
};

using OnLink = std::function<void(connect_state)>;

class SessionClient {

	bool stoplink;
	string syncFlag;
	AnsonMsg<HeartBeat> beatReq;
	int msInterval;

    const OnLink synlink_flipped;
    const OnError beat_err;
public:
    bool heartbeating = false;

    /**
     * @brief jserv with jprotocol instance.
     * A client has it's own protocol knowledge, rather than share protocol semantics across links.
     */
    JServUrl jserv;

    AnsonHeader header;

    SessionInf ssInf;

    SessionClient(const JServUrl &jserv, const OnLink& heartbeating, const OnError& heartbroken)
        : jserv(jserv), synlink_flipped(heartbeating), beat_err(heartbroken) { }

    AnsonHeader Header() {
        if (LangExt::isblank(header.ssid)) {
            header = AnsonHeader{ssInf.uid, ssInf.ssid, ssInf.ssToken};
        }
        return header;
    }

    template<typename Rp, typename R>
    static Rp& commit(const JsonOpt* ctx_ptr, const JServUrl &jserv, AnsonMsg<R> &req, const OnError &err, bool verbose = false) {

        std::stringstream ss;
        req.toBlock(ss, *ctx_ptr); // FIXME performance problem

        stringstream ssview = std::move(ss);

        string url{std::format("{}/{}", jserv.jserv(), req.port.url())};
        anlog(url);

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
            std::cout << "commit(): Success!" << std::endl;
            std::cout << "Response from server: " << r.text << std::endl;

            bool result = Anson::from_json(r.text, *resp, ctx_ptr);

            if (!result) {
                resp->Code(MsgCode::Code::exGeneral);
                if(resp->body_size() == 0)
                    resp->Body(Rp()); // managed by shared_ptr
                resp->body[0]->msg("Parsing response failed: " + r.text);
                err(MsgCode::Code::exGeneral, r.error.message + "\n" + r.text, {});
                // throw SemanticException(resp->Body().m);
                throw SemanticException("jserv: {}, act: {}, code: {}, msg: {}",
                    url, req.Body().a, "exGeneral", resp->Body().m);
            }
            else if (resp->code != MsgCode::Code::ok) {
                err(resp->code, {resp->Body().m}, {});
                throw SemanticException("jserv: {}, act: {}, code: {}, msg: {}",
                    url, req.Body().a, MsgCode::to_string(resp->code), resp->Body().m);
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
            // throw AnsonException(resp->Body().m);
            throw SemanticException("jserv: {}, act: {}, code: {}, msg: {}",
                url, req.Body().a, "exIo", resp->Body().m);
        }
    }

    template<typename Rp, typename R>
    Rp& commit(AnsonMsg<R> &req, const OnError &err, bool if_verbose = false) {
        return SessionClient::commit<Rp, R>(jserv.jprotocol.ctx, jserv, req, err, if_verbose);
    }

    inline static void format_sessionReq(AnSessionReq &req, const string uid, const string & pswd, const string &device);

    /**
     * Your session client must initialized with jserv: SessionClient client{jserv}
     */
    inline void loginWithUri(const string uri, const string uid,
                             const string pswd, const string device, OnError err) ;

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

    SessionClient* openLink(const string& clientUri, int msInterv = 60000) noexcept {
		// link
		syncFlag = "link";
		stoplink = false;

        HeartBeat beat{clientUri, ssInf.ssid, ssInf.uid};
        beatReq = AnsonMsg<HeartBeat>{Port{Port::heartbeat}};
        beatReq .Header(ssInf)
				.Body(beat);
		
		// msInterval = msInterv == null || msInterv.length < 1 ? 60000 : msInterv[0];
        std::thread beat_thread([this, msInterv]() {
			int failed = 0;
            while (!stoplink) {
                try {
                    commit<AnsonResp>(beatReq, beat_err);
                    if (!heartbeating) {
                        anlog("====================================================== link to synode: turn-on");
                        heartbeating = true;
                        synlink_flipped({.synlink = connect_state::online});
                    }
                }
                catch (SemanticException e) {
                    if (heartbeating) {
                        anlog("------------------------------------------------------ link to synode: down");
                        heartbeating = true;
                        heartbeating = false;
                        synlink_flipped({.synlink = connect_state::offline});
                    }

                    anwarn(e.what());
                }
                catch (runtime_error e) { anerror(e.what()); }
                std::this_thread::sleep_for(std::chrono::milliseconds(std::max(2000, msInterv)));
            }
            heartbeating = false;
		});
        beat_thread.detach();
		
        return this;
    }
};

class InsecureClient : public SessionClient {
public:
    InsecureClient(const JServUrl &jserv)
        : SessionClient(jserv, [](connect_state){}, [](MsgCode, const string&, const vector<string>&){}) {}
};

class Clients {
    // const JProtocol* protocol;

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
            const string &uri, const string &msg, OnError err=Clients::err, const string a = EchoReq::A::inet) {

        EchoReq req;
        req.echo = msg;
        // req.a = EchoReq::A::inet;
        req.a = a; // EchoReq::A::inet;
        anlog("[Clinet.pingLess.body] "s + req.toBlock(*jserv.jprotocol.ctx));

        AnsonMsg<EchoReq> anmsg(Port(Port::echo), req);

        anlog(std::format("[Clinet.pingLess Msg] port {} [{}]", anmsg.port.valof(), anmsg.port.url()));

        InsecureClient client{jserv};
        return client.commit<AnsonResp>(anmsg, err, if_verbose);
    }
};

inline void SessionClient::loginWithUri(const string uri, const string uid,
                    const string pswd, const string device, OnError err) {

    AnSessionReq req{};
    req.uri = uri;

    format_sessionReq(req, uid, pswd, device);

    AnsonMsg<AnSessionReq> msg{Port{Port::session}};
    msg.Body(req);

    // AnSessionResp &rply = SessionClient::commit<AnSessionResp>(client.jserv, msg, err);
    AnSessionResp &rply = commit<AnSessionResp>(msg, err);

    this->ssInf = rply.ssInf;
    andebug(std::format("{}: {}", ssInf.ssid, ssInf.ssToken));

    string ssToken = AESHelper2::repackSessionToken(ssInf.ssToken, pswd, uid);
    ssInf.ssToken = ssToken;
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
