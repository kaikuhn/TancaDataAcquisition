#pragma once

#include <chrono>
#include <ctime>

#include <Arduino.h>
#include <DigitizerWrapper.h>
#include <RootTreeWriter.h>
#include <RateCalculator.h>
#include <ConfigHandler.h>
#include <CollectorConfig.h>

#include <ArduinoData.h>
#include <DigitizerConfig.h>

#include <mutex>

class ErrorHandler;


// DataCollector class
class DataCollector {
    public:

        // constructor
        DataCollector();
        DataCollector(
            std::shared_ptr<CollectorConfig> cc,
            std::shared_ptr<DigitizerConfig> dc,
            ErrorHandler *err,
            std::shared_ptr<TimeTagHandler> tth
        );

        // steering acquisition
        bool open();
        bool close();

        bool applyDigitizerConfig();

        bool startAcquisition();
        bool stopAcquisition();

        // wait till Backup is finished
        void joinRTWBackup();

    private:

        bool startReading();
        bool stopReading();

        void readingLoop();

        // status
        std::atomic<bool> isReading = false;
        std::thread readData;

        bool isOpen = false;
        
        // Member Objects
        Arduino AD;
        DigitizerWrapper DW;
        RootTreeWriter RTW;
        RateCalculator RC;

        std::shared_ptr<CollectorConfig> CC;

        // error handling
        bool boolret;
        ErrorHandler *ERR;

        // mutex
        std::mutex mtx;

};