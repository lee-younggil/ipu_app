#include "IPUConfig.h"
#include <QSettings>
#include <QFile>
#include <QDebug>
#include "ui_IPUConfig.h"

CIPUConfig::CIPUConfig(QWidget *parent) : QWidget(parent)
    , ui(new Ui::IPUConfig)
{
    ui->setupUi(this);

    // Default values
    strDefaultConfigFileName = "IPUConfig.ini";
    strDefaultConfigPortLic = "0";
    strDefaultConfigPortBaud = "115200";

    // Default Front Camera values
    strDefaultConfigFrontLeftCamera = "";
    strDefaultConfigFrontRightCamera = "";
    strDefaultConfigFrontTopCamera = "";
    strDefaultConfigFrontBottomCamera = "";

    // Default Rear Camera values
    strDefaultConfigRearLeftCamera = "";
    strDefaultConfigRearRightCamera = "";
    strDefaultConfigRearTopCamera = "";
    strDefaultConfigRearBottomCamera = "";

    // Default VVP Device values
    strDefaultConfigVVPDeviceIP = "";
    strDefaultConfigVVPDevicePort = "20355";

    // Default VDU Device values
    strDefaultConfigVduDeviceIP = "";
    strDefaultConfigVduDevicePort = "20365";

    // Connect signals
    connect(ui->pushButtonSave, &QPushButton::clicked, this, &CIPUConfig::OnSave);
    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &CIPUConfig::OnCancel);

    LoadDefaultConfig();
    LoadConfig();  // 프로그램 시작시 설정 로드
}

CIPUConfig::~CIPUConfig()
{
    delete ui;
}

void CIPUConfig::LoadDefaultConfig()
{
    // Load default configuration
    strConfigFileName = strDefaultConfigFileName;
    strConfigPortLic = strDefaultConfigPortLic;
    strConfigPortBaud = strDefaultConfigPortBaud;

    // Front Camera defaults
    strConfigFrontLeftCamera = strDefaultConfigFrontLeftCamera;
    strConfigFrontRightCamera = strDefaultConfigFrontRightCamera;
    strConfigFrontTopCamera = strDefaultConfigFrontTopCamera;
    strConfigFrontBottomCamera = strDefaultConfigFrontBottomCamera;

    // Rear Camera defaults
    strConfigRearLeftCamera = strDefaultConfigRearLeftCamera;
    strConfigRearRightCamera = strDefaultConfigRearRightCamera;
    strConfigRearTopCamera = strDefaultConfigRearTopCamera;
    strConfigRearBottomCamera = strDefaultConfigRearBottomCamera;

    // VVP Device defaults
    strConfigVVPDeviceIP = strDefaultConfigVVPDeviceIP;
    strConfigVVPDevicePort = strDefaultConfigVVPDevicePort;
    
    // Update UI
    UpdateUIFromSettings();
}

void CIPUConfig::LoadConfig()
{
    qDebug() << "Loading config from file: " << strConfigFileName;
    return;

    // Check if config file exists
    if (!QFile::exists(strConfigFileName))
    {
        // If file doesn't exist, load default values
        LoadDefaultConfig();
        // Create new config file with default values
        SaveConfig();
        return;
    }

    // Load configuration from INI file
    QSettings settings(strConfigFileName, QSettings::IniFormat);
    
    // Read LIC values
    strConfigPortLic = settings.value("Lic/Port", strDefaultConfigPortLic).toString();
    strConfigPortBaud = settings.value("Lic/Baud", strDefaultConfigPortBaud).toString();

    // Read Front Camera values
    strConfigFrontLeftCamera = settings.value("FrontCamera/Left", strDefaultConfigFrontLeftCamera).toString();
    strConfigFrontRightCamera = settings.value("FrontCamera/Right", strDefaultConfigFrontRightCamera).toString();
    strConfigFrontTopCamera = settings.value("FrontCamera/Top", strDefaultConfigFrontTopCamera).toString();
    strConfigFrontBottomCamera = settings.value("FrontCamera/Bottom", strDefaultConfigFrontBottomCamera).toString();

    // Read Rear Camera values
    strConfigRearLeftCamera = settings.value("RearCamera/Left", strDefaultConfigRearLeftCamera).toString();
    strConfigRearRightCamera = settings.value("RearCamera/Right", strDefaultConfigRearRightCamera).toString();
    strConfigRearTopCamera = settings.value("RearCamera/Top", strDefaultConfigRearTopCamera).toString();
    strConfigRearBottomCamera = settings.value("RearCamera/Bottom", strDefaultConfigRearBottomCamera).toString();

    // Read VVP Device values
    strConfigVVPDeviceIP = settings.value("VVPDevice/IP", strDefaultConfigVVPDeviceIP).toString();
    strConfigVVPDevicePort = settings.value("VVPDevice/Port", strDefaultConfigVVPDevicePort).toString();

    // Read VDU Device values
    strConfigVduDeviceIP = settings.value("VduDevice/IP", strDefaultConfigVduDeviceIP).toString();
    strConfigVduDevicePort = settings.value("VduDevice/Port", strDefaultConfigVduDevicePort).toString();

    // Read Unity Server values
    strConfigUnityServerIP = settings.value("UnityServer/IP", strDefaultConfigUnityServerIP).toString();
    strConfigUnityServerPort = settings.value("UnityServer/Port", strDefaultConfigUnityServerPort).toString();

    // Read Remote Server values
    strConfigRemoteServerIP = settings.value("RemoteServer/IP", strDefaultConfigRemoteServerIP).toString();
    strConfigRemoteServerPort = settings.value("RemoteServer/Port", strDefaultConfigRemoteServerPort).toString();

    // Update UI with loaded values
    UpdateUIFromSettings();
}

void CIPUConfig::SaveConfig()
{
    // Update settings from UI before saving
    UpdateSettingsFromUI();

    // Save configuration to INI file
    QSettings settings(strConfigFileName, QSettings::IniFormat);
    
    // Write LIC values
    settings.setValue("Lic/Port", strConfigPortLic);
    settings.setValue("Lic/Baud", strConfigPortBaud);

    // Write Front Camera values
    settings.setValue("FrontCamera/Left", strConfigFrontLeftCamera);
    settings.setValue("FrontCamera/Right", strConfigFrontRightCamera);
    settings.setValue("FrontCamera/Top", strConfigFrontTopCamera);
    settings.setValue("FrontCamera/Bottom", strConfigFrontBottomCamera);

    // Write Rear Camera values
    settings.setValue("RearCamera/Left", strConfigRearLeftCamera);
    settings.setValue("RearCamera/Right", strConfigRearRightCamera);
    settings.setValue("RearCamera/Top", strConfigRearTopCamera);
    settings.setValue("RearCamera/Bottom", strConfigRearBottomCamera);

    // Write VVP Device values
    settings.setValue("VVPDevice/IP", strConfigVVPDeviceIP);
    settings.setValue("VVPDevice/Port", strConfigVVPDevicePort);

    // Write VDU Device values
    settings.setValue("VduDevice/IP", strConfigVduDeviceIP);
    settings.setValue("VduDevice/Port", strConfigVduDevicePort);

    // Write Unity Server values
    settings.setValue("UnityServer/IP", strConfigUnityServerIP);
    settings.setValue("UnityServer/Port", strConfigUnityServerPort);

    // Write Remote Server values
    settings.setValue("RemoteServer/IP", strConfigRemoteServerIP);
    settings.setValue("RemoteServer/Port", strConfigRemoteServerPort);
}

void CIPUConfig::OnSave()
{
    SaveConfig();
}

void CIPUConfig::OnCancel()
{
    // Reload the current settings and update UI
    LoadConfig();
}

void CIPUConfig::UpdateUIFromSettings()
{
    if (ui)
    {
        // Update LIC settings
        ui->spinBoxPort->setValue(strConfigPortLic.toInt());
        int index = ui->comboBoxBaud->findText(strConfigPortBaud);
        if (index >= 0)
        {
            ui->comboBoxBaud->setCurrentIndex(index);
        }

        // Update Front Camera settings
        ui->lineEditFrontLeft->setText(strConfigFrontLeftCamera);
        ui->lineEditFrontRight->setText(strConfigFrontRightCamera);
        ui->lineEditFrontTop->setText(strConfigFrontTopCamera);
        ui->lineEditFrontBottom->setText(strConfigFrontBottomCamera);

        // Update Rear Camera settings
        ui->lineEditRearLeft->setText(strConfigRearLeftCamera);
        ui->lineEditRearRight->setText(strConfigRearRightCamera);
        ui->lineEditRearTop->setText(strConfigRearTopCamera);
        ui->lineEditRearBottom->setText(strConfigRearBottomCamera);

        // Update VVP Device settings
        ui->lineEditVVPDeviceIP->setText(strConfigVVPDeviceIP);
        ui->spinBoxVVPDevicePort->setValue(strConfigVVPDevicePort.toInt());

        // Update VDU Device settings
        ui->lineEditVduDeviceIP->setText(strConfigVduDeviceIP);
        ui->spinBoxVduDevicePort->setValue(strConfigVduDevicePort.toInt());
        
        // Update UnityServer settings
        ui->lineEditUnityServerIP->setText(strConfigUnityServerIP);
        ui->spinBoxUnityServerPort->setValue(strConfigUnityServerPort.toInt());

        // Update RemoteServer settings
        ui->lineEditRemoteServerIP->setText(strConfigRemoteServerIP);
        ui->spinBoxRemoteServerPort->setValue(strConfigRemoteServerPort.toInt());
    }
}

void CIPUConfig::UpdateSettingsFromUI()
{
    if (ui)
    {
        // Update LIC settings
        strConfigPortLic = QString::number(ui->spinBoxPort->value());
        strConfigPortBaud = ui->comboBoxBaud->currentText();

        // Update Front Camera settings
        strConfigFrontLeftCamera = ui->lineEditFrontLeft->text();
        strConfigFrontRightCamera = ui->lineEditFrontRight->text();
        strConfigFrontTopCamera = ui->lineEditFrontTop->text();
        strConfigFrontBottomCamera = ui->lineEditFrontBottom->text();

        // Update Rear Camera settings
        strConfigRearLeftCamera = ui->lineEditRearLeft->text();
        strConfigRearRightCamera = ui->lineEditRearRight->text();
        strConfigRearTopCamera = ui->lineEditRearTop->text();
        strConfigRearBottomCamera = ui->lineEditRearBottom->text();

        // Update VVP Device settings
        strConfigVVPDeviceIP = ui->lineEditVVPDeviceIP->text();
        strConfigVVPDevicePort = QString::number(ui->spinBoxVVPDevicePort->value());

        // Update VDU Device settings
        strConfigVduDeviceIP = ui->lineEditVduDeviceIP->text();
        strConfigVduDevicePort = QString::number(ui->spinBoxVduDevicePort->value());

        // Update UnityServer settings
        strConfigUnityServerIP = ui->lineEditUnityServerIP->text();
        strConfigUnityServerPort = QString::number(ui->spinBoxUnityServerPort->value());

        // Update RemoteServer settings
        strConfigRemoteServerIP = ui->lineEditRemoteServerIP->text();
        strConfigRemoteServerPort = QString::number(ui->spinBoxRemoteServerPort->value());

    }
}
