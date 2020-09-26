#ifndef __STARTUP_STRING_PATCH_H__

#include <memory>
#include <string>
#include <unordered_map>


namespace startup {

using PatchingStrings = std::unordered_map<std::string, std::string>;
PatchingStrings LoadPatchingStrings();

using RefPatchingStrings = std::tuple<int, std::string, std::unordered_map<int, std::string>>;
RefPatchingStrings LoadRefPatchingStrings(const char* filename);

}  // namespace startup

#endif  // __STARTUP_STRING_PATCH_H__
