
#include <QSerialPort>
#include <QTextStream>
#include <QLocale>
#include <cstdint>

#include <Arduino.h>
#include <TimeTagHandler.h>
#include <ErrorHandler.h>
#include <CollectorConfig.h>


// constructor

Arduino::Arduino(
    std::shared_ptr<CollectorConfig> cc,
    ErrorHandler *err,
    std::shared_ptr<TimeTagHandler> tth
) : CC(cc),
    ERR(err),
    TTH(tth)
{
    serialPort = new QSerialPort(this);
}

Arduino::~Arduino() {
    delete serialPort;
}

// connection

bool Arduino::open() {

    // report
    ERR->logInfo("Arduino::open");

    // check whether Arduino UNO is connected
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        // Arduino UNO has VID 0x2341, PID 0x0043
        if (info.vendorIdentifier() == 0x2341 && info.productIdentifier() == 0x0043) {
            
            // configure listening
            serialPort->setPortName(info.portName()); // Or dynamically discover available ports using QSerialPortInfo
            serialPort->setBaudRate(QSerialPort::Baud9600);
            serialPort->setDataBits(QSerialPort::Data8);
            serialPort->setParity(QSerialPort::NoParity);
            serialPort->setStopBits(QSerialPort::OneStop);
            serialPort->setFlowControl(QSerialPort::NoFlowControl);
            
            // try to open connection
            if (!serialPort->open(QIODevice::ReadWrite)) {
                qWarning() << "Fehler beim Öffnen:" << serialPort->errorString();
                return false;
            }

            // successful
            return true;
        }
    }

    return false;
}


// steer data acquisition

bool Arduino::startCollecting() {

    // report
    ERR->logInfo("Arduino::startCollecting");

    // set status
    isCollecting = true;

    // reset before start
    lineNumber = 0;
    eventID = 0;

    buffer.clear();
    serialPort->clear();

    // connect readyRead verbinden
    connect(serialPort, &QSerialPort::readyRead, this, &Arduino::onReadyRead);

    return true;
}

bool Arduino::stopCollecting() {
    
    // report
    ERR->logInfo("Arduino::stopCollecting");

    // check
    if (isCollecting) {

        // end collection by disconnecting slot
        disconnect(serialPort, &QSerialPort::readyRead, this, &Arduino::onReadyRead);
    }

    return true;
}

void Arduino::onReadyRead(){

    // add data to buffer
    buffer.append(serialPort->readAll());

    int index;
    while ((index = buffer.indexOf('\n')) != -1) {

        // report
        ERR->logInfo("Arduino::onReadyRead: newLine: " + std::to_string(lineNumber));

        // get event
        QByteArray line = buffer.left(index).trimmed();
        buffer.remove(0, index + 1);

        // split on komma
        QList<QByteArray> values = line.split(',');

        // skip line if it is not correct
        if (values.size()!=10) {

            // report
            ERR->logInfo("Arduino::collecting Loop: line incomplete");

            // increase lineNumber
            lineNumber++;

            continue;
        }

        // report
        if (CC->detailedLog) {
            ERR->logInfo("Arduino::collecting Loop: eventID: " + std::to_string(eventID));
        }

        // create data
        ArduinoData data = ArduinoData(
            eventID,
            static_cast<Long64_t>(TTH->getTimeStamp()),
            values[0].toDouble(),
            values[1].toDouble()/100.0, // for unit int mbar
            values[2].toDouble(),
            values[3].toDouble(),
            values[4].toDouble(),
            values[5].toDouble(),
            values[6].toDouble(),
            values[7].toDouble(),
            values[8].toDouble(),
            values[9].toDouble()
        );

        // push data to queue
        q.push(std::move(data));

        // increase EventID and lineNumber
        eventID++;
        lineNumber++;
    }



}
