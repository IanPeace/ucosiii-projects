ATSAM4N-XPro
============

This BSP is ported from the Micrium official BSP for [FRDM-KL25Z](https://www.micrium.com/download/micrium_frdm-kl25z/).

Please reference the [SAM4N Xplained Pro Starter Kit website](http://www.microchip.com/DevelopmentTools/ProductDetails/ATSAM4N-XSTK) for more information about this board.


Getting Started
---------------

* Preperation

  - Install [MDK-Arm](https://www.keil.com/download/product/) and the [packs](https://www.keil.com/dd2/Pack/) for **Microchip SAM4** device familly.
  - Download the necessary [SOFTWARE & TOOLS](http://www.microchip.com/developmenttools/ProductDetails/PartNo/ATSAM4N-XPRO) in the Microchip website for **BASIC Usage Guide** of the board.
  - Follow the instructions in the [User Guide](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42104-SAM4N-Xplained-Pro_User-Guide.pdf) to get the board boot up.

* Build and run the image

  - Open the MDK project in the folder `Examples\Atmel\ATSAM4N-XPro\OS3\KeilMDK`, build the image.
  - The build out image is **ATSAM4N-XPro.axf** in the folder `Examples\Atmel\ATSAM4N-XPro\OS3\KeilMDK\Objects`.
  - Plug the board into the computer and wait until all the USB device drivers have been installed.
  - Select **CMSIS-DAP Debugger** in the MDK by click `Options for Target 'ATSAM4N-XPro' -> Debug` tag.
  - Download the image by click `Download` button in the MDK.

* Demo instruction

  - The demo creates a `Key Task` to detect the *LONG* or *SHORT* press of the buttons in the `OLED1 Xplained Pro Extension Kit`, and post the flags to the `App Task`, which do the action procedure of the button presses.

    | Events    | Actions   |
    | :--       | :--       |
    | Button 1 Short Press  | Fill the OLED screen with `hello uGUI` and draw a circle, and flash LED1 every second |
    | Button 1 Long Press   | Stop flashing the LED1                                                                |
    | Button 2 Short Press  | Fill the OLED screen with 4 meshes, and flash LED2 every 1.5 seconds                  |
    | Button 2 Long Press   | Stop flashing the LED2                                                                |
    | Button 3 Short Press  | Clear the OLED screen, and flash LED3 every 2 seconds                                 |
    | Button 3 Long Press   | Stop flashing the LED2                                                                |

  - Also a **UART driver** and the **uC/Shell** feature is added.
  - If the **APP_CFG_GUI_EN** configuration is set to `DEF_DISABLED`, the **STD OUT** will also be mirrored to the **OLED screen**.

Reference
---------

- For more information about **uGUI**, please reference the official website [uGUI](http://embeddedlightning.com/ugui/)
- For the Extension Kits in the package, please refer to the following websites
  - [OLED1 Xplained Pro Extension Kit](http://www.microchip.com/developmenttools/ProductDetails/ATOLED1-XPRO)
  - [I/O1 Xplained Pro Extension Kit](http://www.microchip.com/developmenttools/ProductDetails/ATIO1-XPRO)
  - [PROTO1 Xplained Pro Extension Kit](http://www.microchip.com/developmenttools/ProductDetails/ATPROTO1-XPRO)