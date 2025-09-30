#pragma once

#include <TTree.h>
#include <string>
#include <atomic>
#include <thread>
#include <optional>

#include <DigitizerData.h>
#include <CollectorConfig.h>
#include <DigitizerConfig.h>
#include <TSQueue.h>
#include <ConfigHandler.h>
#include <TimeTagHandler.h>
#include <ErrorHandler.h>

class DigitizerWrapper {
    public:
        // constructor
        DigitizerWrapper(
            std::shared_ptr<CollectorConfig> cc,
            std::shared_ptr<DigitizerConfig> dc,
            ErrorHandler *err,
            std::shared_ptr<TimeTagHandler> tth
        );

        // set configuration to digitizer
        bool applyConfig();

        // steering data acquisition
        bool open();
        bool close();

        bool startCollecting();
        bool stopCollecting();

        // get data from digitizer
        std::optional<DigitizerData> getDigitizerData() { return q.pop(); };
        
    private:
        // status
        std::atomic<bool> isCollecting = false;

        // collectingLoop
        void collectingLoop();
        std::thread collectDigitizerData;

        // Tree variables
        int eventID;
        uint64_t timeTag;
        std::vector<uint16_t> waveforms[3];
        uint32_t numSamples[3];

        // storage for digitizer handle and buffer
        int handle = -1;
        char* buffer = nullptr;
        void* eventPtr = nullptr;
        uint32_t bufferSize = 0;

        uint32_t boardStatus;

        CAEN_DGTZ_UINT16_EVENT_t* evt;

        // queue for exchange
        TSQueue<DigitizerData> q;

        // configuration
        std::shared_ptr<DigitizerConfig> DC;
        std::shared_ptr<CollectorConfig> CC;

        // time stamp handling
        std::shared_ptr<TimeTagHandler> TTH;

        // error handling
        CAEN_DGTZ_ErrorCode ret;
        ErrorHandler *ERR;
};
