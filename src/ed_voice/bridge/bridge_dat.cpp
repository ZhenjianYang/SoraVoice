#include "bridge.h"

#include <cstring>
#include <cstdio>

#include "global/global.h"

constexpr unsigned kFakeCompTagMagic = 0x9CCBB9F7;
constexpr unsigned  kMaxFiles = 4096;
constexpr unsigned  kFilenameBuffSize = 64;

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

int __cdecl bridge::LoadDat(void*, void* buff, int idx, unsigned offset, unsigned size) {
    const DirEntry* entry = *((const DirEntry**)global.addrs.pdirs + idx);
    for (int i = 0;
         i < kMaxFiles && (entry->offset != offset || entry->size != size);
         entry++, i++);
    if (entry->offset != offset || entry->size != size) {
        return 0;
    }
    const char* dir = kRedirectDirDefault;
    for (int i = 0; i < sizeof(kRedirectDirs) / sizeof(*kRedirectDirs); i++) {
        if (kRedirectDirs[i].idx == idx) {
            dir = kRedirectDirs[i].dir;
            break;
        }
    }

    char buff_filename[kFilenameBuffSize];
    std::sprintf(buff_filename, "%s/%s", dir, entry->name);
    FILE* file = std::fopen(buff_filename, "rb");
    if (!file) return 0;

    std::fseek(file, 0, SEEK_END);
    int size_uncomp = (int)ftell(file);
    std::fseek(file, 0, SEEK_SET);

    FakeComp* fc = (FakeComp*)buff;
    fc->size_comp = size;
    fc->size_uncomp = size_uncomp;
    fc->tag = kFakeCompTagMagic;
    fc->data = new char[size_uncomp];
    std::fread(fc->data, 1, size_uncomp, file);
    fclose(file);

    return 1;
}

int __stdcall bridge::DecompressDat(void** compressed, void** uncompressed) {
    FakeComp* fc = *(FakeComp**)compressed;
    if (fc->tag != kFakeCompTagMagic || !fc->data) return 0;

    std::memcpy(*uncompressed, fc->data, fc->size_uncomp);

    *(char**)uncompressed += fc->size_uncomp;
    *(char**)compressed += fc->size_comp;

    delete[] fc->data;
    fc->data = nullptr;
    return fc->size_uncomp;
}
