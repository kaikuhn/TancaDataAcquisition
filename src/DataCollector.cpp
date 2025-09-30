
#include <DataCollector.h>
#include <Arduino.h>


// constructor

DataCollector::DataCollector(
    std::shared_ptr<CollectorConfig> cc,
    std::shared_ptr<DigitizerConfig> dc,
    ErrorHandler *err,
    std::shared_ptr<TimeTagHandler> tth
)
  : CC(cc),
    DW(cc, dc, err, tth),
    RTW(cc, err),
    AD(cc, err, tth),
    ERR(err)
{}


// handeling connections

bool DataCollector::open() {
    
    // lock block for threadsafe
    std::lock_guard<std::mutex> lock(mtx);

    // report
    ERR->logInfo("DataCollector::open");

    if (!isOpen) {

        // open digitizer
        boolret = DW.open();
        if (ERR->CheckError(boolret, "DW.open")) return false;

        // open connection to Arduino
        boolret = AD.open();
        if (ERR->CheckError(boolret, "AD.open")) {
            DW.close();
            return false;
        }
    }
    
    // status
    isOpen = true;

    return true;
}

bool DataCollector::close() {
    
    // lock block for threadsafe
    std::lock_guard<std::mutex> lock(mtx);

    // report
    ERR->logInfo("DataCollector::close");

    if (isOpen) {
        // close digitizer
        boolret = DW.close();
        ERR->CheckError(boolret, "DW.close");

        // close File in RootTreeWriter
        boolret = RTW.closeCurrentFile();
        ERR->CheckError(boolret, "RTW.CloseCurrentFile");
    }

    //status
    isOpen = false;

    return true;
}

bool DataCollector::applyDigitizerConfig() {

    // report 
    ERR->logInfo("DataCollector::applyDigitizerConfig");

    // configure digitizer
    boolret = DW.applyConfig();
    if (ERR->CheckError(boolret, "DW.setStaticConfig")) return false;

    return true;
}

bool DataCollector::startAcquisition() {

    // report
    ERR->logInfo("DataCollector::startAcquisition");

    // start Digitizer
    boolret = DW.startCollecting();
    if (ERR->CheckError(boolret, "DW.startCollecting")) return false;

    // start Arduino
    boolret = AD.startCollecting();
    if (ERR->CheckError(boolret, "AD.startCollecting")) {
        DW.stopCollecting();
        return false;
    }

    // start reading loop
    boolret = startReading();
    if (ERR->CheckError(boolret, "startReading")) {
        DW.stopCollecting();
        AD.stopCollecting();
        return false;
    }

    return true;
}

void DataCollector::joinRTWBackup() {
    RTW.joinBackup();
}

bool DataCollector::stopAcquisition() {

    // report
    ERR->logInfo("DataCollectro::stopAcquisition");

    // stop Digitizer
    boolret = DW.stopCollecting();
    ERR->CheckError(boolret, "DW.stopCollecting");

    // stop Arduino
    boolret = AD.stopCollecting();
    ERR->CheckError(boolret, "AD.stopCollecting");

    // stop Reading
    if (isReading.load()) {
        boolret = stopReading();
        ERR->CheckError(boolret, "stopReading");
    }

    return true;
}

bool DataCollector::startReading() {

    // report
    ERR->logInfo("DataCollector::startReading");

    // set status
    isReading.store(true);

    // start readingLoop
    readData = std::thread(&DataCollector::readingLoop, this);

    return true;
}

bool DataCollector::stopReading() {

    // report
    ERR->logInfo("DataCollector::stopReading");

    // stop is triggered by setting flag isReading to false
    isReading.store(false);

    // wait tread to end
    if (readData.joinable()) readData.join();

    return true;
}

void DataCollector::readingLoop() {

    // open File in RootTreeWriter
    if (!RTW.getFileOpen()) {
        boolret = RTW.openNewFile();
        if (ERR->CheckError(boolret, "RTW.OpenNewFile")) return;
    }

    // get current hour function
    auto getCurrentHour = []() {
        std::time_t t = std::time(nullptr);
        std::tm tm{};
        localtime_r(&t, &tm);
        return tm.tm_hour;
    };

    int currentHour = getCurrentHour();

    int arduinoEventCounter = 0;
    int digitizerEventCounter = 0;
    uint64_t loopCount = 0;
    
    while (isReading.load())
    {   
        // report
        if (CC->detailedLog) {
            ERR->logInfo("DataCollector::readingLoop: loopCount: " + std::to_string(loopCount));
        }

        // file check
        if (currentHour != getCurrentHour()){
            boolret = RTW.closeCurrentFile();
            if (ERR->CheckError(boolret, "closeCurrentFile")) { 
                stopAcquisition(); 
                return; 
            }

            boolret = RTW.openNewFile();
            if (ERR->CheckError(boolret, "openNewFile")) { 
                stopAcquisition(); 
                return; 
            }

            // update current hour
            currentHour = getCurrentHour();

            // reset digitizerEventCounter
            digitizerEventCounter = 0;
        }

        // Get Data from Digitizer
        while (auto DDataOpt = DW.getDigitizerData()) {
            
            // get Data out of Queue
            DigitizerData DData = std::move(*DDataOpt);

            // report
            if (CC->detailedLog) {
                ERR->logInfo("DataCollector::readingLoop: Digitizer eventID: " + std::to_string(DData.eventID));
            }

            // add time stamps to calculate rate
            RC.addElement(DData.eventTime);

            // prepare data1 to write
            if (!CC->enableAcquisitionLimit || digitizerEventCounter < CC->acquisitionLimit) {

                // report
                if (CC->detailedLog) {
                    ERR->logInfo("DataCollector::readingLoop: digitizerEventCount: " + std::to_string(digitizerEventCounter));
                }

                RTW.set_data1(
                    DData.eventTime, 
                    std::move(DData.ch0), 
                    std::move(DData.ch1), 
                    std::move(DData.ch2)
                );
            }

            // increase eventCoutner
            digitizerEventCounter++;
        }

        // Get Data from Arduino
        if (auto ADDataOpt = AD.getArduinoData()) {

            // get Data out of Queue
            ArduinoData ADData = std::move(*ADDataOpt); 

            // report
            if (CC->detailedLog) {
                ERR->logInfo("DataCollector::readingLoop: Arduino eventID: " + std::to_string(ADData.eventID));
            }

            // get rate
            double rate = RC.calcRate();

            // check whether  rate exists
            if (rate != std::numeric_limits<double>::quiet_NaN()) {
                
                // prepare data2 to write
                RTW.set_data2(ADData.event_time, rate, ADData.arduino_p);

                // prepare data3 to write
                if (arduinoEventCounter>5) {
                    RTW.set_data3(
                        ADData.event_time, 
                        ADData.tanca_h1, 
                        ADData.tanca_t1, 
                        ADData.tanca_h2, 
                        ADData.tanca_t2, 
                        ADData.tanca_h3, 
                        ADData.tanca_t3, 
                        ADData.tanca_h4, 
                        ADData.tanca_t4
                    );
                    arduinoEventCounter = -1;
                }

                arduinoEventCounter++;
            } else {
                ERR->ThrowError("DataCollector::readingLoop: Arduino eventID: " + std::to_string(ADData.eventID) + " has no rate");
            }
            

        }
        
        // wait 500ms
        usleep(500000);
        loopCount++;
    }

}
