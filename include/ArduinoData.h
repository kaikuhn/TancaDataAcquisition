#pragma once

#include <TTree.h>
#include <cstdint>

// Arduino Data strucutre
struct ArduinoData{

    // constructor
    ArduinoData(
        uint64_t ID,
        Double_t et,
        Double_t at, 
        Double_t ap, 
        Double_t t1, 
        Double_t h1, 
        Double_t t2, 
        Double_t h2, 
        Double_t t3, 
        Double_t h3, 
        Double_t t4,
        Double_t h4
    ) : eventID(ID),
        event_time(et),
        arduino_t(at),
        arduino_p(ap),
        tanca_t1(t1),
        tanca_h1(h1),
        tanca_t2(t2),
        tanca_h2(h2),
        tanca_t3(t3),
        tanca_h3(h3),
        tanca_t4(t4),
        tanca_h4(h4)
    {};

    // eventID
    uint64_t eventID;

    // time
    Long64_t event_time;

    // data arduino sensors
    Double_t arduino_t, arduino_p;

    // data tanca sensors
    Double_t tanca_t1, tanca_h1;
    Double_t tanca_t2, tanca_h2;
    Double_t tanca_t3, tanca_h3;
    Double_t tanca_t4, tanca_h4;
};