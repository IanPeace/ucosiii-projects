/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                              (c) Copyright 2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      APPLICATION CONFIGURATION
*
*                                        Freescale Kinetis K20
*                                               on the
*                                        Freescale FRDM-KL25Z
*                                          Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT

#include  <lib_def.h>

/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/

#define  SW0_PRES_SHORT_FLAG                        0x0001
#define  BUTTON1_PRES_SHORT_FLAG                    0x0002
#define  BUTTON2_PRES_SHORT_FLAG                    0x0004
#define  BUTTON3_PRES_SHORT_FLAG                    0x0008
#define  SW0_PRES_LONG_FLAG                         0x0010
#define  BUTTON1_PRES_LONG_FLAG                     0x0020
#define  BUTTON2_PRES_LONG_FLAG                     0x0040
#define  BUTTON3_PRES_LONG_FLAG                     0x0080

/*
*********************************************************************************************************
*                                       ADDITIONAL uC/MODULE ENABLES
*********************************************************************************************************
*/

#define  APP_CFG_SHELL_EN                           DEF_ENABLED
#define  APP_CFG_PRINTF_EN                          DEF_ENABLED
#define  APP_CFG_GUI_EN                             DEF_ENABLED

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_PRIO                      2u
#define  APP_CFG_TASK_PRIO                            5u
#define  APP_CFG_TASK_KEY_PRIO                        6u

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE                128u
#define  APP_CFG_TASK_STK_SIZE                      128u
#define  APP_CFG_TASK_KEY_STK_SIZE                  128u

/*
*********************************************************************************************************
*                                          SERIAL CONFIGURATION
*********************************************************************************************************
*/

#define  APP_CFG_SERIAL_EN                          DEF_ENABLED
#define  BSP_CFG_SER_COMM_SEL                       BSP_USART_COM1

/*
*********************************************************************************************************
*                                       TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                               0
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                              1
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                               2
#endif


#if (APP_CFG_PRINTF_EN == DEF_ENABLED)
#define  APP_CFG_TRACE                              printf
#define  BSP_CFG_TRACE                              printf
#else
#define  APP_CFG_TRACE                              BSP_Ser_Printf
#define  BSP_CFG_TRACE                              BSP_Ser_Printf
#endif

#define  APP_CFG_TRACE_LEVEL                        TRACE_LEVEL_INFO
#define  BSP_CFG_TRACE_LEVEL                        TRACE_LEVEL_INFO

#define  APP_TRACE_INFO(x)               ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_CFG_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_CFG_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_CFG_TRACE x) : (void)0)

#define  BSP_TRACE_INFO(x)               ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_INFO) ? (void)(BSP_CFG_TRACE x) : (void)0)
#define  BSP_TRACE_DBG(x)                ((BSP_CFG_TRACE_LEVEL  >= TRACE_LEVEL_DBG)  ? (void)(BSP_CFG_TRACE x) : (void)0)

#endif
