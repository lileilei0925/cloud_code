#include <stdio.h>
#include <string.h>
#include "../inc/fapi_mac2phy_interface.h"
#include "phyctrl_prach.h"
#include "phyctrl_variable.h"

uint16_t ceil_div(uint16_t a, uint16_t b);
uint32_t UlCarrierSampleRateCalc(uint16_t ulBwpBandwdith);
uint32_t UlTtiRequestMessageSizeCalc(uint8_t *srcUlSlotMesagesBuff);
uint32_t UlTtiRequestPrachPduparse(FapiNrMsgPrachPduInfo *fapiPrachPduParaIn, L1PrachPduInfo *l1PracPduOut, uint16_t pudIndex);
uint32_t L1PrachRxInitParas(L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PracRxParaLocal *pracRxParaLocal);
uint32_t L1PrachParaParse2LowPhy(PracRxParaLocal *pracRxParaLocal, PrachLowPhyHacPara *prachLowPhyParaOut);

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
    FapiNrMsgPrachPduInfo *fapiprachPduParaIn  = NULL;
    L1PrachPduInfo        *l1prachPduInfo = NULL;

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
                    fapiprachPduParaIn = (FapiNrMsgPrachPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    l1prachPduInfo     =  &g_prachParaInfoOut[cellIndex].l1prachPduInfo[pduCntPerType[0]];
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

        l1prachParaPduInfoOut->prachPduNum = pduCntPerType[0]; /* 记录每个上行信道的 PDU Number*/
        //l1PuschParaInfoOut->puschPduNum = pduCntPerType[1];
        //l1PucchParaInfoOut->puschPduNum = pduCntPerType[2];
        //l1SrsParaInfoOut->puschPduNum   = pduCntPerType[3];
        
        /************** pduIndex mapping relation with UE **************/
        UlueGoupNumInfo  *ulUeGoupNumInfo = (UlueGoupNumInfo *)((uint8_t *)pduHead); /* sizeof(uint8_t) * (ulUeGoupNumInfo->ueNum + 1) per Group */
        UlPduMappingInfo *ulPduMappingInfo = &g_UlPduMappingInfo[cellIndex][0];
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

uint32_t L1PrachRxInitParas(L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PracRxParaLocal *pracRxParaLocal)
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
    uint8_t tdOcasIdx, numTdOcas, tdOcasStart;
    uint8_t fdOcasIdx, numFdOccas, fdOcasStart;
    uint8_t tdOcasCnt, fdOcasCnt;

    prachPduNum = l1prachParaPduInfoIn->prachPduNum;
    for (pduIndex = 0; pduIndex < prachPduNum; pduIndex++)
    {
        l1PrachPduInfo = &l1prachParaPduInfoIn->l1prachPduInfo[pduIndex];
        if (pduIndex == 0) {/* TD&FD公共参数只计算一次 */
            cellIndex        = l1PrachPduInfo->phyCellID;   /* 后续需要将PCI转换为 本地L1 L2之间的cellIndex */
            prachResCfgIdx   = l1PrachPduInfo->prachResCfgIndex;            
            pracRxParaLocal->cellIdx          = cellIndex;
            pracRxParaLocal->prachResCfgIdx   = prachResCfgIdx;
            pracRxParaLocal->prachConfigIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachConfigIndex;
            prachScs                          = l1PrachConfigInfoIn[prachResCfgIdx].prachSubCSpacing;
            puschScs                          = l1PrachConfigInfoIn[prachResCfgIdx].ulBwpPuschScs; 
            prachScsValue                     = g_PreambleforFraKbar[prachScs][puschScs].prachScsValue;
            preambleFormat                    = l1PrachPduInfo->prachFormat;
            pracRxParaLocal->prachScs         = prachScs;
            pracRxParaLocal->puschScs         = puschScs; 
            pracRxParaLocal->nRaRB            = g_PreambleforFraKbar[prachScs][puschScs].nRaRB;
            pracRxParaLocal->raKbar           = g_PreambleforFraKbar[prachScs][puschScs].raKbar;
            pracRxParaLocal->prachScsValue    = g_PreambleforFraKbar[prachScs][puschScs].prachScsValue;
            pracRxParaLocal->puschScsValue    = g_PreambleforFraKbar[prachScs][puschScs].puschScsValue;
            pracRxParaLocal->bandWidthUl      = g_CellConfigPara[cellIndex].bandWidthUl;
            pracRxParaLocal->prachRaLength    = l1PrachConfigInfoIn[prachResCfgIdx].prachSequenceLength;
            pracRxParaLocal->nNcs             = l1PrachPduInfo->ncsValue;

            if(pracRxParaLocal->prachRaLength == 0)
            {
                pracRxParaLocal->nfftSize     = 1536; 
                pracRxParaLocal->prachZcSize  = 839;
                repeatTimesOcas = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].repeatTimesOcas;
                downSampleValue = g_DownSamplingValue139[prachScsValue];
                nCpLen          = g_PreambleforLRa139[preambleFormat - PRACH_FORMAT_A1].udRaCp >> puschScs;
            }
            else
            {
                pracRxParaLocal->nfftSize     = 256; 
                pracRxParaLocal->prachZcSize  = 139;
                repeatTimesOcas = g_PreambleforLRa839[preambleFormat - PRACH_FORMAT_0].repeatTimesOcas;
                downSampleValue = g_DownSamplingValue839[prachScsValue];
                nCpLen          = g_PreambleforLRa839[preambleFormat - PRACH_FORMAT_0].udRaCp >> puschScs;
            }

            pracRxParaLocal->repeatTimesInOcas = repeatTimesOcas;
            pracRxParaLocal->downSampleValue   = downSampleValue;
            pracRxParaLocal->nCpLen            = nCpLen;

             /* 时域信息 */
            if (g_CellConfigPara[cellIndex].frameDuplexType = FRAME_DUPLEX_TYPE_FDD){
                durationSymbols = g_PrachCfgTableFR1Fdd[pracRxParaLocal->prachConfigIndex].duration; 
            }
            else{
                durationSymbols = g_PrachCfgTableFR1Tdd[pracRxParaLocal->prachConfigIndex].duration; 
            }
        }
        pracRxParaLocal->numFdOccasCfg = l1PrachConfigInfoIn[prachResCfgIdx].numPrachFdOccasions;
        numTdOcas  = l1PrachPduInfo->prachTdOcasNum;
        pracRxParaLocal->numTdOccas += numTdOcas;
        tdOcasStart = pracRxParaLocal->numTdOccas - numTdOcas;
        tdOcasCnt = 0;
        for (tdOcasIdx = tdOcasStart; tdOcasIdx < numTdOcas; tdOcasIdx++)
        {    
            pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].tdOccasIdx = tdOcasIdx;
            pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].startSymb  = l1PrachPduInfo->prachStartSymb + durationSymbols * tdOcasCnt; 
            
            numFdOccas = l1PrachPduInfo->prachFdmNum;
            pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numFdOccas += numFdOccas;
            fdOcasStart = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numFdOccas - numFdOccas;
            fdOcasCnt = 0;
            for (fdOcasIdx = fdOcasStart; fdOcasIdx < numFdOccas; fdOcasIdx++)
            {
                pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].fdOccasIdx[fdOcasIdx]   = l1PrachPduInfo->PrachFdmIndex + fdOcasCnt;
                pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx]       = l1PrachPduInfo->handle;
                pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqIndex[fdOcasIdx] = l1PrachConfigInfoIn[prachResCfgIdx].prachRootSequenceIndex[l1PrachPduInfo->PrachFdmIndex];
                pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numZcRootSeq[fdOcasIdx] = l1PrachConfigInfoIn[prachResCfgIdx].numRootSequences[l1PrachPduInfo->PrachFdmIndex];
                fdOcasCnt ++;
            }
            tdOcasCnt ++;
        }
    }

    return 0;
}

/* Option8 split:  PRACH LowPhy Enable; Option7-2x split:  PRACH LowPhy Disable; */
uint32_t L1PrachParaParse2LowPhy(PracRxParaLocal *pracRxParaLocal, PrachLowPhyHacPara *prachLowPhyParaOut)
{
    uint16_t ulBwpBandwdith;
    uint32_t sampleRateCarry;
    uint32_t downSampleTimes,downSampleValue;
    uint16_t nCpLength;
    uint16_t puschScsValue,prachScsValue;
    uint8_t  numTdOcas,tdOcasIdx;
    uint8_t  numFdOccas,fdOcasIdx;
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
    prachLowPhyParaOut->sfnNum        = pracRxParaLocal->sfnNum;
    prachLowPhyParaOut->slotNum       = pracRxParaLocal->slotNum;
    prachLowPhyParaOut->cellIdx       = pracRxParaLocal->cellIdx;
    prachLowPhyParaOut->rxAntNum      = pracRxParaLocal->rxAntNum;
    prachLowPhyParaOut->longOrShortRa = pracRxParaLocal->prachRaLength;

    /* 时域AGC */
    prachLowPhyParaOut->tdAgcTarget       = 0;/* 待确定后添加 目标AGC因子 */
    prachLowPhyParaOut->repeatTimesInOcas = pracRxParaLocal->repeatTimesInOcas; 

    /* 降采样参数 */  
    ulBwpBandwdith   = g_CellConfigPara[prachLowPhyParaOut->cellIdx].bandWidthUl;
    sampleRateCarry  = UlCarrierSampleRateCalc(ulBwpBandwdith);
    downSampleTimes = sampleRateCarry / downSampleValue; /* 降采样倍数 */
    prachLowPhyParaOut->downSamplingEnBitMap = downSampleTimes - 1;/* 按照bit位图配置降采样滤波器 */ 

    /* 去CP参数 */
    puschScsValue = pracRxParaLocal->puschScsValue;
    nCpLength     = pracRxParaLocal->nCpLen;
    nCpLength = (nCpLength * downSampleValue) / 3072000000;
    if (((puschScsValue == 15000) && ((prachLowPhyParaOut->slotNum % 5) == 0)) || ((puschScsValue == 30000) && ((prachLowPhyParaOut->slotNum % 10) == 0))){
        nCpLength = nCpLength + (16 * downSampleValue) / 3072000000;
    }
    prachLowPhyParaOut->cutCpLen = nCpLength;

    /* FFT 参数 */
    prachLowPhyParaOut->targetFFTSize = pracRxParaLocal->nfftSize; /* 根据序列长度 选择FFT点数 */

    /* 移频参数 */
    numTdOcas = pracRxParaLocal->numTdOccas;
    prachLowPhyParaOut->numTdOccas = numTdOcas;
    raRbLenth     = pracRxParaLocal->nRaRB;
    raKbar        = pracRxParaLocal->raKbar;
    prachScsValue = pracRxParaLocal->prachScsValue;
    for (tdOcasIdx = 0; tdOcasIdx < numTdOcas; tdOcasIdx++)
    {
        prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].tdOccasIdx = tdOcasIdx;
        prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].startSymb  = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].startSymb; 
        
        numFdOccas = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numFdOccas;
        for (fdOcasIdx = 0; fdOcasIdx < numFdOccas; fdOcasIdx++)
        {
            prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].fdOccasIdx[fdOcasIdx] = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].fdOccasIdx[fdOcasIdx];
            prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].handle[fdOcasIdx]     = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx];
            freqOffsetRb0  = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].nK1[fdOcasIdx];
            freqShiftValue = (freqOffsetRb0 - (ulBwpBandwdith - raRbLenth)>>1) * N_SC_PER_PRB * (puschScsValue/prachScsValue) + raKbar; 
            freqShiftValue = freqShiftValue * prachScsValue * 1000;
            prachLowPhyParaOut->tdFdOcasInfoLowPhy[tdOcasIdx].prachPhaseStep[fdOcasIdx] = freqShiftValue;
            //prachLowPhyParaOut->tdOcasAndFdOcasInfo[tdOcasIdx].outPutIQBuffAddr[fdOcasIdx] = 0x00000000;
            //prachLowPhyParaOut->tdOcasAndFdOcasInfo[tdOcasIdx].addrOffsetPerAnt[fdOcasIdx] = 0x00000000;
        }
    }

    return 0;
}

uint32_t L1PrachParaParse2Rpp(PracRxParaLocal *pracRxParaLocal, PrachRPPHacPara *prachRppParaOut)
{
    PrachTdFdOcasRpp    *prachTdFdOcasRpp  = NULL;
    uint8_t  preambleFormat;
    uint8_t  numTdOcas, tdOcasIdx;
    uint8_t  numFdOccas, fdOcasIdx;
    uint8_t  nOcasCnt;

    prachRppParaOut->sfnNum       = pracRxParaLocal->sfnNum;
    prachRppParaOut->slotNum      = pracRxParaLocal->slotNum;
    prachRppParaOut->cellIdx      = pracRxParaLocal->cellIdx;
    prachRppParaOut->rxAntNum     = pracRxParaLocal->rxAntNum;
    prachRppParaOut->prachLength  = pracRxParaLocal->prachRaLength; 
    prachRppParaOut->numOcasions  = pracRxParaLocal->numTdOccas * pracRxParaLocal->numFdOccasCfg;
    preambleFormat                = pracRxParaLocal->prachFormat;
    
    nOcasCnt = 0;
    for (tdOcasIdx = 0; tdOcasIdx < numTdOcas; tdOcasIdx++)
    {
        numFdOccas = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numFdOccas;
        for (fdOcasIdx = 0; fdOcasIdx < numFdOccas; fdOcasIdx++)
        {
            prachTdFdOcasRpp->symbComEn         = 1;
            prachTdFdOcasRpp->symbComOutputEn   = 1;
            prachTdFdOcasRpp                    = &prachRppParaOut->prachTdFdOcasRpp[nOcasCnt];
            prachTdFdOcasRpp->handle            = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx];
            prachTdFdOcasRpp->numRepeatPerOcas  = g_RepeatforLRA[preambleFormat];
            prachTdFdOcasRpp->rootSeqIndex      = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqIndex[fdOcasIdx];
            prachTdFdOcasRpp->rootSeqLength     = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqLength[fdOcasIdx];
            prachTdFdOcasRpp->numZcRootSeq      = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numZcRootSeq[fdOcasIdx];
            
            prachTdFdOcasRpp->ifftSize         = pracRxParaLocal->nfftSize;

            //prachTdFdOcasRpp->numSymbForComb =  合并的符号数，可以是numRepeatPerOcas的子集
            //prachTdFdOcasRpp->symbComOutputAddr   = 0;
            //prachTdFdOcasRpp->SymbComOutputOffset = 0;
            nOcasCnt ++;
        }
    }

    return 0;   
}

uint32_t L1PrachParaParse2Dsp(PracRxParaLocal *pracRxParaLocal, PrachDetectDspPara *prachDspParaOut)
{
    uint8_t  tdOcasIdx, numTdOcasions;
    uint8_t  fdOcasIdx, numFdOcasions;
    uint16_t rootSeqIdx,rootSeqNum;
    uint16_t nNcs, prachLen, qIdx, qValue, rootIdx;
    uint16_t du;

    prachDspParaOut->sfnNum    = pracRxParaLocal->sfnNum;
    prachDspParaOut->slotNum   = pracRxParaLocal->slotNum;
    prachDspParaOut->cellIdx   = pracRxParaLocal->cellIdx;
    prachDspParaOut->rxAntNum  = pracRxParaLocal->rxAntNum;
    prachDspParaOut->nFftSzie  = pracRxParaLocal->nfftSize;

    /* 噪声估计模块参数 */
    prachDspParaOut->thAlpha     = 0x4CCD; /* 门限系数 浮点值为0.6 对应定点值为 19661*/
    prachDspParaOut->thSingleWin = 0;
    prachDspParaOut->thMultieWin = 0;
    prachDspParaOut->numWin      = 1; /* 检测窗个数，取值1,3,5 */
    
    /*搜索窗位置计算模块 */
    prachDspParaOut->nFftSzie     = pracRxParaLocal->nfftSize;
    prachDspParaOut->nNcs         = pracRxParaLocal->nNcs;
    prachDspParaOut->zcSeqSize    = pracRxParaLocal->prachZcSize;
    
    /* 峰值检测模块 */
    prachDspParaOut->thA = 0;
    prachDspParaOut->thR = 0;
    nNcs = prachDspParaOut->nNcs;
    if (((nNcs == 2)||(nNcs == 4)) && (pracRxParaLocal->prachRaLength == 1))
    {
        prachDspParaOut->numExcursion = 1;
    }
    else if (((nNcs == 6)||(nNcs == 8)||(nNcs == 10)) && (pracRxParaLocal->prachRaLength == 1))
    {
        prachDspParaOut->numExcursion = 2;
    }
    else
    {
        prachDspParaOut->numExcursion = 5;
    }

    numTdOcasions = pracRxParaLocal->numTdOccas;
    for (tdOcasIdx = 0; tdOcasIdx < numTdOcasions; tdOcasIdx++)
    {
        prachDspParaOut->prachTdOcasDsp[tdOcasIdx].tdOcasFirstSym    = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].startSymb;
        prachDspParaOut->prachTdOcasDsp[tdOcasIdx].pdpSeqSize        = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqLength[0];
        prachDspParaOut->prachTdOcasDsp[tdOcasIdx].pdpSeqAddr        = 0x00000000;
        prachDspParaOut->prachTdOcasDsp[tdOcasIdx].pdpSeqPerZcOffSet = 0x00000000;
        numFdOcasions = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numFdOccas;
        for (fdOcasIdx = 0; fdOcasIdx < numFdOcasions; fdOcasIdx++)
        {
            prachDspParaOut->prachTdOcasDsp[tdOcasIdx].handle[fdOcasIdx] = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].handle[fdOcasIdx];
            prachDspParaOut->prachTdOcasDsp[tdOcasIdx].numZcRootSeq[fdOcasIdx]  = pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].numZcRootSeq[fdOcasIdx];
            prachDspParaOut->prachTdOcasDsp[tdOcasIdx].fdmAddrOffset[fdOcasIdx] = 0x00000000;

            rootIdx =  pracRxParaLocal->prachRxTdFdOcasInfo[tdOcasIdx].rootSeqIndex[fdOcasIdx];
            for (qIdx = 0; qIdx < prachDspParaOut->zcSeqSize; qIdx++)
            {
                if (((qIdx*rootIdx) % prachDspParaOut->zcSeqSize) == 1 )
                {
                   qValue = qIdx;
                   break;
                }
            }

            if ((qValue >= 0) && (qValue < (prachDspParaOut->zcSeqSize/2)))
            {
                du = qValue;
            }
            else
            {
                du = prachDspParaOut->zcSeqSize - qValue;
            }

            if(pracRxParaLocal->restrictedSetType == 0)
            {
                if(nNcs == 0)
                {
                    prachDspParaOut->nCv = 0;  
                }
                else
                {
                    prachDspParaOut->nCv = nNcs;
                }
            }
            else
            {

            } 

        }
    }

    return 0;
}

#if 0
uint32_t PhyUlTaskPrach (uint8_t cellIndex, L1PrachConfigInfo *l1PrachConfigInfoIn, FapiPrachIndication *prachIndicator)
{
    PrachLowPhyHacPara *prachLowPhyParaOut = NULL;   
    PrachRPPHacPara    *prachPreProcOut    = NULL;
    L1PrachParaPduInfo *l1prachParaPduInfo = NULL; 
    uint8_t  raFdmNum, raFdmIdx;
   
    l1prachParaPduInfo = &g_prachParaInfoOut[cellIndex];
    if(l1prachParaPduInfo->prachPduNum > 0) 
    {

        //L1PrachParaParse2LowPhy(l1prachParaPduInfo,  l1PrachConfigInfoIn, prachLowPhyParaOut); 
        //LowPhyHacRegisterConfig(cellIndex, prachLowPhyParaOut);
        
        //L1PrachParaParse2PreProc(l1prachParaPduInfo, l1PrachConfigInfoIn, prachPreProcOut);
        //RppHacRegisterConfig(cellIndex, prachPreProcOut);

        
        
        prachIndicator->numSFN   = l1prachParaPduInfo->sfnNum;
        prachIndicator->numSlot  = l1prachParaPduInfo->slotNum;
        prachIndicator->numPdus  = l1prachParaPduInfo->prachPduNum;
        for(int16_t pduIdx = 0; pduIdx <prachIndicator->numPdus; pduIdx++)
        {
            prachIndicator->prachMeasPerPdu[pduIdx].handle = l1prachParaPduInfo->l1prachPduInfo[pduIdx].handle;
            prachIndicator->prachMeasPerPdu[pduIdx].startSymbolIndex = l1prachParaPduInfo->l1prachPduInfo[pduIdx].prachStartSymb;
            
        }
        
        
    }
    else
    {
        /* return Errcode */
    }

    return 0;
}
#endif

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