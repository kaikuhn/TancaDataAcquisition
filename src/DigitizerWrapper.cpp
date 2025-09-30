
#include <DigitizerWrapper.h>

#include <ConfigHandler.h>
#include <TimeTagHandler.h>
#include <ErrorHandler.h>

#include <CAENDigitizer.h>


// constructor

DigitizerWrapper::DigitizerWrapper(
    std::shared_ptr<CollectorConfig> cc,
    std::shared_ptr<DigitizerConfig> dc,
    ErrorHandler *err,
    std::shared_ptr<TimeTagHandler> tth
)
  : CC(cc), 
    DC(dc),
    ERR(err),
    TTH(tth)
{}

bool DigitizerWrapper::applyConfig() {

    // report
    ERR->logInfo("DigitizerWrapper::applyConfig");

    // helper function to convert an array to a bitmask
    auto arrayToBitmask = [](const std::array<bool,3> active) {
        uint32_t mask = 0;
        mask |= (active[0] ? 1 : 0) << 0; // Bit 0
        mask |= (active[1] ? 1 : 0) << 1; // Bit 1
        mask |= (active[2] ? 1 : 0) << 2; // Bit 2
        return mask;    
    };
    
    // set record length (e.g. 1000 samples)
    ret = CAEN_DGTZ_SetRecordLength(handle, DC->recordLength);
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetRecordLength")) return false;

    // set post-trigger size (e.g. 80 %)
    ret = CAEN_DGTZ_SetPostTriggerSize(handle, DC->postTriggerPct);
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetPostTriggerSize")) return false;
    
    // Activate channels 0, 1, and/or 2
    ret = CAEN_DGTZ_SetChannelEnableMask(handle, arrayToBitmask(DC->active));
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetChannelEnableMask")) return false;

    // Activate SelfTrigger
    ret = CAEN_DGTZ_SetChannelSelfTrigger(handle, CAEN_DGTZ_TRGMODE_ACQ_ONLY, arrayToBitmask(DC->active));
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetChannelSelfTrigger")) return false;

    // configure every channel
    for (int channel=0; channel <= 2; channel++) {
        
        // for rising or falling edge
        if(DC->polarityPositive[channel]){
            ret = CAEN_DGTZ_SetTriggerPolarity(handle, channel, CAEN_DGTZ_TriggerOnRisingEdge);
        }
        else {
            ret = CAEN_DGTZ_SetTriggerPolarity(handle, channel, CAEN_DGTZ_TriggerOnFallingEdge);
        }
        
        if (ERR->CheckError(ret, "CAEN_DGTZ_SetTriggerPolarity")) return false;
    }

    // congigure majority level and coincidence window;
    uint32_t reg;
    ret = CAEN_DGTZ_ReadRegister(handle, 0x810C, &reg);
    if (ERR->CheckError(ret, "CAEN_DGTZ_ReadRegister")) return false;
    reg &= ~((0xF << 20) | (0x7 << 24));                // delete old bits
    reg |= (15 << 20) | (DC->majorityLevel << 24);      // write new value
    ret = CAEN_DGTZ_WriteRegister(handle, 0x810C, reg);
    if (ERR->CheckError(ret, "CAEN_DGTZ_WriteRegister")) return false;

    // configure buffer
    if (buffer != nullptr) {
        ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
        if (ERR->CheckError(ret, "CAEN_DGTZ_FreeReadoutBuffer")) return false;
    }

    ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle, 1023);
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetMaxNumEventsBLT")) return false;

    // allocate storage for readout-buffer
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &buffer, &bufferSize);
    if (ERR->CheckError(ret, "CAEN_DGTZ_MallocReadoutBuffer")) return false;

    // allocate Event-Container (for 12-/14-bit device: UINT16_EVENT)
    ret = CAEN_DGTZ_AllocateEvent(handle, &eventPtr);
    if (ERR->CheckError(ret, "CAEN_DGTZ_AllocateEvent")) return false;
    evt = reinterpret_cast<CAEN_DGTZ_UINT16_EVENT_t*>(eventPtr);

    // configure every channel
    for (int channel=0; channel <= 2; channel++) {
    
        // set trigger threshold for channel (e.g. ~18 mV)
        ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle, channel, DC->triggerThreshold[channel]);
        if (ERR->CheckError(ret, "CAEN_DGTZ_SetChannelTriggerThreshold")) return false;

        // set DC offset for channel
        ret = CAEN_DGTZ_SetChannelDCOffset(handle, channel, DC->dcOffset[channel]);
        if (ERR->CheckError(ret, "CAEN_DGTZ_SetChannelDCOffset")) return false;
    }
    
    return true;
}


// steering data acquisition

bool DigitizerWrapper::open(){

    // report
    ERR->logInfo("DigitizerWrapper::open");

    // open connection to digitizer
    int usbIndex = 0; // zero for the first digitizer
    ret = CAEN_DGTZ_OpenDigitizer2(CAEN_DGTZ_USB, &usbIndex, 0, 0, &handle);
    if (ERR->CheckError(ret, "CAEN_DGTZ_OpenDigitizer")) return false;

    // reset digitizer (hardware-reset)
    ret = CAEN_DGTZ_Reset(handle);
    if (ERR->CheckError(ret, "CAEN_DGTZ_Reset")) return false;

    // global configuration (acquisition mode: software-controlled)
    ret = CAEN_DGTZ_SetAcquisitionMode(handle, CAEN_DGTZ_SW_CONTROLLED);
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetAcquisitionMode")) return false;

    // deactivate extern trigger
    ret = CAEN_DGTZ_SetExtTriggerInputMode(handle, CAEN_DGTZ_TRGMODE_DISABLED);
    if (ERR->CheckError(ret, "CAEN_DGTZ_SetExtTriggerInputMode")) return false;

    return true;
}

bool DigitizerWrapper::close(){

    // report
    ERR->logInfo("DigitizerWrapper::close");

    // clear storage
    if (buffer != nullptr) {
        ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
        ERR->CheckError(ret, "CAEN_DGTZ_FreeReadoutBuffer");
    }

    // close connection to digitizer
    if (handle != -1) {
        ret = CAEN_DGTZ_CloseDigitizer(handle);
        ERR->CheckError(ret, "CAEN_DGTZ_CloseDigitizer");

        handle = -1; // reset handle  
    }

    return true;
}


bool DigitizerWrapper::startCollecting(){

    // report
    ERR->logInfo("DigitizerWrapper::startCollecting");

    // check
    if (isCollecting.load()) {
        ERR->ThrowError("Tried to start Digitizer, but Digitizer is already collecting");
        return false;
    }

    // set status
    isCollecting.store(true);

    // start data acquisition
    ret = CAEN_DGTZ_SWStartAcquisition(handle);
    if (ERR->CheckError(ret, "CAEN_DGTZ_SWStartAcquisition")) return false;

    // update start time (for absolute time stamps)
    TTH->setStartTime();

    // start collection loop
    collectDigitizerData = std::thread(&DigitizerWrapper::collectingLoop, this);

    return true;
}

bool DigitizerWrapper::stopCollecting(){

    // report
    ERR->logInfo("DigitizerWrapper::stopCollecting");

    // stop data acquisition
    if (isCollecting.load()) {
        ret = CAEN_DGTZ_SWStopAcquisition(handle);
        ERR->CheckError(ret, "CAEN_DGTZ_SWStopAcquisition");

        // stop is triggered by setting flag isCollecting to false
        isCollecting.store(false);

        // wait collectingLoop to end
        if (collectDigitizerData.joinable()) {
            collectDigitizerData.join();
        }
    }

    return true;
}

void DigitizerWrapper::collectingLoop() {

    uint64_t loopCount = 0;
    uint64_t eventID = 0;

    // reading events in a loop
    while (isCollecting.load()) {

        // report
        if (CC->detailedLog) {
            ERR->logInfo("DigitizerWrapper::collectingLoop: loopCount: " + std::to_string(loopCount));
        }

        // wait for events
        usleep(100000); // wait 100 ms

        // check if data is in buffer
        uint32_t aktuelleBufferSize = 0;
        ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, buffer, &aktuelleBufferSize);
        if (ERR->CheckError(ret, "CAEN_DGTZ_ReadData")) return;

        if (aktuelleBufferSize > 0) {
            uint32_t numEvents = 0;
            ret = CAEN_DGTZ_GetNumEvents(handle, buffer, aktuelleBufferSize, &numEvents);
            if (ERR->CheckError(ret, "CAEN_DGTZ_GetNumEvents")) return;

            // report
            ERR->logInfo("Digitizer: loopCount: " + std::to_string(loopCount) + ": " + std::to_string(numEvents) + " event(s) recognized");

            if (numEvents > 0) {

                CAEN_DGTZ_EventInfo_t eventInfo{};
                char* eventPtr = nullptr;
                void* decodedEvent = nullptr;

                // write every event in ttree
                for(int index=0; index<numEvents; index++){

                    // report
                    if (CC->detailedLog) {
                        ERR->logInfo("DigitizerWrapper::collectingLoop: eventID: " + std::to_string(eventID));
                    }
                    
                    // get EventInfo and EventPointer
                    ret = CAEN_DGTZ_GetEventInfo(handle, buffer, aktuelleBufferSize, index, &eventInfo, &eventPtr);
                    if (ERR->CheckError(ret, "CAEN_DGTZ_GetEventInfo")) return;

                    // decode event
                    ret = CAEN_DGTZ_DecodeEvent(handle, eventPtr, &decodedEvent);
                    if (ERR->CheckError(ret, "CAEN_DGTZ_DecodeEvent")) return;

                    // cast to correct type
                    evt = (CAEN_DGTZ_UINT16_EVENT_t*)decodedEvent;
                    
                    // initialize vectors with data
                    std::vector<Double_t> v0(evt->ChSize[0]), v1(evt->ChSize[1]), v2(evt->ChSize[2]);

                    // lambda function that copies data from ADC samples to the data vector
                    auto fillChannel = [](std::vector<Double_t>& dst, const uint16_t* src, uint32_t size) {
                        std::transform(src, src + size, dst.begin(),
                                    [](uint16_t x) { return static_cast<Double_t>(x); });
                    };

                    // fill vectors with dataevt->ChSize[2]
                    if (DC->active[0]) fillChannel(v0, evt->DataChannel[0], evt->ChSize[0]);
                    if (DC->active[1]) fillChannel(v1, evt->DataChannel[1], evt->ChSize[1]);
                    if (DC->active[2]) fillChannel(v2, evt->DataChannel[2], evt->ChSize[2]);

                    // clean event from buffer
                    ret = CAEN_DGTZ_FreeEvent(handle, &decodedEvent);
                    if (ERR->CheckError(ret, "CAEN_DGTZ_FreeEvent")) return;

                    // get time event
                    uint32_t ttt = eventInfo.TriggerTimeTag;
                    
                    // devode event time in absolute time in ns since 1970
                    Long64_t ts = static_cast<Long64_t>(TTH->decode(ttt));

                    // sumarize data
                    DigitizerData DGEvt = DigitizerData(eventID, ts, v0, v1, v2);

                    // add event to queue
                    q.push(std::move(DGEvt));

                    // increase eventID
                    eventID++;
                }
            }
        }

        // increase loopCount
        loopCount++;
    }

    // free readout buffer
    ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    ERR->CheckError(ret, "CAEN_DGTZ_FreeReadoutBuffer");
}
