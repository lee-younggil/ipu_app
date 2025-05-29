#ifndef COMPLETIONSTRUCT_H
#define COMPLETIONSTRUCT_H

#include "Util/Util.h"

#define DEVICE_CODE ( 0x04 )

#define STX	( 0x02 )
#define ETX	( 0x03 )
#define DLE	( 0x10 )

#define PACKET_LENGTH_STX		1
#define PACKET_LENGTH_HEADER	( 2 + 1 + 2 + 2 + 2 )	// SEQ, Retry, Length, DeviceCode, Dummy or CRC16
#define PACKET_LENGTH_MsgID		2
#define PACKET_LENGTH_ETX		1
#define PACKET_LENGTH_MIN		( PACKET_LENGTH_STX + PACKET_LENGTH_HEADER + PACKET_LENGTH_MsgID + PACKET_LENGTH_ETX )
#define PACKET_LENGTH_MAX		8192 // 도공 안종필 차장 Data 5000 byte 보내서 2023.10.25 -PDJ

#define PACKET_BUF_SIZE			( PACKET_LENGTH_MAX * 4 )

#define MsgID_ACK			(0x0001)
#define MsgID_NAK			(0x0002)
#define MsgID_HeartBeat		(0x0003)
#define MsgID_Status		(0x0004)

#define MsgID_Repair		(0x0102)

#define MsgID_TimeSync		(0x1003)
#define MsgID_Command		(0x1004)

#define MsgID_Trigger		(0x3401)
#define MsgID_EntryVehicle	(0x3101)
#define MsgID_ExitVehicle	(0x3102)

#define MsgID_Image			(0x4101)
#define MsgID_ImageResp		(0x1401)

#define KsgID_LicStatus 	(0x4601)
#define MsgID_LicCamZoom	(0x4603)
#define MsgID_LicCamFocus	(0x4604)
#define MsgID_LicCamIris	(0x4605)

#define MsgID_VehicleSpeed	(0x1601)

#define MsgID_TIU_REQ		(0x1801)
#define MsgID_TIU_RESP		(0x8101)

#define MsgID_TIU_WORK_CMD		(0x1809)
#define MsgID_TIU_STATUS_DATA		(0x8109)

#define NAK_ERROR_CODE_CRC_ERROR	(0x01)
#define NAK_ERROR_CODE_SHORT_LENGTH	(0x02)
#define NAK_ERROR_CODE_LONG_LENGTH	(0x03)

#define STATE_SEND_TIME			( 1 * 60 * 1000 )
#define RESEND_TIME_HEART_BEAT	(3500)
#define RETRY_TIME				(1000)
#define RETRY_COUNT				(3)

enum tag_DEVICE_CODE
{
    DEVICE_CODE_MCU = 0x01,
    DEVICE_CODE_VDU = 0x03,
    DEVICE_CODE_IPU = 0x04,
    DEVICE_CODE_VMS = 0x06,
    DEVICE_CODE_ENV = 0x07,
    DEVICE_CODE_TIU = 0x08,
    DEVICE_CODE_NVR = 0x09,
};

#pragma pack(push, 1) // 1바이트 정렬
typedef struct _tag_PACKET
{
    unsigned char	byteSTX;
    unsigned short	usSeqNo;
    unsigned char	byteRetryCnt;
    unsigned short	usLength;
    unsigned char	byteDeviceCode;
    unsigned char	byteDeviceID;
    unsigned short	usCRC;

    // Data
    unsigned short	usMsgID;
    unsigned char	byteData[1];
} PACKET;

typedef struct _tag_PACKET_ACK
{
    unsigned short	usSeqNo;
    unsigned char	DUMMY[2];
} PACKET_ACK;

typedef struct _tag_PACKET_STATE
{
	DATE_YYYYMMDDhhmmssmsms0 dateData;
	unsigned char	byteMajorVersion;
	unsigned char	byteMinorVersion;
	unsigned char	byteDeviceState[20];
	unsigned char	DUMMY[31];
} PACKET_STATE;

typedef struct _tag_PACKET_TIME_SYNC
{
	DATE_YYYYMMDDhhmmssmsms0 dateTime;
	unsigned char DUMMY[3];
} PACKET_TIME_SYNC;

typedef struct _tag_PACKET_ENTRY_TRIGGER_NUM
{
	unsigned char	byteVehiclePosition;// 차량 위치
	DATE_YYYYMMDDhhmmssmsms0 dateData;	// 발생시각
	unsigned char	byteVDSType;		// VDS 구분
	unsigned int	uiTriggerNo;		// trigger No
	unsigned char	DUMMY[7];
} PACKET_ENTRY_TRIGGER_NUM;

typedef struct _VEHILE_INFO
{
    unsigned int triggerNo;
    unsigned long ulLastCommunicationTime;
    unsigned char bitInfoState;

    PACKET_ENTRY_TRIGGER_NUM pktEntryTriggerNum;
} VEHICLE_INFO;

typedef struct _tag_PACKET_IMAGE
{
	unsigned char byteVehiclePosition;
	DATE_YYYYMMDDhhmmssmsms0 dateData;
	unsigned char byteVDSType;
	unsigned int uiTriggerNo;
	struct
	{
		unsigned char byteLaneCode;
		unsigned char baVehicleID[5];
	} taVehicelID[2];

	unsigned char DUMMY[10];
} PACKET_IMAGE;

typedef struct _tag_PACKET_IMAGE_RESP
{
	unsigned int	uiTriggerNoVDS1;
	unsigned int	uiTriggerNoVDS2;

	unsigned char	byteSelImagePosition;
	unsigned char	baICNo[2];
	unsigned char	byteLaneNo;
	unsigned char	byteServiceNo;
	DATE_YYYYMMDD	dateService;
	unsigned char	byteServiceType;
	DATE_YYYYMMDDhhmmss	dateProcess;
	unsigned char	byteTransactionCnt;
	struct 
	{
		unsigned int uiTransactionNo;
		unsigned char byteViolationType;
		unsigned char byteViolationCode;
	} tTransactionData[4];

	unsigned char	DUMMY[10];
} PACKET_IMAGE_RESP;

typedef struct _tag_PACKET_LIC_STATUS
{
	unsigned short usTempurature;
	unsigned short usCdsFirst;
	unsigned short usCdsSecond;
	unsigned char  reserve[4];
} PACKET_LIC_STATUS;

typedef struct _tag_PACKET_LIC_CONTROL
{
	unsigned char byteBossSign;
	unsigned char byteValue;
	unsigned char reserve[4];
} PACKET_LIC_CONTROL;


#pragma pack(pop)

#endif // COMPLETIONSTRUCT_H
