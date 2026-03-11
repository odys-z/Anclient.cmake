#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include <io/odysz/jprotocol.h>
#include <io/odysz/json.h>
#include "io/odysz/clients.h"
#include "io/odysz/semantic/json.expect.h"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

TEST(ANCLIENT, PING) {
    map<string, map<string, int>*> enums;
    register_meta(enums);
    register_semantier(enums);

    OnError errctx = []() {

    }
    Clients::pingLess("Anson.cmake/test", errctx);
}
