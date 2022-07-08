#include <stdio.h>
#include <string.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "phyctrl_prach.h"

uint32_t         g_ulTtiMessageTempBuff[2000] = { 0 }; /* ULTTIMessage 本地buffer */
L1PrachParaInfo  g_prachParaInfoOut = { 0 };           /* Prach 参数本地buffer */
UlueGoupNumInfo  g_ulUeGroupNumInfo[8] = { 0 };        /* ULTTIMessage ueGroupinfo 本地buffer */
UlPduMappingInfo g_ulPduMappingInfo[200] = { 0 };      /* 暂时假设有200个PDU */

uint32_t UlCarrierSampleRateCalc(uint16_t ulBwpBandwdith);
uint32_t UlTtiRequestMessageSizeCalc (uint8_t *srcUlSlotMesagesBuff);
uint32_t UlTtiRequestPrachPduparse (FapiNrMsgPrachPduInfo *fapiPrachPduParaIn, L1PrachPduInfo *l1PracPduOut, uint16_t pudIndex);
uint32_t L1PrachParaInfo2LowPhy(L1PrachParaInfo *l1prachParaInfoIn,  L1PrachConfigInfo *l1PrachConfigInfoIn,  L1CarrierCfgInfo *l1CarrierCfgInfoIn, PrachLowPhyHacPara *prachLowPhyParaOut);

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

        l1prachParaInfoOut->prachPduNum = pduCntPerType[0];/* 记录每个上行信道的 PDU Number*/
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


/* Option8 split:  PRACH LowPhy Enable; Option7-2x split:  PRACH LowPhy Disable; */
uint32_t L1PrachParaInfo2LowPhy(L1PrachParaInfo *l1prachParaInfoIn,  L1PrachConfigInfo *l1PrachConfigInfoIn,  L1CarrierCfgInfo *l1CarrierCfgInfoIn, PrachLowPhyHacPara *prachLowPhyParaOut)
{
    L1PrachPduInfo        *l1PrachPduInfo = NULL;
    PrachLowPhyParaPerPdu *LowPhyParaPerPdu = NULL;
    uint16_t prachPduNum, pduIndex;
    uint16_t sfnNum, slotNum;
    uint8_t  prachFdmNumCfg, raFdmNumInPdu, raFdmIdx, loopIdx;
    uint8_t  prachScs, puschScs;
    uint8_t  raRbLenth, raKbar;
    uint8_t  prachcfgIdx;
    uint16_t prachRootIndex[2];
    uint8_t  prachRootNum[2];
    int16_t  freqOffsetK1[2];
    uint8_t  prachZeroCorrCfg[2]; 
    uint16_t prachResCfgIdx;
    uint16_t freqOffsetRb0;
    uint16_t ulBwpBandwdith;
    uint8_t  puschScsValue;
    int32_t  freqShiftValue;
    uint16_t prachScsValue;
    uint32_t sampleRateCarry;
    uint16_t preambleFormat;
    uint32_t repeatTimesOcas;
    uint32_t sampleNuValue;
    uint16_t sampleCpValue;
    uint16_t cpLenth;
    uint16_t fftSizeIndex;
    uint32_t downSampleValue;
    uint8_t  downSampleTimes;

    prachFdmNumCfg = l1PrachConfigInfoIn->numPrachFdOccasions;
    for (loopIdx = 0; loopIdx < prachFdmNumCfg; loopIdx){
        prachRootIndex[loopIdx]   = l1PrachConfigInfoIn->prachRootSequenceIndex[loopIdx];
        prachRootNum[loopIdx]     = l1PrachConfigInfoIn->numRootSequences[loopIdx];
        freqOffsetK1[loopIdx]     = l1PrachConfigInfoIn->k1[loopIdx];
        prachZeroCorrCfg[loopIdx] = l1PrachConfigInfoIn->prachZeroCorrConf[loopIdx];
    }

    prachLowPhyParaOut->prachFeEn = 1;
    prachLowPhyParaOut->sfnNum    = l1prachParaInfoIn->sfnNum;
    prachLowPhyParaOut->slotNum   = l1prachParaInfoIn->slotNum;

    ulBwpBandwdith = l1CarrierCfgInfoIn->bandWidthUl;
    sampleRateCarry = UlCarrierSampleRateCalc(ulBwpBandwdith);

    prachScs       = l1PrachConfigInfoIn->prachSubCSpacing;
    puschScs       = l1PrachConfigInfoIn->ulBwpPuschScs;
    raRbLenth      = g_PreambleforFraKbar[prachScs][puschScs].nRaRB;
    raKbar         = g_PreambleforFraKbar[prachScs][puschScs].raKbar;
    prachScsValue  = g_PreambleforFraKbar[prachScs][puschScs].prachScsValue;
    puschScsValue  = g_PreambleforFraKbar[prachScs][puschScs].puschScsValue;

    prachPduNum    = l1prachParaInfoIn->prachPduNum;
    for (pduIndex = 0; pduIndex < prachPduNum; pduIndex++){/* 2FDM的情况下 可能存在2个PRACH PDU */
        l1PrachPduInfo   = &l1prachParaInfoIn->l1prachPduInfo[pduIndex];
        LowPhyParaPerPdu = &prachLowPhyParaOut->lowPhyParaPerPdu[pduIndex];
    
        /* FDM-x 移频 参数计算 */
        LowPhyParaPerPdu->freqShiftEn     = 1;
        LowPhyParaPerPdu->cellIdx         = l1PrachPduInfo->phyCellID;
        LowPhyParaPerPdu->indexFdRa       = l1PrachPduInfo->PrachFdmIndex;;
        LowPhyParaPerPdu->prachFdmNum     = l1PrachPduInfo->prachFdmNum;
        LowPhyParaPerPdu->numRxAnt        = l1CarrierCfgInfoIn->rxAntNum;
        LowPhyParaPerPdu->sampleRateCarry = sampleRateCarry;
        raFdmIdx = l1PrachPduInfo->PrachFdmIndex;
        raFdmNumInPdu = LowPhyParaPerPdu->prachFdmNum;
        for (loopIdx = 0; loopIdx < raFdmNumInPdu; loopIdx){
            freqOffsetRb0  = freqOffsetK1[loopIdx] + raFdmIdx * raRbLenth;
            freqShiftValue = (freqOffsetRb0 - (ulBwpBandwdith - raRbLenth)>>1) * N_SC_PER_PRB * (puschScsValue/prachScsValue) + raKbar; 
            freqShiftValue = freqShiftValue * prachScsValue * 1000;
            LowPhyParaPerPdu->freqShiftValue[loopIdx] = freqShiftValue;/* 通过资源计算得到每个FDM的移频值*/
        }

        /* 时域AGC参数 */
        LowPhyParaPerPdu->tdDagcEn = 1;
        preambleFormat = l1PrachPduInfo->prachFormat;
        if ((0 <= (preambleFormat - PRACH_FORMAT_A1)) && (PRACH_FORMAT_NUM_139 > (preambleFormat - PRACH_FORMAT_A1))){
            repeatTimesOcas = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].repeatTimesOcas;
            downSampleValue = g_DownSamplingValue139[prachScsValue];
            sampleCpValue   = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].udRaCp >> puschScs;
            sampleNuValue   = 2048 >> puschScs;
            fftSizeIndex    = 1; /* 1 ：256*/
        }
        else if ((0 <= (preambleFormat - PRACH_FORMAT_0)) && (PRACH_FORMAT_NUM_839 > (preambleFormat - PRACH_FORMAT_0))){
            repeatTimesOcas = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].repeatTimesOcas;
            downSampleValue = g_DownSamplingValue839[preambleFormat];
            sampleCpValue   = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].udRaCp;
            sampleNuValue   = 24576;
            fftSizeIndex    = 11;  /* 11 ：1536*/
        }
        LowPhyParaPerPdu->repeatTimesInOcas = repeatTimesOcas; 
        LowPhyParaPerPdu->tdDagcTarget      = 0;/* 待确定后添加 目标AGC因子 */
        LowPhyParaPerPdu->nFFTSample        = sampleNuValue;

        /* 降采样参数 */  
        downSampleTimes = sampleRateCarry / downSampleValue; /* 降采样倍数 */
        LowPhyParaPerPdu->downSamplingEnBitMap = downSampleTimes - 1;/* 按照bit位图配置降采样滤波器 */  
        
        /* 去CP参数 */
        LowPhyParaPerPdu->cpRemoveEn = 1;
        cpLenth = (sampleCpValue * downSampleValue) / 3072000000;
        if (((puschScsValue == 15000) && ((prachLowPhyParaOut->slotNum == 0) || (prachLowPhyParaOut->slotNum == 5)))
           ||((puschScsValue == 30000) && ((prachLowPhyParaOut->slotNum == 0) || (prachLowPhyParaOut->slotNum == 10))))
        {
            cpLenth = cpLenth + (16 * downSampleValue) / 3072000000;
        }
        LowPhyParaPerPdu->cutCpValue = cpLenth;
        
        /* FFT 参数 */
        LowPhyParaPerPdu->fftSizeIndex = fftSizeIndex; /* 根据序列长度 选择FFT点数 */
    }
    
    return 0;
}

uint32_t UlTtiRequestPrachPduparse (FapiNrMsgPrachPduInfo *fapiPrachPduInfoIn, L1PrachPduInfo *l1PrachPduOut, uint16_t pudIndex)
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

uint32_t UlCarrierSampleRateCalc(uint16_t ulBwpBandwdith)
{
    uint32_t sampleRateCarry;

    if(ulBwpBandwdith >= 60){
        sampleRateCarry = 122880000;
    }
    else if(ulBwpBandwdith >= 30){
        sampleRateCarry = 614400000;
    }
    else if(ulBwpBandwdith >= 15){
        sampleRateCarry = 30720000;
    }
    else if(ulBwpBandwdith >= 10){
        sampleRateCarry = 15360000;
    }
    else{
        sampleRateCarry = 7680000;
    }

    return sampleRateCarry;
}
