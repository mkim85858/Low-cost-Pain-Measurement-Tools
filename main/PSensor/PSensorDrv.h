#ifndef _PN532_DRV_H
#define _PN532_DRV_H

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
extern void I2C_Init(void);
extern void Sensor_ReadPressure(INT8U *buf);
#endif
