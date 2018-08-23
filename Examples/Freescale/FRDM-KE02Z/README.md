FRDM-KE02Z
==========

This BSP is ported from the Micrium official BSP for [FRDM-KL25Z](https://www.micrium.com/download/micrium_frdm-kl25z/).

Please reference the [FRDM-KE02Z website](http://www.nxp.com/products/developer-resources/hardware-development-tools/freedom-development-boards/kinetis-e-series-freedom-development-platform:FRDM-KE02Z) for more information about this board.


Getting Started
---------------

* Preperation

  - Install [MDK-Arm](https://www.keil.com/download/product/) and the [packs](https://www.keil.com/dd2/Pack/) for **Kinetis_KExx** device familly.
  - Download the necessary [SOFTWARE & TOOLS](https://www.nxp.com/products/processors-and-microcontrollers/arm-based-processors-and-mcus/kinetis-cortex-m-mcus/e-series5v-robustm0-plus-m4/kinetis-e-series-freedom-development-platform:FRDM-KE02Z?tab=Design_Tools_Tab) in the NXP website for **BASIC Usage Guide** of the board.
  - Follow the instructions in the [User Guide](https://www.nxp.com/docs/en/user-guide/FRDM-KE02ZUM.pdf) to get the board boot up.
  - Download and update the latest **OpenSDA** firmware and driver in [PEmicro](http://www.pemicro.com/opensda/).

* Build and run the image

  - Open the MDK project in the folder `Examples\Freescale\FRDM-KE02Z\OS3\KeilMDK`, build the image.
  - The build out image is **outfile.s19** in the folder `Examples\Freescale\FRDM-KE02Z\OS3\KeilMDK\Objects`.
  - Plug the board into the computer, **Copy & Paste** the `outfile.s19` file to the MSD device **FRDM-KE02Z**.

* Demo instruction

  - The demo creates a simple task to flash the **RGB LED**.
  - Also a **UART driver** and the **uC/Shell** feature is added. Note that the uC/Shell would almost drained the SRAM, so it is **NOT** recommended to use uC/Shell in the application usage for this board. Although it is enabled default in this demo.

Reference
---------

- For the CPU frequence of KE02Z is at a low level，so the **OS_CFG_TICK_RATE_HZ** is changed from `1000` to `100`.
- For the SRAM size of KE02Z is only **4K Bytes**, so some OS features, e.g. `OS_CFG_APP_HOOKS_EN`, need to be tailored. The following features are also disabled
```
    OS_CFG_APP_HOOKS_EN
    OS_CFG_CALLED_FROM_ISR_CHK_EN
    OS_CFG_DBG_EN
    OS_CFG_MEM_EN
    OS_CFG_Q_EN
    OS_CFG_PEND_MULTI_EN
    OS_CFG_SCHED_LOCK_TIME_MEAS_EN
    OS_CFG_FLAG_MODE_CLR_EN
    OS_CFG_FLAG_PEND_ABORT_EN
    OS_CFG_MUTEX_PEND_ABORT_EN
    OS_CFG_SEM_PEND_ABORT_EN
    OS_CFG_SEM_SET_EN
    OS_CFG_TIME_DLY_RESUME_EN
    OS_CFG_TMR_EN
```
- The **OS_CFG_PRIO_MAX** is shrinked to 8.
- The **OS_CFG_IDLE_TASK_STK_SIZE** is minimized from 128 to 64.