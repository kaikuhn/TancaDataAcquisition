#include <CAENDigitizer.h>
#include <tuple>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <thread>

#include <CollectorConfig.h>
#include <DigitizerWrapper.h>
#include <RootTreeWriter.h>

// constructor

RootTreeWriter::RootTreeWriter(
    std::shared_ptr<CollectorConfig> cc,
    ErrorHandler *err
) : CC(cc),
    ERR(err)
{}


// file handling

bool RootTreeWriter::openNewFile() {

    // report
    ERR->logInfo("RootTreeWriter::openNewFile");

    // check
    if (file) {
        ERR->ThrowError("openNewFile: File is already open");
        return false;
    }

    // set fileName, folderName and filePath
    auto createFilePath = [=](std::string workingDir) {
        // get time
        auto t = std::time(nullptr);
        std::tm tm{};
        gmtime_r(&t, &tm);

        char buf[32];

        // get file name
        std::strftime(buf, sizeof(buf), "%Y_%m_%d_%H", &tm);
        fileName = static_cast<std::string>(buf) + "_tanca.root";

        // get folder name
        std::strftime(buf, sizeof(buf), "%Y_%m_%d", &tm);
        folderName = static_cast<std::string>(buf);

        // get total file path
        filePath = fs::path(workingDir) / folderName / fileName;
    };

    // set variables of backup
    createFilePath(CC->workingDir);

    // create folder if it doesnt exist
    fs::create_directories(filePath.parent_path());

    // open new file and check
    file = new TFile(filePath.c_str(), "RECREATE");
    if (!file || file->IsZombie()) {
        ERR->ThrowError("error when opening the ROOT current file");
        return false;
    }

    // create new TTree
    data1 = new TTree("data1", "Digitizer Data");
    data2 = new TTree("data2", "Arduino Data 1");
    data3 = new TTree("data3", "Arduino Data 2");

    // define Branches
    data1->Branch("ts_data1",   &ts_data1,   "ts_data1/L");
    data1->Branch("ch0", &ch0);
    data1->Branch("ch1", &ch1);
    data1->Branch("ch2", &ch2);

    data2->Branch("ts_data2",   &ts_data2,   "ts_data2/L");
    data2->Branch("rate",       &rate,       "rate/D");
    data2->Branch("pressure",   &pressure,   "pressure/D");

    data3->Branch("ts_data3",   &ts_data3,   "ts_data3/L");
    data3->Branch("tanca_h2",   &tanca_h2,   "tanca_h2/D");
    data3->Branch("tanca_t1",   &tanca_t1,   "tanca_t1/D");
    data3->Branch("tanca_h1",   &tanca_h1,   "tanca_h1/D");
    data3->Branch("tanca_t2",   &tanca_t2,   "tanca_t2/D");
    data3->Branch("tanca_t3",   &tanca_t3,   "tanca_t3/D");
    data3->Branch("tanca_h3",   &tanca_h3,   "tanca_h3/D");
    data3->Branch("tanca_t4",   &tanca_t4,   "tanca_t4/D");
    data3->Branch("tanca_h4",   &tanca_h4,   "tanca_h4/D");

    return true;
}


bool RootTreeWriter::closeCurrentFile() {

    // report
    ERR->logInfo("RootTreeWriter::closeCurrentFile");

    // check
    if (!file) {
        ERR->ThrowError("No File open");
        return false;
    }

    file->cd();          // change to file dir

    // write TTrees in file
    if (data1 && data2 && data3) {
        data1->Write();
        data2->Write();
        data3->Write();
    }

    file->Close();       // close the ROOT file (will also delete the TTrees)
    delete file;         // clear storage
    file = nullptr;      // reset pointer
    data1 = nullptr;
    data2 = nullptr;
    data3 = nullptr;

    // start backup
    if (CC->enableBackup) writeBackup();

    return true;  
}


// add events

void RootTreeWriter::set_data1(Long64_t ts_data1_, std::vector<Double_t>&& ch0_, std::vector<Double_t>&& ch1_, std::vector<Double_t>&& ch2_) {
    ts_data1 = ts_data1_;
    ch0 = ch0_;
    ch1 = ch1_;
    ch2 = ch2_;

    // fill data
    data1->Fill();
}

void RootTreeWriter::set_data2(Long64_t ts_data2_, Double_t rate_, Double_t pressure_) {
    ts_data2 = ts_data2_;
    rate = rate_;
    pressure = pressure_;

    // fill data
    data2->Fill();
}

void RootTreeWriter::set_data3(Long64_t ts_data3_, Double_t tanca_h1_, Double_t tanca_t1_, Double_t tanca_h2_, Double_t tanca_t2_, Double_t tanca_h3_, Double_t tanca_t3_, Double_t tanca_h4_, Double_t tanca_t4_) {
    ts_data3 = ts_data3_;
    tanca_h1 = tanca_h1_;
    tanca_t1 = tanca_t1_;
    tanca_h2 = tanca_h2_;
    tanca_t2 = tanca_t2_;
    tanca_h3 = tanca_h3_;
    tanca_t3 = tanca_t3_;
    tanca_h4 = tanca_h4_;
    tanca_t4 = tanca_t4_;

    // fill data
    data3->Fill();
}


// write backup

bool RootTreeWriter::writeBackup() {
    
    // prepare source and dest file paths
    fs::path source = filePath;
    fs::path dest = fs::path(CC->backupDir) / folderName / fileName;

    // report
    ERR->logInfo("RootTreeWriter::writeBackup: " + source.string());

    // start copy
    if (!backupRunning.load()) {
        backupThread = std::thread(&RootTreeWriter::copyFile, this, source, dest);
        return true;
    }

    ERR->ThrowError("RootTreeWriter::writeBackup: Backup failed: " + source.string());
    ERR->ThrowError("Previous backup is still running");

    return false;
}

void RootTreeWriter::copyFile(fs::path source, fs::path dest) {

    // status
    backupRunning.store(true);

    // create dest folder if it doesnt exist
    fs::create_directories(dest.parent_path());

    std::cout << "copy file " << source << " to " << dest << std::endl;
    fs::copy_file(
        source,
        dest,
        fs::copy_options::overwrite_existing
    );

    // status
    backupRunning.store(false);
}


// join backup

void RootTreeWriter::joinBackup() {
    
    // wait till copy is finished
    if (backupThread.joinable()) {
        backupThread.join();
    }
}