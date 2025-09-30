
#include <TimeTagHandler.h>

#include <chrono>
#include <TTree.h>
#include <ErrorHandler.h>

// constructors
TimeTagHandler::TimeTagHandler(ErrorHandler *err) 
  : ERR(err)
{}

// get Time Stamp in ns (since 1970-01-01 UTC)) to compare it with start time
uint64_t TimeTagHandler::getTimeStamp() {
    
    // get time now in ns
    auto timeStamp = std::chrono::system_clock::now().time_since_epoch();

    // convert to uint64_t and ns
    return std::chrono::duration_cast<std::chrono::duration<uint64_t, std::nano>>(timeStamp).count();
}

// set start time (ns since 1970-01-01 UTC)
void TimeTagHandler::setStartTime() {

    // set start time to current time
    startTime = getTimeStamp();

    // reset
    lastTriggerTag = 0;
    overflowCounter = 0; 
}

// Decode 32-bit TriggerTag into continuous timestamp [ns since 1970]
uint64_t TimeTagHandler::decode(uint32_t triggerTag) {

    // recognice overflow (new value smaller then previous)
    if (triggerTag < lastTriggerTag) {
        overflowCounter++;

        // report
        ERR->logInfo("TimeTagHandler::decode: overflowCOunter++");
    }

    // update trigger tag
    lastTriggerTag = triggerTag;
    
    // full ticks including overflows since start
    uint64_t fullTicks = (static_cast<uint64_t>(overflowCounter) << 32) | triggerTag;

    // trigger offset in ns
    uint64_t triggerOffsetNS = fullTicks * timeTagLSBNS;

    // absolute timestamp in ns (system epoch + trigger offset)
    return startTime + triggerOffsetNS;
}