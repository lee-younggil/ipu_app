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
    void UpdateUIFromSettings();
    void UpdateSettingsFromUI();

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
    void LoadDefaultConfig();
    void LoadConfig();
    void SaveConfig();

    // Getter functions
    QString GetPortLic() const { return strConfigPortLic; }
    QString GetPortBaud() const { return strConfigPortBaud; }

    // Front Camera Getters
    QString GetFrontLeftCamera() const { return strConfigFrontLeftCamera; }
    QString GetFrontRightCamera() const { return strConfigFrontRightCamera; }
    QString GetFrontTopCamera() const { return strConfigFrontTopCamera; }
    QString GetFrontBottomCamera() const { return strConfigFrontBottomCamera; }

    // Rear Camera Getters
    QString GetRearLeftCamera() const { return strConfigRearLeftCamera; }
    QString GetRearRightCamera() const { return strConfigRearRightCamera; }
    QString GetRearTopCamera() const { return strConfigRearTopCamera; }
    QString GetRearBottomCamera() const { return strConfigRearBottomCamera; }

    // VVP Device Getters
    QString GetVVPDeviceIP() const { return strConfigVVPDeviceIP; }
    QString GetVVPDevicePort() const { return strConfigVVPDevicePort; }

    // VDU Device Getters
    QString GetVduDeviceIP() const { return strConfigVduDeviceIP; }
    QString GetVduDevicePort() const { return strConfigVduDevicePort; }
};

#endif // IPUCONFIG_H
