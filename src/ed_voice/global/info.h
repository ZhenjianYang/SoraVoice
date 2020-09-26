#ifndef __GLOBAL_INFO_H__
#define __GLOBAL_INFO_H__

enum Game {
    GameUnknown = 0,
    GameSoraFC,
    GameSoraSC,
    GameSora3rd,
    GameTitsFC,
    GameTitsSC3rd,
    GameZero,
    GameAo
};

typedef struct Info {
    int game;
} Info;

#endif  // __GLOBAL_INFO_H__
