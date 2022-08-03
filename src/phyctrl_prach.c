#include <stdio.h>
#include <string.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "phyctrl_prach.h"
#include "phyctrl_variable.h"

uint16_t ceil_div(uint16_t a, uint16_t b);
uint32_t UlCarrierSampleRateCalc(uint16_t ulBwpBandwdith);
uint32_t UlTtiRequestMessageSizeCalc(uint8_t *srcUlSlotMesagesBuff);
uint32_t UlTtiRequestPrachPduparse(FapiNrMsgPrachPduInfo *fapiPrachPduParaIn, L1PrachPduInfo *l1PracPduOut, uint16_t pudIndex);
uint32_t L1PrachRxInitParas(L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachRxParaLocal *prachRxParaLocal);
uint32_t L1PrachParaParse2LowPhy(PrachRxParaLocal *prachRxParaLocal, PrachLowPhyHacPara *prachLowPhyParaOut);
uint16_t PrachCvCalc(PrachRxParaLocal *prachRxParaLocal);


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
uint32_t MessageUlTtiRequestParse(uint8_t cellIndex, uint8_t *srcUlSlotMesagesBuff)
{
    FapiNrMsgPrachPduInfo *fapiPrachPduParaIn  = NULL;
    FapiNrMsgPuschPduInfo *fapiPuschPduParaIn  = NULL;
    L1PrachPduInfo        *l1PrachPduInfo = NULL;

    uint16_t sfnNum,slotNum,ulPduNum;
    uint16_t pduNumPerType[MAX_UL_PDU_TYPES] = { 0 };
    uint16_t pduCntPerType[MAX_UL_PDU_TYPES] = { 0 };
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

        /******************** Slot Messages Ul_TTI.request信息 parsing *******************/
        UlTtiRequestHeadInfo *ulRequestHead = (UlTtiRequestHeadInfo *)g_ulTtiMessageTempBuff;
        sfnNum     = ulRequestHead->sfnNum;
        slotNum    = ulRequestHead->slotNum;
        ulPduNum   = ulRequestHead->pduNum;
        ulPduTypes = ulRequestHead->ulPduTypes;
        ueGroupNum = ulRequestHead->ueGroupNum;
        memcpy(&pduNumPerType[0], ulRequestHead->pduNumPerType, sizeof(uint16_t) * MAX_UL_PDU_TYPES);

        L1PrachParaPduInfo  *l1prachParaPduInfoOut = &g_prachParaInfoOut[cellIndex];
        l1prachParaPduInfoOut->sfnNum  = sfnNum;
        l1prachParaPduInfoOut->slotNum = slotNum;

        PduHeadInfo *pduHead = (PduHeadInfo *)((uint8_t *)&g_ulTtiMessageTempBuff[0] + sizeof(UlTtiRequestHeadInfo));
        for (pduIndex = 0; pduIndex < ulPduNum; pduIndex++){
            pduType = pduHead->pduType;
            switch (pduType)
            {
                case UL_PDU_TYPE_PRACH:
                    fapiPrachPduParaIn = (FapiNrMsgPrachPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    l1PrachPduInfo     =  &g_prachParaInfoOut[cellIndex].l1PrachPduInfo[pduCntPerType[0]];
                    UlTtiRequestPrachPduparse (fapiPrachPduParaIn, l1PrachPduInfo, pduIndex);
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

        l1prachParaPduInfoOut->prachPduNum = pduCntPerType[0]; /* 记录每个上行信道的 PDU Number*/
        //l1PuschParaInfoOut->puschPduNum = pduCntPerType[1];
        //l1PucchParaInfoOut->puschPduNum = pduCntPerType[2];
        //l1SrsParaInfoOut->puschPduNum   = pduCntPerType[3];
        
        /************** pduIndex mapping relation with UE **************/
        UlueGoupNumInfo  *ulUeGoupNumInfo = (UlueGoupNumInfo *)((uint8_t *)pduHead); /* sizeof(uint8_t) * (ulUeGoupNumInfo->ueNum + 1) per Group */
        UlPduMappingInfo *ulPduMappingInfo = &g_ulPduMappingInfo[cellIndex][0];
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

uint32_t L1PrachRxParasInit(L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachRxParaLocal *prachRxParaLocal)
{
    L1PrachPduInfo *l1PrachPduInfo = NULL;
    uint8_t prachPduNum, pduIndex;
    uint8_t prachScs, puschScs;
    uint8_t cellIndex;
    uint8_t durationSymbols;
    uint8_t preambleFormat;
    uint8_t repeatTimesOcas;
    uint16_t prachResCfgIdx;
    uint16_t nCpLen;
    uint32_t prachScsValue;
    uint32_t downSampleValue;
    uint8_t tdOcasIdx, numTdOcas;
    uint8_t fdOcasIdx,fdOcasStart, fdOcasCnt;

    prachPduNum = l1prachParaPduInfoIn->prachPduNum;
    prachRxParaLocal->numTdOccas = 0;
    prachRxParaLocal->numFdOccas = 0;

    for (pduIndex = 0; pduIndex < prachPduNum; pduIndex++){
        l1PrachPduInfo = &l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex];
        if (pduIndex == 0) {/* TD&FD公共参数只计算一次 */
            cellIndex        = l1PrachPduInfo->phyCellID;   /* 后续需要将PCI转换为 本地L1 L2之间的cellIndex */
            prachResCfgIdx   = l1PrachPduInfo->prachResCfgIndex;            
            prachRxParaLocal->cellIdx          = cellIndex;
            prachRxParaLocal->prachResCfgIdx   = prachResCfgIdx;
            prachRxParaLocal->prachConfigIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachConfigIndex;
            prachScs                          = l1PrachConfigInfoIn[prachResCfgIdx].prachSubCSpacing;
            puschScs                          = l1PrachConfigInfoIn[prachResCfgIdx].ulBwpPuschScs; 
            prachScsValue                     = g_PreambleforFraKbar[prachScs][puschScs].prachScsValue;
            preambleFormat                    = l1PrachPduInfo->prachFormat;
            prachRxParaLocal->prachScs         = prachScs;
            prachRxParaLocal->puschScs         = puschScs; 
            prachRxParaLocal->nRaRB            = g_PreambleforFraKbar[prachScs][puschScs].nRaRB;
            prachRxParaLocal->raKbar           = g_PreambleforFraKbar[prachScs][puschScs].raKbar;
            prachRxParaLocal->prachScsValue    = g_PreambleforFraKbar[prachScs][puschScs].prachScsValue;
            prachRxParaLocal->puschScsValue    = g_PreambleforFraKbar[prachScs][puschScs].puschScsValue;
            prachRxParaLocal->bandWidthUl      = g_cellConfigPara[cellIndex].bandWidthUl;
            prachRxParaLocal->prachRaLength    = l1PrachConfigInfoIn[prachResCfgIdx].prachSequenceLength;
            prachRxParaLocal->nNcs             = l1PrachPduInfo->ncsValue;
            
            g_prachPduHandle[cellIndex].numPdu =  prachPduNum;

            if(prachRxParaLocal->prachRaLength == 0)
            {
                prachRxParaLocal->nfftSize     = 1536; 
                prachRxParaLocal->prachZcSize  = 839;
                repeatTimesOcas = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].repeatTimesOcas;
                downSampleValue = g_DownSamplingValue139[prachScsValue];
                nCpLen          = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].udRaCp >> puschScs;
            }
            else
            {
                prachRxParaLocal->nfftSize     = 256; 
                prachRxParaLocal->prachZcSize  = 139;
                repeatTimesOcas = g_PreambleforLRa839[preambleFormat - PRACH_FORMAT_0].repeatTimesOcas;
                downSampleValue = g_DownSamplingValue839[prachScsValue];
                nCpLen          = g_PreambleforLRa839[preambleFormat - PRACH_FORMAT_0].udRaCp >> puschScs;
            }

            prachRxParaLocal->repeatTimesInOcas = repeatTimesOcas;
            prachRxParaLocal->downSampleValue   = downSampleValue;
            prachRxParaLocal->nCpLen            = nCpLen;

             /* 时域信息 */
            if (g_cellConfigPara[cellIndex].frameDuplexType = FRAME_DUPLEX_TYPE_FDD){
                durationSymbols = g_PrachCfgTableFR1Fdd[prachRxParaLocal->prachConfigIndex].duration; 
            }
            else{
                durationSymbols = g_PrachCfgTableFR1Tdd[prachRxParaLocal->prachConfigIndex].duration; 
            }

            numTdOcas  = l1PrachPduInfo->prachTdOcasNum;
            for (tdOcasIdx = 0; tdOcasIdx < numTdOcas; tdOcasIdx++){
                prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].tdOccasIdx = tdOcasIdx;
                prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].startSymb  = l1PrachPduInfo->prachStartSymb + durationSymbols * tdOcasIdx; 
            }
        }

        g_prachPduHandle[cellIndex].handle[pduIndex] = l1PrachPduInfo->handle;

        prachRxParaLocal->numFdOccas += l1PrachPduInfo->prachFdmNum;
        fdOcasStart = prachRxParaLocal->numFdOccas - l1PrachPduInfo->prachFdmNum;
        fdOcasCnt = 0;
        for (tdOcasIdx = 0; tdOcasIdx < numTdOcas; tdOcasIdx++){    
            for (fdOcasIdx = fdOcasStart; fdOcasIdx < prachRxParaLocal->numFdOccas; fdOcasIdx++){ /* fdOcasIdx 从0起始 */
                prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].fdOccasIdx[fdOcasIdx]   = l1PrachPduInfo->PrachFdmIndex + fdOcasCnt;
                prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx]       = l1PrachPduInfo->handle;
                prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqIndex[fdOcasIdx] = l1PrachConfigInfoIn[prachResCfgIdx].prachRootSequenceIndex[l1PrachPduInfo->PrachFdmIndex];
                prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numZcRootSeq[fdOcasIdx] = l1PrachConfigInfoIn[prachResCfgIdx].numRootSequences[l1PrachPduInfo->PrachFdmIndex];
                fdOcasCnt++;
            }
        }
    }

    return 0;
}

/* Option8 split:  PRACH LowPhy Enable; Option7-2x split:  PRACH LowPhy Disable; */
uint32_t L1PrachParaParse2LowPhy(PrachRxParaLocal *prachRxParaLocal, PrachLowPhyHacPara *prachLowPhyParaOut)
{
    uint16_t ulBwpBandwdith;
    uint32_t sampleRateCarry;
    uint32_t downSampleTimes,downSampleValue;
    uint16_t nCpLength;
    uint16_t puschScsValue,prachScsValue;
    uint8_t  numTdOcas,tdOcasIdx;
    uint8_t  fdOcasIdx;
    uint32_t freqOffsetRb0;
    float32  freqShiftValue;
    uint16_t raRbLenth, raKbar;

    prachLowPhyParaOut->prachFeEn     = 1;
    prachLowPhyParaOut->freqShiftEn   = 1;
    prachLowPhyParaOut->tdAgcCompEn   = 1;
    prachLowPhyParaOut->vgaCompEn     = 1;
    prachLowPhyParaOut->cpRemoveEn    = 1;
    prachLowPhyParaOut->fftShifEn     = 1;
    prachLowPhyParaOut->fdRssiEn      = 1;
    prachLowPhyParaOut->outIQAlignEn  = 1; 
    prachLowPhyParaOut->sfnNum        = prachRxParaLocal->sfnNum;
    prachLowPhyParaOut->slotNum       = prachRxParaLocal->slotNum;
    prachLowPhyParaOut->cellIdx       = prachRxParaLocal->cellIdx;
    prachLowPhyParaOut->rxAntNum      = prachRxParaLocal->rxAntNum;
    prachLowPhyParaOut->longOrShortRa = prachRxParaLocal->prachRaLength;

    /* 时域AGC */
    prachLowPhyParaOut->tdAgcTarget       = 0;/* 待确定后添加 目标AGC因子 */
    prachLowPhyParaOut->repeatTimesInOcas = prachRxParaLocal->repeatTimesInOcas; 

    /* 降采样参数 */  
    ulBwpBandwdith   = g_cellConfigPara[prachLowPhyParaOut->cellIdx].bandWidthUl;
    downSampleValue  = prachRxParaLocal->downSampleValue;
    sampleRateCarry  = UlCarrierSampleRateCalc(ulBwpBandwdith);
    downSampleTimes  = sampleRateCarry / downSampleValue; /* 降采样倍数 */
    prachLowPhyParaOut->downSamplingEnBitMap = downSampleTimes - 1;/* 按照bit位图配置降采样滤波器 */ 

    /* 去CP参数 */
    puschScsValue = prachRxParaLocal->puschScsValue;
    nCpLength     = prachRxParaLocal->nCpLen;
    nCpLength = (nCpLength * downSampleValue) / 3072000000;
    if (((puschScsValue == 15000) && ((prachLowPhyParaOut->slotNum % 5) == 0)) || ((puschScsValue == 30000) && ((prachLowPhyParaOut->slotNum % 10) == 0))){
        nCpLength = nCpLength + (16 * downSampleValue) / 3072000000;
    }
    prachLowPhyParaOut->cutCpLen = nCpLength;

    /* FFT 参数 */
    prachLowPhyParaOut->targetFFTSize = prachRxParaLocal->nfftSize; /* 根据序列长度 选择FFT点数 */

    /* 移频参数 */
    numTdOcas = prachRxParaLocal->numTdOccas;
    prachLowPhyParaOut->numTdOccas = numTdOcas;
    raRbLenth     = prachRxParaLocal->nRaRB;
    raKbar        = prachRxParaLocal->raKbar;
    prachScsValue = prachRxParaLocal->prachScsValue;
    for (tdOcasIdx = 0; tdOcasIdx < numTdOcas; tdOcasIdx++)
    {
        prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].tdOccasIdx = tdOcasIdx;
        prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].startSymb  = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].startSymb; 
        
        for (fdOcasIdx = 0; fdOcasIdx < prachRxParaLocal->numFdOccas; fdOcasIdx++)
        {
            prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].fdOccasIdx[fdOcasIdx] = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].fdOccasIdx[fdOcasIdx];
            prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].handle[fdOcasIdx]     = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx];
            freqOffsetRb0  = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].nK1[fdOcasIdx];
            freqShiftValue = (freqOffsetRb0 - (ulBwpBandwdith - raRbLenth)>>1) * N_SC_PER_PRB * (puschScsValue/prachScsValue) + raKbar; 
            freqShiftValue = freqShiftValue * prachScsValue * 1000;
            prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].prachPhaseStep[fdOcasIdx] = freqShiftValue;
            //prachLowPhyParaOut->tdOcasAndFdOcasInfo[tdOcasIdx].outPutIQBuffAddr[fdOcasIdx] = 0x00000000;
            //prachLowPhyParaOut->tdOcasAndFdOcasInfo[tdOcasIdx].addrOffsetPerAnt[fdOcasIdx] = 0x00000000;
        }
    }

    return 0;
}

uint32_t L1PrachParaParse2Rpp(PrachRxParaLocal *prachRxParaLocal, PrachRPPHacPara *prachRppParaOut)
{
    PrachTdFdOcasRpp    *prachTdFdOcasRpp  = NULL;
    uint8_t  preambleFormat;
    uint8_t  tdOcasIdx, fdOcasIdx, rootIdx;

    prachRppParaOut->sfnNum           = prachRxParaLocal->sfnNum;
    prachRppParaOut->slotNum          = prachRxParaLocal->slotNum;
    prachRppParaOut->cellIdx          = prachRxParaLocal->cellIdx;
    prachRppParaOut->rxAntNum         = prachRxParaLocal->rxAntNum;
    prachRppParaOut->prachLength      = prachRxParaLocal->prachRaLength;
    prachRppParaOut->ifftSize         = prachRxParaLocal->nfftSize;
    prachRppParaOut->numRepeatPerOcas = prachRxParaLocal->repeatTimesInOcas;
    prachRppParaOut->numSymbForComb   = prachRppParaOut->numRepeatPerOcas; /* 需要合并的符号数是 prachRppParaOut->numRepeatPerOcas 的子集 */
    prachRppParaOut->symbComEn        = 1;
    prachRppParaOut->symbComOutputEn  = 1;
    prachRppParaOut->numTdOccas   = prachRxParaLocal->numTdOccas;
    prachRppParaOut->numFdOccas   = prachRxParaLocal->numFdOccas;
    
    for (tdOcasIdx = 0; tdOcasIdx < prachRppParaOut->numTdOccas; tdOcasIdx++)
    {
        for (fdOcasIdx = 0; fdOcasIdx < prachRppParaOut->numFdOccas; fdOcasIdx++)
        {
            prachTdFdOcasRpp               = &prachRppParaOut->prachTdFdOcasRpp[tdOcasIdx][fdOcasIdx];
            prachTdFdOcasRpp->handle       = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx];
            prachTdFdOcasRpp->rootSeqIndex = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqIndex[fdOcasIdx];
            prachTdFdOcasRpp->numZcRootSeq = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numZcRootSeq[fdOcasIdx];

            if(prachRppParaOut->prachLength = 0){
                prachTdFdOcasRpp->inputZcBuffAddr = 0x00000001; /* 输入根序列 DDr Buffer首地址， 长序列地址 */ 
            }
            else{
                prachTdFdOcasRpp->inputZcBuffAddr = 0x00000002; /* 输入根序列 DDr Buffer首地址， 短序列地址 */ 
            }
            
            prachTdFdOcasRpp->SymbComOutputOffset = prachRppParaOut->prachLength;
            prachTdFdOcasRpp->symbComOutputAddr   = 0x00000003; /* 输出符号合并天线合并后的时域数据 Addr[SlotIdx%2][CellIdx][TdmIdx][FdmIdx]*/
            for (rootIdx = 0; rootIdx < 0; rootIdx++){
                prachTdFdOcasRpp->symbComOutputAddr += rootIdx * prachTdFdOcasRpp->SymbComOutputOffset;
            }   
        }
    }

    return 0;   
}

uint32_t L1PrachParaParse2Dsp(PrachRxParaLocal *prachRxParaLocal, PrachDetectDspPara *prachDspParaOut)
{
    uint8_t  tdOcasIdx, fdOcasIdx;
    uint16_t rootSeqIdx,rootSeqNum;
    uint16_t nNcs, nCv;

    prachDspParaOut->sfnNum    = prachRxParaLocal->sfnNum;
    prachDspParaOut->slotNum   = prachRxParaLocal->slotNum;
    prachDspParaOut->cellIdx   = prachRxParaLocal->cellIdx;
    prachDspParaOut->rxAntNum  = prachRxParaLocal->rxAntNum;
    prachDspParaOut->nFftSzie  = prachRxParaLocal->nfftSize;

    /* 噪声估计模块参数 */
    prachDspParaOut->thAlpha     = 0x4CCD; /* 门限系数 浮点值为0.6 对应定点值为 19661*/
    prachDspParaOut->thSingleWin = 0;      /* 系数暂时还没有，待算法更新 */
    prachDspParaOut->thMultieWin = 0;      /* 系数暂时还没有，待算法更新 */

    if (prachRxParaLocal->restrictedSetType == 0){
       prachDspParaOut->numWin = 1; 
    }
    else if(prachRxParaLocal->restrictedSetType == 1){
       prachDspParaOut->numWin = 3; 
    }
    else{
       prachDspParaOut->numWin = 5; 
    }
    
    /*搜索窗位置计算模块 */
    prachDspParaOut->nFftSzie     = prachRxParaLocal->nfftSize;
    prachDspParaOut->nNcs         = prachRxParaLocal->nNcs;
    prachDspParaOut->zcSeqSize    = prachRxParaLocal->prachZcSize;
    
    /* 峰值检测模块 */
    prachDspParaOut->thA = 0;
    prachDspParaOut->thR = 0;
    nNcs = prachDspParaOut->nNcs;
    if (((nNcs == 2)||(nNcs == 4)) && (prachRxParaLocal->prachRaLength == 1)){
        prachDspParaOut->numExcursion = 1;
    }
    else if (((nNcs == 6)||(nNcs == 8)||(nNcs == 10)) && (prachRxParaLocal->prachRaLength == 1)){
        prachDspParaOut->numExcursion = 2;
    }
    else{
        prachDspParaOut->numExcursion = 5;
    }

    nCv = PrachCvCalc(prachRxParaLocal);

    for (tdOcasIdx = 0; tdOcasIdx < prachRxParaLocal->numTdOccas; tdOcasIdx++)
    {
        prachDspParaOut->prachTdOcasDsp[tdOcasIdx].tdOcasFirstSym    = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].startSymb;
        prachDspParaOut->prachTdOcasDsp[tdOcasIdx].pdpSeqAddr        = 0x00000000;
        for (fdOcasIdx = 0; fdOcasIdx < prachRxParaLocal->numFdOccas; fdOcasIdx++)
        {
            //prachDspParaOut->prachTdOcasDsp[tdOcasIdx].pdpSeqPerZcOffSet = 0x00000000;
            prachDspParaOut->prachTdOcasDsp[tdOcasIdx].handle[fdOcasIdx] = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx];
            prachDspParaOut->prachTdOcasDsp[tdOcasIdx].numZcRootSeq[fdOcasIdx]  = prachRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numZcRootSeq[fdOcasIdx]; 
            //prachDspParaOut->prachTdOcasDsp[tdOcasIdx].fdmAddrOffset[fdOcasIdx] = 0x00000000;
        }
    }

    return 0;
}

uint32_t UlTtiRequestPrachPduparse(FapiNrMsgPrachPduInfo *fapiPrachPduInfoIn, L1PrachPduInfo *l1PrachPduOut, uint16_t pudIndex)
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

    l1PrachPduOut->trpScheme          = fapiPrachPduInfoIn->rxBeamFormingInfo.trpScheme;
    l1PrachPduOut->prgNum             = fapiPrachPduInfoIn->rxBeamFormingInfo.numPRGs;
    l1PrachPduOut->prgSize            = fapiPrachPduInfoIn->rxBeamFormingInfo.prgSize;
    l1PrachPduOut->digitalBfNum       = fapiPrachPduInfoIn->rxBeamFormingInfo.digBfInterface;

    digitalBfNum = l1PrachPduOut->digitalBfNum;
    for (prgIndex = 0; prgIndex < l1PrachPduOut->prgNum; prgIndex++){
        beamIndex = (uint16_t *)&fapiPrachPduInfoIn->rxBeamFormingInfo.beamIndex[0] + prgIndex * digitalBfNum;
        for (digitalBfIndex = 0; digitalBfIndex < digitalBfNum; digitalBfIndex++){
            l1PrachPduOut->beamIndex[prgIndex][digitalBfIndex] = *beamIndex;
            beamIndex++;
        }
    }

    return 0;
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

uint16_t PrachCvCalc(PrachRxParaLocal *prachRxParaLocal)
{
    uint16_t rootIdx;
    uint16_t raLength;
    uint16_t qIdx, qValue, du;
    uint16_t nNcs, nCv;
    uint16_t nRaShift, dStart, nRaGroup, nBarRaShift;
    uint16_t wRestrictedA;

    rootIdx  = prachRxParaLocal->prachRxTdFdOcasInfo[0].rootSeqIndex[0];
    raLength = prachRxParaLocal->prachZcSize;
    nNcs     = prachRxParaLocal->nNcs;
    for (qIdx = 0; qIdx < raLength; qIdx++){
        if (((qIdx*rootIdx) % raLength) == 1 ){
            qValue = qIdx;
            break;
        }
    }

    if ((qValue >= 0) && (qValue < (raLength >> 1))){
        du = qValue;
    }
    else{
        du = raLength - qValue;
    }

    if(prachRxParaLocal->restrictedSetType == 0){
        if(nNcs == 0){
            nCv = 0;  
        }
        else{
            nCv = nNcs;
        }
    }
    else if(prachRxParaLocal->restrictedSetType == 1){
        if((nNcs <= du) && (du < (raLength/3))){
            nRaShift    = du / nNcs;
            dStart      = 2 * du + nRaShift * nNcs;
            nRaGroup    = raLength / dStart;
            nBarRaShift = (raLength - 2 * du - nRaGroup * dStart) / nNcs;
            if(nBarRaShift <= 0){
                nBarRaShift = 0;
            }
        }
        else if (((raLength / 3) <= du) && (du <= ((raLength - nNcs) / 2))){
            nRaShift    = (raLength - 2 * du) / nNcs;
            dStart      = raLength - 2 * du + nRaShift * nNcs;
            nRaGroup    = du / dStart;
            nBarRaShift = (du - nRaGroup * dStart) / nNcs;
            if(nBarRaShift <= 0){
                nBarRaShift = 0;
            }
            if(nBarRaShift > nRaShift){
                nBarRaShift = nRaShift;
            }
        }
        wRestrictedA =  nRaShift * nRaGroup + nBarRaShift;

    }
    else
    {
        return 0xFFFF;
    } 

    return nCv;
}

uint16_t ceil_div(uint16_t a, uint16_t b)
{
	uint16_t c = a / b;
	if (a > b * c)
	{
		return (c + 1);
	}
	else
	{
		return c;
	}
}
