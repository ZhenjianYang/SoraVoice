#include "load_config.h"

#include <fstream>
#include <string>

#include "utils/build_date.h"

namespace {
constexpr char kSoraVoice[] = "SoraVoice (Lite)";
constexpr char kUrl[] = "https://github.com/ZhenjianYang/SoraVoice";

#define DEFINE_CONFIG(name, config_name, dft_value, comment) \
    constexpr char k##name[] = #config_name; \
    constexpr char k##name##Cmt[] = comment; \
    constexpr char k##name##Dft = dft_value;

#define OUTPUT_CONFIG(strm, config, name) \
    strm << k##name << " = " << config->name << "\n"; \
    if constexpr (k##name##Cmt[0]) { \
        strm << k##name##Cmt << "\n"; \
    } \
    strm << "\n";

#define SET_TO_DEFAULT(config, name) \
    config->name = k##name##Dft;

#define SET_CONFIG(config, name, s, v) \
    if (k##name == s) { config->name = v; }

DEFINE_CONFIG(volume, Volume, 100, "# Volume: 0 ~ 100");
DEFINE_CONFIG(disable_text_se, DisableTextSe, 1, "# (Voiced lines only) Disable dialog text beep SE.");
DEFINE_CONFIG(disable_dialog_se, DisableDialogSe, 1, "# (Voiced lines only) Disable dialog closing SE.");
DEFINE_CONFIG(disable_ao_ori_voice, DisableAoOriVoice, 1,
              "# (AO only) Disable original scenario voice. (Play evo voice only)");
DEFINE_CONFIG(show_info, ShwoInfo, 1, "# Show information of SoraVoice (Lite) in the title bar.");

static std::string Trim(const std::string& s) {
    if (s.empty()) {
        return s;
    }
    std::size_t b = 0, e = s.length() - 1;
    for (; b <= e && s[b] == ' '; b++);
    for (; b <= e && s[e] == ' '; e--);
    return e >= b ? s.substr(b, e - b + 1) : "";
}

static bool GetNameAndValue(const std::string& s, std::string* name, int* value) {
    auto peq = s.find('=');
    if (peq == s.npos) {
        return false;
    }
    *name = Trim(s.substr(0, peq));
    std::string sv = Trim(s.substr(peq + 1));
    char* es;
    *value = strtol(sv.c_str(), &es, 10);
    if (es != sv.c_str() + sv.length()) {
        return false;
    }
    return true;
}

}  // namespace

bool core::LoadConfig(Config* config, const char* filename) {
    SET_TO_DEFAULT(config, volume);
    SET_TO_DEFAULT(config, disable_text_se);
    SET_TO_DEFAULT(config, disable_dialog_se);
    SET_TO_DEFAULT(config, disable_ao_ori_voice);
    SET_TO_DEFAULT(config, show_info);

    std::ifstream ifs(filename, std::ios::in);
    if (!ifs) {
        return false;
    }
    std::string s;
    while (std::getline(ifs, s)) {
        if (s.empty() || s[0] == '#' || s[0] == ';') {
            continue;
        }
        std::string name;
        int value;
        if (!GetNameAndValue(s, &name, &value)) {
            continue;
        }
        SET_CONFIG(config, volume, name, value);
        SET_CONFIG(config, disable_text_se, name, value);
        SET_CONFIG(config, disable_dialog_se, name, value);
        SET_CONFIG(config, disable_ao_ori_voice, name, value);
        SET_CONFIG(config, show_info, name, value);
    }

    return true;
}

bool core::SaveConfig(const Config* config, const char* filename, const Info* info) {
    std::ofstream ofs(filename, std::ios::out);
    if (!ofs) {
        return false;
    }

    ofs << "# " << kSoraVoice << " " << kBuildDate << "\n";
    ofs << "# " << kUrl << "\n" << "\n" << "\n";

    OUTPUT_CONFIG(ofs, config, show_info);
    OUTPUT_CONFIG(ofs, config, volume);
    OUTPUT_CONFIG(ofs, config, disable_text_se);
    OUTPUT_CONFIG(ofs, config, disable_dialog_se);
    if (info->game == GameAo) {
        OUTPUT_CONFIG(ofs, config, disable_ao_ori_voice);
    }

    return true;
}
