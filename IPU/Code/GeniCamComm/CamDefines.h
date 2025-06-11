#ifndef CAMDEFINES_H
#define CAMDEFINES_H

#pragma once

#include <QMessageBox>
#include <PvResult.h>


#define SAFE_DELETE( a ) \
    if ( ( a ) != NULL ) \
    { \
        delete ( a ); \
        ( a ) = NULL; \
    }


inline
void PvMessageBox( QWidget *aWidget, PvResult &aResult )
{
    QString lError = aResult.GetCodeString().GetAscii();
    QString lDescription = aResult.GetDescription().GetAscii();
    QMessageBox::critical( aWidget, "Error", lError + "\r\n\r\n" + lDescription + "\r\n" );
}

#endif // CAMDEFINES_H
