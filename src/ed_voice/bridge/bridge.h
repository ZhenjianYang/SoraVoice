#ifndef __BRIDGE_BRIDGE_H__
#define __BRIDGE_BRIDGE_H__

namespace bridge {

int __stdcall Play(void* b);
int __cdecl Stop();

int __cdecl LoadDat(void*, void* buff, int idx, unsigned offset, unsigned size);
int __cdecl LoadDatF(void* buff, int idx, void*, unsigned offset, unsigned size);
int __cdecl LoadDat2(void*, void* buff, int idx);
int __cdecl DecompressDat(void*, void** uncompressed, void** compressed);

int __cdecl RedirectSceanPath(void*, char* buff, const char*, char* dir, const char* scn);

}  // bridge

#endif  // __BRIDGE_BRIDGE_H__
