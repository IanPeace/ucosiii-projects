/*
*********************************************************************************************************
*                                               uC/Shell
*                                            Shell utility
*
*                           (c) Copyright 2007-2013; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              TERMINAL
*
*                                    TEMPLATE COMMUNICATIONS PORT
*
* Filename      : terminal_serial.c
* Version       : V1.03.01
* Programmer(s) : BAN
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include <terminal.h>
#include <bsp.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        TerminalSerial_Init()
*
* Description : Initialize serial communications.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if interface was opened.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Terminal_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  TerminalSerial_Init (void)
{
#if (APP_CFG_SHELL_EN == DEF_ENABLED)
    BSP_Console_Enable();
    return (DEF_OK);
#else
    return (DEF_FAIL);
#endif
}


/*
*********************************************************************************************************
*                                        TerminalSerial_Exit()
*
* Description : Uninitialize serial communications.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  TerminalSerial_Exit (void)
{

}


/*
*********************************************************************************************************
*                                         TerminalSerial_Wr()
*
* Description : Serial output.
*
* Argument(s) : pbuf        Pointer to the buffer to transmit.
*
*               buf_len     Number of bytes in the buffer.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Out().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16S  TerminalSerial_Wr (void        *pbuf,
                               CPU_SIZE_T   buf_len)
{
#if (APP_CFG_SHELL_EN == DEF_ENABLED)
    CPU_SIZE_T idx;
    char *cbuf;

    if (pbuf == NULL) {
        return (-1);
    }

    for (idx = 0, cbuf = (char *)pbuf; idx < buf_len; idx++) {
        BSP_Console_PutChar(cbuf[idx]);
    }
    return (buf_len);
#else
    return (-1);
#endif
}


/*
*********************************************************************************************************
*                                       TerminalSerial_RdByte()
*
* Description : Serial byte input.
*
* Argument(s) : none.
*
* Return(s)   : Byte read from port.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  TerminalSerial_RdByte (void)
{
#if (APP_CFG_SHELL_EN == DEF_ENABLED)
    return BSP_Console_GetChar();
#else
    return (0);
#endif
}


/*
*********************************************************************************************************
*                                       TerminalSerial_WrByte()
*
* Description : Serial byte output.
*
* Argument(s) : c           Byte to write.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  TerminalSerial_WrByte (CPU_INT08U  c)
{
#if (APP_CFG_SHELL_EN == DEF_ENABLED)
    BSP_Console_PutChar((char)c);
#endif
}
