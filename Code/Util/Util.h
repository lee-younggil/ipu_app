#ifndef UTIL_H
#define UTIL_H

#include <QObject>
#include <QDateTime>


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
class CUtil
{
public:
    CUtil();

public:
	static unsigned char INTtoBCD( unsigned int iTemp );
	static int BCDtoINT( unsigned char iTemp );
	static int BCDtoINT( const unsigned char *pucaTemp, int iLen );
	static unsigned long GetMiliSecTimer();
	static DATE_YYYYMMDDhhmmssmsms0 GetYYYYMMDDhhmmssmsms0(QDateTime currentDateTime);
};

#endif // UTIL_H
