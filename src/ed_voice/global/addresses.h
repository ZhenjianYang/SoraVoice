#ifndef __GLOBAL_ADDRESSES_H__
#define __GLOBAL_ADDRESSES_H__

typedef struct Addresses {
    void** pHwnd;

    void* text_next;
    void* text_jmp;

    void* pdirs;
    void* ldat_next;
    void* dcdat_next;

    void* textse_next;
    void* textse_jmp;
    void* dlgse_next;
    void* dlgse_jmp;
} Addresses;

#endif  // __GLOBAL_ADDRESSES_H__
