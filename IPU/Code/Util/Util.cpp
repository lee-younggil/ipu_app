#include "Util.h"

CUtil::CUtil()
{

}

DATE_YYYYMMDDhhmmssmsms0 CUtil::GetYYYYMMDDhhmmssmsms0(QDateTime DateTime)
{
    DATE_YYYYMMDDhhmmssmsms0 dateData;

    // QDateTime에서 날짜와 시간 추출
    int year = DateTime.date().year();
    int month = DateTime.date().month();
    int day = DateTime.date().day();
    int hour = DateTime.time().hour();
    int minute = DateTime.time().minute();
    int second = DateTime.time().second();
    int msec = DateTime.time().msec();

    // DATE_YYYYMMDDhhmmssmsms0 구조체에 값 설정
    dateData.bcd_YYYY[0] = INTtoBCD(year / 100);
    dateData.bcd_YYYY[1] = INTtoBCD(year % 100);
    dateData.bcd_MM = INTtoBCD(month);
    dateData.bcd_DD = INTtoBCD(day);
    dateData.bcd_hh = INTtoBCD(hour);
    dateData.bcd_mm = INTtoBCD(minute);
    dateData.bcd_ss = INTtoBCD(second);
    dateData.bcd_msms0[0] = INTtoBCD(msec / 10);
    dateData.bcd_msms0[1] = INTtoBCD((msec % 10) * 10); // Assuming reserved is the last byte

    return dateData;

}

unsigned char CUtil::INTtoBCD( unsigned int iTemp )
{
	if( iTemp > 99 )
		return 0;

	return (unsigned char)((( iTemp / 10 ) << 4 ) + ( iTemp % 10 ));
}

int CUtil::BCDtoINT( unsigned char iTemp )
{
	if( ( iTemp > 0x99 ) || ( ( iTemp & (unsigned char)0x0F ) > 0x09 ) )
		return 0;

	return ( ( iTemp & (unsigned char)0xF0 ) >> 4 ) * 10 + ( iTemp & (unsigned char)0x0F );
}

int CUtil::BCDtoINT( const unsigned char *pucaTemp, int iLen )
{
	int iResult = 0;
	for( int i = 0; i < iLen ; i++ )
	{
		iResult *= 100;
		iResult += BCDtoINT( pucaTemp[i] );
	}
	return iResult;
}

unsigned long CUtil::GetMiliSecTimer()
{
	return static_cast<unsigned long>(QDateTime::currentDateTime().toSecsSinceEpoch());
}
