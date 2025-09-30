
#pragma once

#include <filesystem>

struct CollectorConfig {
    private:
        static std::filesystem::path expandHome(const std::string& path) {
            if (!path.empty() && path[0] == '~') {
                const char* home = std::getenv("HOME"); 
                if (home) {
                    return std::filesystem::path(home) / path.substr(2);
                }
            }
            return path;
        }

    public:
        // Directories
        std::string workingDir = expandHome("~/TancaData/").string();
        std::string backupDir  = expandHome("~/TancaBackup/").string();

        bool enableBackup = false;
        bool detailedLog = false;

        bool enableAcquisitionLimit = false;
        int acquisitionLimit = 0;
};