# TancaDataAcquisition (C++)

C++ software for real-time data acquisition with a CAEN DT5720B digitizer and Arduino Uno, measuring muon radiation and environmental conditions

## Description

This software was developed during a project internship at the Universidade Estadual de Campinas in the lab of Professor Anderson Fauth.
It is designed for data acquisition in the Tanca Project, an experimental setup to detect cosmic muon radiation.

## The software

- Collects and stores signals from three photomultiplier tubes (PMTs) connected to a CAEN DT5720B digitizer.

- Integrates readings from environmental sensors (pressure and temperature) connected to an Arduino Uno.

- Uses a Producer-Consumer architecture to handle concurrent data acquisition and storage efficiently.

- Stores the collected data in ROOT file format.

## Technologies

- C++ (multithreading, Producer-Consumer pattern)

- Qt6 (GUI)

- CAEN Digitizer Library

- Arduino Uno (serial communication)

- Linux environment

## Key Features

- Real-time acquisition of PMT and sensor data

- Thread-safe data handling

- Modular design for extensibility

## Usage

- Tested on Ubuntu 22.04 LTS

- Install the necessary libraries from the CAEN website:

    - `CAENDigitizer`: tested version `2.18.0`

    - `CAENComm`: tested version `1.7.0`

    - `CAENVMELib`: tested version `4.0.2`

    - `CAENUSBdrvB`: tested version `1.6.1`

- These libraries are linked to each other and must be installed in this order.

- Ensure the CAEN digitizer, Arduino, and all sensors are properly connected.

- Clone the Git repository and compile the program.

- Sensor and acquisition settings can be adjusted via the GUI.

- Data acquisition can be started and stopped from the GUI.

<div style="display: flex; gap: 20px;">
  <img src="screenshots/setting.png" alt="Programm Setting Page" width="300"/>
  <img src="screenshots/running.png" alt="Programm Running Page4" width="300"/>
</div>

## Class Diagram

The following diagram illustrates the Producer-Consumer relationship:

```mermaid
classDiagram 
    
    class AD["Arduino"] {
        -QSerialPort *serialPort
        -QByteArry buffer;
        -TSQueue<ArduinoData> q
        +Arduino(cc, *err, tth)
        +open() bool
        +startCollecting() bool
        +stopCollecting() bool
        +getArduinoData() std::optional<ArduinoData>
        -collectingLoop()
    }

    class DW["DigitizerWrapper"] {
        -TSQueue<DigitizerData> q
        +applyConfig() bool
        +open() bool
        +close() bool
        +startCollecting() bool
        +stopCollecting() bool
        +getDigitizerEvent() std::optional<DigitizerData>
        -collectingLoop()
    }

    class DataCollector {
        +DataCollector()
        +DataCollector(cc, dc, err, tth)
        +open() bool
        +close() bool
        +startAcquisition() bool
        +stopAcquisition(): bool
        -startReading() bool
        -stopReading() bool
        -readingLoop()
    }

    class DData["DigitizerData"] {
        dataset recorded by CAEN Digitizer
    }

    class ADData["ArduinoData"] {
        dataset recorded by Arduino
    }

    AD "1" --> "*" ADData : produces
    DW "1" --> "*" DData : produces
    DataCollector "1" --> "*" ADData: consumes
    DataCollector "1" --> "*" DData: consumes
    DataCollector "1" --> "1" AD: steers
    DataCollector "1" --> "1" DW: steers

```

The following class diagram shows the ownership and relations:

- `uses` pointer relation
- `has` ownership
- `handles` load/save configuration


```mermaid
classDiagram

    class AD["Arduino"] {
        steers the Arduino
    }

    class CC["CollectorConfig"] {
        acquisition settings
    }

    class DataCollector {
        manages data acquisition
    }

    class DC["DigitizerConfig"] {
        device settings for acquisition
    }

    class CH["ConfigHandler"]{
        saves and loads configuration
    }

    class DW["DigitizerWrapper"]{
        steers the CAEN Digitizer
    }

    class ERR["ErrorHandler"] {
        handles messages and terminal output
    }

    class RC["RateCalculator"] {
        calculates the rate of digitizer events
    }

    class RTW["RootTreeWriter"] {
        saves data to ROOT files
    }

    class TTH["TimeTagHandler"] {
        retrieves timestamps and decodes event time
    }

    AD "1" --> "1" TTH: uses
    AD "1" --> "1" ERR: uses
    AD "1" --> "1" CC: uses

    CH "1" --> "1" ERR: uses
    CH "1" --> "1" CC: handles
    CH "1" --> "1" DC: handles

    DataCollector "1" --> "1" AD : has
    DataCollector "1" --> "1" DW : has
    DataCollector "1" --> "1" RTW : has
    DataCollector "1" --> "1" RC : has
    DataCollector "1" --> "1" CC : uses
    DataCollector "1" --> "1" ERR : uses

    DW "1" --> "1" DC : uses
    DW "1" --> "1" CC : uses
    DW "1" --> "1" TTH : uses
    DW "1" --> "1" ERR : uses

    RTW "1" --> "1" CC : uses
    RTW "1" --> "1" ERR : uses

```