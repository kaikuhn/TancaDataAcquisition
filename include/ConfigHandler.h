
#pragma once

#include <nlohmann/json.hpp>
#include <DigitizerConfig.h>
#include <CollectorConfig.h>

class ErrorHandler;


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    DigitizerConfig, 
    recordLength, 
    postTriggerPct,
    majorityLevel,
    dcOffset, 
    triggerThreshold, 
    polarityPositive, 
    active
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    CollectorConfig, 
    workingDir,
    backupDir,
    enableBackup,
    enableAcquisitionLimit,
    acquisitionLimit
)

class ConfigHandler {
    public:

        // constructor
        ConfigHandler(ErrorHandler* err);

        // Digitizer config

        DigitizerConfig loadDigitizerConfig();
        bool saveDigitizerConfig(DigitizerConfig cfg);

        
        // Collector config

        CollectorConfig loadCollectorConfig();
        bool saveCollectorConfig(CollectorConfig cfg);
    
    private:

        // Error Handler
        ErrorHandler *ERR;
};
