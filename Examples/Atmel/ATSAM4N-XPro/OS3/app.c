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

#include  <app_key.h>
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

#define APP_LED0_TMR_TICK            5                          /* Timer Task Tick Rate (10 Hz Typ.) */
#define APP_LED1_TMR_TICK            10                         /* Timer Task Tick Rate (10 Hz Typ.) */
#define APP_LED2_TMR_TICK            15                         /* Timer Task Tick Rate (10 Hz Typ.) */
#define APP_LED3_TMR_TICK            20                         /* Timer Task Tick Rate (10 Hz Typ.) */
#define APP_LED4_TMR_TICK            5                          /* Timer Task Tick Rate (10 Hz Typ.) */
#define APP_uGUI_RF_TMR_TICK         1                          /* Timer Task Tick Rate (10 Hz Typ.) */

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB        App_TaskStartTCB;
static  CPU_STK_SIZE  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB        App_TaskTCB;
static  CPU_STK_SIZE  App_TaskStk[APP_CFG_TASK_STK_SIZE];

static  OS_TCB        App_TaskKeyTCB;
static  CPU_STK_SIZE  App_TaskKeyStk[APP_CFG_TASK_KEY_STK_SIZE];

static OS_TMR   App_LED0Tmr;
static OS_TMR   App_LED1Tmr;
static OS_TMR   App_LED2Tmr;
static OS_TMR   App_LED3Tmr;
static OS_TMR   App_LED4Tmr;
static OS_TMR   App_uGUIRFTmr;

OS_FLAG_GRP APP_KeyFlagGrp;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_ObjCreate       (void);
static  void  App_TaskCreate      (void);

static  void  App_TaskStart       (void *p_arg);
static  void  App_Task            (void *p_arg);
static  void  App_LEDTmrCallback  (void *p_tmr_os, void *p_arg);
static  void  App_uGUIRFTmrCallback (void *p_tmr_os, void *p_arg);

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
    CPU_NameSet((CPU_CHAR *)"ATSAM4N16C",
                (CPU_ERR  *)&cpu_err);
#endif

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

#if (defined(TRACE_CFG_EN) && (TRACE_CFG_EN > 0u))
    TRACE_INIT();                                               /* Initialize the ¦ÌC/Trace recorder.                    */
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

    BSP_SSD1306_uGUI_Init();
    UG_FillScreen(0);

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
    OS_ERR      os_err;

    OSFlagCreate(&APP_KeyFlagGrp, "Key Flags", 0, &os_err);

    OSTmrCreate(&App_LED0Tmr, "Led0 Tmr",
                0, APP_LED0_TMR_TICK, OS_OPT_TMR_PERIODIC,
                App_LEDTmrCallback, (void *)BSP_LED0, &os_err);
    OSTmrCreate(&App_LED1Tmr, "Led1 Tmr",
                0, APP_LED1_TMR_TICK, OS_OPT_TMR_PERIODIC,
                App_LEDTmrCallback, (void *)BSP_LED1, &os_err);
    OSTmrCreate(&App_LED2Tmr, "Led2 Tmr",
                0, APP_LED2_TMR_TICK, OS_OPT_TMR_PERIODIC,
                App_LEDTmrCallback, (void *)BSP_LED2, &os_err);
    OSTmrCreate(&App_LED3Tmr, "Led3 Tmr",
                0, APP_LED3_TMR_TICK, OS_OPT_TMR_PERIODIC,
                App_LEDTmrCallback, (void *)BSP_LED3, &os_err);
    OSTmrCreate(&App_LED4Tmr, "Led4 Tmr",
                0, APP_LED4_TMR_TICK, OS_OPT_TMR_PERIODIC,
                App_LEDTmrCallback, (void *)BSP_LED4, &os_err);

#if (APP_CFG_GUI_EN == DEF_ENABLED)
    OSTmrCreate(&App_uGUIRFTmr, "uGUI Refresh Tmr",
                0, APP_uGUI_RF_TMR_TICK, OS_OPT_TMR_PERIODIC,
                App_uGUIRFTmrCallback, (void *)NULL, &os_err);
    OSTmrStart(&App_uGUIRFTmr, &os_err);
#endif
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


    OSTaskCreate((OS_TCB     *)&App_TaskTCB,                    /* Create Application task.                             */
                 (CPU_CHAR   *)"App Task",
                 (OS_TASK_PTR ) App_Task,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_PRIO,
                 (CPU_STK    *)&App_TaskStk[0],
                 (CPU_STK     )(APP_CFG_TASK_STK_SIZE / 10u),
                 (CPU_STK_SIZE) APP_CFG_TASK_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSTaskCreate((OS_TCB     *)&App_TaskKeyTCB,      /* Create the Key task                                  */
                 (CPU_CHAR   *)"App Key Task",
                 (OS_TASK_PTR ) AppKeyTask,
                 (void       *) 0,
                 (OS_PRIO     ) APP_CFG_TASK_KEY_PRIO,
                 (CPU_STK    *)&App_TaskKeyStk[0],
                 (CPU_STK_SIZE) APP_CFG_TASK_KEY_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_CFG_TASK_KEY_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
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
*                                          App_Task()
*
* Description : This is an example of an application task.
*
* Argument(s) : p_arg   is the argument passed to 'App_Task()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_Task (void *p_arg)
{
     OS_ERR      os_err;
     CPU_TS      pend_ts;
     CPU_CHAR    *key_name;
     OS_FLAGS    flags_rdy;
     OS_FLAGS    flags_wait = SW0_PRES_SHORT_FLAG | SW0_PRES_LONG_FLAG | \
                              BUTTON1_PRES_SHORT_FLAG | BUTTON1_PRES_LONG_FLAG | \
                              BUTTON2_PRES_SHORT_FLAG | BUTTON2_PRES_LONG_FLAG | \
                              BUTTON3_PRES_SHORT_FLAG | BUTTON3_PRES_LONG_FLAG;

    (void)p_arg;                                                /* See Note #1                                              */

    OSTmrStart(&App_LED0Tmr, &os_err);

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        flags_rdy = OSFlagPend(&APP_KeyFlagGrp, flags_wait, 0,
                               (OS_OPT_PEND_FLAG_SET_ANY |
                                OS_OPT_PEND_FLAG_CONSUME),
                               &pend_ts, &os_err);
        if ((flags_rdy & flags_wait) != 0) {
            switch (flags_rdy) {
                case SW0_PRES_SHORT_FLAG:
                    key_name = "SW0";
                    OSTmrStart(&App_LED4Tmr, &os_err);
                    break;
                case SW0_PRES_LONG_FLAG:
                    key_name = "SW0 long";
                    OSTmrStop(&App_LED4Tmr, OS_OPT_TMR_NONE, NULL, &os_err);
                    BSP_LED_Set(BSP_LED4, BSP_LED_OFF);
                    break;
                case BUTTON1_PRES_SHORT_FLAG:
                    key_name = "BUTTON 1";
                    OSTmrStart(&App_LED1Tmr, &os_err);
                    UG_FillScreen(0);
                    UG_FontSelect(&FONT_6X10);
                    UG_PutString(2, 16, "hello");
                    UG_FontSelect(&FONT_12X16);
                    UG_PutString(38, 10, "u");
                    UG_FontSelect(&FONT_16X26);
                    UG_PutString(52, 3, "GUI");
                    UG_FillCircle(112, 15, 10, 1);
                    break;
                case BUTTON1_PRES_LONG_FLAG:
                    key_name = "BUTTON 1 long";
                    OSTmrStop(&App_LED1Tmr, OS_OPT_TMR_NONE, NULL, &os_err);
                    BSP_LED_Set(BSP_LED1, BSP_LED_OFF);
                    break;
                case BUTTON2_PRES_SHORT_FLAG:
                    key_name = "BUTTON 2";
                    OSTmrStart(&App_LED2Tmr, &os_err);
                    UG_FillScreen(0);
                    UG_DrawMesh(0, 0, 63, 15, 0);
                    UG_DrawMesh(64, 0, 127, 15, 1);
                    UG_DrawMesh(64, 16, 127, 31, 0);
                    UG_DrawMesh(0, 16, 63, 31, 1);
                    break;
                case BUTTON2_PRES_LONG_FLAG:
                    key_name = "BUTTON 2 long";
                    OSTmrStop(&App_LED2Tmr, OS_OPT_TMR_NONE, NULL, &os_err);
                    BSP_LED_Set(BSP_LED2, BSP_LED_OFF);
                    break;
                case BUTTON3_PRES_SHORT_FLAG:
                    key_name = "BUTTON 3";
                    OSTmrStart(&App_LED3Tmr, &os_err);
                    UG_FillScreen(1);
                    break;
                case BUTTON3_PRES_LONG_FLAG:
                    key_name = "BUTTON 3 long";
                    OSTmrStop(&App_LED3Tmr, OS_OPT_TMR_NONE, NULL, &os_err);
                    BSP_LED_Set(BSP_LED3, BSP_LED_OFF);
                    break;
                default:
                    break;
            }
            APP_TRACE_INFO(("%s pressed\n\r", key_name));
        }
    }
}

static void App_LEDTmrCallback (void *p_tmr_os, void *p_arg)
{
    BSP_LED_Toggle((CPU_INT32U)p_arg);
}

static void App_uGUIRFTmrCallback (void *p_tmr_os, void *p_arg)
{
    ssd1306_update();
}

