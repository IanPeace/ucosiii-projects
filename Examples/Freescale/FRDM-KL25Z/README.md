FRDM-KL25Z
==========

This BSP is modified from the Micrium official BSP for [FRDM-KL25Z](https://www.micrium.com/download/micrium_frdm-kl25z/).

Please reference the [FRDM-KL25Z website](https://www.nxp.com/support/developer-resources/evaluation-and-development-boards/freedom-development-boards/mcu-boards/freedom-development-platform-for-kinetis-kl14-kl15-kl24-kl25-mcus:FRDM-KL25Z?) for more information about this board.


Getting Started
---------------

* Preperation

  - Install [MDK-Arm](https://www.keil.com/download/product/) and the [packs](https://www.keil.com/dd2/Pack/) for **Kinetis_KLxx** device familly.
  - Download the necessary [SOFTWARE & TOOLS](https://www.nxp.com/support/developer-resources/evaluation-and-development-boards/freedom-development-boards/mcu-boards/freedom-development-platform-for-kinetis-kl14-kl15-kl24-kl25-mcus:FRDM-KL25Z?tab=Design_Tools_Tab) in the NXP website for **BASIC Usage Guide** of the board.
  - Follow the instructions in the [User Guide](https://www.nxp.com/docs/en/user-guide/FRDMKL25ZUM.zip) to get the board boot up.
  - Download and update the latest **OpenSDA** firmware and driver in [PEmicro](http://www.pemicro.com/opensda/).

* Build and run the image

  - Open the MDK project in the folder `Examples\Freescale\FRDM-KL25Z\OS3\KeilMDK`, build the image.
  - The build out image is **outfile.s19** in the folder `Examples\Freescale\FRDM-KL25Z\OS3\KeilMDK\Objects`.
  - Plug the board into the computer, **Copy & Paste** the `outfile.s19` file to the MSD device **FRDM-KL25Z**.

* Demo instruction

  - The **RGB LED** task flash the R, G, B LEDs alternately. And the brilliance is controlled from *Dark* to *Bright* and inversely like the **PWM** control.
  - Also a **UART driver** and the **uC/Shell** feature is added.

Reference
---------

- The original flash the RGB LED so fast that it could not be recognized by eyes. Therefore the **RGB LED** task is modified. 