#pragma once

#include <io/odysz/anson.h>

namespace anson {

class SessionInf : public Anson {
public:
    string ssid;
    string uid;
    string roleId;
    string userName;
    string roleName;
    string ssToken;
    int seq;
    string device;

    SessionInf() {}
    SessionInf(string ssid, string uid): ssid(ssid), uid(uid) {}
    SessionInf(string ssid, string uid, string roleId): ssid(ssid), uid(uid), roleId(roleId) {}
};

}
