#pragma once

#include <thread>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

#include <ArduinoData.h>
#include <TSQueue.h>

class TimeTagHandler;
class CollectorConfig;
class ErrorHandler;

class Arduino : public QObject{

    // macro to implement connect
    Q_OBJECT

    public:

        // constructor and destructor
        Arduino(
            std::shared_ptr<CollectorConfig> cc,
            ErrorHandler *err,
            std::shared_ptr<TimeTagHandler> tth
        );
        ~Arduino() override;

        // connection (and configuration)
        bool open();

        // steer data acquisition
        bool startCollecting();
        bool stopCollecting();
        
        // get data
        std::optional<ArduinoData> getArduinoData() { return q.pop(); };

    private slots:
        // collect Data
        void onReadyRead();

    private:
        // status
        bool isCollecting = false;
        uint64_t lineNumber = 0;
        uint64_t eventID = 0;

        // serial port to connect to Arduino
        QSerialPort *serialPort;

        // buffer to store stream data
        QByteArray buffer;

        // queue for the data from Arduino
        TSQueue<ArduinoData> q;

        // Time Tag Handler
        std::shared_ptr<TimeTagHandler> TTH;

        // Error handler
        ErrorHandler *ERR;

        // config
        std::shared_ptr<CollectorConfig> CC;
};