/**
  ******************************************************************************
  * @file    usbd_composite.c
  * @author  MCD Application Team
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                COMPOSITE Class  Description
  *          ===================================================================
  *
  *
  *
  *
  *
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_composite.h"
#include "usbd_ctlreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_COMPOSITE
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_COMPOSITE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_Defines
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_FunctionPrototypes
  * @{
  */

static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMPOSITE_EP0_TxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMPOSITE_SOF(USBD_HandleTypeDef *pdev);
static uint8_t USBD_COMPOSITE_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_COMPOSITE_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t *USBD_COMPOSITE_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_COMPOSITE_GetDeviceQualifierDesc(uint16_t *length);

/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_COMPOSITE =
    {
        USBD_COMPOSITE_Init,
        USBD_COMPOSITE_DeInit,
        USBD_COMPOSITE_Setup,
        USBD_COMPOSITE_EP0_TxReady,
        USBD_COMPOSITE_EP0_RxReady,
        USBD_COMPOSITE_DataIn,
        USBD_COMPOSITE_DataOut,
        USBD_COMPOSITE_SOF,
		USBD_COMPOSITE_IsoINIncomplete,
        USBD_COMPOSITE_IsoOutIncomplete,
        USBD_COMPOSITE_GetFSCfgDesc,
        USBD_COMPOSITE_GetFSCfgDesc,
        USBD_COMPOSITE_GetFSCfgDesc,
        USBD_COMPOSITE_GetDeviceQualifierDesc
    };

typedef struct USBD_COMPOSITE_CFG_DESC_t
{
  uint8_t CONFIG_DESC[USB_CONF_DESC_SIZE];
  uint8_t USBD_HID_DESC[USB_HID_CONFIG_DESC_SIZE - 0x09];
  uint8_t USBD_AUDIO_DESC[USB_AUDIO_CONFIG_DESC_SIZE - 0x09];
} __PACKED USBD_COMPOSITE_CFG_DESC_t;

#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
__ALIGN_BEGIN USBD_COMPOSITE_CFG_DESC_t USBD_COMPOSITE_FSCfgDesc, USBD_COMPOSITE_HSCfgDesc __ALIGN_END;
uint8_t USBD_Track_String_Index = (USBD_IDX_INTERFACE_STR + 1);

#if defined(__ICCARM__) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_COMPOSITE_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
    {
        USB_LEN_DEV_QUALIFIER_DESC,
        USB_DESC_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0xEF,
        0x02,
        0x01,
        0x40,
        0x01,
        0x00,
};

/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_COMPOSITE_Init
  *         Initialize the COMPOSITE interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_HID.Init(pdev, cfgidx);
  USBD_AUDIO.Init(pdev, cfgidx);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_Init
  *         DeInitialize the COMPOSITE layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_HID.DeInit(pdev, cfgidx);
  USBD_AUDIO.DeInit(pdev, cfgidx);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_Setup
  *         Handle the COMPOSITE specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev,
                                    USBD_SetupReqTypedef *req)
{
  if (LOBYTE(req->wIndex) == HID_ITF_NBR)
  {
    return USBD_HID.Setup(pdev, req);
  }

  if (LOBYTE(req->wIndex) == AUDIO_AC_ITF_NBR || LOBYTE(req->wIndex) == AUDIO_AS_ITF_NBR)
  {
    return USBD_AUDIO.Setup(pdev, req);
  }

  return USBD_FAIL;
}

/**
  * @brief  USBD_COMPOSITE_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == (HID_IN_EP & 0x7F))
  {
    return USBD_HID.DataIn(pdev, epnum);
  }

  return USBD_FAIL;
}

/**
  * @brief  USBD_COMPOSITE_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_AUDIO.EP0_RxReady(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_COMPOSITE_EP0_TxReady(USBD_HandleTypeDef *pdev)
{
  USBD_AUDIO.EP0_TxSent(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_COMPOSITE_SOF(USBD_HandleTypeDef *pdev)
{
  USBD_AUDIO.SOF(pdev);

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_IsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == AUDIO_OUT_EP)
  {
    USBD_AUDIO.IsoOUTIncomplete(pdev, epnum);
  }

  return (uint8_t)USBD_OK;
}
/**
  * @brief  USBD_COMPOSITE_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  if (epnum == AUDIO_OUT_EP)
  {
    return USBD_AUDIO.DataOut(pdev, epnum);
  }

  return USBD_FAIL;
}

/**
  * @brief  USBD_COMPOSITE_GetFSCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_COMPOSITE_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_COMPOSITE_FSCfgDesc);
  return (uint8_t *)&USBD_COMPOSITE_FSCfgDesc;
}

/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_COMPOSITE_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_COMPOSITE_DeviceQualifierDesc);
  return USBD_COMPOSITE_DeviceQualifierDesc;
}

void USBD_COMPOSITE_Mount_Class(void)
{
  uint16_t len = 0;
  uint8_t *ptr = NULL;

  ptr = USBD_HID.GetFSConfigDescriptor(&len);
  USBD_Update_HID_DESC(ptr, 0x00, 0x81, 0);
  memcpy(USBD_COMPOSITE_FSCfgDesc.USBD_HID_DESC, ptr + 0x09, len - 0x09);

#if 1
  ptr = USBD_HID.GetHSConfigDescriptor(&len);
  USBD_Update_HID_DESC(ptr, 0x00, 0x81, 0);
  memcpy(USBD_COMPOSITE_HSCfgDesc.USBD_HID_DESC, ptr + 0x09, len - 0x09);
#endif

  ptr = USBD_AUDIO.GetFSConfigDescriptor(&len);
  USBD_Update_AUDIO_DESC(ptr,
                         0x01,
                         0x02,
						 0x01,
                         0);
  memcpy(USBD_COMPOSITE_FSCfgDesc.USBD_AUDIO_DESC, ptr + 0x09, len - 0x09);

#if 1
  ptr = USBD_AUDIO.GetHSConfigDescriptor(&len);
  USBD_Update_AUDIO_DESC(ptr,
                         0x01,
                         0x02,
						 0x01,
                         0);

  memcpy(USBD_COMPOSITE_HSCfgDesc.USBD_AUDIO_DESC, ptr + 0x09, len - 0x09);
#endif

  uint16_t CFG_SIZE = sizeof(USBD_COMPOSITE_CFG_DESC_t);
#if 1
  ptr = USBD_COMPOSITE_HSCfgDesc.CONFIG_DESC;
  /* Configuration Descriptor */
  ptr[0] = 0x09;                        /* bLength: Configuration Descriptor size */
  ptr[1] = USB_DESC_TYPE_CONFIGURATION; /* bDescriptorType: Configuration */
  ptr[2] = LOBYTE(CFG_SIZE);            /* wTotalLength:no of returned bytes */
  ptr[3] = HIBYTE(CFG_SIZE);
  ptr[4] = 0x03;               /* bNumInterfaces: 3 interface */
  ptr[5] = 0x01;               /* bConfigurationValue: Configuration value */
  ptr[6] = 0x00;               /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  ptr[7] = 0xC0; /* bmAttributes: Bus Powered according to user configuration */
#else
  ptr[7] = 0x80; /* bmAttributes: Bus Powered according to user configuration */
#endif
  ptr[8] = USBD_MAX_POWER; /* MaxPower 500 mA */
#endif

  ptr = USBD_COMPOSITE_FSCfgDesc.CONFIG_DESC;
  /* Configuration Descriptor */
  ptr[0] = 0x09;                        /* bLength: Configuration Descriptor size */
  ptr[1] = USB_DESC_TYPE_CONFIGURATION; /* bDescriptorType: Configuration */
  ptr[2] = LOBYTE(CFG_SIZE);            /* wTotalLength:no of returned bytes */
  ptr[3] = HIBYTE(CFG_SIZE);
  ptr[4] = 0x03;               /* bNumInterfaces: 3 interface */
  ptr[5] = 0x01;               /* bConfigurationValue: Configuration value */
  ptr[6] = 0x00;               /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  ptr[7] = 0xC0; /* bmAttributes: Bus Powered according to user configuration */
#else
  ptr[7] = 0x80; /* bmAttributes: Bus Powered according to user configuration */
#endif
  ptr[8] = USBD_MAX_POWER; /* MaxPower 500 mA */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
