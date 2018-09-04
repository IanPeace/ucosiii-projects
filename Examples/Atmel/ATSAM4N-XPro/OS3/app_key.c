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
* Filename      : app_key.c
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

#include <app_key.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define SW0_TRIG_TIME_L             100
#define SW0_TRIG_TIME_S             2
#define BUTTON1_TRIG_TIME_L         100
#define BUTTON1_TRIG_TIME_S         2
#define BUTTON2_TRIG_TIME_L         100
#define BUTTON2_TRIG_TIME_S         2
#define BUTTON3_TRIG_TIME_L         100
#define BUTTON3_TRIG_TIME_S         2

#define KEY_SCAN_INTERVAL_MS        10


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

extern OS_FLAG_GRP  APP_KeyFlagGrp;

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             AppKeyTask()
*
* Description : This task monitors the state of the push buttons and passes flags to AppTask
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

CPU_BOOLEAN  AppKeyTask (void *p_arg)
{
    CPU_INT32U  sw0_hold;
    CPU_INT32U  sw0;
    CPU_INT32U  button1_hold;
    CPU_INT32U  button1;
    CPU_INT32U  button2_hold;
    CPU_INT32U  button2;
    CPU_INT32U  button3_hold;
    CPU_INT32U  button3;
    OS_ERR      err;

    (void)p_arg;

    sw0_hold        = 0;
    button1_hold    = 0;
    button2_hold    = 0;
    button3_hold    = 0;

    while (DEF_TRUE) {
        sw0     = BSP_Key_Status(BSP_SW0);
        button1 = BSP_Key_Status(BSP_BUTTON1);
        button2 = BSP_Key_Status(BSP_BUTTON2);;
        button3 = BSP_Key_Status(BSP_BUTTON3);;

        /* SW0 */

        if (sw0 == 0) {           
            sw0_hold++;
            if (sw0_hold == SW0_TRIG_TIME_L) {
                OSFlagPost(&APP_KeyFlagGrp, SW0_PRES_LONG_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
        } else {
            if ((sw0_hold >= SW0_TRIG_TIME_S) &&
                (sw0_hold < SW0_TRIG_TIME_L)) {
                OSFlagPost(&APP_KeyFlagGrp, SW0_PRES_SHORT_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
            sw0_hold = 0;
        }

        /* BUTTON1 */

        if (button1 == 0) {           
            button1_hold++;
            if (button1_hold == BUTTON1_TRIG_TIME_L) {
                OSFlagPost(&APP_KeyFlagGrp, BUTTON1_PRES_LONG_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
        } else {
            if ((button1_hold >= BUTTON1_TRIG_TIME_S) &&
                (button1_hold < BUTTON1_TRIG_TIME_L)) {
                OSFlagPost(&APP_KeyFlagGrp, BUTTON1_PRES_SHORT_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
            button1_hold = 0;
        }

        /* BUTTON2 */

        if (button2 == 0) {           
            button2_hold++;
            if (button2_hold == BUTTON2_TRIG_TIME_L) {
                OSFlagPost(&APP_KeyFlagGrp, BUTTON2_PRES_LONG_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
        } else {
            if ((button2_hold >= BUTTON2_TRIG_TIME_S) &&
                (button2_hold < BUTTON2_TRIG_TIME_L)) {
                OSFlagPost(&APP_KeyFlagGrp, BUTTON2_PRES_SHORT_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
            button2_hold = 0;
        }

        /* BUTTON3 */

        if (button3 == 0) {           
            button3_hold++;
            if (button3_hold == BUTTON3_TRIG_TIME_L) {
                OSFlagPost(&APP_KeyFlagGrp, BUTTON3_PRES_LONG_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
        } else {
            if ((button3_hold >= BUTTON3_TRIG_TIME_S) &&
                (button3_hold < BUTTON3_TRIG_TIME_L)) {
                OSFlagPost(&APP_KeyFlagGrp, BUTTON3_PRES_SHORT_FLAG,
                           OS_OPT_POST_FLAG_SET, &err);
            }
            button3_hold = 0;
        }

        OSTimeDlyHMSM(0, 0, 0, KEY_SCAN_INTERVAL_MS,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}


