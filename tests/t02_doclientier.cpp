#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN     // excludes OLE/COM/RPC stuff → no rpcndr.h
#define NOMINMAX                // prevents min/max macro clashes
#endif

#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>
#include <gen/jserv.hpp>

#include "io/odysz/semantier.h"
#include "io/odysz/clients.h"
#include "expect/doctier.hpp"

using namespace std;
using namespace std;
using namespace anson;
using namespace entt;
using namespace entt::literals;

TEST(Syncpage, Query) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    load_ansessionreqAst(asts, "ast-cpy/session-req.ast.json");
    load_ansessionrespAst(asts, "ast-cpy/session-resp.ast.json");

}
