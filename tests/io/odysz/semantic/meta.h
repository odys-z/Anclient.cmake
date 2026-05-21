#pragma once

#include <io/odysz/dbmeta.h>

namespace anson {

class ExpDocTableMeta : public SynEntityMeta {
public:

    // ExpDocTableMeta(const string &t) : doctbl(t) {}

    ExpDocTableMeta(const string &t, const string &pk, const string &device, const string & conn)
        : SynEntityMeta(t, pk, device, conn) {}
};


class PhotoMeta : public ExpDocTableMeta {

public:
    const string tags;
    const string exif;
    const string family;
    const string geox;
    const string geoy;
    const string css;

PhotoMeta(const string &conn) : ExpDocTableMeta("h_photos", "pid", "device", conn),
        tags("tags"), exif("exif"), family("family"), geox("geox"), geoy("geoy"), css("css") {
        // ddlSqlite = loadSqlite(PhotoMeta.class, "h_photos.sqlite.ddl");
    }
};
}
