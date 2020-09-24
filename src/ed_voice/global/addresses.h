#ifndef __GLOBAL_ADDRESSES_H__
#define __GLOBAL_ADDRESSES_H__

typedef struct Addresses {
    void** pHwnd;

    void* text_next;
    void* text_jmp;
} Addresses;

#endif  // __GLOBAL_ADDRESSES_H__
