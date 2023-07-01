/**
  ******************************************************************************
  * @file    usbd_composite.h
  * @author  MCD Application Team
  * @brief   Header file for the usbd_composite.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_COMPOSITE_H
#define __USB_COMPOSITE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdbool.h"
#include "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_COMPOSITE
  * @brief This file is the header file for usbd_composite.c
  * @{
  */


/** @defgroup USBD_COMPOSITE_Exported_Defines
  * @{
  */

#include "usbd_hid.h"
#include "usbd_audio_if.h"

/**
  * @}
  */


/** @defgroup USBD_COMPOSITE_Exported_TypesDefinitions
  * @{
  */

/**
  * @}
  */



/** @defgroup USBD_COMPOSITE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_COMPOSITE;
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
void USBD_COMPOSITE_Mount_Class(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_COMPOSITE_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
