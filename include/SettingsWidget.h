#pragma once

#include <QFrame>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDebug>
#include <QString>

class CollectorConfig;
class ConfigHandler;
class DigitizerConfig;

class Settings : public QFrame {
    Q_OBJECT

    public:
        explicit Settings(QWidget *parent = nullptr);
        ~Settings() override = default;

        void applySettings(
            std::shared_ptr<CollectorConfig> cc,
            std::shared_ptr<ConfigHandler> ch,
            std::shared_ptr<DigitizerConfig> cd
        );

        void getSettings(
            std::shared_ptr<CollectorConfig> cc,
            std::shared_ptr<ConfigHandler> ch,
            std::shared_ptr<DigitizerConfig> dc
        );

    private:

        // settings layout
        QGroupBox *generalSettingsGB;
        QGroupBox *digitizerSettingsGB;

        QLabel *workingDirL;
        QLineEdit *workingDirLE;
        QPushButton *workingDirPB;

        QLabel *backupDirL;
        QLineEdit *backupDirLE;
        QPushButton *backupDirPB;

        QCheckBox *enableBackupCB;
        QCheckBox *enableDetailledLogCB;

        QCheckBox *enableAcquisitionLimitCB;
        QSpinBox *acquisitionLimitSB;

        QLabel *recordLengthL;
        QComboBox *recordLengthCB;

        QLabel *postTriggerPctL;
        QSpinBox *postTriggerPctSB;

        QLabel *majorityLevelL;
        QSpinBox *majorityLevelSB;

        QCheckBox *channel0CB;
        QCheckBox *channel1CB;
        QCheckBox *channel2CB;

        QLabel *dcOffsetL;
        QSpinBox *dcOffsetSB0;
        QSpinBox *dcOffsetSB1;
        QSpinBox *dcOffsetSB2;

        QLabel *triggerThresholdL;
        QSpinBox *triggerThresholdSB0;
        QSpinBox *triggerThresholdSB1;
        QSpinBox *triggerThresholdSB2;

        // file browser
        void fileBrowser(QLineEdit *line);
};