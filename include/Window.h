#pragma once

#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>

#include <SettingsWidget.h>
#include <DataCollector.h>

class CollectorConfig;
class ConfigHandler;
class DigitizerConfig;
class ErrorHandler;
class TimeTagHandler;

class Window : public QWidget
{
    Q_OBJECT

    public:
        explicit Window(QWidget *parent = nullptr);
        ~Window() override;

    
    protected:
        void closeEvent(QCloseEvent *event) override;

    private:

        // create Error and TimeTag Handler
        std::shared_ptr<ErrorHandler> ERR;
        std::shared_ptr<TimeTagHandler> TTH;

        // configuration
        std::shared_ptr<ConfigHandler> CH;
        std::shared_ptr<CollectorConfig> CC;
        std::shared_ptr<DigitizerConfig> DC;

        std::shared_ptr<DataCollector> DataC;

        QStackedWidget *stack;
        Settings *settingsWgt;
        QPushButton *startButton;

        void onStart();

        // error handling
        bool boolret;

};
