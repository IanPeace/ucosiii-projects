/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
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
*                                        BOARD SUPPORT PACKAGE
*
*                                        Freescale Kinetis K20
*                                               on the
*                                        Freescale FRDM-KE02Z
*                                          Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : JPB
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_MODULE
#include  <bsp.h>

#ifdef  BSP_CFG_OS
#include  <bsp_os.h>
#endif

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_CORE_DIV              (1u)
#define  BSP_BUS_DIV               (2u)
#define  BSP_FLEXBUS_DIV           (3u)
#define  BSP_FLASH_DIV             (6u)


#define WDOG_CNT                    *((volatile CPU_INT16U *)&WDOG_CNTH)
#define WDOG_TOVAL                  *((volatile CPU_INT16U *)&WDOG_TOVALH)
#define WDOG_WIN                    *((volatile CPU_INT16U *)&WDOG_WINH)

#define WDOG_UNLOCK()               WDOG_CNT = 0x20C5; WDOG_CNT = 0x28D9    /* NOTE: can not use 16-bit access */


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


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  CPU_INT32U      BSP_CPU_ClkFreq_MHz;

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  BSP_ICS_Init       (void);
static  void  BSP_RGB_LED_Init   (void);

/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  DWT_CR      *(CPU_REG32 *)0xE0001000                   /* Data Watchpoint and Trace (DWT) Control Register.    */
#define  DBGMCU_CR   *(CPU_REG32 *)0xE0042004


/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  DBGMCU_CR_TRACE_IOEN_MASK       0x10
#define  DBGMCU_CR_TRACE_MODE_ASYNC      0x00
#define  DBGMCU_CR_TRACE_MODE_SYNC_01    0x40
#define  DBGMCU_CR_TRACE_MODE_SYNC_02    0x80
#define  DBGMCU_CR_TRACE_MODE_SYNC_04    0xC0
#define  DBGMCU_CR_TRACE_MODE_MASK       0xC0


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if ((CPU_CFG_TS_TMR_EN          != DEF_ENABLED) && \
     (APP_CFG_PROBE_OS_PLUGIN_EN == DEF_ENABLED) && \
     (OS_PROBE_HOOKS_EN          == 1))
#error  "CPU_CFG_TS_EN                  illegally #define'd in 'cpu.h'"
#error  "                              [MUST be  DEF_ENABLED] when    "
#error  "                               using uC/Probe COM modules    "
#endif


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*
*               (2) CPU instruction / data tracing requires the use of the following pins :
*                   (a) (1) Aysynchronous     :  PB[3]
*                       (2) Synchronous 1-bit :  PE[3:2]
*                       (3) Synchronous 2-bit :  PE[4:2]
*                       (4) Synchronous 4-bit :  PE[6:2]
*
*                   (b) The application may wish to adjust the trace bus width depending on I/O
*                       requirements.
*********************************************************************************************************
*/

#define SPI0_PINREMAP
#define ICS_TRIM_VALUE          0x54

void  BSP_Init (void)
{
    BSP_IntInit();                                              /* Initialize Interrupts.                               */

                                                                /* If not trimmed, do trim first                        */
    ICS_C3 =  ICS_TRIM_VALUE;
    ICS_C4 = 1;
    while (!(ICS_S & ICS_S_LOCK_MASK)) {
        ;
    }
                                                                /* Enable SWD pin, RESET pin                            */
                                                                /* NOTE: please make sure other register bits are also  */
    SIM_SOPT = SIM_SOPT_SWDE_MASK | SIM_SOPT_RSTPE_MASK;        /* write-once and need add other bit mask here if need  */

#if defined(SPI0_PINREMAP)
    SIM_PINSEL |= SIM_PINSEL_SPI0PS_MASK;
#endif

#if defined(OUTPUT_BUSCLK)
    SIM_SOPT |= SIM_SOPT_CLKOE_MASK;                            /* output bus clock if enabled                          */
#endif

    BSP_ICS_Init();                                             /* Initialize the internal clock module                 */

    SIM_SCGC |= (SIM_SCGC_UART0_MASK |                          /* make sure clocks to peripheral modules are enabled   */
                 SIM_SCGC_UART1_MASK |
                 SIM_SCGC_UART2_MASK);

#ifdef  BSP_CFG_OS
                                                                /* Init System tick times.                              */
                                                                /* Clear pending interrupt.                             */

    CPU_REG_NVIC_ST_RELOAD  = (2000000UL);                      /* Set period.                                          */
    CPU_REG_NVIC_ST_CTRL   |=  SysTick_CSR_CLKSOURCE_MASK;      /* Clock source - System Clock.                         */
    CPU_REG_NVIC_ST_CTRL   |=  SysTick_CSR_TICKINT_MASK;        /* Enable interrupt.                                    */
    CPU_REG_NVIC_ST_CTRL   |=  SysTick_CSR_ENABLE_MASK;         /* Start Sys Timer.                                     */
#endif

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq() / (CPU_INT32U)1000000;

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq_MHz;                  /* Surpress compiler warning BSP_CPU_ClkFreq_MHz.       */

    BSP_RGB_LED_Init();                                         /* Initialize the I/Os for the RGB LED.                 */

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    BSP_Ser_Init(115200);
#endif

#ifdef TRACE_EN                                                 /* See project / compiler preprocessor options.         */
    DBGMCU_CR |=  DBGMCU_CR_TRACE_IOEN_MASK;                    /* Enable tracing (see Note #2).                        */
    DBGMCU_CR &= ~DBGMCU_CR_TRACE_MODE_MASK;                    /* Clr trace mode sel bits.                             */
    DBGMCU_CR |=  DBGMCU_CR_TRACE_MODE_SYNC_04;                 /* Cfg trace mode to synch 4-bit.                       */
#endif
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
  return ((CPU_INT32U)20000000UL);                              /* The board runs the CPU at 20MHz.                     */
}


/*
*********************************************************************************************************
*                                            BSP_Tick_Init()
*
* Description : Initialize all the peripherals that required OS Tick services (OS initialized)
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
#ifdef  BSP_CFG_OS
void BSP_Tick_Init (void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;


    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */

#if (OS_VERSION >= 30000u)
    cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;       /* Determine nbr SysTick increments.                    */
#else
    cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* Determine nbr SysTick increments.                    */
#endif

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
}
#endif


/*
*********************************************************************************************************
*                                         ICS INITIALIZATION
*
* Description : This function is called to initialize the ICS.
*
* Arguments   : none
*********************************************************************************************************
*/

#define EXT_CLK_CRYST   10000   /* in KHz, PT60 tower board includes 10MHz crystal */

static  void  BSP_ICS_Init (void)
{
    /* assume external crystal is 8Mhz or 4MHz */
    /* enable OSC with high gain, high range and select oscillator output as OSCOUT*/
    OSC_CR = OSC_CR_OSCEN_MASK
           | OSC_CR_OSCSTEN_MASK                                /* enable stop                                          */
#if defined(CRYST_HIGH_GAIN)
           | OSC_CR_HGO_MASK                                    /* Rs must be added and be large up to 200K             */
#endif
#if  (EXT_CLK_CRYST >= 4000)
           | OSC_CR_RANGE_MASK
#endif
           | OSC_CR_OSCOS_MASK;                                 /* for crystal only */

    /* wait for OSC to be initialized */
    while (!(OSC_CR & OSC_CR_OSCINIT_MASK)) {
        ;
    }

    /* divide down external clock frequency to be within 31.25K to 39.0625K */

#if (EXT_CLK_CRYST == 8000)|| (EXT_CLK_CRYST == 10000)
    /* 8MHz */
    ICS_C1 = ICS_C1 & ~(ICS_C1_RDIV_MASK) | ICS_C1_RDIV(3);     /* now the divided frequency is 8000/256 = 31.25K       */
#elif (EXT_CLK_CRYST == 4000)
    /* 4MHz */
    ICS_C1 = ICS_C1 & ~(ICS_C1_RDIV_MASK) | ICS_C1_RDIV(2);     /* now the divided frequency is 4000/128 = 31.25K       */    
#elif (EXT_CLK_CRYST == 16000)
    /* 16MHz */
    ICS_C1 = ICS_C1 & ~(ICS_C1_RDIV_MASK) | ICS_C1_RDIV(4);     /* now the divided frequency is 16000/512 = 31.25K      */
#elif (EXT_CLK_CRYST == 20000)
    /* 20MHz */
    ICS_C1 = ICS_C1 & ~(ICS_C1_RDIV_MASK) | ICS_C1_RDIV(4);     /* now the divided frequency is 20000/512 = 39.0625K    */
#elif (EXT_CLK_CRYST == 32)
    ICS_C1 = ICS_C1 & ~(ICS_C1_RDIV_MASK);
#else
    #error "Error: crystal value not supported!\n";
#endif

    /* change FLL reference clock to external clock */
    ICS_C1 =  ICS_C1 & ~ICS_C1_IREFS_MASK;

    /* wait for the reference clock to be changed to external */
    while (ICS_S & ICS_S_IREFST_MASK) {
        ;
    }

    /* wait for FLL to lock */
    while (!(ICS_S & ICS_S_LOCK_MASK)) {
        ;
    }

    /* now FLL output clock is 31.25K*512*2 = 32MHz */
    if (((ICS_C2 & ICS_C2_BDIV_MASK) >> 5) != 1) {
        ICS_C2 = (ICS_C2 & ~(ICS_C2_BDIV_MASK)) | ICS_C2_BDIV(1);
    }
    /* now system/bus clock is the target frequency */
    /* clear Loss of lock sticky bit */
    ICS_S |= ICS_S_LOLS_MASK;

    return;
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             RGB LED FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_RGB_LED_Init()
*
* Description : Initialize the I/O for the RGB LED.
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

#define LED0_INIT               GPIOB_PDDR |= (2 << 24)         /* RED Light */
#define LED0_TOGGLE             GPIOB_PTOR = (2 << 24)
#define LED0_ON                 GPIOB_PCOR = (2 << 24)
#define LED0_OFF                GPIOB_PSOR = (2 << 24)

#define LED1_INIT               GPIOB_PDDR |= (4 << 24)         /* Green */
#define LED1_TOGGLE             GPIOB_PTOR = (4 << 24)
#define LED1_ON                 GPIOB_PCOR = (4 << 24)
#define LED1_OFF                GPIOB_PSOR = (4 << 24)

#define LED2_INIT               GPIOB_PDDR |= (0x01 << 7)
#define LED2_TOGGLE             GPIOB_PTOR = (0x01 << 7)
#define LED2_ON                 GPIOB_PCOR = (0x01 << 7)
#define LED2_OFF                GPIOB_PSOR = (0x01 << 7)

#define RED_SET                 LED0_OFF
#define RED_CLEAR               LED0_ON
#define RED_TOGGLE              LED0_TOGGLE

#define GREEN_SET               LED1_OFF
#define GREEN_CLEAR             LED1_ON
#define GREEN_TOGGLE            LED1_TOGGLE

#define BLUE_SET                LED2_OFF
#define BLUE_CLEAR              LED2_ON
#define BLUE_TOGGLE             LED2_TOGGLE


static  void  BSP_RGB_LED_Init (void)
{
    LED0_INIT;
    LED1_INIT;
    LED2_INIT;
    return;
}


/*
*********************************************************************************************************
*                                             BSP_RGB_LED_SetColor()
*
* Description : Sets the color by re-configuring the duty cycle of the PWM signals.
*
* Argument(s) : color  The color code in 4-byte ARGB additive color model enconding:
*
*                      | MSB      |          |          | LSB      |
*                      | Byte-3   | Byte-2   | Byte-1   | Byte-0   |
*                      | Alpha-Ch | Red-Ch   | Green-Ch | Blue-Ch  |
*
*                       Where each channel is a number between 0 and 255 that
*                       specifies the intensity of each color in the mixture.
*                       from fully-off (0) to fully-on (255).
*                       The alpha channel represents the opacity of the entire
*                       mixture of colors. If 0 then it is fully transparent and
*                       thus invisible whereas a value of 255 gives a fully opaque
*                       mixture of colors.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_RGB_LED_SetColor (CPU_INT32U color)
{
    CPU_INT08U  ch_red;
    CPU_INT08U  ch_green;
    CPU_INT08U  ch_blue;
    static CPU_INT32U  temp = 0;

                                                                /* Decode the RGB color code.                           */
    ch_red   = (temp ^ color) & 0x00000004;
    ch_green = (temp ^ color) & 0x00000002;
    ch_blue  = (temp ^ color) & 0x00000001;

    temp     = color;

    if (ch_red != 0) {
        RED_TOGGLE;
    }
    
    if (ch_green != 0) {
        GREEN_TOGGLE;
    }

    if (ch_blue != 0) {
        BLUE_TOGGLE;
    }

    return;
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            KE02-SC FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

void BSP_WDOG_Disable (void)
{
    WDOG_UNLOCK();                                              /* Unlock the watchdog so that we can write registers   */
                                                                /* Write all 6 registers once within 128 bus cycles     */
    WDOG_CS2 = 0;
    WDOG_TOVAL = 0xFFFF;
    WDOG_WIN = 0;
    WDOG_CS1 = 0x20;                                            /* WDOGA = 1 to allow reconfigure watchdog at any time  */

    return;
}

