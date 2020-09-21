#ifndef __UTILS_LOAD_OGG_APIS_H__
#define __UTILS_LOAD_OGG_APIS_H__

namespace utils {
bool LoadOggApis(void* *ov_open_callbacks, void* *ov_info, void* *ov_read, void* *ov_clear, void* *ov_pcm_total);
}  // namespace utils

#endif // __UTILS_LOAD_OGG_APIS_H__
