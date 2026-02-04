#pragma once
#include <io/odysz/jprotocol.h>

namespace anson {
class WSEchoReq : public AnsonBody {
public:
    inline static const string _type_ = "io.oz.anclient.socketier.WSEchoReq";
    string echo;

    WSEchoReq(string a) :AnsonBody(a, _type_) { }
};

}
