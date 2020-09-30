#ifndef __UTILS_WIN_TITLE_H__
#define __UTILS_WIN_TITLE_H__

#include <string>

namespace utils {
std::string GetWinTitle(void* hwnd);
bool SetWinTitle(void* hwnd, const std::string& title);
}  // namespace utils

#endif  // __UTILS_WIN_TITLE_H__
