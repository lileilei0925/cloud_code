#include <stdio.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "ul_phy_interface.h"

PrachRxParaStruct g_prachRxParaStruct;

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
uint8_t MessageUlTtiRequestparse(uint8_t *srcBuff)
{
    UlTtiRequest *ulTtiRequest      = NULL;
    UlTtiPduInfo *ulTtiPduInfo      = NULL;
    PrachRxParaStruct  *prachRxPara = NULL;
    PduStruct *pduHead = NULL;

    uint16_t sfnNum;
    uint16_t slotNum;
    uint16_t ulPduNum;
    uint8_t  pduType;
    uint16_t pudIndex;

    sfnNum          = ulTtiRequest->numSFN;
    slotNum         = ulTtiRequest->numSlot;

    ulTtiRequest = (UlTtiRequest *)srcBuff;
    ulPduNum = ulTtiRequest->numPdus;

    pduHead = ulTtiRequest->ulTtiPduInfo;
    
    for(pudIndex = 0; pudIndex < ulPduNum; pudIndex++){
        pduType = ulTtiPduInfo->pduType;
        if (pduType == UL_PDU_TYPE_PRACH){

        }

        if (pduType == UL_PDU_TYPE_PUSCH){

        }

        if (pduType == UL_PDU_TYPE_PUCCH){

        }

        if (pduType == UL_PDU_TYPE_SRS){

        }
    }

    return 0;
}
