
#pragma once

#include <TTree.h>
#include <array>

// Digitizer Configuration
struct DigitizerConfig {

    // global
    uint32_t recordLength = 1024;   // samples
    uint32_t postTriggerPct = 50;   // 0..100
    int majorityLevel = 1;

    // per channel
    std::array<uint16_t,3> dcOffset = {32768, 32768, 32768};        // 0...65535
    std::array<uint16_t,3> triggerThreshold = {1900,1900,1900};     // 0...4095
    std::array<bool,3> polarityPositive = {false, false, false};    // true=Rising, false=Falling
    std::array<bool,3> active = {true, true, true};                 // true=active, false=inactive
};
