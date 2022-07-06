#include <stdio.h>
#include <string.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "ul_phy_interface.h"

uint32_t         g_ulTtiMessageTempBuff[2000]; /* ULTTIMessage 本地buffer */
L1PrachParaInfo  g_prachParaInfoOut;           /* Prach 参数本地buffer */
UlueGoupNumInfo  g_ulUeGroupNumInfo[8];        /* ULTTIMessage ueGroupinfo 本地buffer */
UlPduMappingInfo g_ulPduMappingInfo[200];      /* 暂时假设有200个PDU */

uint32_t UlTtiRequestMessageSizeCalc (uint8_t *srcUlSlotMesagesBuff);
void UlTtiRequestPrachPduparse (FapiNrMsgPrachPduInfo *fapiPrachPduParaIn, L1PrachPduInfo *l1PracPduOut, uint16_t pudIndex);

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
    FapiNrMsgPrachPduInfo *fapiprachPduParaIn  = NULL;
    L1PrachPduInfo        *l1prachPduInfo = NULL;

    uint16_t sfnNum,slotNum,ulPduNum;
    uint16_t pduNumPerType[5] = { 0 };
    uint16_t pduCntPerType[4] = { 0 };
    uint16_t pduType, pduSize;
    uint8_t  ulPduTypes,ueGroupNum;

    uint8_t  ueNumInGroup;
    uint8_t  antPortNum;
    int16_t  prgIndex;
    uint16_t pduIndex;
    uint8_t  groupIndex;
    uint8_t  ueIndex; 
    uint8_t  ueNumPerGroup;
    uint32_t ulTtirequestMessageSize;
    
    if (srcUlSlotMesagesBuff != NULL){
        /******************** UL_TTI.request Slot Message 的总大小计算 ********************/
        ulTtirequestMessageSize = UlTtiRequestMessageSizeCalc (srcUlSlotMesagesBuff);
        memcpy(&g_ulTtiMessageTempBuff[0], srcUlSlotMesagesBuff, ulTtirequestMessageSize); /* Slot Messages Ul_TTI.request信息从共享DDR copy到Arm核内,后期改DMA搬移 */

        /*********************Slot Messages Ul_TTI.request信息 parsing********************/
        UlTtiRequestHeadInfo *ulRequestHead = (UlTtiRequestHeadInfo *)g_ulTtiMessageTempBuff;
        sfnNum     = ulRequestHead->sfnNum;
        slotNum    = ulRequestHead->slotNum;
        ulPduNum   = ulRequestHead->pduNum;
        ulPduTypes = ulRequestHead->ulPduTypes;
        ueGroupNum = ulRequestHead->ueGroupNum;
        memcpy(&pduNumPerType[0], ulRequestHead->pduNumPerType, sizeof(uint16_t)*MAX_UL_PDU_TYPES);

        L1PrachParaInfo  *l1prachParaInfoOut = &g_prachParaInfoOut;
        l1prachParaInfoOut->sfnNum  = sfnNum;
        l1prachParaInfoOut->slotNum = slotNum;

        PduHeadInfo *pduHead = (PduHeadInfo *)((uint8_t *)&g_ulTtiMessageTempBuff[0] +  sizeof(UlTtiRequestHeadInfo));
        for (pduIndex = 0; pduIndex < ulPduNum; pduIndex++){
            pduType = pduHead->pduType;
            switch (pduType)
            {
                case UL_PDU_TYPE_PRACH:
                    fapiprachPduParaIn = (FapiNrMsgPrachPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    l1prachPduInfo     =  &g_prachParaInfoOut.l1prachPduInfo[pduCntPerType[0]];
                    UlTtiRequestPrachPduparse (fapiprachPduParaIn, l1prachPduInfo, pduIndex);
                    pduCntPerType[0]++;
                    break;

                case UL_PDU_TYPE_PUSCH:
                    /* code */
                    pduCntPerType[1]++;
                    break;

                case UL_PDU_TYPE_PUCCH:
                    /* code */
                    pduCntPerType[2]++;
                    break;
        
                case UL_PDU_TYPE_SRS:
                    /* code */
                    pduCntPerType[3]++;
                    break;

                default:
                break;
            }
            pduSize = pduHead->pduSize; 
            pduHead = (PduHeadInfo *)((uint8_t *)pduHead + pduSize);
        }

        //l1prachParaInfoOut->prachPduNum = pduCntPerType[0];/* 记录每个上行信道的 PDU Number*/
        //l1PuschParaInfoOut->puschPduNum = pduCntPerType[1];
        //l1PucchParaInfoOut->puschPduNum = pduCntPerType[2];
        //l1SrsParaInfoOut->puschPduNum   = pduCntPerType[3];
        
        /************** pduIndex mapping relation with UE **************/
        UlueGoupNumInfo  *ulUeGoupNumInfo = (UlueGoupNumInfo *)((uint8_t *)pduHead); /* sizeof(uint8_t) * (ulUeGoupNumInfo->ueNum + 1) per Group */
        UlPduMappingInfo *ulPduMappingInfo = &g_ulPduMappingInfo[0];
        for (groupIndex = 0; groupIndex < ueGroupNum; groupIndex++){
            ueNumInGroup = ulUeGoupNumInfo->ueNum;
            for (ueIndex = 0; ueIndex < ueNumInGroup; ueIndex++){
                pduIndex = ulUeGoupNumInfo->pduIdx[ueIndex];
                ulPduMappingInfo[pduIndex].ueIndex = ueIndex;
                ulPduMappingInfo[pduIndex].groupIndex = groupIndex;
            }
            ulUeGoupNumInfo = ulUeGoupNumInfo +  (sizeof(uint8_t) * (ulUeGoupNumInfo->ueNum + 1));
        }
    }

    return 0;
}

uint8_t L1PrachParaInfo2LowPhy(L1PrachParaInfo  *l1prachParaInfoIn, L1PrachConfigInfo *l1PrachConfigInfo, L1CarrierCfgInfo *l1CarrierCfgInfo)
{
    uint16_t prachPduNum;
    uint16_t sfnNum, slotNum;
    uint8_t  prachFdmNum, loopIdx;
    uint8_t  prachScs, puschScs;
    uint8_t  prachcfgIdx;
    uint16_t prachRootIndex[2];
    uint8_t  prachRootNum[2];
    int16_t  freqOffsetK1[2];
    uint8_t  prachZeroCorrCfg[2]; 
    uint16_t prachResCfgIdx;

    sfnNum         = l1prachParaInfoIn->sfnNum;
    slotNum        = l1prachParaInfoIn->slotNum;
    prachPduNum    = l1prachParaInfoIn->prachPduNum;

    prachScs       = l1PrachConfigInfo->prachSubCSpacing;
    puschScs       = l1PrachConfigInfo->ulBwpPuschScs;
    prachcfgIdx    = l1PrachConfigInfo->prachConfigIndex; 
    prachResCfgIdx = l1PrachConfigInfo->prachResConfigIndex;
    prachFdmNum    = l1PrachConfigInfo->numPrachFdOccasions;
    for(loopIdx = 0; loopIdx < prachFdmNum; loopIdx)
    {
        prachRootIndex[loopIdx]   = l1PrachConfigInfo->prachRootSequenceIndex[loopIdx];
        prachRootNum[loopIdx]     = l1PrachConfigInfo->numRootSequences[loopIdx];
        freqOffsetK1[loopIdx]     = l1PrachConfigInfo->k1[loopIdx];
        prachZeroCorrCfg[loopIdx] = l1PrachConfigInfo->prachZeroCorrConf[loopIdx];
    }
    
    
    return 0;
    
}

uint32_t UlTtiRequestMessageSizeCalc (uint8_t *srcUlSlotMesagesBuff)
{
    uint32_t ulTtirequestMessageSize = 0;
    uint16_t ulPduNum, pduSize, pudIndex;
    uint8_t  ueGroupNum, groupIndex,ueNumInGroup; 

    UlTtiRequestHeadInfo *ulRequestHead = (UlTtiRequestHeadInfo *)srcUlSlotMesagesBuff;
    ulPduNum   = ulRequestHead->pduNum;
    ueGroupNum = ulRequestHead->ueGroupNum;
    ulTtirequestMessageSize = sizeof(UlTtiRequestHeadInfo);/* Add Head length */
    
    if(ulPduNum == 0)
    {
        return 0;  //Add errCode ;
    }

    PduHeadInfo *pduHead = (PduHeadInfo *)((uint8_t *)&srcUlSlotMesagesBuff + ulTtirequestMessageSize);
    for (pudIndex = 0; pudIndex < ulPduNum; pudIndex++){
        pduSize = pduHead->pduSize;
        pduHead = (PduHeadInfo *)((uint8_t *)pduHead + pduSize);
        ulTtirequestMessageSize = ulTtirequestMessageSize + pduSize;/* Add PDU length */
    }

    UlueGoupNumInfo *ulUeGoupNumInfo = (UlueGoupNumInfo *)((uint8_t *)&srcUlSlotMesagesBuff + ulTtirequestMessageSize);
    for (groupIndex = 0; groupIndex < ueGroupNum; groupIndex++){
        ueNumInGroup    = ulUeGoupNumInfo->ueNum;
        ulUeGoupNumInfo = ulUeGoupNumInfo + (sizeof(uint8_t) * (ueNumInGroup + 1));
        ulTtirequestMessageSize = ulTtirequestMessageSize + sizeof(uint8_t) * (ueNumInGroup + 1);/* Add ueGroup length */
    }

    return ulTtirequestMessageSize;
}

void UlTtiRequestPrachPduparse (FapiNrMsgPrachPduInfo *fapiPrachPduInfoIn, L1PrachPduInfo *l1PrachPduOut, uint16_t pudIndex)
{
    uint16_t prgIndex;
    uint8_t  digitalBfNum, digitalBfIndex;
    uint16_t *beamIndex = NULL;

    l1PrachPduOut->pduIndex           = pudIndex;
    l1PrachPduOut->phyCellID          = fapiPrachPduInfoIn->physCellID;
    l1PrachPduOut->prachTdOcasNum     = fapiPrachPduInfoIn->numPrachOcas;
    l1PrachPduOut->prachFormat        = fapiPrachPduInfoIn->prachFormat;
    l1PrachPduOut->PrachFdmIndex      = fapiPrachPduInfoIn->indexFdRa;
    l1PrachPduOut->prachStartSymb     = fapiPrachPduInfoIn->prachStartSymbol;
    l1PrachPduOut->ncsValue           = fapiPrachPduInfoIn->numCs;

    l1PrachPduOut->handle             = fapiPrachPduInfoIn->prachParaInV3.handle;
    l1PrachPduOut->prachCfgScope      = fapiPrachPduInfoIn->prachParaInV3.prachCfgScope;
    l1PrachPduOut->prachResCfgIndex   = fapiPrachPduInfoIn->prachParaInV3.prachResCfgIndex;
    l1PrachPduOut->prachFdmNum        = fapiPrachPduInfoIn->prachParaInV3.numFdRa;
    l1PrachPduOut->startPreambleIndex = fapiPrachPduInfoIn->prachParaInV3.startPreambleIndex;
    l1PrachPduOut->preambleIndicesNum = fapiPrachPduInfoIn->prachParaInV3.numPreambleIndices;

    l1PrachPduOut->trpScheme          = fapiPrachPduInfoIn->rxBeamFormingPara.trpScheme;
    l1PrachPduOut->prgNum             = fapiPrachPduInfoIn->rxBeamFormingPara.numPRGs;
    l1PrachPduOut->prgSize            = fapiPrachPduInfoIn->rxBeamFormingPara.prgSize;
    l1PrachPduOut->digitalBfNum       = fapiPrachPduInfoIn->rxBeamFormingPara.digBfInterface;

    digitalBfNum = l1PrachPduOut->digitalBfNum;
    for (prgIndex = 0; prgIndex < l1PrachPduOut->prgNum; prgIndex++){
        beamIndex = (uint16_t *)&fapiPrachPduInfoIn->rxBeamFormingPara.beamIndex[0] + prgIndex * digitalBfNum;
        for (digitalBfIndex = 0; digitalBfIndex < digitalBfNum; digitalBfIndex++){
            l1PrachPduOut->beamIndex[prgIndex][digitalBfIndex] = *beamIndex;
            beamIndex++;
        }
    }
}

