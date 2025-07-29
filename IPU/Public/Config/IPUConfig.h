#ifndef IPUCONFIG_H
#define IPUCONFIG_H

#include <QObject>
#include <QWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

#include "Public/IFService.h"
#include "IPUDefine.h"
#include "ui_IPUConfig.h"

class CIPUConfig : public QWidget
{
    Q_OBJECT
public:
    explicit CIPUConfig(QWidget *parent = nullptr);
    ~CIPUConfig();

protected:
    void CreateLayout();
    
private slots:
    void OnSave();
    void OnCancel();
    void updateUIFromSettings();
    void updateSettingsFromUI();

private:
    Ui::IPUConfig* ui;

    // UI controls
    QSpinBox* spinBoxPort;
    QComboBox* comboBoxBaud;
    QPushButton* pushButtonSave;
    QPushButton* pushButtonCancel;
    QLineEdit* lineEditLeft;
    QLineEdit* lineEditRight;

    // Config values
    QString strConfigFileName;
    QString strConfigPortLic;
    QString strConfigPortBaud;

    // Front Camera Config
    QString strConfigFrontLeftCamera;
    QString strConfigFrontRightCamera;
    QString strConfigFrontTopCamera;
    QString strConfigFrontBottomCamera;

    // Rear Camera Config
    QString strConfigRearLeftCamera;
    QString strConfigRearRightCamera;
    QString strConfigRearTopCamera;
    QString strConfigRearBottomCamera;

    // VVP Device IP
    QString strConfigVVPDeviceIP;
    QString strConfigVVPDevicePort;

    // VDU Device IP
    QString strConfigVduDeviceIP;
    QString strConfigVduDevicePort;

    // Unity Server IP
    QString strConfigUnityServerIP;
    QString strConfigUnityServerPort;

    // Remote Server IP
    QString strConfigRemoteServerIP;
    QString strConfigRemoteServerPort;

    // Default values
    QString strDefaultConfigFileName;
    QString strDefaultConfigPortLic;
    QString strDefaultConfigPortBaud;
    // Lane Classification
    QString strConfigLaneClassification;
    QString strDefaultConfigLaneClassification;

    // Default Front Camera
    QString strDefaultConfigFrontLeftCamera;
    QString strDefaultConfigFrontRightCamera;
    QString strDefaultConfigFrontTopCamera;
    QString strDefaultConfigFrontBottomCamera;

    // Default Rear Camera
    QString strDefaultConfigRearLeftCamera;
    QString strDefaultConfigRearRightCamera;
    QString strDefaultConfigRearTopCamera;
    QString strDefaultConfigRearBottomCamera;

    // Default VVP Device
    QString strDefaultConfigVVPDeviceIP;
    QString strDefaultConfigVVPDevicePort;

    // Default VDU Device
    QString strDefaultConfigVduDeviceIP;
    QString strDefaultConfigVduDevicePort;

    // Default Unity Server
    QString strDefaultConfigUnityServerIP;
    QString strDefaultConfigUnityServerPort;
    
    // Default Remote Server
    QString strDefaultConfigRemoteServerIP;
    QString strDefaultConfigRemoteServerPort;

public:
    void loadDefaultConfig();
    void loadConfig();
    void saveConfig();

    // Getter functions
    QString getPortLic() const { return strConfigPortLic; }
    QString getPortBaud() const { return strConfigPortBaud; }

    // Front Camera Getters
    QString getFrontLeftCamera() const { return strConfigFrontLeftCamera; }
    QString getFrontRightCamera() const { return strConfigFrontRightCamera; }
    QString getFrontTopCamera() const { return strConfigFrontTopCamera; }
    QString getFrontBottomCamera() const { return strConfigFrontBottomCamera; }

    // Rear Camera Getters
    QString getRearLeftCamera() const { return strConfigRearLeftCamera; }
    QString getRearRightCamera() const { return strConfigRearRightCamera; }
    QString getRearTopCamera() const { return strConfigRearTopCamera; }
    QString getRearBottomCamera() const { return strConfigRearBottomCamera; }

    // VVP Device Getters
    QString getVVPDeviceIP() const { return strConfigVVPDeviceIP; }
    QString getVVPDevicePort() const { return strConfigVVPDevicePort; }

    // VDU Device Getters
    QString getVduDeviceIP() const { return strConfigVduDeviceIP; }
    QString getVduDevicePort() const { return strConfigVduDevicePort; }

    // Lane Type Getter
    QString getLaneClassification() const { return strConfigLaneClassification; }
};

#endif // IPUCONFIG_H
