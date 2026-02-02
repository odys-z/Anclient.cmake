#pragma once

#include <functional>
#include <thread>
#include <chrono>
#include <string>

#include "semantier.h"

using namespace std;

namespace anson {


/**
 * TODO move to a stand alone protocol tier?
 * JProtocol.OnProgress
 */
using OnProgress = std::function<void(const string& path, std::string status)>;

class WSClient {

};

class Doclientier : public WSClient {

    string device;
public:
    Doclientier(string device) : device(device) {}

    void push_files(const vector<string>& paths, OnProgress onprc) {

        // PathsPage sync_page(device);
        DocsReq reqbd{device, DocsReq::A::requestSyn};

        vector<string>::const_iterator it = paths.begin();
        for (; it != paths.end(); ++it) {
            reqbd.syncingPage.append(*it, "synching");
            onprc(*it, "synching");
        }

        AnsonMsg<DocsReq> req{Port::docstier};
        req.Body(reqbd);

    }
};
}
