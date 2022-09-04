#pragma once

#include <cstdint>

class SteamFriends {
public:
    int getSmallFriendAvatar(std::uint64_t steamid)
    {
        using original_fn = int(__thiscall*)(SteamFriends*, std::uint64_t);
        return (*(original_fn**)this)[34](this, steamid);
    }
};

class SteamUtils {
public:
    bool getImageRGBA(int image, std::uint8_t* buff, int buffSize)
    {
        using original_fn = bool(__thiscall*)(SteamUtils*, int, std::uint8_t*, int);
        return (*(original_fn**)this)[6](this, image, buff, buffSize);
    }
};

struct SteamAPIContext {
    void* steamClient;
    void* steamUser;
    SteamFriends* steamFriends;
    SteamUtils* steamUtils;
};