#ifndef __BRIDGE_BRIDGE_H__

namespace bridge {

int __stdcall Play(void* b);
int __cdecl LoadDat(void*, void* buff, int idx, unsigned offset, unsigned size);
int __stdcall DecompressDat(void** compressed, void** uncompressed);

}  // bridge

#endif  // __BRIDGE_BRIDGE_H__
