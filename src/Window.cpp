#include <Window.h>
#include <QVBoxLayout>

#include <ErrorHandler.h>
#include <TimeTagHandler.h>
#include <ConfigHandler.h>
#include <CollectorConfig.h>
#include <DigitizerConfig.h>

Window::Window(QWidget *parent)
    : QWidget(parent)
{   
    
    // create Error and TimeTag Handler
    ErrorHandler *ERR = new ErrorHandler(this);
    TTH = std::make_shared<TimeTagHandler>(ERR);

    // configuration
    CH = std::make_shared<ConfigHandler>(ERR);
    CC = std::make_shared<CollectorConfig>(CH->loadCollectorConfig());
    DC = std::make_shared<DigitizerConfig>(CH->loadDigitizerConfig());

    DataC = std::make_shared<DataCollector>(CC, DC, ERR, TTH);

    stack = new QStackedWidget;

    settingsWgt = new Settings(this);
    startButton = new QPushButton("Start");
    
    stack->addWidget(settingsWgt);
    stack->addWidget(ERR);

    // Layout hinzufügen
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stack);
    layout->addWidget(startButton);

    connect(startButton, &QPushButton::clicked, this, [=]() {
        onStart();
    });

    // load gui values
    settingsWgt->getSettings(CC, CH, DC);
}

Window::~Window() {
    DataC->joinRTWBackup();
    DataC->close();
}

void Window::closeEvent(QCloseEvent *event) {
    DataC->joinRTWBackup();
    DataC->close();
    QWidget::closeEvent(event);
}

void Window::onStart() {

    // switch start/stop button
    if (startButton->text() == "Start") {
        
        // load and save config
        settingsWgt->applySettings(CC, CH, DC);
        
        // check whether at least one channel is enabled
        if (!DC->active[0] && !DC->active[1] && !DC->active[2]){
            ERR->sendNotification(
                "WARNING",
                "At least one Channel has to be activated!",
                "dialog-warning"
            );
            return;
        }

        startButton->setText("Stop");
        stack->setCurrentIndex(1);

        // load and save config
        settingsWgt->applySettings(CC, CH, DC);

        boolret = DataC->open();
        if (!boolret) {
            startButton->setText("Start");
            stack->setCurrentIndex(0);
            return;
        }

        // apply config to digitizer
        boolret = DataC->applyDigitizerConfig();
        if (!boolret) {
            startButton->setText("Start");
            stack->setCurrentIndex(0);
            return;
        }

        // start acquisition
        boolret = DataC->startAcquisition();
        if (!boolret) {
            startButton->setText("Start");
            stack->setCurrentIndex(0);
            return;
        }
    }
    else {
        startButton->setVisible(false);

        // stop acquisition
        DataC->stopAcquisition();

        startButton->setText("Start");
        startButton->setVisible(true);
        stack->setCurrentIndex(0);
    }
}
