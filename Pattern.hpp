#pragma once

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <Psapi.h>

uintptr_t FindPattern(const char* module, const char* pattern) {
    MODULEINFO moduleInfo;
    HMODULE hModule = GetModuleHandleA(module);
    if (hModule == NULL) {
        return 0;
    }

    GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));
    uintptr_t baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);

    std::istringstream patternStream(pattern);
    std::string byteStr;
    bool isFirstNibble = true;
    std::vector<uint8_t> patternBytes;

    while (patternStream >> byteStr) {
        if (byteStr == "?") {
            patternBytes.push_back(0);
        }
        else {
            patternBytes.push_back(static_cast<uint8_t>(std::stoul(byteStr, 0, 16)));
        }
    }

    for (uintptr_t i = 0; i < moduleInfo.SizeOfImage - patternBytes.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < patternBytes.size(); j++) {
            if (patternBytes[j] != 0 && patternBytes[j] != *reinterpret_cast<const uint8_t*>(baseAddress + i + j)) {
                found = false;
                break;
            }
        }
        if (found) {
            return baseAddress + i;
        }
    }

    return 0;
}
