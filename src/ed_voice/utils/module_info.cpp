#include "module_info.h"

#include <Windows.h>

#include <DbgHelp.h>
#include <Psapi.h>

namespace utils {

bool utils::GetCurrentModuleInformation(byte* *base, int *size) {
    MODULEINFO module_info;
    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(NULL),
                              &module_info, sizeof(module_info))) {
        return false;
    }
    *base = static_cast<byte*>(module_info.lpBaseOfDll);
    *size = static_cast<int>(module_info.SizeOfImage);
    return true;
}

std::vector<SectionInfo> utils::GetSectionsInfo(byte* base) {
    std::vector<SectionInfo> secs;

    IMAGE_NT_HEADERS* nt_header = ImageNtHeader(base);
    IMAGE_SECTION_HEADER* sec_header = (IMAGE_SECTION_HEADER*)(nt_header + 1);

    char sec_name[sizeof(sec_header->Name) + 1] {};
    for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++, sec_header++) {
        std::copy_n(sec_header->Name, sizeof(sec_header->Name), sec_name);

        secs.push_back({ sec_name,
                            base + sec_header->VirtualAddress,
                            base + sec_header->VirtualAddress + sec_header->Misc.VirtualSize,
                            (int)sec_header->Misc.VirtualSize });
    }

    return secs;
}

}  // namespace utils
