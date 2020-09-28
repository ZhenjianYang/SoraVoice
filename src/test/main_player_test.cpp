#include <Windows.h>

#include <iostream>
#include <string>

#include "player/player.h"
#include "utils/create_dsound.h"
#include "utils/log.h"

namespace {
using PlayId = player::Player::PlayId;
using StopType = player::Player::StopType;
using utils::CreateDSound8;
static HWND GetHWND() {
    char title[1024];
    GetConsoleTitle(title, 1024);
    return FindWindow(NULL, title);
}
}  // namespace

int main(int argc, char* argv[]) {
    void* pDS = nullptr;
    HWND hwnd = GetHWND();
    CreateDirectory("voice", NULL);

    auto pDS8 = CreateDSound8(hwnd);

    auto player = player::Player::GetPlayer(pDS8);
    player->SetVolume(30);
    if (!player) {
        return 0;
    }
    while (true) {
        std::string f;
        std::cin >> f;
        if (f.empty()) {
            continue;
        }
        else if (f == "exit") {
            break;
        }
        else if (f == "stop") {
            player->StopAll();
        }
        else {
            if (!f.empty() && f[0] == 'v') {
                int volume;
                if (std::sscanf(f.c_str() + 1, "%d", &volume)
                    && volume >= 0 && volume <= 100) {
                    player->SetVolume(volume);
                    continue;
                }
            }

            auto pattr = f.rfind('.');
            std::string attr;
            if (pattr != std::string::npos) {
                attr = f.substr(pattr);
            }
            if (attr.length() >= 5 || attr.empty()) {
                f += ".ogg";
            }
            if (argc > 1) {
                f = std::string(argv[1]) + "\\" + f;
            }
            auto pid = player->Play(f, [f](PlayId pid, StopType stop) {
                LOG("CallBack: PlayId = %d, StopType = %d, File = %s",
                    pid, (int)stop, f.c_str());
            });
            LOG("Playing %d: %s", pid, f.c_str());
        }
    }

    return 0;
}
