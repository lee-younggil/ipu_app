#ifndef UTIL_H
#define UTIL_H

#pragma once

#pragma pack(push)
#pragma pack(1)

typedef struct _DATE_YYYYMMDD
{
    unsigned char bcd_YYYY[2];
    unsigned char bcd_MM;
    unsigned char bcd_DD;

    bool operator ==(const struct _DATE_YYYYMMDD &T ) const
    {
        return (	bcd_YYYY[0]	== T.bcd_YYYY[0]	&&
                    bcd_YYYY[1]	== T.bcd_YYYY[1]	&&
                    bcd_MM		== T.bcd_MM			&&
                    bcd_DD		== T.bcd_DD			);
    }
} DATE_YYYYMMDD;

typedef struct _DATE_YYYYMMDDhhmm
{
    unsigned char bcd_YYYY[2];
    unsigned char bcd_MM;
    unsigned char bcd_DD;
    unsigned char bcd_hh;
    unsigned char bcd_mm;
} DATE_YYYYMMDDhhmm;

typedef struct _DATE_YYYYMMDDhhmmss
{
    unsigned char bcd_YYYY[2];
    unsigned char bcd_MM;
    unsigned char bcd_DD;
    unsigned char bcd_hh;
    unsigned char bcd_mm;
    unsigned char bcd_ss;
} DATE_YYYYMMDDhhmmss;

typedef struct _DATE_YYYYMMDDhhmmssmsms0
{
    unsigned char bcd_YYYY[2];
    unsigned char bcd_MM;
    unsigned char bcd_DD;
    unsigned char bcd_hh;
    unsigned char bcd_mm;
    unsigned char bcd_ss;
    unsigned char bcd_msms0[2];
} DATE_YYYYMMDDhhmmssmsms0;

typedef struct _DATE_MMDDhh
{
    unsigned char bcd_MM;
    unsigned char bcd_DD;
    unsigned char bcd_hh;
}DATE_MMDDhh;

typedef struct _TIME_hhmm
{
    unsigned char	bcd_hh;
    unsigned char	bcd_mm;
} TIME_hhmm;

typedef struct _TIME_hhmmss
{
    unsigned char	bcd_hh;
    unsigned char	bcd_mm;
    unsigned char	bcd_ss;
} TIME_hhmmss;

#pragma pack(pop)

typedef enum _OS_VERSION
{
    OS_VERSION_UNKNOWN = 0,
    OS_VERSION_WES2009,
    OS_VERSION_WES7,
    OS_VERSION_WIN10,
} OS_VERSION;

// 비트
#define	BIT31	0x80000000
#define	BIT30	0x40000000
#define	BIT29	0x20000000
#define	BIT28	0x10000000
#define	BIT27	0x08000000
#define	BIT26	0x04000000
#define	BIT25	0x02000000
#define	BIT24	0x01000000
#define	BIT23	0x00800000
#define	BIT22	0x00400000
#define	BIT21	0x00200000
#define	BIT20	0x00100000
#define	BIT19	0x00080000
#define	BIT18	0x00040000
#define	BIT17	0x00020000
#define	BIT16	0x00010000
#define	BIT15	0x00008000
#define	BIT14	0x00004000
#define	BIT13	0x00002000
#define	BIT12	0x00001000
#define	BIT11	0x00000800
#define	BIT10	0x00000400
#define	BIT9	0x00000200
#define	BIT8	0x00000100
#define	BIT7	0x00000080
#define	BIT6	0x00000040
#define	BIT5	0x00000020
#define	BIT4	0x00000010
#define	BIT3	0x00000008
#define	BIT2	0x00000004
#define	BIT1	0x00000002
#define	BIT0	0x00000001

#define BIT(n)	((0x00000001) << (n))

// 비트 연산
#define	BITSET(X, MASK)				((X) |= (MASK))
#define	BITSCLR(X, SMASK, CMASK)	((X) = (((X) | (SMASK)) & ~(CMASK)))
#define	BITCSET(X, CMASK, SMASK)	((X) = (((X) & ~(CMASK)) | (SMASK)))
#define	BITCLR(X, MASK)				((X) &= ~(MASK))
#define	BITXOR(X, MASK)				((X) ^= (MASK))
#define	BITFLIP(X, MASK)			((X) ^= (MASK))
#define	BITINVERT(X, MASK)			((X) ^= (MASK))

#define	BITGET(X, MASK)				((X) & (MASK))
#define	ISSET(X, MASK)				((X) & (MASK))
#define	IS(X, MASK)					((X) & (MASK))
#define	ISONE(X, MASK)				((X) & (MASK))

#define	ISALLONE(X, MASK)			(((X) & (MASK)) == (MASK))

#define	ISCLR(X, MASK)				(!((X) & (MASK)))
#define	ISZERO(X, MASK)				(!((X) & (MASK)))
#define	ISNOT(X, MASK)				(!((X) & (MASK)))

// BCD를 HEX로 바꿔주는 매크로
#define BCD2HEX(n) (((((n) & 0xF0) >> 4) * 10) + ((n) & 0x0F))
// HEX를 BCD로 바꿔주는 매크로
#define HEX2BCD(n) ((((n) / 10) << 4) + ((n) % 10))

// make byte (low-order nibble, high-order nibble) e.g. MAKEBYTE(0x08, 0x04) -> 0x48
#ifndef MAKEBYTE
#define MAKEBYTE(nl, nh)	((BYTE)(((nl) & 0x0F) | (((nh) & 0x0F) << 4)))
#endif
// get high-order nibble.	e.g. 0x48  -> 0x04
#ifndef HINIBBLE
#define HINIBBLE(byte)		((byte) >> 4)
#endif
// get low-order nibble.	e.g. 0x48  -> 0x08
#ifndef LONIBBLE
#define LONIBBLE(byte)		((byte) & 0x0F)
#endif

// swap : 두 변수의 값 바꾸기, 비트 연산 ^(XOR)를 사용하기 때문에 정수형 변수만 사용 가능
#define SWAP(a, b)	(a)^=(b)^=(a)^=(b)

#define countof(array)   (sizeof(array) / sizeof(*(array)))

#define INTERNAL_IP_ADDRESS_MASK	(_T("192.168.100"))

typedef enum NEW_OLD
{
    NEW = 0,
    OLD = 1,
} ENUM_NEW_OLD;

class CUtil
{
public:
    CUtil(void);
    virtual ~CUtil(void);

public:
    static const int CONVERT_SEC_MS = 1000;

    static int			Round( double val );
    static double		Round( double dNumber, int iNumDigits);
    static double		MRound( double dNumber, double dNumDigits );

    static bool			INTtoBCD( UCHAR *pDst, UINT iSrc, int iDstLen );
    static UCHAR		INTtoBCD( UINT iTemp );
    static int			BCDtoINT( UCHAR iTemp );
    static int			BCDtoINT( const UCHAR *pucaTemp, int iLen );
    static bool			IsIncludeNumber( const int iNum, const int iMin, const int iMax );
    static bool			IsBCD( const UCHAR bcd_No );

    static BOOL			IsDirectoryExist( const CString &pDir );
    static BOOL			IsFileExist( const char *pFileName );
    static BOOL			CreateDir( const CString &path );
    static BOOL			deleteDir(const char *path);

    static bool			SetTimeZone();
    static ULONG		DurationTime (ULONG ulReferenceTime, ULONG ulOldTimeA, ULONG ulOldTimeB);
    static ULONG		GetQueryMiliSecTimer();
    static ULONG		GetMiliSecTimer();
    static unsigned long long GetMiliSecond (const DATE_YYYYMMDDhhmmssmsms0 & rtmsec);

    static CTime		GetCTime( int YYYY, int MM, int DD, int hh, int mm, int ss );
    static CTime		GetCTime( UCHAR bcd_YYYY0, UCHAR bcd_YYYY1, UCHAR bcd_MM, UCHAR bcd_DD, UCHAR bcd_hh, UCHAR bcd_mm, UCHAR bcd_ss );
    static CTime		GetCTime( DATE_YYYYMMDDhhmmssmsms0 t );
    static CTime		GetCTime( DATE_YYYYMMDDhhmmss t );
    static CTime		GetCTime( DATE_YYYYMMDDhhmm t );
    static CTime		GetCTime( DATE_YYYYMMDD t );

    static DATE_YYYYMMDDhhmmss	GetYYYYMMDDhhmmss( const CTime &tTime );
    static DATE_YYYYMMDDhhmmss	GetYYYYMMDDhhmmss( SYSTEMTIME tTime );
    static DATE_YYYYMMDDhhmm	GetYYYYMMDDhhmm( const CTime &tTime );
    static DATE_YYYYMMDD		GetYYYYMMDD( const CTime &tTime );
    static DATE_YYYYMMDDhhmmssmsms0 GetYYYYMMDDhhmmssmsms0( SYSTEMTIME st );
    static DATE_YYYYMMDDhhmmssmsms0 GetYYYYMMDDhhmmssmsms0( const CTime &tTime );

    static DATE_YYYYMMDDhhmmssmsms0 AddTime_ms( DATE_YYYYMMDDhhmmssmsms0 t, USHORT usAddTime );

    static int CalcMonthLastDay( int y, int m );

    static CTimeSpan	GetCTimeSpan( TIME_hhmm t );
    static ENUM_NEW_OLD GetConfirmTime( const CTime &NewTime, const CTime &CloseTime, const CTime &CurrentTime, const CTime &EntranceTime );
    static ENUM_NEW_OLD GetConfirmTime( const CTime &NewTime, const CTime &CurrentTime );

    static bool CheckErrorDateYYYYMMDDhhmm( const CTime &date );
    static bool CheckErrorDateYYYYMMDD( const CTime &date );

    static bool IsDataNull( const UCHAR *pucData, const int iSize );
    static int CStringToHex( CString strSrc, UCHAR *pucaDst );
    static int CStringToHex( CString strSrc, UCHAR *pucaDst, USHORT usDstLen );	// khjeon

    static void GetIPAddress( DWORD &dwIP, DWORD &dwSubnetMask, DWORD &dwGateway );
    static void SetIPAddress( const DWORD dwIP, const DWORD dwSubnetMask, const DWORD dwGateway );
    static void CompressFolder( const CString &strFolder );

    static bool CString2bool( const CString &strBOOL );

    static bool IsValidCardNo( const UCHAR *bcd_CardNo );
    static void Rebooting();
    static int CuPayTypeToInt( const UCHAR *pCuPayType );
    static void IntToCuPayType( int iFee, UCHAR *pDestPayTypeValue );

    static int GetOSVersion();

    static int GetMacAddress( BYTE pucaLanAdapterAddress[][6], UINT uiMaxAddressNo );

    static void DelayRebooting(DWORD dwMilliseconds);

    static UINT DelayRebootingThread( LPVOID pvParam );

    static __int64 FileSeek (HANDLE hf, __int64 distance, void *DUMMY, DWORD MoveMethod);

    static bool IsCTimeYear( WORD wYear );
    static bool IsCTimeMonth( WORD wMonth );
    static bool IsCTimeDay( WORD wDay );
    static bool IsCTimeHour( WORD wHour );
    static bool IsCTimeMin( WORD wMin );
    static bool IsCTimeSec( WORD wSec );

    static void inet_ntoa(UINT in, unsigned char *buf);
    static CString CInetNtoStr(DWORD in);
    static void T_ShowAdapterInfo(CStringArray *pStrInfoList);
    static bool GetIPAddressEx(DWORD &dwIP, DWORD &dwSubnetMask, DWORD &dwGateway, char* caAdapterName = NULL, bool inter = false);
    static bool SetIPAddressEx(char* caAdapterName, const DWORD dwIP, const DWORD dwSubnetMask, const DWORD dwGateway );
    static DWORD TransmitARP(LPCTSTR lpctIP, ULONG *pulMACAddr = NULL, ULONG ulSize = 6);
    static int GetAvailableIPAddress(char caBuffer[][24], int iMaxCount);

    static bool ReadRegistry(HKEY hKey, CString strSubKey, CString strValueName, CString & strValue);
    static bool WriteRegistry(HKEY hKey, CString strSubKey, CString strValueName, const CString & strValue);

    static void GetIPAddressInUse(CStringArray * pIpAddress);
    static bool GetFriendlyNameByAdapterName(const char * pcaAdapterName, char * paFriendlyName);
};


#endif // UTIL_H
