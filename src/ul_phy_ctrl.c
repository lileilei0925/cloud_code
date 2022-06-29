#include <stdio.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "ul_phy_interface.h"

PrachPduParaInfo g_prachRxParaInfo;

int main(void)
{
  uint16_t a = 16;
  uint16_t b = 16;
  uint16_t c = 0;

  printf("c = %d;\n",c);
  printf("___Hello World___;\n");
  
  return 0;
}

/* UL_TTI.request slot messages parsing */
uint8_t MessageUlTtiRequestparse(uint8_t *srcSlotMesagesBuff)
{
    FapiUlTtiRequest *fapiulTtiRequest = NULL;
    FapiUlTtiPduInfo *fapiulTtiPduInfo = NULL;
    PrachPduParaInfo *prachRxParaInfo = NULL;
    PduHeadInfo *pduHead = NULL;

    uint16_t sfnNum;
    uint16_t slotNum;
    uint16_t ulPduNum;
    uint8_t  pduType;
    uint8_t  pduSize;
    uint16_t pudIndex;

    fapiulTtiRequest = (FapiUlTtiRequest *)srcSlotMesagesBuff;
    sfnNum   = fapiulTtiRequest->numSFN;
    slotNum  = fapiulTtiRequest->numSlot;
    ulPduNum = fapiulTtiRequest->numPdus;
    
    pduHead = (PduHeadInfo *)((uint8_t *)&fapiulTtiRequest->ulTtiPduInfo[0]);
    for(pudIndex = 0; pudIndex < ulPduNum; pudIndex++){

        pduType = pduHead->pduType;
        pduSize = pduHead->pduSize; /*This length value includes the 4 bytes required for the PDU type and PDU size parameters*/
        if (pduType == UL_PDU_TYPE_PRACH){ /*PRACH PDU parsing*/
           
        }

        if (pduType == UL_PDU_TYPE_PUSCH){/*PUSCH PDU parsing*/
            
        }

        if (pduType == UL_PDU_TYPE_PUCCH){/*PUCCH PDU parsing*/
            
        }

        if (pduType == UL_PDU_TYPE_SRS){  /*SRS PDU parsing*/
           
        }
        pduHead = (PduHeadInfo *)(pduHead + pduSize);
    }

    return 0;
}
