#pragma once

#include <string>
#include <TFile.h>
#include <TTree.h>
#include <vector>
#include <atomic>
#include <thread>

#include <ErrorHandler.h>

class CollectorConfig;

namespace fs = std::filesystem;

class RootTreeWriter {
    public:
    
        // constructor
        RootTreeWriter(
            std::shared_ptr<CollectorConfig> cc,
            ErrorHandler *err
        );

        // check fileOpen
        bool getFileOpen() { if (file) return true; return false; }
        
        // file handling
        bool openNewFile();
        bool closeCurrentFile();

        // add events
        void set_data1(Long64_t ts_data1_, std::vector<Double_t>&& ch0_, std::vector<Double_t>&& ch1_, std::vector<Double_t>&& ch2_);
        void set_data2(Long64_t ts_data2_, Double_t rate_, Double_t pressure_);
        void set_data3(Long64_t ts_data3_, Double_t tanca_h2_, Double_t tanca_t1_, Double_t tanca_h1_, Double_t tanca_t2_, Double_t tanca_t3_, Double_t tanca_h3_, Double_t tanca_t4_, Double_t tanca_h4_);

        // write backup
        bool writeBackup();

        // join backup thread
        void joinBackup();

    private:

        // file paths
        std::shared_ptr<CollectorConfig> CC;

        // backup
        std::thread backupThread;
        std::atomic<bool> backupRunning = false;

        std::string folderName;
        std::string fileName;
        std::filesystem::path filePath;

        void copyFile(fs::path source, fs::path dest);

        // data handling
        TFile* file = nullptr;

        // trees
        TTree* data1 = nullptr;
        TTree* data2 = nullptr;
        TTree* data3 = nullptr;

        // branch placeholder variables
        Long64_t ts_data1;
        std::vector<Double_t> ch0;
        std::vector<Double_t> ch1;
        std::vector<Double_t> ch2;

        Long64_t ts_data2;
        Double_t rate;
        Double_t pressure;

        Long64_t ts_data3;
        Double_t tanca_h1;
        Double_t tanca_t1;
        Double_t tanca_h2;
        Double_t tanca_t2;
        Double_t tanca_h3;
        Double_t tanca_t3;
        Double_t tanca_h4;
        Double_t tanca_t4;

        // error handling
        ErrorHandler *ERR;
};
