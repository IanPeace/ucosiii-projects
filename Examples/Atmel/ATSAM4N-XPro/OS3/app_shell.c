/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              uC/DHCP-c
*                                           APPLICATION CODE
*
* Filename      : app_sail.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               ENABLE
*********************************************************************************************************
*/


#include <stdio.h>
#include <string.h>

#include <math.h>

#include <bsp.h>
#include <os.h>

#include <cmdshell.h>
#include <app_shell.h>

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

#if (APP_CFG_SHELL_EN == DEF_ENABLED)

CPU_INT32U cmdTest(CPU_INT32U argc, CPU_INT08U *argv[]);


static cmdsh_key_t f_cmdshCommands[] =
{
    {"test",    cmdTest}
};


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/


CPU_INT32U cmdTest (CPU_INT32U argc, CPU_INT08U *argv[])
{
    CPU_INT32U loadaddr;

    if (argc == 2) {
        loadaddr = cmdshAtoi(argv[1]);
        APP_TRACE_INFO(("\n\rcmdTest 0x%08x\n\r", loadaddr & 0xFFFF));
    } else {
        cmdshPutSyntax(argv, " <addr>");
    }
    return 1;
}


/*
*********************************************************************************************************
*                                             AppShellTask()
*
* Description : This task monitors the state of the sail
*
* Argument(s) : p_arg   is the argument passed to 'AppKeyTask()' by 'AppTaskCreate()'.
*
* Return(s)  : none.
*
* Caller(s)  : This is a task.
*
* Note(s)    : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  AppShellTask (void *p_arg)
{
    (void)p_arg;

    BSP_Console_Enable();

    cmdshInit(BSP_Console_PutChar, f_cmdshCommands,
              sizeof(f_cmdshCommands) / sizeof(cmdsh_key_t));

    while (DEF_TRUE) {
        cmdshExec((CPU_INT32U)BSP_Console_GetChar());
    }
}

#endif  /* (APP_CFG_SHELL_EN == DEF_ENABLED) */

