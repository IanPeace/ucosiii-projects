/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               This BSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*                                       SERIAL (UART) INTERFACE
*
* Filename      : bsp_ser.c
* Version       : V1.00
* Programmer(s) : EHS
*                 SR
*                 AA
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_SER_MODULE
#include <bsp.h>
#include <bsp_os.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


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

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)

static  BSP_OS_SEM   BSP_SerTxWait;
static  BSP_OS_SEM   BSP_SerRxWait;
static  BSP_OS_SEM   BSP_SerLock;
static  CPU_INT08U   BSP_SerRxData;
static  CPU_INT08U   BSP_SerTxData;

#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
static  CPU_CHAR     BSP_SerCmdHistory[BSP_CFG_SER_CMD_HISTORY_LEN];
#endif

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        BSP_Ser_WrByteUnlocked  (CPU_INT08U  c);
static  CPU_INT08U  BSP_Ser_RdByteUnlocked  (void);
static  void        BSP_Ser_ISR_Handler     (void);



/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
**                                         GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          BSP_Ser_Init()
*
* Description : Initialize a serial port for communication.
*
* Argument(s) : baud_rate           The desire RS232 baud rate.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_Init (CPU_INT32U  baud_rate)
{
    CPU_INT32U      sbr;
    CPU_INT32U      osr;
    CPU_INT32U      baud_calc;
    CPU_INT32U      baud_diff;
    CPU_INT32U      reg_temp;
    CPU_INT08U      temp;
    UART0_MemMapPtr UART_base = UART0_BASE_PTR;

    BSP_OS_SemCreate(&BSP_SerTxWait,   0, "Serial Tx Wait");
    BSP_OS_SemCreate(&BSP_SerRxWait,   0, "Serial Rx Wait");
    BSP_OS_SemCreate(&BSP_SerLock,     1, "Serial Lock");

#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
    BSP_SerCmdHistory[0] = (CPU_CHAR)'\0';
#endif

    /*
     * Enable the pins for the selected UART
     * UART_TXD function on PTA1, UART_TXD function on PTA2
     */
    PORTA_PCR1 = PORT_PCR_MUX(0x2);
    PORTA_PCR2 = PORT_PCR_MUX(0x2);

    /* Select the PLLFLLCLK as UART0 clock source */
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); 

    /* Enable the clock to the selected UART */    
    if (UART_base == UART0_BASE_PTR) {
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
    } else {
        if (UART_base == (UART0_MemMapPtr)UART1_BASE_PTR) {
            SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
        } else {
            if (UART_base == (UART0_MemMapPtr)UART2_BASE_PTR) {
                SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
            } else {
                SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
            }
        }
    }

    /* Make sure that the transmitter and receiver are disabled while we 
     * change settings.
     */
    UART_C2_REG(UART_base) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );

    /* Configure the UART for 8-bit mode, no parity */
    UART_C1_REG(UART_base) = 0;

    /* Calculate baud settings */
    sbr = (BSP_CPU_ClkFreq() / (baud_rate * 4));
    baud_calc = (BSP_CPU_ClkFreq() / (4 * sbr));
    if (baud_calc > baud_rate)
        baud_diff = baud_calc - baud_rate;
    else
        baud_diff = baud_rate - baud_calc;

    osr = 4;

    /* Select the best OSR value */
    for (temp = 5; temp <= 32; temp++)
    {
        sbr = (BSP_CPU_ClkFreq() / (baud_rate * temp));
        baud_calc = (BSP_CPU_ClkFreq() / (temp * sbr));
        
        if (baud_calc > baud_rate)
            temp = baud_calc - baud_rate;
        else
            temp = baud_rate - baud_calc;
        
        if (temp <= baud_diff)
        {
            baud_diff = temp;
            osr = temp; 
        }
    }

    /* If the OSR is between 4x and 8x then both edge sampling MUST be turned on */
    if ((osr >3) && (osr < 9))
        UART0_C5 |= UART0_C5_BOTHEDGE_MASK;
    
    /* Setup OSR value */
    reg_temp = UART0_C4;
    reg_temp &= ~UART0_C4_OSR_MASK;
    reg_temp |= UART0_C4_OSR(osr - 1);
    
    /* Write reg_temp to C4 register */
    UART0_C4 = reg_temp;
    
    reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
    sbr = (CPU_INT32U)(BSP_CPU_ClkFreq() / (baud_rate * (reg_temp)));
    
    /* Save off the current value of the uartx_BDH except for the SBR field */
    reg_temp = UART0_BDH & ~(UART0_BDH_SBR(0x1F));
    
    UART0_BDH = reg_temp | UART0_BDH_SBR(((sbr & 0x1F00) >> 8));
    UART0_BDL = (CPU_INT08U)(sbr & UART0_BDL_SBR_MASK);
    
    /* disable receiver and transmitter */
    UART_C2_REG(UART_base) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );

    /* Enable TIE and RIE
     * Transmit Interrupt Enable for TDRE and 
     * Receiver Interrupt Enable for RDRF
     */
    //UART_C2_REG(UART_base) |= UART_C2_RIE_MASK;

    BSP_IntVectSet(BSP_INT_ID_UART0_RX_TX, BSP_Ser_ISR_Handler);
    BSP_IntEn(BSP_INT_ID_UART0_RX_TX);

    /* Enable receiver and transmitter */
    UART_C2_REG(UART_base) |= (UART_C2_TE_MASK | UART_C2_RE_MASK );
}


/*
*********************************************************************************************************
*                                         BSP_Ser_ISR_Handler()
*
* Description : Serial ISR.
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

void  BSP_Ser_ISR_Handler (void)
{
    CPU_INT08U  status;
    UART0_MemMapPtr  UART_base = UART0_BASE_PTR;

    status = UART_S1_REG(UART_base);
    if (status & UART_S1_RDRF_MASK) {
        BSP_SerRxData = UART_D_REG(UART_base);                  /* Read one byte from the receive data register.      */
        BSP_OS_SemPost(&BSP_SerRxWait);                         /* Post to the sempahore                              */
    }

    if ((UART_C2_REG(UART_base) & UART_C2_TIE_MASK) &&
        (status & UART_S1_TDRE_MASK)) {
        UART_D_REG(UART_base) = BSP_SerTxData;
        UART_C2_REG(UART_base) &= ~UART_C2_TIE_MASK;
        BSP_OS_SemPost(&BSP_SerTxWait);                         /* Post to the semaphore                              */
    }
}


/*
*********************************************************************************************************
*                                           BSP_Ser_Printf()
*
* Description : Print formatted data to the output serial port.
*
* Argument(s) : format      String that contains the text to be written.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function output a maximum of BSP_SER_PRINTF_STR_BUF_SIZE number of bytes to the
*                   serial port.  The calling function hence has to make sure the formatted string will
*                   be able fit into this string buffer or hence the output string will be truncated.
*********************************************************************************************************
*/

void  BSP_Ser_Printf (CPU_CHAR  *format, ...)
{
    CPU_CHAR  buf_str[BSP_SER_PRINTF_STR_BUF_SIZE + 1u];
    va_list   v_args;


    va_start(v_args, format);
    (void)vsnprintf((char       *)&buf_str[0],
                    (size_t      )sizeof(buf_str),
                    (char const *)format,
                    v_args);
    va_end(v_args);

    BSP_Ser_WrStr(buf_str);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_RdByte()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This functions blocks until a data is received.
*
*               (2) It can not be called from an ISR.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByte (void)
{
    CPU_INT08U  rx_byte;

    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface.             */

    rx_byte = BSP_Ser_RdByteUnlocked();

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface.            */

    return (rx_byte);
}


/*
*********************************************************************************************************
*                                       BSP_Ser_RdByteUnlocked()
*
* Description : Receive a single byte.
*
* Argument(s) : none.
*
* Return(s)   : The received byte.
*
* Caller(s)   : BSP_Ser_RdByte()
*               BSP_Ser_RdStr()
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  BSP_Ser_RdByteUnlocked (void)
{
    CPU_INT08U   rx_byte;
    UART0_MemMapPtr  UART_base = UART0_BASE_PTR;

    UART_C2_REG(UART_base) |= UART_C2_RIE_MASK;                 /* Enable the Receive not empty interrupt             */

    BSP_OS_SemWait(&BSP_SerRxWait, 0);                          /* Wait until data is received                        */

    UART_C2_REG(UART_base) &= ~UART_C2_RIE_MASK;                /* Disable the Receive not empty interrupt            */

    rx_byte = BSP_SerRxData;                                    /* Read the data from the temporary register          */

    return (rx_byte);
}

/*
*********************************************************************************************************
*                                                BSP_Ser_RdStr()
*
* Description : This function reads a string from a UART.
*
* Argument(s) : p_str       A pointer to a buffer at which the string can be stored.
*
*               len         The size of the string that will be read.
*
* Return(s)   : none.
*
* Caller(s)   : Application
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_RdStr (CPU_CHAR    *p_str,
                     CPU_INT16U   len)
{
    CPU_CHAR     *p_char;
    CPU_BOOLEAN   rxd_history_char0;
    CPU_CHAR      rx_data;
    CPU_BOOLEAN   err;


    rxd_history_char0 = DEF_NO;
    p_str[0]          = (CPU_CHAR)'\0';
    p_char            = p_str;

    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface                */

    if (err != DEF_OK ) {
        return;
    }

    while (DEF_TRUE)
    {
        rx_data = BSP_Ser_RdByteUnlocked();

        if ((rx_data == ASCII_CHAR_CARRIAGE_RETURN) ||          /* Is it '\r' or '\n' character  ?                      */
            (rx_data == ASCII_CHAR_LINE_FEED      )) {

            BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_LINE_FEED);
            BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_CARRIAGE_RETURN);
           *p_char = (CPU_CHAR)'\0';                            /* set the null character at the end of the string      */
#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
            Str_Copy(BSP_SerCmdHistory, p_str);
#endif
            break;                                              /* exit the loop                                        */
        }

        if (rx_data == ASCII_CHAR_BACKSPACE) {                  /* Is backspace character                               */
            if (p_char > p_str) {
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                p_char--;                                       /* Decrement the index                                  */
            }
        }

        if ((ASCII_IsPrint(rx_data)      ) &&
            (rxd_history_char0 == DEF_NO)) {                    /* Is it a printable character ... ?                    */
            BSP_Ser_WrByteUnlocked((CPU_INT08U)rx_data);        /* Echo-back                                            */
           *p_char = rx_data;                                   /* Save the received character in the buffer            */
            p_char++;                                           /* Increment the buffer index                           */
            if (p_char >= &p_str[len]) {
                p_char  = &p_str[len];
            }

        } else if ((rx_data           == ASCII_CHAR_ESCAPE) &&
                   (rxd_history_char0 == DEF_NO           )) {
            rxd_history_char0 = DEF_YES;

#if (BSP_CFG_SER_CMD_HISTORY_LEN > 0u)
        } else if ((rx_data           == ASCII_CHAR_LEFT_SQUARE_BRACKET) &&
                   (rxd_history_char0 == DEF_YES                       )) {

            while (p_char != p_str) {
                BSP_Ser_WrByteUnlocked((CPU_INT08U)ASCII_CHAR_BACKSPACE);
                p_char--;                                       /* Decrement the index                                  */
            }

            Str_Copy(p_str, BSP_SerCmdHistory);

            while (*p_char != '\0') {
                BSP_Ser_WrByteUnlocked(*p_char++);
            }
#endif
        } else {
            rxd_history_char0 = DEF_NO;
        }
    }

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface               */
}


/*
*********************************************************************************************************
*                                          BSP_Ser_WrByteUnlocked()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : c           The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Ser_WrByte()
*               BSP_Ser_WrByteUnlocked()
*
* Note(s)     : (1) This function blocks until room is available in the UART for the byte to be sent.
*********************************************************************************************************
*/

void  BSP_Ser_WrByteUnlocked (CPU_INT08U c)
{
    UART0_MemMapPtr  UART_base = UART0_BASE_PTR;

    BSP_SerTxData = c;
    UART_C2_REG(UART_base) |= UART_C2_TIE_MASK;                 /* Enable the Transmit empty interrupt                  */
    BSP_OS_SemWait(&BSP_SerTxWait, 0);
}


/*
*********************************************************************************************************
*                                                BSP_Ser_WrByte()
*
* Description : Writes a single byte to a serial port.
*
* Argument(s) : tx_byte     The character to output.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrByte(CPU_INT08U  c)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    BSP_Ser_WrByteUnlocked(c);

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}


/*
*********************************************************************************************************
*                                                BSP_Ser_WrStr()
*
* Description : Transmits a string.
*
* Argument(s) : p_str       Pointer to the string that will be transmitted.
*
* Caller(s)   : Application.
*
* Return(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_Ser_WrStr (CPU_CHAR  *p_str)
{
    CPU_BOOLEAN  err;


    if (p_str == (CPU_CHAR *)0) {
        return;
    }


    err = BSP_OS_SemWait(&BSP_SerLock, 0);                      /* Obtain access to the serial interface              */
    if (err != DEF_OK ) {
        return;
    }

    while ((*p_str) != (CPU_CHAR )0) {
        if (*p_str == ASCII_CHAR_LINE_FEED) {
            BSP_Ser_WrByteUnlocked(ASCII_CHAR_CARRIAGE_RETURN);
            BSP_Ser_WrByteUnlocked(ASCII_CHAR_LINE_FEED);
            p_str++;
        } else {
            BSP_Ser_WrByteUnlocked(*p_str++);
        }
    }

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}


#if (APP_CFG_SHELL_EN == DEF_ENABLED)

/* For Shell usage */

#define CONSOLE_BUF_SIZE                        128             /* Must be 2^x  */

static char CBuf[CONSOLE_BUF_SIZE];
static CPU_INT08U CBuf_Top = 0;
static CPU_INT08U CBuf_Cnt = 0;


static void  BSP_Console_ISR_Handler (void)
{
    CPU_INT08U  status;
    UART0_MemMapPtr  UART_base = UART0_BASE_PTR;

    status = UART_S1_REG(UART_base);

    if (status & UART_S1_RDRF_MASK) {
        CBuf[(CBuf_Top - CBuf_Cnt + CONSOLE_BUF_SIZE) & (CONSOLE_BUF_SIZE - 1)] =
            UART_D_REG(UART_base);                              /* Read one byte from the receive data register.      */

        if (CBuf_Cnt != CONSOLE_BUF_SIZE) {
           CBuf_Cnt++;
        } else {
           CBuf_Top = (CBuf_Top + (CONSOLE_BUF_SIZE - 1)) & (CONSOLE_BUF_SIZE - 1);
        }

        BSP_OS_SemPost(&BSP_SerRxWait);                         /* Post to the sempahore                              */
    }

    if ((UART_C2_REG(UART_base) & UART_C2_TIE_MASK) &&
        (status & UART_S1_TDRE_MASK)) {
        UART_D_REG(UART_base) = BSP_SerTxData;
        UART_C2_REG(UART_base) &= ~UART_C2_TIE_MASK;
        BSP_OS_SemPost(&BSP_SerTxWait);                         /* Post to the semaphore                              */
    }
}

void  BSP_Console_Enable (void)
{
    UART0_MemMapPtr  UART_base = UART0_BASE_PTR;

    BSP_IntVectSet(BSP_INT_ID_UART0_RX_TX, BSP_Console_ISR_Handler);
    UART_C2_REG(UART_base) |= UART_C2_RIE_MASK;                 /* Enable the Receive not empty interrupt             */
    return;
}

CPU_INT08U  BSP_Console_GetChar (void)
{
    CPU_INT08U      rx_byte;
    UART0_MemMapPtr  UART_base = UART0_BASE_PTR;

    BSP_OS_SemWait(&BSP_SerRxWait, 0);                          /* Wait until data is received                        */

    UART_C2_REG(UART_base) &= ~UART_C2_RIE_MASK;                /* Disable the Receive not empty interrupt            */

    rx_byte = CBuf[CBuf_Top];
    CBuf_Top = (CBuf_Top - 1 + CONSOLE_BUF_SIZE) & (CONSOLE_BUF_SIZE - 1);
    CBuf_Cnt--;

    UART_C2_REG(UART_base) |= UART_C2_RIE_MASK;                 /* Enable the Receive not empty interrupt             */

    return (rx_byte);
}

void  BSP_Console_PutChar (char c)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    BSP_Ser_WrByteUnlocked((CPU_INT08U)c);

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */
}
#endif  /* (APP_CFG_SHELL_EN == DEF_ENABLED) */


#if (APP_CFG_PRINTF_EN == DEF_ENABLED)

/* For printf usage */

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

int _sys_exit (int x)
{
    x = x;
    return 0;
}

int  fputc (int ch, FILE *f)
{
    BSP_OS_SemWait(&BSP_SerLock, 0);                            /* Obtain access to the serial interface              */

    BSP_Ser_WrByteUnlocked((CPU_INT08U)ch);

    BSP_OS_SemPost(&BSP_SerLock);                               /* Release access to the serial interface             */

    return ch;
}
#endif  /* (APP_CFG_PRINTF_EN == DEF_ENABLED) */

#endif  /* (APP_CFG_SERIAL_EN == DEF_ENABLED) */

