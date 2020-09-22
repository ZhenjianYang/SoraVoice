#include <Windows.h>

#include <iostream>

#include "player/player.h"

int main(int argc, char* argv[]) {
    void* pDS = nullptr;
    auto player = player::Player::GetPlayer(&pDS);
}
