
#include <iostream>
#include <chrono>
#include <mutex>

#include <QObject>
#include <QPlainTextEdit>
#include <QDBusInterface>
#include <QDBusReply>
#include <QColor>
#include <QString>

#include <ErrorHandler.h>

// constructor

ErrorHandler::ErrorHandler(QWidget *parent) 
  : QPlainTextEdit(parent),
    logFile("errors.log") 
{   
    setReadOnly(true);
    setMaximumBlockCount(1000);

    // connect logMessage
    connect(
        this, &ErrorHandler::logMessage,
        this, &ErrorHandler::generateColeredText,
        Qt::QueuedConnection
    );

    // connect systemNotification
    connect(
        this, &ErrorHandler::systemNotification,
        this, &ErrorHandler::sendNotification,
        Qt::DirectConnection
    );
}


// Error handling functions

void ErrorHandler::TraducirError(CAEN_DGTZ_ErrorCode ret) {
    
    // lock block for threadsafe
    {
        std::lock_guard<std::mutex> lock(mtx);

        // output terminal
        std::cerr << "ERROR: " << CAEN_DGTZ_ErrorCode(ret) << std::endl;

        // output log file
        logFile << timeStamp() << "ERROR: " << CAEN_DGTZ_ErrorCode(ret) << std::endl;
    }

    // output internal terminal
    emit logMessage(
        QString::fromStdString("ERROR: "), 
        QString::number(CAEN_DGTZ_ErrorCode(ret)), 
        Qt::red
    );

    // system notification
    emit systemNotification(
        QString::fromStdString("ERROR"), 
        QString::number(CAEN_DGTZ_ErrorCode(ret)),
        QString::fromStdString("dialog-error")
    );
}

bool ErrorHandler::CheckError(CAEN_DGTZ_ErrorCode ret, const char* mensaje) {

    // show error
    if (ret != CAEN_DGTZ_Success) {

        // lock block for threadsafe
        {
            std::lock_guard<std::mutex> lock(mtx);

            // output terminal
            std::cerr << "ERROR: " << mensaje << ": " << CAEN_DGTZ_ErrorCode(ret) << std::endl;
            
            // output log file
            logFile << timeStamp() << "ERROR: " << mensaje << ": " << CAEN_DGTZ_ErrorCode(ret) << std::endl;
        }

        // output internal terminal
        std::string mensaje_str = mensaje;
        emit logMessage(
            QString::fromStdString("ERROR: "),
            QString::fromStdString(mensaje_str + ": " + std::to_string(CAEN_DGTZ_ErrorCode(ret))), 
            Qt::red
        );

        // system notification
        emit systemNotification(
            QString::fromStdString("ERROR"), 
            QString::fromStdString(mensaje_str + ": " + std::to_string(CAEN_DGTZ_ErrorCode(ret))),
            QString::fromStdString("dialog-error")
        );

        return true;
    }

    return false;
}

bool ErrorHandler::CheckError(bool const ret, std::string const errormessage) {

    // show error
    if (!ret){
        
        // lock block for threadsafe
        {
            std::lock_guard<std::mutex> lock(mtx);

            // output terminal
            std::cerr << "ERROR: " << errormessage << std::endl;

            // output log file
            logFile << timeStamp() << "ERROR: " << errormessage << std::endl;
        }

        // output internal terminal
        emit logMessage(
            QString::fromStdString("ERROR: "), 
            QString::fromStdString(errormessage), 
            Qt::red
        );

        // system notification
        emit systemNotification(
            QString::fromStdString("ERROR"), 
            QString::fromStdString(errormessage),
            QString::fromStdString("dialog-error")   
        );

        return true;
    }

    return false;
} 

bool ErrorHandler::ThrowError(std::string const errormessage) {
    
    // lock block for threadsafe
    {
        std::lock_guard<std::mutex> lock(mtx);

        // output terminal
        std::cerr << "ERROR: " << errormessage << std::endl;

        // output log file
        logFile << timeStamp() << "ERROR: " << errormessage << std::endl;
    }

    // output terminal
    emit logMessage(
        QString::fromStdString("ERROR: "), 
        QString::fromStdString(errormessage), 
        Qt::red
    );

    // system notification
    emit systemNotification(
        QString::fromStdString("ERROR"), 
        QString::fromStdString(errormessage),
        QString::fromStdString("dialog-error")
    );

    return true;
}

void ErrorHandler::logInfo(std::string const infomessage) {
    
    // lock block for threadsafe
    {
        std::lock_guard<std::mutex> lock(mtx);

        // output terminal
        std::cerr << "INFO: " << infomessage << std::endl;

        // output log file
        logFile << timeStamp() << "INFO: " << infomessage << std::endl;
    }  

    // output internal terminal
    emit logMessage(
        QString::fromStdString("INFO: "), 
        QString::fromStdString(infomessage), 
        Qt::green
    );


}

void ErrorHandler::warningMessage(std::string const message) {

    // system notification
    emit systemNotification(
        QString::fromStdString("WARNING"),
        QString::fromStdString(message),
        QString::fromStdString("dialog-error")
    );
}


// send notifications

void ErrorHandler::sendNotification(
    const QString &title, 
    const QString &message,
    const QString &icon
) {
    QDBusInterface notifyApp(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        QDBusConnection::sessionBus()
    );

    QVariantList args;
    args << "Tanca DataAcquisition"                 // app name
         << uint(0)                                 // replaces_id
         << icon                                    // app icon
         << "Tanca DataAcquisition: " + title       // summary
         << message                                 // body
         << QStringList()                           // actions
         << QVariantMap()                           // hints
         << int(-1);                                // expire timeout

    notifyApp.callWithArgumentList(QDBus::AutoDetect, "Notify", args);
}

// generate ColoredText

void ErrorHandler::generateColeredText(
    const QString &markup,
    const QString &message,
    QColor color
) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);

    // colored part (red)
    QTextCharFormat fmtColor;
    fmtColor.setForeground(color);
    cursor.insertText(markup, fmtColor);

    // normal part (black)
    QTextCharFormat fmtNormal;
    fmtNormal.setForeground(Qt::black);
    cursor.insertText(message + "\n", fmtNormal);

    setTextCursor(cursor);
}


// time stamp

std::string ErrorHandler::timeStamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S]");
    return oss.str();
}

