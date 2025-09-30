#pragma once

#include <chrono>
#include <TTree.h>

class ErrorHandler;

class TimeTagHandler {
    public:

        TimeTagHandler(ErrorHandler *err);

        // get Time Stamp in ns (since 1970-01-01 UTC)) to compare it with start time
        uint64_t getTimeStamp();
        
        // set start time (ns since 1970-01-01 UTC)
        void setStartTime();

        // Decode 32-bit TriggerTag into continuous timestamp [ns since 1970]
        uint64_t decode(uint32_t triggerTag);

    private:
        // trigger time tag
        uint64_t timeTagLSBNS = 4;     // ticksize in ns

        // starting time
        uint64_t startTime;
        uint32_t lastTriggerTag = 0;    // letzter 32-bit Wert
        uint64_t overflowCounter = 0;   // Anzahl Overflows

        ErrorHandler *ERR;
};
