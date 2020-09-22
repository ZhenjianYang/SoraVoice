#include <Windows.h>

#include <iostream>
#include <string>

#include "player/player.h"
#include "utils/log.h"

namespace {
using PlayId = player::Player::PlayId;
using StopType = player::Player::StopType;
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

    auto player = player::Player::GetPlayer(&pDS, hwnd);
    if (!player) {
        return 0;
    }
    auto callback = [](PlayId pid, StopType stop) {
        LOG("CallBack: PlayId = %d, StopType = %d", pid, (int)stop);
    };
    while (true) {
        std::string f;
        std::cin >> f;
        player->SetVolume(30);
        if (f == "stop") {
            player->StopAll();
        }
        else {
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
            auto pid = player->Play(f, callback);
            LOG("Playing %d: %s", pid, f.c_str());
        }
    }

    return 0;
}
