/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                        Freescale Kinetis K20
*                                               on the
*                                        Freescale FRDM-KL25Z
*                                          Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <app_cfg.h>
#include  <includes.h>

#if (APP_CFG_SHELL_EN == DEF_ENABLED)
#include  <shell.h>
#include  <sh_shell.h>
#include  <terminal.h>
#endif


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

static  OS_TCB        App_TaskStartTCB;
static  CPU_STK_SIZE  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB        App_TaskRGB_LED_TCB;
static  CPU_STK_SIZE  App_TaskRGB_LED_Stk[APP_CFG_TASK_RGB_LED_STK_SIZE];

static  CPU_INT32U    App_RGB_LED_ColorCode;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_ObjCreate       (void);
static  void  App_TaskCreate      (void);

static  void  App_TaskStart       (void *p_arg);
static  void  App_TaskRGB_LED     (void *p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This the main standard entry point.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR     cpu_err;
#endif

    CPU_Init();                                                 /* Initialize the CPU abstraction layer.                */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"MKE02Z64VQH2",
                (CPU_ERR  *)&cpu_err);
#endif

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

#if (defined(TRACE_CFG_EN) && (TRACE_CFG_EN > 0u))
    TRACE_INIT();                                               /* Initialize the µC/Trace recorder.                    */
    TRACE_START();                                              /* Start recording.                                     */
#endif

    OSInit(&err);                                               /* Initialize "uC/OS-III, The Real-Time Kernel".        */

    OSTaskCreate((OS_TCB     *)&App_TaskStartTCB,               /* Create the startup task.                             */
                 (CPU_CHAR   *)"Startup Task",
                 (OS_TASK_PTR ) App_TaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_START_PRIO,
                 (CPU_STK    *)&App_TaskStartStk[0],
                 (CPU_STK     )(APP_CFG_TASK_START_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

    while(DEF_ON) {                                             /* Should Never Get Here                                */
    };
}


/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR  os_err;

    (void)p_arg;                                                /* See Note #1.                                         */

    BSP_Init();                                                 /* Start BSP and tick initialization.                   */

    BSP_Tick_Init();                                            /* Start Tick Initialization.                           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&os_err);                            /* Compute CPU capacity with no task running            */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    APP_TRACE_INFO(("Creating Application Events...\n\r"));
    App_ObjCreate();                                            /* Create Applicaton kernel objects.                    */

    APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    App_TaskCreate();                                           /* Create Application tasks.                            */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0, 0, 0, 100,
                      OS_OPT_TIME_HMSM_STRICT, &os_err);
    }
}


/*
*********************************************************************************************************
*                                      App_ObjCreate()
*
* Description:  Creates the application kernel objects.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  App_ObjCreate (void)
{
}


/*
*********************************************************************************************************
*                                      App_TaskCreate()
*
* Description :  This function creates the application tasks.
*
* Argument(s) :  none.
*
* Return(s)   :  none.
*
* Caller(s)   :  App_TaskStart().
*
* Note(s)     :  none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{
    OS_ERR  err;


    OSTaskCreate((OS_TCB     *)&App_TaskRGB_LED_TCB,            /* Create the RGB LED task.                             */
                 (CPU_CHAR   *)"RGB LED Task",
                 (OS_TASK_PTR ) App_TaskRGB_LED,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_RGB_LED_PRIO,
                 (CPU_STK    *)&App_TaskRGB_LED_Stk[0],
                 (CPU_STK     )(APP_CFG_TASK_RGB_LED_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_RGB_LED_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

#if (APP_CFG_SHELL_EN == DEF_ENABLED)
    Shell_Init();
    ShShell_Init();
    Terminal_Init();
#endif
}


/*
*********************************************************************************************************
*                                          App_TaskRGB_LED()
*
* Description : This is an example of an application task.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskRGB_LED()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskRGB_LED (void *p_arg)
{
     OS_ERR      os_err;


    (void)p_arg;                                                /* See Note #1                                              */

    App_RGB_LED_ColorCode = 0;

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.           */

        BSP_RGB_LED_SetColor(App_RGB_LED_ColorCode++);          /* Set the color of the RGB LED.                            */

        OSTimeDlyHMSM(0, 0, 1, 0,
                      OS_OPT_TIME_HMSM_STRICT, &os_err);        /* Suspend the task execution for 50 milliseconds.          */
    }
}

