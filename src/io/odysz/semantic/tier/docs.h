#pragma once

#include <io/odysz/anson.h>
#include <io/odysz/common.h>

namespace anson {
// class ShareFlag : public IJsonable, public JavaConsts<string> {
class ShareFlag : public IJsonable {
public:
    /** Kept as private file ('🔒') at private node. */
    static inline const string prv = "A"; //R"("🔒")";

    /**
     * To be pushed (shared) to hub ('⇈')
     * @deprecated confusing with synchronizing state.
     */
    static inline const string pushing = "B"; // R"("⇈")";

    /** synchronized (shared) with a synode ('🌎') */
    static inline const string publish = "C"; // R"("🌎")";

    /**created at a device (client) node ('📱') */
    static inline const string device = "D"; // R"("📱")";

    /**
     * The doc is locally removed, and the task is waiting to push to a synode ('Ⓛ')
     * @deprecated confusing with synchronizing state.
     */
    static inline const string loc_remove = "E"; // R"("Ⓛ")";

    /** what's this for ? */
    static inline const string deny = "F"; // R"("⛔")";

    /** what's this for ? */
    static inline const string unknown = "F"; // R"("⚠")";

    string v;

    ShareFlag(string& f) : IJsonable("") , v(f) {}

    string toBlock(JsonOpt& opts) override {
        // stream.put('\"');
        // stream.write(name().c_str(), name().size());
        // stream.put('\"');
        // return this;
        return {'"' + v + '"'};
    }

    IJsonable* toJson(string& buf) override {
        buf += '\"';
        buf += v;
        buf += '\"';
        return this;
    }
};

}
