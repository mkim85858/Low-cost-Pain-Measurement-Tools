#ifndef _LCD_DRV_H
#define _LCD_DRV_H

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
extern void LCD_Init(void);
extern void LCD_WriteProgressBar(INT8U page, INT16U column, BOOLEAN type);
extern void LCD_ClearPage(INT8U page);
extern void LCD_ClearScreen();
#endif
