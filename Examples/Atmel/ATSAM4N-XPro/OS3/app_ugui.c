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

#include <app_ugui.h>

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

#if (APP_CFG_GUI_EN == DEF_ENABLED)

#define MAX_OBJECTS             10

UG_WINDOW window_1;
UG_TEXTBOX textbox_1;

UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];

void window_1_callback(UG_MESSAGE *msg)
{
    return;
}

CPU_INT32U ugWindowCreate(void)
{
    UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback) ;
    UG_WindowSetTitleTextFont(&window_1, &FONT_12X20);
}

CPU_INT32U ugWindowTitle(char *title)
{
    UG_WindowSetTitleText(&window_1, title);
}

CPU_INT32U ugWindowShow(void)
{
    UG_WindowShow(&window_1);
}


CPU_INT32U ugTextboxCreate(UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye)
{
    UG_TextboxCreate(&window_1, &textbox_1, TXB_ID_0, xs, ys, xe, ye);
    UG_TextboxSetFont(&window_1, TXB_ID_0, &FONT_12X16);
}

CPU_INT32U ugTextboxText(char *text)
{
    UG_TextboxSetText(&window_1, TXB_ID_0, text);
}

CPU_INT32U ugTextboxColor(UG_COLOR fc)
{
    UG_TextboxSetForeColor(&window_1, TXB_ID_0, fc);
}

CPU_INT32U ugTextboxAlign(UG_U8 align)
{
    UG_TextboxSetAlignment(&window_1, TXB_ID_0, align);
}

#endif  /* (APP_CFG_SHELL_EN == DEF_ENABLED) */

