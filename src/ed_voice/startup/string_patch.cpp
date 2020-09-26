#include "string_patch.h"

#include <codecvt>
#include <locale>
#include <fstream>

#include "utils/log.h"

namespace {
constexpr struct {
    const char* str_old;
    const char* str_new;
} kDirectPatchingStrings[] = {
    {"%c#2C\n#56IEarth Sepith x%d,\n#57IWater Sepith x%d,\n#58IFire Sepith x%d,\n#59IWind Sepith x%d,\n#62ITime Sepith x%d,\n#60ISpace Sepith x%d,\n#0and#2C #61IMirage Sepith x%d#0.",
     "%c#2C\n#56IEarth Sepith x%d,\n#57IWater Sepith x%d,\n#58IFire Sepith x%d,\n#59IWind Sepith x%d,\n#62ITime Sepith x%d,\n#60ISpace Sepith x%d,\n#0Cand#2C #61IMirage Sepith x%d#0C."}
};

using startup::RefPatchingStrings;
using startup::PatchingStrings;

inline static std::string GetStrOld(const std::string& s) {
    std::string r;
    r.reserve(s.length());
    std::size_t last_sharp = std::string::npos;
    for (char ch : s) {
        if (ch == '#') {
            last_sharp = r.length();
        }
        else if (last_sharp != std::string::npos) {
            if (ch == 'v') {
                r.resize(last_sharp);
                last_sharp = std::string::npos;
                continue;
            } else if (ch < '0' || ch > '9') {
                last_sharp = std::string::npos;
            }
        }
        r.push_back(ch);
    }
    return r;
}

inline static std::wstring Utf8ToUtf16(const std::string& utf8str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv("", L"");
    return wconv.from_bytes(utf8str.c_str());
}

inline static std::string Utf16ToStr(const std::wstring& utf16str, const std::string& codename) {
    using conv = std::codecvt_byname<wchar_t, char, std::mbstate_t>;
    std::wstring_convert<conv> con(new conv(codename));
    return con.to_bytes(utf16str);
}

inline static std::string Utf8ToStr(const std::string& utf8str, const std::string& codename) {
    return Utf16ToStr(Utf8ToUtf16(utf8str), codename);
}

std::vector<std::pair<int, std::string>> LoadStringsFromFile(const char* filename) {
    std::vector<std::pair<int, std::string>> ret;
    std::ifstream ifs(filename, std::ios::in);
    if (!ifs) {
        return ret;
    }
    int offset = 0;
    std::string codename, str;
    std::string s;
    while (std::getline(ifs, s)) {
        if (s.empty() || s[0] == ';') {
            ret.emplace_back(offset, Utf8ToStr(str, codename));

            offset = 0;
            codename.clear();
            str.clear();

            continue;
        }
        if (offset == 0) {
            char* es;
            offset = strtol(s.c_str(), &es, 16);
            continue;
        }
        if (codename.empty()) {
            codename = s;
            continue;
        }
        const char* es = s.c_str();
        while (*es) {
            if (*es == '\\' && es[1] == 'n') {
                str.push_back('\n');
                es += 2;
            } else {
                str.push_back(*es);
                es += 1;
            }

        }
    }
    return ret;
}

}  // namespace

PatchingStrings startup::LoadPatchingStrings() {
    LOG("Loading Strings...");
    std::unordered_map<std::string, std::string> ret;
    for (const auto& pstrings : kDirectPatchingStrings) {
        ret[pstrings.str_old] = pstrings.str_new;
    }
    LOG("%d Strings loaded.", ret.size());
    return ret;
}

RefPatchingStrings startup::LoadRefPatchingStrings(
        const char* filename) {
    LOG("Loading Strings from %s", filename);
    RefPatchingStrings ret;
    auto strs = LoadStringsFromFile(filename);
    if (strs.empty()) {
        return ret;
    }
    std::get<0>(ret) = strs.front().first;
    std::get<1>(ret) = GetStrOld(strs.front().second);
    for (const auto& str : strs) {
        std::get<2>(ret)[str.first] = str.second;
    }
    LOG("%d Strings loaded.", std::get<2>(ret).size());
    return ret;
}
