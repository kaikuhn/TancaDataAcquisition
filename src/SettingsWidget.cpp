
#include <SettingsWidget.h>

#include <QGridLayout>
#include <QVBoxLayout>

#include <CollectorConfig.h>
#include <DigitizerConfig.h>
#include <ConfigHandler.h>


Settings::Settings (QWidget *parent)
  : QFrame(parent)
{   
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    // General Settings Group Box
    generalSettingsGB = new QGroupBox("General Settings", this);

    QGridLayout *generalSettingsLayout = new QGridLayout;
    generalSettingsGB->setLayout(generalSettingsLayout);

    workingDirL = new QLabel("Working Dir:");
    workingDirLE = new QLineEdit;
    workingDirPB = new QPushButton("Browse");

    connect(workingDirPB, &QPushButton::clicked, this, [=]() {
        fileBrowser(workingDirLE);
    });

    backupDirL = new QLabel("Backup Dir:");
    backupDirLE =  new QLineEdit;
    backupDirPB = new QPushButton("Browse");

    connect(backupDirPB, &QPushButton::clicked, this, [=]() {
        fileBrowser(backupDirLE);
    });

    enableBackupCB = new QCheckBox("Enable Backup");
    enableDetailledLogCB = new QCheckBox("Enable Detailled Log");

    enableAcquisitionLimitCB = new QCheckBox("Enable Acuisition Limit");
    acquisitionLimitSB = new QSpinBox;
    acquisitionLimitSB->setRange(0, 1000000);

    connect(enableAcquisitionLimitCB, &QCheckBox::clicked, this, [=]() {
        if (enableAcquisitionLimitCB->checkState()) {
            acquisitionLimitSB->setVisible(true);
        }
        else {
            acquisitionLimitSB->setVisible(false);
        }
    }),

    generalSettingsLayout->addWidget(workingDirL, 0, 0, 1, 1);
    generalSettingsLayout->addWidget(workingDirLE, 0, 1, 1, 2);
    generalSettingsLayout->addWidget(workingDirPB, 0, 3, 1, 1);

    generalSettingsLayout->addWidget(backupDirL, 1, 0, 1, 1);
    generalSettingsLayout->addWidget(backupDirLE, 1, 1, 1, 2);
    generalSettingsLayout->addWidget(backupDirPB, 1, 3, 1, 1);

    generalSettingsLayout->addWidget(enableBackupCB, 2, 0, 1, 2);
    generalSettingsLayout->addWidget(enableDetailledLogCB, 2, 2, 1, 2);

    generalSettingsLayout->addWidget(enableAcquisitionLimitCB, 3, 0, 1, 2);
    generalSettingsLayout->addWidget(acquisitionLimitSB, 3, 2, 1, 2);

    // Digitizer Settings Group Box
    digitizerSettingsGB = new QGroupBox("Digitizer Settings", this);

    QGridLayout *digitizerSettingsLayout = new QGridLayout;
    digitizerSettingsGB->setLayout(digitizerSettingsLayout);

    recordLengthL = new QLabel("Record Length [samples]:");
    recordLengthCB = new QComboBox;
    recordLengthCB->addItems({"64", "128", "256", "512", "1024"});

    postTriggerPctL = new QLabel("Post Trigger [%]:");
    postTriggerPctSB = new QSpinBox;
    postTriggerPctSB->setRange(0, 100);

    majorityLevelL = new QLabel("Majority Level:");
    majorityLevelSB = new QSpinBox;
    majorityLevelSB->setRange(0, 2);

    channel0CB = new QCheckBox("Channel 0");
    channel1CB = new QCheckBox("Channel 1");
    channel2CB = new QCheckBox("Channel 2");

    dcOffsetL = new QLabel("dcOffset:");
    dcOffsetSB0 = new QSpinBox;
    dcOffsetSB0->setRange(0, 65535);
    dcOffsetSB1 = new QSpinBox;
    dcOffsetSB1->setRange(0, 65535);
    dcOffsetSB2 = new QSpinBox;
    dcOffsetSB2->setRange(0, 65535);

    triggerThresholdL = new QLabel("triggerThreshold:");
    triggerThresholdSB0 = new QSpinBox;
    triggerThresholdSB0->setRange(0, 4095);
    triggerThresholdSB1 = new QSpinBox;
    triggerThresholdSB1->setRange(0, 4095);
    triggerThresholdSB2 = new QSpinBox;
    triggerThresholdSB2->setRange(0, 4095);

    digitizerSettingsLayout->addWidget(recordLengthL, 0, 0, 1, 3);
    digitizerSettingsLayout->addWidget(recordLengthCB, 0, 3, 1, 3);

    digitizerSettingsLayout->addWidget(postTriggerPctL, 1, 0, 1, 3);
    digitizerSettingsLayout->addWidget(postTriggerPctSB, 1, 3, 1, 3);

    digitizerSettingsLayout->addWidget(majorityLevelL, 2, 0, 1, 3);
    digitizerSettingsLayout->addWidget(majorityLevelSB, 2, 3, 1, 3);

    digitizerSettingsLayout->addWidget(channel0CB, 3, 0, 1, 2);
    digitizerSettingsLayout->addWidget(channel1CB, 3, 2, 1, 2);
    digitizerSettingsLayout->addWidget(channel2CB, 3, 4, 1, 2);

    digitizerSettingsLayout->addWidget(dcOffsetL, 4, 0, 1, 1);
    digitizerSettingsLayout->addWidget(dcOffsetSB0, 5, 0, 1, 2);
    digitizerSettingsLayout->addWidget(dcOffsetSB1, 5, 2, 1, 2);
    digitizerSettingsLayout->addWidget(dcOffsetSB2, 5, 4, 1, 2);

    digitizerSettingsLayout->addWidget(triggerThresholdL, 6, 0, 1, 1);
    digitizerSettingsLayout->addWidget(triggerThresholdSB0, 7, 0, 1, 2);
    digitizerSettingsLayout->addWidget(triggerThresholdSB1, 7, 2, 1, 2);
    digitizerSettingsLayout->addWidget(triggerThresholdSB2, 7, 4, 1, 2);

    layout->addWidget(generalSettingsGB);
    layout->addWidget(digitizerSettingsGB);
}


void Settings::applySettings(
    std::shared_ptr<CollectorConfig> cc,
    std::shared_ptr<ConfigHandler> ch,
    std::shared_ptr<DigitizerConfig> dc
) {
    
    // apply collector config settings
    cc->workingDir = workingDirLE->text().toStdString();
    cc->backupDir = backupDirLE->text().toStdString();
    
    cc->enableBackup = enableBackupCB->isChecked();
    cc->detailedLog = enableDetailledLogCB->checkState();

    cc->enableAcquisitionLimit = enableAcquisitionLimitCB->isChecked();
    cc->acquisitionLimit = acquisitionLimitSB->value();

    // apply digitizer config settings
    dc->recordLength = static_cast<uint32_t>(recordLengthCB->currentText().toInt());
    dc->postTriggerPct = static_cast<uint32_t>(postTriggerPctSB->value());
    dc->majorityLevel = majorityLevelSB->value();

    dc->dcOffset = {
        static_cast<uint16_t>(dcOffsetSB0->value()),
        static_cast<uint16_t>(dcOffsetSB1->value()),
        static_cast<uint16_t>(dcOffsetSB2->value())
    };

    dc->triggerThreshold = {
        static_cast<uint16_t>(triggerThresholdSB0->value()),
        static_cast<uint16_t>(triggerThresholdSB1->value()),
        static_cast<uint16_t>(triggerThresholdSB2->value())
    };

    dc->active = {
        channel0CB->isChecked(),
        channel1CB->isChecked(),
        channel2CB->isChecked()
    };

    // save settings to file
    ch->saveCollectorConfig(*cc);
    ch->saveDigitizerConfig(*dc);
}

void Settings::getSettings(
        std::shared_ptr<CollectorConfig> cc,
        std::shared_ptr<ConfigHandler> ch,
        std::shared_ptr<DigitizerConfig> dc
) {
        
    // get collector config settings
    workingDirLE->setText(QString::fromStdString(cc->workingDir));
    backupDirLE->setText(QString::fromStdString(cc->backupDir));
    enableBackupCB->setChecked(cc->enableBackup);
    enableDetailledLogCB->setChecked(cc->detailedLog);

    enableAcquisitionLimitCB->setChecked(cc->enableAcquisitionLimit);
    if (!cc->enableAcquisitionLimit) acquisitionLimitSB->setVisible(false);
    acquisitionLimitSB->setValue(cc->acquisitionLimit);

    // apply digitizer config settings
    recordLengthCB->setCurrentText(QString::fromStdString(std::to_string(dc->recordLength)));
    postTriggerPctSB->setValue(static_cast<int>(dc->postTriggerPct));
    majorityLevelSB->setValue(dc->majorityLevel);

    dcOffsetSB0->setValue(static_cast<int>(dc->dcOffset[0]));
    dcOffsetSB1->setValue(static_cast<int>(dc->dcOffset[1]));
    dcOffsetSB2->setValue(static_cast<int>(dc->dcOffset[2]));

    triggerThresholdSB0->setValue(static_cast<int>(dc->triggerThreshold[0]));
    triggerThresholdSB1->setValue(static_cast<int>(dc->triggerThreshold[1]));
    triggerThresholdSB2->setValue(static_cast<int>(dc->triggerThreshold[2]));

    channel0CB->setChecked(dc->active[0]);
    channel1CB->setChecked(dc->active[1]);
    channel2CB->setChecked(dc->active[2]);
}


// file Browser

void Settings::fileBrowser(QLineEdit *line) {
    QString dir = QFileDialog::getExistingDirectory(this, "Ordner auswählen", QDir::homePath());
    if (!dir.isEmpty()) {
        line->setText(dir);
    }
}