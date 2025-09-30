
#pragma once

#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <CAENDigitizer.h>

#include <QObject>
#include <QPlainTextEdit>
#include <QColor>
#include <QString>
#include <QPlainTextEdit>

class ErrorHandler : public QPlainTextEdit{

    Q_OBJECT

    public:
        // constructor
        explicit ErrorHandler(QWidget *parent = nullptr);
        
        // Error functions
        bool CheckError(CAEN_DGTZ_ErrorCode ret, const char* mensaje);
        bool CheckError(bool const ret, std::string const errormessage);
        void TraducirError(CAEN_DGTZ_ErrorCode code);
        bool ThrowError(std::string const errormessage);
        void logInfo(std::string const infomessage);
        void warningMessage(std::string const message);

        // send notifications
        void sendNotification(
            const QString &title, 
            const QString &message,
            const QString &icon
        ); 

    signals:

        // log message for terminal
        void logMessage(const QString &markup, const QString &message, QColor color);

        // system notification
        void systemNotification(const QString &title, const QString &message, const QString &icon);
        
    private:

        // generate colorText
        void generateColeredText(
            const QString &markup,
            const QString &message,
            QColor color
        );

        // log file
        std::ofstream logFile; 

        // time stamp
        std::string timeStamp();

        // lock for threadsafe
        std::mutex mtx;
};
