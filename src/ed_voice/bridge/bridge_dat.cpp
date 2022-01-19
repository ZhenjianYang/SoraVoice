#include "bridge.h"

#include <cstring>
#include <cstdio>

#include "global/global.h"

namespace {
constexpr unsigned kFakeCompTagMagic = 0x9CCBB9F7;
constexpr unsigned kMaxEntries = 0x100;
constexpr unsigned kMaxFiles = 4096;
constexpr unsigned kFilenameBuffSize = 64;

struct RedirectDir {
    int idx;
    const char* dir;
};

struct FakeComp {
    unsigned size_comp;
    unsigned size_uncomp;
    unsigned tag;
    void* data;
};

struct DirEntry {
    const char name[0x10];
    unsigned size;
    unsigned unknown[3];
    unsigned offset;
};

constexpr RedirectDir kRedirectDirs[] = {
    { 0x00, "voice/fonts" },
    { 0x20, "voice/fonts" },
    { 0x01, "voice/scena" },
    { 0x21, "voice/scena" },
    { 0x22, "voice/scena" }
};
static const char kRedirectDirDefault[] = "voice/files";

inline int LoadFile(void* buff, const DirEntry* entry, const char* dir) {
    char buff_filename[kFilenameBuffSize];
    std::sprintf(buff_filename, "%s/%s", dir, entry->name);
    FILE* file = std::fopen(buff_filename, "rb");
    if (!file) {
        return 0;
    }

    std::fseek(file, 0, SEEK_END);
    int size_uncomp = (int)ftell(file);
    std::fseek(file, 0, SEEK_SET);

    FakeComp* fc = (FakeComp*)buff;
    fc->size_comp = entry->size;
    fc->size_uncomp = size_uncomp;
    fc->tag = kFakeCompTagMagic;
    fc->data = new char[size_uncomp];
    std::fread(fc->data, 1, size_uncomp, file);
    fclose(file);

    return fc->size_comp;
}

inline const char* GetDir(int idx) {
    const char* dir = nullptr;
    for (int i = 0; i < sizeof(kRedirectDirs) / sizeof(*kRedirectDirs); i++) {
        if (kRedirectDirs[i].idx == idx) {
            dir = kRedirectDirs[i].dir;
            break;
        }
    }
    return dir;
}

}  // namespace

int __cdecl bridge::LoadDatF(void* buff, int idx, void*, unsigned offset, unsigned size) {
    return bridge::LoadDat(nullptr, buff, idx, offset, size);
}

int __cdecl bridge::LoadDat(void*, void* buff, int idx, unsigned offset, unsigned size) {
    const char* dir = GetDir(idx);
    if (!dir) {
        return 0;
    }

    const DirEntry* entry = *((const DirEntry**)global.addrs.pdirs + idx);
    for (int i = 0;
         i < kMaxFiles && (entry->offset != offset || entry->size != size);
         entry++, i++);
    if (entry->offset != offset || entry->size != size) {
        return 0;
    }

    if (!LoadFile(buff, entry, dir)) {
        return 0;
    }
    return 1;
}

int __cdecl bridge::LoadDat2(void*, void* buff, int idx) {
    int ientry = idx >> 16;
    int ifile = idx & 0xFFFF;
    if (ientry >= kMaxEntries || ifile >= kMaxFiles) {
        return 0;
    }
    const char* dir = GetDir(ientry);
    if (!dir) {
        return 0;
    }

    DirEntry** pentry = (DirEntry**)global.addrs.pdirs + ientry;
    if (!pentry || !(*pentry + ifile)) {
        return 0;
    }
    return LoadFile(buff, *pentry + ifile, dir);
}

int __cdecl bridge::DecompressDat(void*, void** uncompressed, void** compressed) {
    FakeComp* fc = *(FakeComp**)compressed;
    if (fc->tag != kFakeCompTagMagic || !fc->data) return 0;

    std::memcpy(*uncompressed, fc->data, fc->size_uncomp);

    *(char**)uncompressed += fc->size_uncomp;
    *(char**)compressed += fc->size_comp;

    delete[] fc->data;
    fc->data = nullptr;
    return fc->size_uncomp;
}
