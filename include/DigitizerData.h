
#pragma once

struct DigitizerData {

    // constructor
    DigitizerData(
        uint64_t id_,
        Double_t et_, 
        std::vector<Double_t> ch0_, 
        std::vector<Double_t> ch1_, 
        std::vector<Double_t> ch2_
    )
      : eventID(id_),
        eventTime(et_), 
        ch0(std::move(ch0_)), 
        ch1(std::move(ch1_)), 
        ch2(std::move(ch2_)) 
    {}

    // eventID
    uint64_t eventID;

    // time tag
    Long64_t eventTime;

    // channel rows
    std::vector<Double_t> ch0, ch1, ch2;
};