#pragma once

#include "io/odysz/anson.h"

namespace anson {

class TestSettings : public Anson {

public:
    inline static const string _type_ = "io.oz.syn.test.TestSettings";

    /** "http://localhost:8964/jserv-album" */
    string jserv;

    bool readonly;

    string domain;

    /** "admin": "admin" */
    string admin;

    string doamin_token;

    /** "regiserv": "http://..:../regist-alpha", */
    string regiserv;

    string centralPswd;

    TestSettings() {}

    TestSettings(const string &jserv) : jserv(jserv) {}
};
}
