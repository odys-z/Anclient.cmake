#pragma once

#include <io/odysz/anson.h>
#include <io/odysz/common.h>

namespace anson {
// class ShareFlag : public IJsonable, public JavaConsts<string> {
class ShareFlag : public JavaEnum {
public:
    /** Kept as private file ('🔒') at private node. */
    static inline const string prv = "🔒";

    /**
     * To be pushed (shared) to hub ('⇈')
     * @deprecated confusing with synchronizing state.
     */
    static inline const string pushing = "⇈";

    /** synchronized (shared) with a synode ('🌎') */
    static inline const string publish = "🌎";

    /**created at a device (client) node ('📱') */
    static inline const string device = "📱";

    /**
     * The doc is locally removed, and the task is waiting to push to a synode ('Ⓛ')
     * @deprecated confusing with synchronizing state.
     */
    static inline const string loc_remove = "Ⓛ";

    /** what's this for ? */
    static inline const string deny = "⛔";

    /** what's this for ? */
    static inline const string unknown = "⚠";

    // ShareFlag(string& f) : JavaConsts(f) {}

    // IJsonable* toBlock(ostream& stream, JsonOpt& opts) override {
    //     stream.put('\"');
    //     stream.write(name().c_str(), name().size());
    //     stream.put('\"');
    //     return this;
    // }

    // IJsonable* toJson(string& buf) override {
    //     buf += '\"';
    //     buf += name();
    //     buf += '\"';
    //     return this;
    // }
};

}
