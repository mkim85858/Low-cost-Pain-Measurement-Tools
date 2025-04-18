#ifndef _BLUETOOTH_H
#define _BLUETOOTH_H

#include "Globals.h"

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void Bluetooth_Init(void);
extern void Bluetooth_SendToPC(INT8U *data, INT16U len);
extern void Bluetooth_SendPressureTime(INT16U pressure, INT8U time);
#endif
