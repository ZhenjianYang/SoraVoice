#include "win_title.h"

#include <Windows.h>

namespace {
constexpr int kMaxTitleLen = 255;
}

std::string utils::GetWinTitle(void* hwnd) {
    char buff[kMaxTitleLen + 1];
    GetWindowTextA((HWND)hwnd, buff, sizeof(buff));
    return std::string(buff);
}

bool utils::SetWinTitle(void* hwnd, const std::string& title) {
    return SetWindowTextA((HWND)hwnd, title.c_str());
}
