#include <stdio.h>
#include <string.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "ul_phy_interface.h"

uint32_t g_ulTtiMessageTempBuff[2000];
L1PrachPduParaInfo g_prachPduInfoOut[2]; /* FDM为2时 可以有2个prach PDU*/
UeGoupNumInfo      g_ueGroupNumInfo[8];

uint32_t UlTtiRequestMessageSizeCalc (uint8_t *srcUlSlotMesagesBuff);
void UlTtiRequestPrachPduparse (FapiPrachPduInfo *fapiPrachPduParaIn, L1PrachPduParaInfo *l1PrachPduParaOut, uint16_t pudIndex);

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
uint8_t MessageUlTtiRequestParse(uint8_t *srcUlSlotMesagesBuff)
{
    FapiPrachPduInfo   *prachPduParaInAddr  = NULL;
    L1PrachPduParaInfo *prachPduParaOutAddr = NULL;

    uint16_t sfnNum;
    uint16_t slotNum;
    uint16_t ulPduNum;
    uint8_t  pduType;
    uint8_t  pduSize;
    uint8_t  ueGroupNum;
    uint8_t  antPortNum;
    int16_t  prgIndex;
    uint16_t pudIndex;
    uint8_t  groupIndex;
    uint8_t  ueIndex = 0; 
    uint8_t  ueNumPerGroup;
    uint32_t ulTtirequestMessageSize;
    uint8_t  pduNumPerType[4] = { 0 };

    /*********************** UL_TTI.request Slot Message 的总大小计算 ***********************/
    ulTtirequestMessageSize = UlTtiRequestMessageSizeCalc (srcUlSlotMesagesBuff);

    /****************Slot Messages Ul_TTI.request信息从共享DDR copy到Arm核内*****************/
    memcpy(&g_ulTtiMessageTempBuff[0], srcUlSlotMesagesBuff, ulTtirequestMessageSize); // 后期改DMA搬移


    /***********************Slot Messages Ul_TTI.request信息 parsing***********************/
    pduHead = (PduHeadInfo *)((uint8_t *)&g_ulTtiMessageTempBuff[0] +  sizeof(UlTtiRequestHeadInfo));
    for (pudIndex = 0; pudIndex < ulPduNum; pudIndex++){
        pduType = pduHead->pduType;
        switch (pduType)
        {
            case UL_PDU_TYPE_PRACH:
                 prachPduParaInAddr  = (FapiPrachPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                 prachPduParaOutAddr = (L1PrachPduParaInfo *)&g_prachPduInfoOut[pduNumPerType[0]];
                 UlTtiRequestPrachPduparse (prachPduParaInAddr, prachPduParaOutAddr, pudIndex);
                 pduNumPerType[0]++;
            break;

            case UL_PDU_TYPE_PUSCH:
            /* code */
            pduNumPerType[1]++;
            break;

            case UL_PDU_TYPE_PUCCH:
            /* code */
            pduNumPerType[2]++;
            break;
        
            case UL_PDU_TYPE_SRS:
            /* code */
            pduNumPerType[3]++;
            break;

            default:
            break;
        }
        pduSize = pduHead->pduSize; 
        pduHead = (PduHeadInfo *)((uint8_t *)pduHead + pduSize);
    }
    
    ueGoupNumInfo = (UeGoupNumInfo *)((uint8_t *)pduHead); /* ueNum + pduIndex per UE */
    for (groupIndex = 0; groupIndex < ueGroupNum; groupIndex++)
    {
        g_ueGroupNumInfo[groupIndex].ueNum = ueGoupNumInfo->ueNum;
        for (ueIndex = 0; ueIndex < ueNumPerGroup; ueIndex++)
        {
            g_ueGroupNumInfo[groupIndex].pduIdx[ueIndex] = ueGoupNumInfo->pduIdx[ueIndex];
        }
        ueGoupNumInfo = ueGoupNumInfo +  (sizeof(uint8_t) * (ueGoupNumInfo->ueNum + 1));
    }

    return 0;
}

void UlTtiRequestPrachPduparse (FapiPrachPduInfo *fapiPrachPduParaIn, L1PrachPduParaInfo *l1PrachPduParaOut, uint16_t pudIndex)
{
     uint16_t prgNum, prgIndex;
     uint8_t  digitalBfNum, digitalBfIndex;
     uint16_t * beamIndexValue = NULL;

     l1PrachPduParaOut->pduIndex           = pudIndex;
     l1PrachPduParaOut->phyCellID          = fapiPrachPduParaIn->physCellID;
     l1PrachPduParaOut->prachTdOcasNum     = fapiPrachPduParaIn->numPrachOcas;
     l1PrachPduParaOut->prachFormat        = fapiPrachPduParaIn->prachFormat;
     l1PrachPduParaOut->PrachFdmIndex      = fapiPrachPduParaIn->fdRaIndex;
     l1PrachPduParaOut->prachStartSymb     = fapiPrachPduParaIn->prachStartSymb;
     l1PrachPduParaOut->ncsValue           = fapiPrachPduParaIn->numCs;

     l1PrachPduParaOut->handle             = fapiPrachPduParaIn->prachNewInV3.handle;
     l1PrachPduParaOut->prachCfgScope      = fapiPrachPduParaIn->prachNewInV3.prachCfgScope;
     l1PrachPduParaOut->prachResCfgIndex   = fapiPrachPduParaIn->prachNewInV3.prachResCfgIndex;
     l1PrachPduParaOut->prachFdmNum        = fapiPrachPduParaIn->prachNewInV3.numFdRa;
     l1PrachPduParaOut->startPreambleIndex = fapiPrachPduParaIn->prachNewInV3.startPreambleIndex;
     l1PrachPduParaOut->preambleIndicesNum = fapiPrachPduParaIn->prachNewInV3.numPreambleIndices;

     l1PrachPduParaOut->trpScheme          = fapiPrachPduParaIn->beamFormingInfo.trpScheme;
     l1PrachPduParaOut->prgNum             = fapiPrachPduParaIn->beamFormingInfo.numPRGs;
     l1PrachPduParaOut->prgSize            = fapiPrachPduParaIn->beamFormingInfo.prgSize;
     l1PrachPduParaOut->digitalBfNum       = fapiPrachPduParaIn->beamFormingInfo.digBfInterface;

     prgNum       = l1PrachPduParaOut->prgNum;
     digitalBfNum = l1PrachPduParaOut->digitalBfNum;
     for (prgIndex = 0; prgIndex < prgNum; prgIndex++){
         beamIndexValue = (uint16_t *)&fapiPrachPduParaIn->beamFormingInfo.beamIndex[0] + prgIndex * digitalBfNum;
         for (digitalBfIndex = 0; digitalBfIndex < digitalBfNum; digitalBfIndex++){
             l1PrachPduParaOut->beamIndex[prgIndex][digitalBfIndex] = *beamIndexValue;
             beamIndexValue++;
         }
     }
}

uint32_t UlTtiRequestMessageSizeCalc (uint8_t *srcUlSlotMesagesBuff)
{
    uint32_t ulTtirequestMessageSize;
    uint16_t ulPduNum,  pudIndex;
    uint8_t  ueGroupNum, groupIndex;

    UlTtiRequestHeadInfo *ulRequestHead = (UlTtiRequestHeadInfo *)srcUlSlotMesagesBuff;
    ulPduNum   = ulRequestHead->pduNum;
    ueGroupNum = ulRequestHead->ueGroupNum;
    ulTtirequestMessageSize = sizeof(UlTtiRequestHeadInfo);/* Add Head length */

    PduHeadInfo *pduHead = (PduHeadInfo *)((uint8_t *)&srcUlSlotMesagesBuff + ulTtirequestMessageSize);
    for (pudIndex = 0; pudIndex < ulPduNum; pudIndex++){
        pduSize = pduHead->pduSize;
        pduHead = (PduHeadInfo *)((uint8_t *)pduHead + pduSize);
        ulTtirequestMessageSize = ulTtirequestMessageSize + pduSize;/* Add PDU length */
    }

    UeGoupNumInfo *ueGoupNumInfo = (UeGoupNumInfo *)((uint8_t *)&srcUlSlotMesagesBuff + ulTtirequestMessageSize);
    for (groupIndex = 0; groupIndex < ueGroupNum; groupIndex++){
        ueNumPerGroup = ueGoupNumInfo->ueNum;
        ueGoupNumInfo = ueGoupNumInfo  + (sizeof(uint8_t) * (ueNumPerGroup + 1));
        ulTtirequestMessageSize = ulTtirequestMessageSize + sizeof(uint8_t) * (ueNumPerGroup + 1);/* Add ueGroup length */
    }

    return ulTtirequestMessageSize;
}
