
#include <ConfigHandler.h>

#include <fstream>
#include <array>

#include <ErrorHandler.h>


// constructor

ConfigHandler::ConfigHandler(ErrorHandler* err)
  : ERR(err)
{}


// Digitizer config

DigitizerConfig ConfigHandler::loadDigitizerConfig() {

    // report
    ERR->logInfo("ConfigHandler::loadDigitizerConfig");

    const std::string filename = "DigitizerConfig.json";

    // check if config exists and create file, if it doesnt exist
    std::ifstream test(filename);
    if (!test.good()) {
        saveDigitizerConfig(DigitizerConfig());
    }

    // load config file
    std::ifstream in(filename);

    if (!in.is_open() || in.peek() == std::ifstream::traits_type::eof()) {
        ERR->logInfo("Config file " + filename + " doesnt exist.");
    }

    // get config from file
    nlohmann::json j;
    in >> j;

    return j.get<DigitizerConfig>();
};


bool ConfigHandler::saveDigitizerConfig(DigitizerConfig cfg) {

    // report
    ERR->logInfo("ConfigHandler::saveDigitizerConfig");

    nlohmann::json j = cfg;
    std::ofstream("DigitizerConfig.json") << j.dump(4);
    return true;
};


// Collector config

CollectorConfig ConfigHandler::loadCollectorConfig() {

    // report
    ERR->logInfo("ConfigHandler::loadCollectorConfig");

    const std::string filename = "CollectorConfig.json";

    // check if config exists and create file, if it doesnt exist
    std::ifstream test(filename);
    if (!test.good()) {
        saveCollectorConfig(CollectorConfig());
    }

    // load config file
    std::ifstream in(filename);

    if (!in.is_open() || in.peek() == std::ifstream::traits_type::eof()) {
        ERR->logInfo("Config file " + filename + " doesnt exist.");
    }

    // get config from file
    nlohmann::json j;
    in >> j;

    return j.get<CollectorConfig>();
}


bool ConfigHandler::saveCollectorConfig(CollectorConfig cfg) {

    // report
    ERR->logInfo("CollectorConfig::saveCollectorConfig");

    nlohmann::json j = cfg;
    std::ofstream("CollectorConfig.json") << j.dump(4);
    return true;
}
