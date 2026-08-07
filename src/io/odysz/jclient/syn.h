#pragma once

#include "../clients.h"
#include "io/odysz/gen/anclient_settings.hpp"
#include "io/odysz/gen/doctier.hpp"
#include "io/odysz/gen/registry.hpp"

#include <io/odysz/gen/registry.hpp>

namespace anson {

class Semantier {
public:
    std::string mtabl;
};

class Doclientier : public Semantier {
public:
    string sysuri;
    const string synuri;
    const string doctbl;
    const OnError & err;

    SessionClient client;

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
        this->client.loginWithUri(sysuri, uid, pswd, device, err);
        return this;
    }
};

class RegistryClient : public SessionClient {
public:
    string market;
    string orgid;
    string orgname;

    string settings_json;
    AnclientSettings& appsettings;
    // SynodeConfig& syncfg;

    RegistryClient(AnclientSettings& settings, const JServUrl& jserv, const OnLink& onbeat, const OnError& errctx)
        : SessionClient(jserv, onbeat, errctx), appsettings(settings) {}

    OnError onErr;

    /** A::queryDomConfig */
    void asyquery_orgdoms(const string & org, const OnOk& ok, const OnError& err) {
      std::thread query_thread([this, &org, &ok, &err]() {
        try {
            anlog(ssInf.toBlock(*jserv.jprotocol.ctx));
            if (LangExt::isblank(ssInf.ssid)) {
                loginWithUri(this->appsettings.sysuri, this->appsettings.admin,
                this->appsettings.centralPswd, this->appsettings.device, err);
            }

            if (!heartbeating) {
                openLink(appsettings.sysuri);
            }

            if (LangExt::isblank(ssInf.ssid)) {
                anwarn("Cannot login to "s + this->jserv.jserv());
                return;
            }

            header.Act(appsettings.sysuri, Centralport::regist, RegistReq::A::queryDomx, "query sync");

            RegistReq req;
            SynodeConfig dict;
            dict.org.orgId = orgid;
            dict.org.orgName = orgname;

            req.a = RegistReq::A::queryDomx;
            req.market = market;
            req.diction = dict;

            anlog("=========================\n"s + ssInf.toBlock());
            AnsonMsg<RegistReq> q = userReq(appsettings.sysuri, Centralport{Centralport::regist}, req)
                    .Header(ssInf);
            anlog("=========================\n"s + q.toBlock());

            anlog(q.toBlock(*jserv.jprotocol.ctx));
            RegistResp resp = commit<RegistResp>(q, err);
            ok(resp);
        }
        catch (const SemanticException& e) {
            anerror(e.what());
        }
        catch (const AnsonException& e) {
            anerror(e.what());
        }
        catch (const std::exception& e) {
            anerror(e.what());
        }
        catch (...) {
            anerror("Caught unknown exception.");
        }
      });
      query_thread.detach();
    }

    void asyquery_domconfig(const string & org, const string& domid, const OnOk& ok, const OnError& err) {
        std::thread query_thread([this, &org, &ok, &err, &domid]() {
            if (LangExt::isblank(ssInf.ssid)) {
                loginWithUri(this->appsettings.sysuri, this->appsettings.admin,
                             this->appsettings.centralPswd, this->appsettings.device, err);
            }

            if (!heartbeating) {
                openLink(appsettings.sysuri);
            }

            if (LangExt::isblank(ssInf.ssid)) {
                anwarn("Cannot login to "s + this->jserv.jserv());
                return;
            }

            header.Act(appsettings.sysuri, Centralport::regist, RegistReq::A::queryDomConfig, "query sync");

            RegistReq req;
            SynodeConfig dict;
            dict.org.orgId = orgid;
            dict.org.orgName = orgname;
            dict.domain = domid;

            req.a = RegistReq::A::queryDomConfig;
            req.market = market;
            req.diction = dict;

            anlog("=========================\n"s + ssInf.toBlock());
            AnsonMsg<RegistReq> q = userReq(appsettings.sysuri, Centralport{Centralport::regist}, req)
                                        .Header(ssInf);
            anlog("=========================\n"s + q.toBlock());

            try {
                anlog(q.toBlock(*jserv.jprotocol.ctx));
                RegistResp resp = commit<RegistResp>(q, err);
                ok(resp);
            }
            catch (const SemanticException& e) {
                anerror(e.what());
            }
            catch (const AnsonException& e) {
                anerror(e.what());
            }
            catch (const std::exception& e) {
                anerror(e.what());
            }
            catch (...) {
                anerror("Caught unknown exception.");
            }
        });
        query_thread.detach();
    }
};

}
