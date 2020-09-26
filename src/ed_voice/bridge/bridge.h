#ifndef __BRIDGE_BRIDGE_H__
#define __BRIDGE_BRIDGE_H__

namespace bridge {

int __stdcall Play(void* b);
int __cdecl Stop();

int __cdecl LoadDat(void*, void* buff, int idx, unsigned offset, unsigned size);
int __stdcall DecompressDat(void** compressed, void** uncompressed);

int __cdecl RedirectSceanPath(void*, char* buff, const char*, char* dir, const char* scn);

}  // bridge

#endif  // __BRIDGE_BRIDGE_H__
