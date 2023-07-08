/**
  ******************************************************************************
  * @file    usbd_composite_builder.c
  * @author  MCD Application Team
  * @brief   This file provides all the composite builder functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  * @verbatim
  *
  *          ===================================================================
  *                                Composite Builder  Description
  *          ===================================================================
  *
  *           The composite builder builds the configuration descriptors based on
  *           the selection of classes by user.
  *           It includes all USB Device classes in order to instantiate their
  *           descriptors, but for better management, it is possible to optimize
  *           footprint by removing unused classes. It is possible to do so by
  *           commenting the relative define in usbd_conf.h.
  *
  *  @endverbatim
  *
  ******************************************************************************
  */

/* BSPDependencies
- None
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_composite_builder.h"

#ifdef USE_USBD_COMPOSITE

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup CMPSIT_CORE
  * @brief Mass storage core module
  * @{
  */

/** @defgroup CMPSIT_CORE_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup CMPSIT_CORE_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup CMPSIT_CORE_Private_Macros
  * @{
  */
/**
  * @}
  */


/** @defgroup CMPSIT_CORE_Private_FunctionPrototypes
  * @{
  */
/* uint8_t  USBD_CMPSIT_Init (USBD_HandleTypeDef *pdev,
                            uint8_t cfgidx); */ /* Function not used for the moment */

/* uint8_t  USBD_CMPSIT_DeInit (USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx); */ /* Function not used for the moment */

uint8_t  *USBD_CMPSIT_GetFSCfgDesc(uint16_t *length);

uint8_t  *USBD_CMPSIT_GetOtherSpeedCfgDesc(uint16_t *length);

uint8_t  *USBD_CMPSIT_GetDeviceQualifierDescriptor(uint16_t *length);

static uint8_t USBD_CMPSIT_FindFreeIFNbr(USBD_HandleTypeDef *pdev);

static void  USBD_CMPSIT_AddConfDesc(uint32_t Conf, __IO uint32_t *pSze);

static void  USBD_CMPSIT_AssignEp(USBD_HandleTypeDef *pdev, uint8_t Add, uint8_t Type, uint32_t Sze);


#if USBD_CMPSIT_ACTIVATE_HID == 1U
static void  USBD_CMPSIT_HIDMouseDesc(USBD_HandleTypeDef *pdev, uint32_t pConf, __IO uint32_t *Sze, uint8_t speed);
#endif /* USBD_CMPSIT_ACTIVATE_HID == 1U */

#if USBD_CMPSIT_ACTIVATE_AUDIO == 1U
static void  USBD_CMPSIT_AUDIODesc(USBD_HandleTypeDef *pdev, uint32_t pConf, __IO uint32_t *Sze, uint8_t speed);
#endif /* USBD_CMPSIT_ACTIVATE_AUDIO == 1U */

/**
  * @}
  */


/** @defgroup CMPSIT_CORE_Private_Variables
  * @{
  */
/* This structure is used only for the Configuration descriptors and Device Qualifier */
USBD_ClassTypeDef  USBD_CMPSIT =
{
  NULL, /* Init, */
  NULL, /* DeInit, */
  NULL, /* Setup, */
  NULL, /* EP0_TxSent, */
  NULL, /* EP0_RxReady, */
  NULL, /* DataIn, */
  NULL, /* DataOut, */
  NULL, /* SOF,  */
  NULL,
  NULL,
  USBD_CMPSIT_GetFSCfgDesc,
  USBD_CMPSIT_GetFSCfgDesc,
  USBD_CMPSIT_GetFSCfgDesc,
  USBD_CMPSIT_GetDeviceQualifierDescriptor,
#if (USBD_SUPPORT_USER_STRING_DESC == 1U)
  NULL,
#endif /* USBD_SUPPORT_USER_STRING_DESC */
};

/* The generic configuration descriptor buffer that will be filled by builder
   Size of the buffer is the maximum possible configuration descriptor size. */
__ALIGN_BEGIN static uint8_t USBD_CMPSIT_FSCfgDesc[USBD_CMPST_MAX_CONFDESC_SZ]  __ALIGN_END = {0};
static uint8_t *pCmpstFSConfDesc = USBD_CMPSIT_FSCfgDesc;
/* Variable that dynamically holds the current size of the configuration descriptor */
static __IO uint32_t CurrFSConfDescSz = 0U;

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CMPSIT_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,      /* bLength */
  USB_DESC_TYPE_DEVICE_QUALIFIER,  /* bDescriptorType */
  0x00,                            /* bcdDevice low */
  0x02,                            /* bcdDevice high */
  0xEF,                            /* Class */
  0x02,                            /* SubClass */
  0x01,                            /* Protocol */
  0x40,                            /* bMaxPacketSize0 */
  0x01,                            /* bNumConfigurations */
  0x00,                            /* bReserved */
};

/**
  * @}
  */


/** @defgroup CMPSIT_CORE_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CMPSIT_AddClass
  *         Register a class in the class builder
  * @param  pdev: device instance
  * @param  pclass: pointer to the class structure to be added
  * @param  class: type of the class to be added (from USBD_CompositeClassTypeDef)
  * @param  cfgidx: configuration index
  * @retval status
  */
uint8_t  USBD_CMPSIT_AddClass(USBD_HandleTypeDef *pdev,
                              USBD_ClassTypeDef *pclass,
                              USBD_CompositeClassTypeDef class,
                              uint8_t cfgidx)
{
  if ((pdev->classId < USBD_MAX_SUPPORTED_CLASS) && (pdev->tclasslist[pdev->classId].Active == 0U))
  {
    /* Store the class parameters in the global tab */
    pdev->pClass[pdev->classId] = pclass;
    pdev->tclasslist[pdev->classId].ClassId = pdev->classId;
    pdev->tclasslist[pdev->classId].Active = 1U;
    pdev->tclasslist[pdev->classId].ClassType = class;

    /* Call configuration descriptor builder and endpoint configuration builder */
    if (USBD_CMPSIT_AddToConfDesc(pdev) != (uint8_t)USBD_OK)
    {
      return (uint8_t)USBD_FAIL;
    }
  }

  UNUSED(cfgidx);

  return (uint8_t)USBD_OK;
}


/**
  * @brief  USBD_CMPSIT_AddToConfDesc
  *         Add a new class to the configuration descriptor
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  USBD_CMPSIT_AddToConfDesc(USBD_HandleTypeDef *pdev)
{
  uint8_t idxIf = 0U;
  uint8_t iEp = 0U;

  /* For the first class instance, start building the config descriptor common part */
  if (pdev->classId == 0U)
  {
    /* Add configuration and IAD descriptors */
    USBD_CMPSIT_AddConfDesc((uint32_t)pCmpstFSConfDesc, &CurrFSConfDescSz);
  }

  switch (pdev->tclasslist[pdev->classId].ClassType)
  {
#if USBD_CMPSIT_ACTIVATE_HID == 1
    case CLASS_TYPE_HID:
      /* Setup Max packet sizes (for HID, no dependency on USB Speed, both HS/FS have same packet size) */
      pdev->tclasslist[pdev->classId].CurrPcktSze = HID_EPIN_SIZE;

      /* Find the first available interface slot and Assign number of interfaces */
      idxIf = USBD_CMPSIT_FindFreeIFNbr(pdev);
      pdev->tclasslist[pdev->classId].NumIf = 1U;
      pdev->tclasslist[pdev->classId].Ifs[0] = idxIf;

      /* Assign endpoint numbers */
      pdev->tclasslist[pdev->classId].NumEps = 1U; /* EP1_IN */

      /* Set IN endpoint slot */
      iEp = pdev->tclasslist[pdev->classId].EpAdd[0];

      /* Assign IN Endpoint */
      USBD_CMPSIT_AssignEp(pdev, iEp, USBD_EP_TYPE_INTR, pdev->tclasslist[pdev->classId].CurrPcktSze);

      /* Configure and Append the Descriptor */
      USBD_CMPSIT_HIDMouseDesc(pdev, (uint32_t)pCmpstFSConfDesc, &CurrFSConfDescSz, (uint8_t)USBD_SPEED_FULL);

      break;
#endif /* USBD_CMPSIT_ACTIVATE_HID */

#if USBD_CMPSIT_ACTIVATE_AUDIO == 1
    case CLASS_TYPE_AUDIO:
      /* Setup Max packet sizes*/
      pdev->tclasslist[pdev->classId].CurrPcktSze = USBD_AUDIO_GetEpPcktSze(pdev, 0U, 0U);

      /* Find the first available interface slot and Assign number of interfaces */
      idxIf = USBD_CMPSIT_FindFreeIFNbr(pdev);
      pdev->tclasslist[pdev->classId].NumIf = 2U;
      pdev->tclasslist[pdev->classId].Ifs[0] = idxIf;
      pdev->tclasslist[pdev->classId].Ifs[1] = (uint8_t)(idxIf + 1U);

      /* Assign endpoint numbers */
      pdev->tclasslist[pdev->classId].NumEps = 1U; /* EP1_OUT*/

      /* Set OUT endpoint slot */
      iEp = pdev->tclasslist[pdev->classId].EpAdd[0];

      /* Assign OUT Endpoint */
      USBD_CMPSIT_AssignEp(pdev, iEp, USBD_EP_TYPE_ISOC, pdev->tclasslist[pdev->classId].CurrPcktSze);

      /* Configure and Append the Descriptor (only FS mode supported) */
      USBD_CMPSIT_AUDIODesc(pdev, (uint32_t)pCmpstFSConfDesc, &CurrFSConfDescSz, (uint8_t)USBD_SPEED_FULL);

      break;
#endif /* USBD_CMPSIT_ACTIVATE_AUDIO */

    default:
      UNUSED(idxIf);
      UNUSED(iEp);
      UNUSED(USBD_CMPSIT_FindFreeIFNbr);
      UNUSED(USBD_CMPSIT_AssignEp);
      break;
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_CMPSIT_GetFSCfgDesc
  *         return configuration descriptor for both FS and HS modes
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t  *USBD_CMPSIT_GetFSCfgDesc(uint16_t *length)
{
  *length = (uint16_t)CurrFSConfDescSz;

  return USBD_CMPSIT_FSCfgDesc;
}

/**
  * @brief  USBD_CMPSIT_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t  *USBD_CMPSIT_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = (uint16_t)CurrFSConfDescSz;

  return USBD_CMPSIT_FSCfgDesc;
}

/**
  * @brief  DeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t  *USBD_CMPSIT_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)(sizeof(USBD_CMPSIT_DeviceQualifierDesc));
  return USBD_CMPSIT_DeviceQualifierDesc;
}

/**
  * @brief  USBD_CMPSIT_FindFreeIFNbr
  *         Find the first interface available slot
  * @param  pdev: device instance
  * @retval The interface number to be used
  */
static uint8_t USBD_CMPSIT_FindFreeIFNbr(USBD_HandleTypeDef *pdev)
{
  uint32_t idx = 0U;

  /* Unroll all already activated classes */
  for (uint32_t i = 0U; i < pdev->NumClasses; i++)
  {
    /* Unroll each class interfaces */
    for (uint32_t j = 0U; j < pdev->tclasslist[i].NumIf; j++)
    {
      /* Increment the interface counter index */
      idx++;
    }
  }

  /* Return the first available interface slot */
  return (uint8_t)idx;
}

/**
  * @brief  USBD_CMPSIT_AddToConfDesc
  *         Add a new class to the configuration descriptor
  * @param  pdev: device instance
  * @retval none
  */
static void  USBD_CMPSIT_AddConfDesc(uint32_t Conf, __IO uint32_t *pSze)
{
  /* Intermediate variable to comply with MISRA-C Rule 11.3 */
  USBD_ConfigDescTypeDef *ptr = (USBD_ConfigDescTypeDef *)Conf;

  ptr->bLength = (uint8_t)sizeof(USBD_ConfigDescTypeDef);
  ptr->bDescriptorType = USB_DESC_TYPE_CONFIGURATION;
  ptr->wTotalLength = 0U;
  ptr->bNumInterfaces = 0U;
  ptr->bConfigurationValue = 1U;
  ptr->iConfiguration = USBD_CONFIG_STR_DESC_IDX;

#if (USBD_SELF_POWERED == 1U)
  ptr->bmAttributes = 0xC0U;   /* bmAttributes: Self Powered according to user configuration */
#else
  ptr->bmAttributes = 0x80U;   /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */

  ptr->bMaxPower = USBD_MAX_POWER;

  *pSze += sizeof(USBD_ConfigDescTypeDef);
}

/**
  * @brief  USBD_CMPSIT_AssignEp
  *         Assign and endpoint
  * @param  pdev: device instance
  * @param  Add: Endpoint address
  * @param  Type: Endpoint type
  * @param  Sze: Endpoint max packet size
  * @retval none
  */
static void  USBD_CMPSIT_AssignEp(USBD_HandleTypeDef *pdev, uint8_t Add, uint8_t Type, uint32_t Sze)
{
  uint32_t idx = 0U;

  /* Find the first available endpoint slot */
  while (((idx < (pdev->tclasslist[pdev->classId]).NumEps) && \
          ((pdev->tclasslist[pdev->classId].Eps[idx].is_used) != 0U)))
  {
    /* Increment the index */
    idx++;
  }

  /* Configure the endpoint */
  pdev->tclasslist[pdev->classId].Eps[idx].add = Add;
  pdev->tclasslist[pdev->classId].Eps[idx].type = Type;
  pdev->tclasslist[pdev->classId].Eps[idx].size = (uint8_t)Sze;
  pdev->tclasslist[pdev->classId].Eps[idx].is_used = 1U;
}

#if USBD_CMPSIT_ACTIVATE_HID == 1
/**
  * @brief  USBD_CMPSIT_HIDMouseDesc
  *         Configure and Append the HID Mouse Descriptor
  * @param  pdev: device instance
  * @param  pConf: Configuration descriptor pointer
  * @param  Sze: pointer to the current configuration descriptor size
  * @retval None
  */
static void  USBD_CMPSIT_HIDMouseDesc(USBD_HandleTypeDef *pdev, uint32_t pConf,
                                      __IO uint32_t *Sze, uint8_t speed)
{
  static USBD_IfDescTypeDef *pIfDesc;
  static USBD_EpDescTypeDef *pEpDesc;
  static USBD_HIDDescTypeDef *pHidMouseDesc;

  /* Append HID Interface descriptor to Configuration descriptor */
  __USBD_CMPSIT_SET_IF(pdev->tclasslist[pdev->classId].Ifs[0], 0U, \
                       (uint8_t)(pdev->tclasslist[pdev->classId].NumEps), 0x03U, 0x01U, 0x01U, 0U);

  /* Append HID Functional descriptor to Configuration descriptor */
  pHidMouseDesc = ((USBD_HIDDescTypeDef *)(pConf + *Sze));
  pHidMouseDesc->bLength = (uint8_t)sizeof(USBD_HIDDescTypeDef);
  pHidMouseDesc->bDescriptorType = HID_DESCRIPTOR_TYPE;
  pHidMouseDesc->bcdHID = 0x0111U;
  pHidMouseDesc->bCountryCode = 0x00U;
  pHidMouseDesc->bNumDescriptors = 0x01U;
  pHidMouseDesc->bHIDDescriptorType = 0x22U;
  pHidMouseDesc->wItemLength = HID_KEYBOARD_REPORT_DESC_SIZE;
  *Sze += (uint32_t)sizeof(USBD_HIDDescTypeDef);

  /* Append Endpoint descriptor to Configuration descriptor */
  __USBD_CMPSIT_SET_EP(pdev->tclasslist[pdev->classId].Eps[0].add, USBD_EP_TYPE_INTR, HID_EPIN_SIZE, \
                       HID_HS_BINTERVAL, HID_FS_BINTERVAL);

  /* Update Config Descriptor and IAD descriptor */
  ((USBD_ConfigDescTypeDef *)pConf)->bNumInterfaces += 1U;
  ((USBD_ConfigDescTypeDef *)pConf)->wTotalLength  = *Sze;
}
#endif /* USBD_CMPSIT_ACTIVATE_HID == 1 */

#if USBD_CMPSIT_ACTIVATE_AUDIO == 1
/**
  * @brief  USBD_CMPSIT_AUDIODesc
  *         Configure and Append the AUDIO Descriptor
  * @param  pdev: device instance
  * @param  pConf: Configuration descriptor pointer
  * @param  Sze: pointer to the current configuration descriptor size
  * @retval None
  */
static void  USBD_CMPSIT_AUDIODesc(USBD_HandleTypeDef *pdev, uint32_t pConf, __IO uint32_t *Sze, uint8_t speed)
{
  static USBD_IfDescTypeDef *pIfDesc;
  static USBD_IadDescTypeDef *pIadDesc;
  UNUSED(speed);

  /* Append AUDIO Interface descriptor to Configuration descriptor */
  USBD_SpeakerIfDescTypeDef            *pSpIfDesc;
  USBD_SpeakerInDescTypeDef            *pSpInDesc;
  USBD_SpeakerFeatureDescTypeDef       *pSpFDesc;
  USBD_SpeakerOutDescTypeDef           *pSpOutDesc;
  USBD_SpeakerStreamIfDescTypeDef      *pSpStrDesc;
  USBD_SpeakerIIIFormatIfDescTypeDef   *pSpIIIDesc;
  USBD_SpeakerEndDescTypeDef           *pSpEpDesc;
  USBD_SpeakerEndStDescTypeDef         *pSpEpStDesc;

#if USBD_COMPOSITE_USE_IAD == 1
  pIadDesc                          = ((USBD_IadDescTypeDef *)(pConf + *Sze));
  pIadDesc->bLength                 = (uint8_t)sizeof(USBD_IadDescTypeDef);
  pIadDesc->bDescriptorType         = USB_DESC_TYPE_IAD; /* IAD descriptor */
  pIadDesc->bFirstInterface         = pdev->tclasslist[pdev->classId].Ifs[0];
  pIadDesc->bInterfaceCount         = 2U;    /* 2 interfaces */
  pIadDesc->bFunctionClass          = USB_DEVICE_CLASS_AUDIO;
  pIadDesc->bFunctionSubClass       = AUDIO_SUBCLASS_AUDIOCONTROL;
  pIadDesc->bFunctionProtocol       = AUDIO_PROTOCOL_UNDEFINED;
  pIadDesc->iFunction               = 0U; /* String Index */
  *Sze                             += (uint32_t)sizeof(USBD_IadDescTypeDef);
#endif /* USBD_COMPOSITE_USE_IAD == 1 */

  /* Append AUDIO Interface descriptor to Configuration descriptor */
  __USBD_CMPSIT_SET_IF(pdev->tclasslist[pdev->classId].Ifs[0], 0U, 0U, USB_DEVICE_CLASS_AUDIO, \
                       AUDIO_SUBCLASS_AUDIOCONTROL, AUDIO_PROTOCOL_UNDEFINED, 0U);

  /* Append AUDIO USB Speaker Class-specific AC Interface descriptor to Configuration descriptor */
  pSpIfDesc = ((USBD_SpeakerIfDescTypeDef *)(pConf + *Sze));
  pSpIfDesc->bLength = (uint8_t)sizeof(USBD_IfDescTypeDef);
  pSpIfDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpIfDesc->bDescriptorSubtype = AUDIO_CONTROL_HEADER;
  pSpIfDesc->bcdADC = 0x0100U;
  pSpIfDesc->wTotalLength = 0x0027U;
  pSpIfDesc->bInCollection = 0x01U;
  pSpIfDesc->baInterfaceNr = 0x01U;
  *Sze += (uint32_t)sizeof(USBD_IfDescTypeDef);

  /* Append USB Speaker Input Terminal Descriptor to Configuration descriptor*/
  pSpInDesc = ((USBD_SpeakerInDescTypeDef *)(pConf + *Sze));
  pSpInDesc->bLength = (uint8_t)sizeof(USBD_SpeakerInDescTypeDef);
  pSpInDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpInDesc->bDescriptorSubtype = AUDIO_CONTROL_INPUT_TERMINAL;
  pSpInDesc->bTerminalID = 0x01U;
  pSpInDesc->wTerminalType = 0x0101U;
  pSpInDesc->bAssocTerminal = 0x00U;
  pSpInDesc->bNrChannels = 0x02U;
  pSpInDesc->wChannelConfig = 0x0003U;
  pSpInDesc->iChannelNames = 0x00U;
  pSpInDesc->iTerminal = 0x00U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerInDescTypeDef);

  /*Append USB Speaker Audio Feature Unit Descriptor to Configuration descriptor */
  pSpFDesc = ((USBD_SpeakerFeatureDescTypeDef *)(pConf + *Sze));
  pSpFDesc->bLength = (uint8_t)sizeof(USBD_SpeakerFeatureDescTypeDef);
  pSpFDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpFDesc->bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT;
  pSpFDesc->bUnitID = AUDIO_OUT_STREAMING_CTRL;
  pSpFDesc->bSourceID = 0x01U;
  pSpFDesc->bControlSize = 0x01U;
  pSpFDesc->bmaControlMaster = AUDIO_CONTROL_MUTE;
  pSpFDesc->bmaControl0 = 0x00U;
  pSpFDesc->bmaControl1 = 0x00U;
  pSpFDesc->iTerminal = 0x00U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerFeatureDescTypeDef);

  /*Append USB Speaker Output Terminal Descriptor to Configuration descriptor*/
  pSpOutDesc = ((USBD_SpeakerOutDescTypeDef *)(pConf + *Sze));
  pSpOutDesc->bLength = (uint8_t)sizeof(USBD_SpeakerOutDescTypeDef);
  pSpOutDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpOutDesc->bDescriptorSubtype = AUDIO_CONTROL_OUTPUT_TERMINAL;
  pSpOutDesc->bTerminalID = 0x03U;
  pSpOutDesc->wTerminalType = 0x0301U;
  pSpOutDesc->bAssocTerminal = 0x00U;
  pSpOutDesc->bSourceID = 0x02U;
  pSpOutDesc->iTerminal = 0x00U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerOutDescTypeDef);

  /*Append USB Speaker Audio Feature Unit Descriptor to Configuration descriptor */
  pSpFDesc = ((USBD_SpeakerFeatureDescTypeDef *)(pConf + *Sze));
  pSpFDesc->bLength = (uint8_t)sizeof(USBD_SpeakerFeatureDescTypeDef);
  pSpFDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpFDesc->bDescriptorSubtype = AUDIO_CONTROL_FEATURE_UNIT;
  pSpFDesc->bUnitID = AUDIO_OUT_STREAMING_CTRL;
  pSpFDesc->bSourceID = 0x03U;
  pSpFDesc->bControlSize = 0x01U;
  pSpFDesc->bmaControlMaster = AUDIO_CONTROL_MUTE;
  pSpFDesc->bmaControl0 = 0x00U;
  pSpFDesc->bmaControl1 = 0x00U;
  pSpFDesc->iTerminal = 0x00U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerFeatureDescTypeDef);

  /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwidth */
  /* Interface 1, Alternate Setting 0*/
  __USBD_CMPSIT_SET_IF(pdev->tclasslist[pdev->classId].Ifs[1], 0U, 0U, USB_DEVICE_CLASS_AUDIO, \
                       AUDIO_SUBCLASS_AUDIOSTREAMING, AUDIO_PROTOCOL_UNDEFINED, 0U);

  /* USB Speaker Standard AS Interface Descriptor -Audio Streaming Operational */
  /* Interface 1, Alternate Setting 1*/
  __USBD_CMPSIT_SET_IF(pdev->tclasslist[pdev->classId].Ifs[1], 0x01U, 0x01U, USB_DEVICE_CLASS_AUDIO, \
                       AUDIO_SUBCLASS_AUDIOSTREAMING, AUDIO_PROTOCOL_UNDEFINED, 0U);

  /* USB Speaker Audio Streaming Interface Descriptor */
  pSpStrDesc = ((USBD_SpeakerStreamIfDescTypeDef *)(pConf + *Sze));
  pSpStrDesc->bLength = (uint8_t)sizeof(USBD_SpeakerStreamIfDescTypeDef);
  pSpStrDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpStrDesc->bDescriptorSubtype = AUDIO_STREAMING_GENERAL;
  pSpStrDesc->bTerminalLink = 0x01U;
  pSpStrDesc->bDelay = 0x01U;
  pSpStrDesc->wFormatTag = 0x0001U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerStreamIfDescTypeDef);

  /* USB Speaker Audio Type III Format Interface Descriptor */
  pSpIIIDesc = ((USBD_SpeakerIIIFormatIfDescTypeDef *)(pConf + *Sze));
  pSpIIIDesc->bLength = (uint8_t)sizeof(USBD_SpeakerIIIFormatIfDescTypeDef);
  pSpIIIDesc->bDescriptorType = AUDIO_INTERFACE_DESCRIPTOR_TYPE;
  pSpIIIDesc->bDescriptorSubtype = AUDIO_STREAMING_FORMAT_TYPE;
  pSpIIIDesc->bFormatType = AUDIO_FORMAT_TYPE_I;
  pSpIIIDesc->bNrChannels = 0x02U;
  pSpIIIDesc->bSubFrameSize = 0x02U;
  pSpIIIDesc->bBitResolution = 16U;
  pSpIIIDesc->bSamFreqType = 1U;
  pSpIIIDesc->tSamFreq2 = 0x80U;
  pSpIIIDesc->tSamFreq1 = 0xBBU;
  pSpIIIDesc->tSamFreq0 = 0x00U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerIIIFormatIfDescTypeDef);

  /* Endpoint 1 - Standard Descriptor */
  pSpEpDesc = ((USBD_SpeakerEndDescTypeDef *)(pConf + *Sze));
  pSpEpDesc->bLength = 0x09U;
  pSpEpDesc->bDescriptorType = USB_DESC_TYPE_ENDPOINT;
  pSpEpDesc->bEndpointAddress = pdev->tclasslist[pdev->classId].Eps[0].add;
  pSpEpDesc->bmAttributes = USBD_EP_TYPE_ISOC;
  pSpEpDesc->wMaxPacketSize = USBD_AUDIO_GetEpPcktSze(pdev, 0U, 0U);
  pSpEpDesc->bInterval = 0x01U;
  pSpEpDesc->bRefresh = 0x00U;
  pSpEpDesc->bSynchAddress = 0x00U;
  *Sze += 0x09U;

  /* Endpoint - Audio Streaming Descriptor*/
  pSpEpStDesc = ((USBD_SpeakerEndStDescTypeDef *)(pConf + *Sze));
  pSpEpStDesc->bLength = (uint8_t)sizeof(USBD_SpeakerEndStDescTypeDef);
  pSpEpStDesc->bDescriptorType = AUDIO_ENDPOINT_DESCRIPTOR_TYPE;
  pSpEpStDesc->bDescriptor = AUDIO_ENDPOINT_GENERAL;
  pSpEpStDesc->bmAttributes = 0x00U;
  pSpEpStDesc->bLockDelayUnits = 0x00U;
  pSpEpStDesc->wLockDelay = 0x0000U;
  *Sze += (uint32_t)sizeof(USBD_SpeakerEndStDescTypeDef);

  /* Update Config Descriptor and IAD descriptor */
  ((USBD_ConfigDescTypeDef *)pConf)->bNumInterfaces += 2U;
  ((USBD_ConfigDescTypeDef *)pConf)->wTotalLength = *Sze;
}
#endif /* USBD_CMPSIT_ACTIVATE_AUDIO */

/**
  * @brief  USBD_CMPSIT_SetClassID
  *         Find and set the class ID relative to selected class type and instance
  * @param  pdev: device instance
  * @param  Class: Class type, can be CLASS_TYPE_NONE if requested to find class from setup request
  * @param  Instance: Instance number of the class (0 if first/unique instance, >0 otherwise)
  * @retval The Class ID, The pdev->classId is set with the value of the selected class ID.
  */
uint32_t  USBD_CMPSIT_SetClassID(USBD_HandleTypeDef *pdev, USBD_CompositeClassTypeDef Class, uint32_t Instance)
{
  uint32_t idx;
  uint32_t inst = 0U;

  /* Unroll all already activated classes */
  for (idx = 0U; idx < pdev->NumClasses; idx++)
  {
    /* Check if the class correspond to the requested type and if it is active */
    if (((USBD_CompositeClassTypeDef)(pdev->tclasslist[idx].ClassType) == Class) && ((pdev->tclasslist[idx].Active) == 1U))
    {
      if (inst == Instance)
      {
        /* Set the new class ID */
        pdev->classId = idx;

        /* Return the class ID value */
        return (idx);
      }
      else
      {
        /* Increment instance index and look for next instance */
        inst++;
      }
    }
  }

  /* No class found, return 0xFF */
  return 0xFFU;
}

/**
  * @brief  USBD_CMPSIT_GetClassID
  *         Returns the class ID relative to selected class type and instance
  * @param  pdev: device instance
  * @param  Class: Class type, can be CLASS_TYPE_NONE if requested to find class from setup request
  * @param  Instance: Instance number of the class (0 if first/unique instance, >0 otherwise)
  * @retval The Class ID (this function does not set the pdev->classId field.
  */
uint32_t  USBD_CMPSIT_GetClassID(USBD_HandleTypeDef *pdev, USBD_CompositeClassTypeDef Class, uint32_t Instance)
{
  uint32_t idx;
  uint32_t inst = 0U;

  /* Unroll all already activated classes */
  for (idx = 0U; idx < pdev->NumClasses; idx++)
  {
    /* Check if the class correspond to the requested type and if it is active */
    if (((USBD_CompositeClassTypeDef)(pdev->tclasslist[idx].ClassType) == Class) && ((pdev->tclasslist[idx].Active) == 1U))
    {
      if (inst == Instance)
      {
        /* Return the class ID value */
        return (idx);
      }
      else
      {
        /* Increment instance index and look for next instance */
        inst++;
      }
    }
  }

  /* No class found, return 0xFF */
  return 0xFFU;
}

/**
  * @brief  USBD_CMPST_ClearConfDesc
  *         Reset the configuration descriptor
  * @param  pdev: device instance (reserved for future use)
  * @retval Status.
  */
uint8_t USBD_CMPST_ClearConfDesc(USBD_HandleTypeDef *pdev)
{
  UNUSED(pdev);

  /* Reset the configuration descriptor pointer to default value and its size to zero */
  pCmpstFSConfDesc = USBD_CMPSIT_FSCfgDesc;
  CurrFSConfDescSz = 0U;

  /* All done, can't fail */
  return (uint8_t)USBD_OK;
}

#endif /* USE_USBD_COMPOSITE */

/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */


