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

static  void  BSP_PIO_Init   (void);

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

void  BSP_Init (void)
{
    BSP_IntInit();                                              /* Initialize Interrupts.                               */

#ifdef  BSP_CFG_OS
                                                                /* Init System tick times.                              */
                                                                /* Clear pending interrupt.                             */

    CPU_REG_NVIC_ST_RELOAD  = (2000000UL);                      /* Set period.                                          */
    CPU_REG_NVIC_ST_CTRL   |=  CPU_REG_NVIC_ST_CTRL_CLKSOURCE;  /* Clock source - System Clock.                         */
    CPU_REG_NVIC_ST_CTRL   |=  CPU_REG_NVIC_ST_CTRL_TICKINT;    /* Enable interrupt.                                    */
    CPU_REG_NVIC_ST_CTRL   |=  CPU_REG_NVIC_ST_CTRL_ENABLE;     /* Start Sys Timer.                                     */
#endif

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq() / (CPU_INT32U)1000000;

    BSP_CPU_ClkFreq_MHz = BSP_CPU_ClkFreq_MHz;                  /* Surpress compiler warning BSP_CPU_ClkFreq_MHz.       */

    BSP_PIO_Init();                                             /* Initialize clock for peripherals IO.                 */

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    BSP_Ser_Init(115200);
#endif

    ssd1306_init();

#if 0
    BSP_Timer_Init();                                           /* Initialize the Low-Power timer.                      */
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
    return BOARD_MCK;                                           /* System Clock Frequency (Core Clock) Max 100MHz   */
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
*                                             BSP_PIO_Init()
*
* Description : Initialize the clock for the peripherals IO.
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

static  void  BSP_PIO_Init (void)
{
    pmc_set_writeprotect(0);
    pmc_enable_periph_clk(ID_PIOA);                             /* Enable the clock for PIOA                            */
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
    pmc_enable_periph_clk(ID_UART0);                            /* Configure UART and PIOA clock                        */
#endif
    pmc_enable_periph_clk(ID_PIOB);                             /* Enable the clock for PIOB                            */
    pmc_enable_periph_clk(ID_PIOC);                             /* Enable the clock for PIOC                            */
    pmc_enable_periph_clk(ID_PIOC);                             /* Enable the clock for PIOC                            */
    pmc_enable_periph_clk(ID_SPI);                              /* Configure SPI clock                                  */
    pmc_set_writeprotect(1);

                                                                /* Initialize LED0, turned off                          */
    ioport_enable_pin(LED_0_PIN);
    ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);
    ioport_set_pin_dir(LED_0_PIN, IOPORT_DIR_OUTPUT);

                                                                /* Initialize SW0                                       */
    ioport_enable_pin(SW0_PIN);
    ioport_set_pin_mode(SW0_PIN, IOPORT_MODE_PULLUP);
    ioport_set_pin_dir(SW0_PIN, IOPORT_DIR_INPUT);

                                                                /* Initialize EXT2 LED                                  */
    ioport_enable_pin(EXT2_PIN_7);
    ioport_set_pin_level(EXT2_PIN_7, IOPORT_PIN_LEVEL_HIGH);
    ioport_set_pin_dir(EXT2_PIN_7, IOPORT_DIR_OUTPUT);

                                                                /* Initialize EXT3 BUTTON1 & BUTTON2 & BUTTON3          */
    ioport_enable_pin(OLED1_GPIO_PUSH_BUTTON_1);
    ioport_set_pin_mode(OLED1_GPIO_PUSH_BUTTON_1, IOPORT_MODE_PULLUP);
    ioport_set_pin_dir(OLED1_GPIO_PUSH_BUTTON_1, IOPORT_DIR_INPUT);
    ioport_enable_pin(OLED1_GPIO_PUSH_BUTTON_2);
    ioport_set_pin_mode(OLED1_GPIO_PUSH_BUTTON_2, IOPORT_MODE_PULLUP);
    ioport_set_pin_dir(OLED1_GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT);
    ioport_enable_pin(OLED1_GPIO_PUSH_BUTTON_3);
    ioport_set_pin_mode(OLED1_GPIO_PUSH_BUTTON_2, IOPORT_MODE_PULLUP);
    ioport_set_pin_dir(OLED1_GPIO_PUSH_BUTTON_2, IOPORT_DIR_INPUT);

                                                                /* Initialize EXT3 LED0, LED1 & LED2, turned off        */
    ioport_enable_pin(OLED1_LED1_PIN);
    ioport_set_pin_level(OLED1_LED1_PIN, !OLED1_LED1_ACTIVE);
    ioport_set_pin_dir(OLED1_LED1_PIN, IOPORT_DIR_OUTPUT);
    ioport_enable_pin(OLED1_LED2_PIN);
    ioport_set_pin_level(OLED1_LED2_PIN, !OLED1_LED2_ACTIVE);
    ioport_set_pin_dir(OLED1_LED2_PIN, IOPORT_DIR_OUTPUT);
    ioport_enable_pin(OLED1_LED3_PIN);
    ioport_set_pin_level(OLED1_LED3_PIN, !OLED1_LED3_ACTIVE);
    ioport_set_pin_dir(OLED1_LED3_PIN, IOPORT_DIR_OUTPUT);

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
                                                                /* Configure UART pins                                  */
    ioport_set_port_mode(PINS_UART0_PORT, PINS_UART0, PINS_UART0_FLAGS);
    ioport_disable_port(PINS_UART0_PORT, PINS_UART0);           /* Disable pin mode, but enable peripheral              */
    pio_disable_interrupt(PINS_UART0_PORT, PINS_UART0);
#endif

    ioport_set_pin_mode(SPI_MISO_GPIO, SPI_MISO_FLAGS);
    ioport_disable_pin(SPI_MISO_GPIO);                          /* Disable pin mode, but enable peripheral              */
    ioport_set_pin_mode(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
    ioport_disable_pin(SPI_MOSI_GPIO);
    ioport_set_pin_mode(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
    ioport_disable_pin(SPI_SPCK_GPIO);
    ioport_set_pin_mode(SPI_NPCS2_GPIO, SPI_NPCS2_FLAGS);
    ioport_disable_pin(SPI_NPCS2_GPIO);

	ioport_set_pin_dir(UG_2832HSWEG04_DATA_CMD_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(UG_2832HSWEG04_DATA_CMD_GPIO, IOPORT_MODE_PULLUP);
	ioport_set_pin_dir(UG_2832HSWEG04_RESET_GPIO, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(UG_2832HSWEG04_RESET_GPIO, IOPORT_MODE_PULLUP);

    return;
}

/*
*********************************************************************************************************
*                                             BSP_LED_Set()
*
* Description : Toggle the LED.
*
* Argument(s) : led_id      LED ID
*               led_on      BSP_LED_ON      set the LED on
*                           BSP_LED_OFF     set the LED off
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Set (CPU_INT32U led_id, CPU_BOOLEAN led_on)
{
    CPU_INT32U          led_pin;

    switch (led_id) {
        case BSP_LED1:
            led_pin = OLED1_LED1_PIN;
            break;
        case BSP_LED2:
            led_pin = OLED1_LED2_PIN;
            break;
        case BSP_LED3:
            led_pin = OLED1_LED3_PIN;
            break;
        case BSP_LED4:
            led_pin = EXT2_PIN_7;
            break;
        case BSP_LED0:
        default:
            led_pin = LED_0_PIN;
            break;
    }

    if (led_on) {
        ioport_set_pin_level(led_pin, IOPORT_PIN_LEVEL_LOW);
    } else {
        ioport_set_pin_level(led_pin, IOPORT_PIN_LEVEL_HIGH);
    }

    return;
}

/*
*********************************************************************************************************
*                                             BSP_LED_Toggle()
*
* Description : Toggle the LED.
*
* Argument(s) : LED ID
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT32U led_id)
{
    CPU_INT32U          led_pin;

    switch (led_id) {
        case BSP_LED1:
            led_pin = OLED1_LED1_PIN;
            break;
        case BSP_LED2:
            led_pin = OLED1_LED2_PIN;
            break;
        case BSP_LED3:
            led_pin = OLED1_LED3_PIN;
            break;
        case BSP_LED4:
            led_pin = EXT2_PIN_7;
            break;
        case BSP_LED0:
        default:
            led_pin = LED_0_PIN;
            break;
    }

    ioport_toggle_pin_level(led_pin);

    return;
}


/*
*********************************************************************************************************
*                                             BSP_Key_Status()
*
* Description : Get the status of the key.
*
* Argument(s) : Key ID
*
* Return(s)   : Key status.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_Key_Status (CPU_INT32U key_id)
{
    CPU_INT32U          key_pin;

    switch (key_id) {
        case BSP_BUTTON1:
            key_pin = OLED1_GPIO_PUSH_BUTTON_1;
            break;
        case BSP_BUTTON2:
            key_pin = OLED1_GPIO_PUSH_BUTTON_2;
            break;
        case BSP_BUTTON3:
            key_pin = OLED1_GPIO_PUSH_BUTTON_3;
            break;
        case BSP_SW0:
        default:
            key_pin = SW0_PIN;
            break;
    }

    return ioport_get_pin_level(key_pin);
}



#if 0
/*
*********************************************************************************************************
*********************************************************************************************************
*                                            TIMER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_Timer_Init()
*
* Description : Initialize the Low-Power Timer (LPTMR0).
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

static  void  BSP_Timer_Init (void)
{
    SIM_SCGC5 |=  SIM_SCGC5_LPTMR_MASK;                         /* Enable the clock for LPTMR0.                         */

    BSP_REG_LPTMR0_CSR =  0x00000000u;                          /* Disable LPTMR0.                                      */

    BSP_REG_LPTMR0_PSR = (LPTMR_PSR_PRESCALE(0x1u) |            /* Select (MCGIRCLK / 4) as the timer clock.            */
                          LPTMR_PSR_PCS(0x0u));

    BSP_REG_LPTMR0_CSR =  0x00000005u;                          /* Enable LPTMR0 in Free Running Counter mode.          */
}


/*
*********************************************************************************************************
*                                            BSP_Timer_Dly()
*
* Description : Delay CPU for the specified time.
*
* Argument(s) : ms          number of milliseconds to delay
*
*               us          number of microseconds to delay
*
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Timer_Dly (CPU_INT16U  ms,
                     CPU_INT16U  us)
{
    CPU_INT32U  ticks;

    ticks = (ms * 1000u) + us;

    while (ticks > LPTMR_CNR_COUNTER_MASK) {
        BSP_REG_LPTMR0_CNR  =  0x00000000u;                     /* Register current timer value into the temporary reg. */
        BSP_REG_LPTMR0_CMR  =  BSP_REG_LPTMR0_CNR + LPTMR_CNR_COUNTER_MASK;
        BSP_REG_LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;

        while (DEF_BIT_IS_CLR(BSP_REG_LPTMR0_CSR, LPTMR_CSR_TCF_MASK)) {
            ;
        }

        ticks -=  LPTMR_CNR_COUNTER_MASK;
    }

    if (ticks > 0) {
        BSP_REG_LPTMR0_CNR  =  0x00000000u;
        BSP_REG_LPTMR0_CMR  =  BSP_REG_LPTMR0_CNR + ticks;
        BSP_REG_LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;

        while (DEF_BIT_IS_CLR(BSP_REG_LPTMR0_CSR, LPTMR_CSR_TCF_MASK)) {
            ;
        }
    }
}
#endif

