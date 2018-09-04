/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2013; Micrium, Inc.; Weston, FL
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
*                                        BOARD SUPPORT PACKAGE
*
*                                        Freescale Kinetis K20
*                                               on the
*                                        Freescale FRDM-KL25Z
*                                          Evaluation Board
*
* Filename      : bsp_int.c
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_INT_MODULE
#include <bsp.h>

#ifdef  BSP_CFG_OS
#include <bsp_os.h>
#endif


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_SRC_NBR                                 32


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

static  CPU_FNCT_VOID  BSP_IntVectTbl[BSP_INT_SRC_NBR];


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

static  void  BSP_IntHandler      (CPU_DATA  int_id);
static  void  BSP_IntHandlerDummy (void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              BSP_IntClr()
*
* Description : Clear interrupt.
*
* Argument(s) : int_id      Interrupt to clear.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) An interrupt does not need to be cleared within the interrupt controller.
*********************************************************************************************************
*/

void  BSP_IntClr (CPU_DATA  int_id)
{

}


/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}


/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcEn(int_id + 16);
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntVectSet()
*
* Description : Assign ISR handler.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               isr         Handler to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntVectSet (CPU_DATA       int_id,
                      CPU_FNCT_VOID  isr)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        BSP_IntVectTbl[int_id] = isr;
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*                                            BSP_IntPrioSet()
*
* Description : Assign ISR priority.
*
* Argument(s) : int_id      Interrupt for which vector will be set.
*
*               prio        Priority to assign
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntPrioSet (CPU_DATA    int_id,
                      CPU_INT08U  prio)
{
    CPU_SR_ALLOC();


    if (int_id < BSP_INT_SRC_NBR) {
        CPU_CRITICAL_ENTER();
        CPU_IntSrcPrioSet(int_id + 16, prio);
        CPU_CRITICAL_EXIT();
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              BSP_IntInit()
*
* Description : Initialize interrupts:
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntInit (void)
{
    CPU_DATA  int_id;


    for (int_id = 0; int_id < BSP_INT_SRC_NBR; int_id++) {
        BSP_IntVectSet(int_id, BSP_IntHandlerDummy);
    }
}


/*
*********************************************************************************************************
*                                        BSP_IntHandler####()
*
* Description : Handle an interrupt.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : This is an ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntHandlerSUPC             (void)  { BSP_IntHandler(BSP_INT_ID_SUPC);                      }
void  BSP_IntHandlerRSTC             (void)  { BSP_IntHandler(BSP_INT_ID_RSTC);                      }
void  BSP_IntHandlerRTC              (void)  { BSP_IntHandler(BSP_INT_ID_RTC);                       }
void  BSP_IntHandlerRTT              (void)  { BSP_IntHandler(BSP_INT_ID_RTT);                       }
void  BSP_IntHandlerWDT              (void)  { BSP_IntHandler(BSP_INT_ID_WDT);                       }
void  BSP_IntHandlerPMC              (void)  { BSP_IntHandler(BSP_INT_ID_PMC);                       }
void  BSP_IntHandlerEFC              (void)  { BSP_IntHandler(BSP_INT_ID_EFC);                       }
void  BSP_IntHandlerRSVD7            (void)  { BSP_IntHandler(BSP_INT_ID_RSVD7);                     }
void  BSP_IntHandlerUART0            (void)  { BSP_IntHandler(BSP_INT_ID_UART0);                     }
void  BSP_IntHandlerUART1            (void)  { BSP_IntHandler(BSP_INT_ID_UART1);                     }
void  BSP_IntHandlerUART2            (void)  { BSP_IntHandler(BSP_INT_ID_UART2);                     }
void  BSP_IntHandlerPIOA             (void)  { BSP_IntHandler(BSP_INT_ID_PIOA);                      }
void  BSP_IntHandlerPIOB             (void)  { BSP_IntHandler(BSP_INT_ID_PIOB);                      }
void  BSP_IntHandlerPIOC             (void)  { BSP_IntHandler(BSP_INT_ID_PIOC);                      }
void  BSP_IntHandlerUSART0           (void)  { BSP_IntHandler(BSP_INT_ID_USART0);                    }
void  BSP_IntHandlerUSART1           (void)  { BSP_IntHandler(BSP_INT_ID_USART1);                    }
void  BSP_IntHandlerUART3            (void)  { BSP_IntHandler(BSP_INT_ID_UART3);                     }
void  BSP_IntHandlerUSART2           (void)  { BSP_IntHandler(BSP_INT_ID_USART2);                    }
void  BSP_IntHandlerRSVD18           (void)  { BSP_IntHandler(BSP_INT_ID_RSVD18);                    }
void  BSP_IntHandlerTWI0             (void)  { BSP_IntHandler(BSP_INT_ID_TWI0);                      }
void  BSP_IntHandlerTWI1             (void)  { BSP_IntHandler(BSP_INT_ID_TWI1);                      }
void  BSP_IntHandlerSPI              (void)  { BSP_IntHandler(BSP_INT_ID_SPI);                       }
void  BSP_IntHandlerTWI2             (void)  { BSP_IntHandler(BSP_INT_ID_TWI2);                      }
void  BSP_IntHandlerTC0              (void)  { BSP_IntHandler(BSP_INT_ID_TC0);                       }
void  BSP_IntHandlerTC1              (void)  { BSP_IntHandler(BSP_INT_ID_TC1);                       }
void  BSP_IntHandlerTC2              (void)  { BSP_IntHandler(BSP_INT_ID_TC2);                       }
void  BSP_IntHandlerTC3              (void)  { BSP_IntHandler(BSP_INT_ID_TC3);                       }
void  BSP_IntHandlerTC4              (void)  { BSP_IntHandler(BSP_INT_ID_TC4);                       }
void  BSP_IntHandlerTC5              (void)  { BSP_IntHandler(BSP_INT_ID_TC5);                       }
void  BSP_IntHandlerADC              (void)  { BSP_IntHandler(BSP_INT_ID_ADC);                       }
void  BSP_IntHandlerDACC             (void)  { BSP_IntHandler(BSP_INT_ID_DACC);                      }
void  BSP_IntHandlerPWM              (void)  { BSP_IntHandler(BSP_INT_ID_PWM);                       }


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          BSP_IntHandler()
*
* Description : Central interrupt handler.
*
* Argument(s) : int_id          Interrupt that will be handled.
*
* Return(s)   : none.
*
* Caller(s)   : ISR handlers.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandler (CPU_DATA  int_id)
{
    CPU_FNCT_VOID  isr;

#ifdef  BSP_CFG_OS
    CPU_SR_ALLOC();


    CPU_CRITICAL_ENTER();                                       /* Tell the OS that we are starting an ISR            */

    OSIntEnter();

    CPU_CRITICAL_EXIT();
#endif

    if (int_id < BSP_INT_SRC_NBR) {
        isr = BSP_IntVectTbl[int_id];
        if (isr != (CPU_FNCT_VOID)0) {
            isr();
        }
    }

#ifdef  BSP_CFG_OS
    OSIntExit();                                                /* Tell the OS that we are leaving the ISR            */
#endif
}


/*
*********************************************************************************************************
*                                        BSP_IntHandlerDummy()
*
* Description : Dummy interrupt handler.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_IntHandler().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  BSP_IntHandlerDummy (void)
{

}
