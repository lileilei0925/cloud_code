#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../../../../common/inc/phy_ctrl_common.h"
#include "../../../../common/src/common.c"

#include "../inc/phyctrl_prach.h"
#include "../inc/prach_variable.h"

uint32_t UlCarrierSampleRateCalc(uint16_t ulBwpBandwdith);
uint32_t L1PrachParaParse2LowPhyHac(uint8_t uwCellIdx, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachLowPhyHacPara *prachLowPhyParaOut);
uint32_t L1PrachParaParse2RppHac(uint8_t uwCellIdx, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachRPPHacPara *prachRppParaOut);
uint32_t L1PrachParaParse2Dsp(uint8_t uwCellIdx, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachDetectDspPara *prachDspParaOut);
uint32_t PrachWinStartAndEndCalc(uint8_t restrictedSetCfg, uint8_t rootSeqNum, uint16_t rootSeqIndex, uint16_t raLength, uint16_t nNcs, NrPrachRootDetWin *prachRootDetWin);

#if 0
int main(void)
{
  uint16_t a = 16;
  uint16_t b = 16;
  uint16_t c = 0;

  printf("c = %d;\n",a+b+1);
  printf("___Hello World___;\n");

  return 0;
}
#endif


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

uint32_t PrachWinStartAndEndCalc(uint8_t restrictedSetCfg, uint8_t rootSeqNum, uint16_t rootSeqIndex, uint16_t raLength, uint16_t nNcs, NrPrachRootDetWin *prachRootDetWin)
{
    uint16_t vIdx, qIdx, qValue, du;
    uint16_t nRaShift, dStart, nRaGroup, nBarRaShift;
    uint16_t wRestrictedA, preambleNum;
    uint16_t rootIndex, fftDdc;
    uint16_t cyclicShift;
    uint16_t winStart, winEnd, winLen;
    uint16_t winLenMax[3] = { 0 };
    uint64_t tempWinStart, tempWinEnd;
    uint64_t recip2LraLen;
    uint16_t saturateUint16 = 65535; 
    uint16_t *raAlgDuValue  = NULL;
     
#if 0 
    for (qIdx = 0; qIdx < raLength; qIdx++){
        if (((qIdx * rootSeqIndex) % raLength) == 1 ){
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
#endif

    recip2LraLen = (raLength == PRACH_ZC_LEN_839) ? 0x00270E57 : 0x00EBBDB3; 
    fftDdc       = (raLength == PRACH_ZC_LEN_839) ? 1536 : 256;
    for (rootIndex = 0; rootIndex < rootSeqNum; rootSeqNum++){
        if (restrictedSetCfg == UNRESTRICTED){
            preambleNum =  (nNcs == 0) ? 1 : raLength / nNcs; 
            prachRootDetWin[rootIndex].preambleNum = preambleNum;
            
            for (vIdx = 0; vIdx < preambleNum; vIdx++){
                cyclicShift = vIdx * nNcs;
                winStart = (raLength - cyclicShift) % raLength;
                winEnd   = winStart + nNcs - 1;
                tempWinStart = ((2 * winStart * fftDdc + raLength) * recip2LraLen) >> 32;
                tempWinEnd   = ((2 * winEnd   * fftDdc + raLength) * recip2LraLen) >> 32;
                winStart = (tempWinStart > saturateUint16) ? saturateUint16 : (uint16_t)tempWinStart;
                winEnd   = (tempWinEnd   > saturateUint16) ? saturateUint16 : (uint16_t)tempWinEnd;
                winLen   = winEnd - winStart + 1;
                prachRootDetWin[rootIndex].detWin[0].winStart[vIdx] = winStart;
                prachRootDetWin[rootIndex].detWin[0].winEnd[vIdx]   = winEnd;
                prachRootDetWin[rootIndex].detWin[0].winLen[vIdx]   = winLen; 
            }
            prachRootDetWin[rootIndex].detWin[0].maxWinLength   = winLen; /* 非限制集类型 所有窗长一样，不需要计算所有窗的最大值 */
        }
        else if (restrictedSetCfg == RESTRICTED_TYPE_A){
            /* du 使用算法提供值 */
            raAlgDuValue = (raLength == PRACH_ZC_LEN_839) ? &g_prachDuValue839[0] : &g_prachDuValue139[0]; 
            du = raAlgDuValue[rootSeqIndex + rootIndex];
            if ((nNcs <= du) && (du < (raLength / 3))){
                nRaShift    = du / nNcs;
                dStart      = 2 * du + nRaShift * nNcs;
                nRaGroup    = raLength / dStart;
                nBarRaShift = (raLength - 2 * du - nRaGroup * dStart) / nNcs;
                if (nBarRaShift <= 0){
                    nBarRaShift = 0;
                }
            }
            else if (((raLength / 3) <= du) && (du <= ((raLength - nNcs) / 2))){
                nRaShift    = (raLength - 2 * du) / nNcs;
                dStart      = raLength - 2 * du + nRaShift * nNcs;
                nRaGroup    = du / dStart;
                nBarRaShift = (du - nRaGroup * dStart) / nNcs;
                if (nBarRaShift <= 0){
                    nBarRaShift = 0;
                }
                if (nBarRaShift > nRaShift){
                    nBarRaShift = nRaShift;
                }
            }
            else { 
                // printf("Not support Restricted Set B!\n");
            }

            preambleNum =  nRaShift * nRaGroup + nBarRaShift;
            for (vIdx = 0; vIdx < preambleNum; vIdx++){
                cyclicShift = dStart * (vIdx / nRaShift) + (vIdx % nRaShift) * nNcs;
                
                /* 主窗 */
                winStart = (raLength - cyclicShift) % raLength;
                winEnd   = winStart + nNcs - 1;
                tempWinStart = ((2 * winStart * fftDdc + raLength) * recip2LraLen) >> 32;
                tempWinEnd   = ((2 * winEnd   * fftDdc + raLength) * recip2LraLen) >> 32;
                winStart = (tempWinStart > saturateUint16) ? saturateUint16 : (uint16_t)tempWinStart;
                winEnd   = (tempWinEnd   > saturateUint16) ? saturateUint16 : (uint16_t)tempWinEnd;
                winLen   = winEnd - winStart + 1;
                prachRootDetWin[rootIndex].detWin[0].winStart[vIdx] = winStart;
                prachRootDetWin[rootIndex].detWin[0].winEnd[vIdx]   = winEnd;
                prachRootDetWin[rootIndex].detWin[0].winLen[vIdx]   = winLen; 
                if (winLenMax[0] < winLen){
                   winLenMax[0] = winLen;
                }

                /* 左窗 */
                winStart = (raLength - cyclicShift - du) % raLength;
                winEnd   = winStart + nNcs - 1;
                tempWinStart = ((2 * winStart * fftDdc + raLength) * recip2LraLen) >> 32;
                tempWinEnd   = ((2 * winEnd   * fftDdc + raLength) * recip2LraLen) >> 32;
                winStart = (tempWinStart > saturateUint16) ? saturateUint16 : (uint16_t)tempWinStart;
                winEnd   = (tempWinEnd   > saturateUint16) ? saturateUint16 : (uint16_t)tempWinEnd;
                winLen   = winEnd - winStart + 1;
                prachRootDetWin[rootIndex].detWin[1].winStart[vIdx] = winStart;
                prachRootDetWin[rootIndex].detWin[1].winEnd[vIdx]   = winEnd;
                prachRootDetWin[rootIndex].detWin[1].winLen[vIdx]   = winLen; 
                if (winLenMax[1] < winLen){
                   winLenMax[1] = winLen;
                }

                /* 右窗 */
                winStart = (raLength - cyclicShift + du) % raLength;
                winEnd   = winStart + nNcs - 1;
                tempWinStart = ((2 * winStart * fftDdc + raLength) * recip2LraLen) >> 32;
                tempWinEnd   = ((2 * winEnd   * fftDdc + raLength) * recip2LraLen) >> 32;
                winStart = (tempWinStart > saturateUint16) ? saturateUint16 : (uint16_t)tempWinStart;
                winEnd   = (tempWinEnd   > saturateUint16) ? saturateUint16 : (uint16_t)tempWinEnd;
                winLen   = winEnd - winStart + 1;
                prachRootDetWin[rootIndex].detWin[2].winStart[vIdx] = winStart;
                prachRootDetWin[rootIndex].detWin[2].winEnd[vIdx]   = winEnd;
                prachRootDetWin[rootIndex].detWin[2].winLen[vIdx]   = winLen; 
                if (winLenMax[2] < winLen){
                   winLenMax[2] = winLen;
                }
            }
            prachRootDetWin[rootIndex].detWin[0].maxWinLength =  winLenMax[0];
            prachRootDetWin[rootIndex].detWin[1].maxWinLength =  winLenMax[1];
            prachRootDetWin[rootIndex].detWin[2].maxWinLength =  winLenMax[2];
        }  
    }

    return 0;
}

/* Option8 split:PRACH LowPhy Enable;  Option7-2x split: PRACH LowPhy Disable; */
uint32_t L1PrachParaParse2LowPhyHac(uint8_t cellIndex, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachLowPhyHacPara *prachLowPhyParaOut)
{
    uint8_t  pduNum, pduIndex;
    uint8_t  prachConfigIndex, preambleFormat, prachLengthFlag;
    uint8_t  fdOcasIdx, fdOcasNum, fdLoop, tdOccasNum, fdOccasCnt;
    uint8_t  prachScs, puschScs, repeatTimesInOcas;
    uint16_t ulBwpBandwdith, prachRaLength;
    uint16_t puschScsValue,prachScsValue;
    uint16_t raRbNum, raKbar;
    uint16_t prachResCfgIdx;
    uint32_t sampleRateCarry;
    uint32_t downSampleTimes,downSampleValue;
    uint32_t freqOffsetRb0, freqShiftValue;
    uint32_t dataOffsetPerFdOccas, dataOffsetPerAnt; 

    prachLowPhyParaOut->sfnIndex  = l1prachParaPduInfoIn->sfnIndex;
    prachLowPhyParaOut->slotIndex = l1prachParaPduInfoIn->slotIndex;
    prachLowPhyParaOut->cellIdx   = cellIndex;
    prachLowPhyParaOut->rxAntNum  = g_cellConfigPara[cellIndex].rxAntNum;
    prachLowPhyParaOut->prachLowPhyEn = 1;
    prachLowPhyParaOut->vgaCompEn = 1;
    prachLowPhyParaOut->agcUintDb = 0xFF; /* 待定 */
    prachLowPhyParaOut->agcAutoSelect = 1;
    prachLowPhyParaOut->gainManualRatio = 0xFF; /* 待定 */
    
    pduNum           = l1prachParaPduInfoIn->prachPduNum;
    prachResCfgIdx   = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachResCfgIndex;
    preambleFormat   = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachFormat;
    prachConfigIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachConfigIndex;
    prachScs         = l1PrachConfigInfoIn[prachResCfgIdx].prachSubCSpacing;
    puschScs         = l1PrachConfigInfoIn[prachResCfgIdx].ulBwpPuschScs; 
    prachLengthFlag  = l1PrachConfigInfoIn[prachResCfgIdx].prachSequenceLength;
    prachScsValue    = g_preambleforFraKbar[prachScs][puschScs].prachScsValue;
    puschScsValue    = g_preambleforFraKbar[prachScs][puschScs].puschScsValue;
    
    prachLowPhyParaOut->timeDagcEn  = 1;
    prachLowPhyParaOut->tdAgcTarget = 0xFF; /* 待定 */
    prachLowPhyParaOut->fftIfftFlag = 0;
    if (prachLengthFlag == 0){
        prachLowPhyParaOut->targetFFTSize = PRACH_FFT_LEN_1536;
        prachLowPhyParaOut->raLengthFlag  = PRACH_FFT_LEN_1536;
        prachLowPhyParaOut->repeatTimesInOcas = g_preambleforLRa839[preambleFormat - PRACH_FORMAT_0].repeatTimesOcas;
        downSampleValue = g_downSamplingValue139[prachScsValue]; 
        prachRaLength   = PRACH_ZC_LEN_839;
    }
    else{
        prachLowPhyParaOut->targetFFTSize = PRACH_FFT_LEN_256; 
        prachLowPhyParaOut->raLengthFlag  = PRACH_FFT_LEN_256;
        prachLowPhyParaOut->repeatTimesInOcas = g_preambleforLRa139[preambleFormat - PRACH_FORMAT_A1].repeatTimesOcas;
        downSampleValue = g_downSamplingValue839[prachScsValue];
        prachRaLength   = PRACH_ZC_LEN_139;
    }
    repeatTimesInOcas = prachLowPhyParaOut->repeatTimesInOcas;

    /* 降采样 去CP */ 
    ulBwpBandwdith  = g_cellConfigPara[cellIndex].bandWidthUl;
    sampleRateCarry = UlCarrierSampleRateCalc(ulBwpBandwdith);
    downSampleTimes = sampleRateCarry / downSampleValue; /* 降采样倍数 */
    prachLowPhyParaOut->downSamplingEnBitMap = downSampleTimes - 1;/* 按照bit位图配置降采样滤波器 */  // 滤波器系数待定
    prachLowPhyParaOut->prachFormat = preambleFormat;
    prachLowPhyParaOut->rssiCalcEn  = 1;
    
    tdOccasNum = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachTdOcasNum;
    prachLowPhyParaOut->tdOccasNum  = tdOccasNum;
    prachLowPhyParaOut->startSymbol = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachStartSymb;
    raRbNum = g_preambleforFraKbar[prachScs][puschScs].nRaRB;
    raKbar  = g_preambleforFraKbar[prachScs][puschScs].raKbar;
    dataOffsetPerFdOccas = tdOccasNum * repeatTimesInOcas * (prachRaLength + 1) * g_cellConfigPara[cellIndex].rxAntNum;
    dataOffsetPerAnt = tdOccasNum * repeatTimesInOcas * prachRaLength;
    
    /* 移频参数计算和输出地址指示*/
    fdOccasCnt = 0;
    for (pduIndex = 0; pduIndex < pduNum; pduIndex){
        fdOcasNum = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachFdmNum;
        fdOcasIdx = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].PrachFdmIndex;
        for (fdLoop = fdOcasIdx; fdLoop < fdOcasIdx + fdOcasNum; fdLoop++){
            freqOffsetRb0  = l1PrachConfigInfoIn[prachResCfgIdx].k1[fdLoop];
            freqShiftValue = (freqOffsetRb0 - (ulBwpBandwdith - raRbNum)>>1) * N_SC_PER_PRB * (puschScsValue / prachScsValue) + raKbar; /* ulBwpBandwdith 修改 */
            freqShiftValue = freqShiftValue * prachScsValue * 1000;
            prachLowPhyParaOut->fdOcasInfoLowPhy[fdOccasCnt].handle = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].handle;
            prachLowPhyParaOut->fdOcasInfoLowPhy[fdOccasCnt].prachPhaseStep      = freqShiftValue;
            *(prachLowPhyParaOut->fdOcasInfoLowPhy[fdOccasCnt].outPutIQBuffAddr) = 0x00000000 + fdLoop * dataOffsetPerFdOccas; /* 0x00000000为Lowphy输出首地址待定，配置每个FD Occasion的首地址 */
            prachLowPhyParaOut->fdOcasInfoLowPhy[fdOccasCnt].addrOffsetPerAnt    = dataOffsetPerAnt;  /* 每个FD下 没根天线的偏移量 */
            fdOccasCnt++;
        }
        prachLowPhyParaOut->fdOccasNum = fdOccasCnt;
    }

    return 0;
}

uint32_t L1PrachParaParse2RppHac(uint8_t cellIndex, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachRPPHacPara *prachRppParaOut)
{
    uint8_t  pduNum, pduIndex, fdOccasCnt;
    uint8_t  prachConfigIndex, preambleFormat, prachLengthFlag;
    uint8_t  fdOcasIdx, fdOcasNum, fdLoop, tdOccasNum;
    uint8_t  repeatTimesInOcas, rooSeqtNum;
    uint16_t prachResCfgIdx;
    uint16_t prachRaLength;
    uint32_t dataInOffsetPerFdOccas, dataInOffsetPerAnt; 
    uint32_t dataOutOffsetPerFdOccas, dataOutOffsetPerTdOccas;
   
    prachRppParaOut->sfnIndex  = l1prachParaPduInfoIn->sfnIndex;
    prachRppParaOut->slotIndex = l1prachParaPduInfoIn->slotIndex;
    prachRppParaOut->cellIdx   = cellIndex;
    prachRppParaOut->rxAntNum  = g_cellConfigPara[cellIndex].rxAntNum;
    
    pduNum           = l1prachParaPduInfoIn->prachPduNum;
    prachResCfgIdx   = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachResCfgIndex;
    preambleFormat   = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachFormat;
    prachConfigIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachConfigIndex;
    prachLengthFlag  = l1PrachConfigInfoIn[prachResCfgIdx].prachSequenceLength;

    if (prachLengthFlag == 0){
        prachRppParaOut->ifftSize  = PRACH_FFT_LEN_1536;
        prachRppParaOut->repeatNumPerOcas = g_preambleforLRa839[preambleFormat - PRACH_FORMAT_0].repeatTimesOcas;
        *(prachRppParaOut->rootSeqBaseAddr) = 0x00000839;
        prachRaLength = PRACH_ZC_LEN_839;
    }
    else{
        prachRppParaOut->ifftSize  = PRACH_FFT_LEN_256; 
        prachRppParaOut->repeatNumPerOcas = g_preambleforLRa139[preambleFormat - PRACH_FORMAT_A1].repeatTimesOcas;
        *(prachRppParaOut->rootSeqBaseAddr) = 0x00000139;
        prachRaLength = PRACH_ZC_LEN_139;
    }
    repeatTimesInOcas = prachRppParaOut->repeatNumPerOcas;
    prachRppParaOut->prachLength = prachRaLength;
    prachRppParaOut->symbComEn   = 1;
  
    prachRppParaOut->tdOccasNum = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachTdOcasNum;
    tdOccasNum = prachRppParaOut->tdOccasNum;
    dataInOffsetPerFdOccas  = tdOccasNum * repeatTimesInOcas * (prachRaLength + 1) * g_cellConfigPara[cellIndex].rxAntNum;
    dataInOffsetPerAnt      = tdOccasNum * repeatTimesInOcas * prachRaLength;
    
    fdOccasCnt = 0;
    for (pduIndex = 0; pduIndex < pduNum; pduIndex){
        fdOcasNum = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachFdmNum;
        fdOcasIdx = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].PrachFdmIndex;
        for (fdLoop = fdOcasIdx; fdLoop < fdOcasIdx + fdOcasNum; fdLoop++){
            prachRppParaOut->prachFdOcasRpp[fdOccasCnt].handle       = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].handle;      
            prachRppParaOut->prachFdOcasRpp[fdOccasCnt].rootSeqIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachRootSequenceIndex[fdLoop];
            prachRppParaOut->prachFdOcasRpp[fdOccasCnt].rootSeqNum   = l1PrachConfigInfoIn[prachResCfgIdx].rootSequenceNum[fdLoop];
            *(prachRppParaOut->prachFdOcasRpp[fdOccasCnt].inPutIqDataAddr) = 0x00000000 + fdLoop * dataInOffsetPerFdOccas; /* 0x00000000为Lowphy输出首地址待定，然后给出每个FD Occasion的首地址 */
            prachRppParaOut->prachFdOcasRpp[fdOccasCnt].inPutOffsetPerAnt  = dataInOffsetPerAnt;  /* 每个FD下 每根天线的偏移量 */
            
            rooSeqtNum = prachRppParaOut->prachFdOcasRpp[fdOccasCnt].rootSeqNum;
            dataOutOffsetPerFdOccas = tdOccasNum * (prachRaLength *  rooSeqtNum + 1);
            dataOutOffsetPerTdOccas = prachRaLength *  rooSeqtNum + 1;
            *(prachRppParaOut->prachFdOcasRpp[fdOccasCnt].outPutPdpAddr) = 0x00000000 + fdLoop * dataOutOffsetPerFdOccas; /* 0x00000000为RPP输出基地址待定， 配置每个FD的首地址 */
            prachRppParaOut->prachFdOcasRpp[fdOccasCnt].outPutPdpOffset  = dataOutOffsetPerTdOccas;                       /* 每个FD下， 每个TD Occasion的偏移 */
            fdOccasCnt++;
        }
        prachRppParaOut->fdOccasNum = fdOccasCnt;
    }

    return 0;
}

uint32_t L1PrachParaParse2Dsp(uint8_t cellIndex, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, PrachDetectDspPara *prachDspParaOut)
{
    uint8_t  pduNum, pduIndex;
    uint8_t  winNum, fdOccasCnt, excursionNum;
    uint8_t  restrictedSetCfg, tdOccasNum, totalFdOccas;
    uint8_t  fdOcasNum, fdOcasIdx, fdLoop, rooSeqtNum, rootSeqIndex;
    uint8_t  prachConfigIndex, preambleFormat, prachLengthFlag;
    uint16_t ncsValue, zcSeqSize;
    uint16_t prachResCfgIdx;
    uint32_t dataOutOffsetPerFdOccas, dataOutOffsetPerTdOccas;
    NrPrachRootDetWin *prachRootDetWin = NULL;

    prachDspParaOut->sfnIndex  = l1prachParaPduInfoIn->sfnIndex;
    prachDspParaOut->slotIndex = l1prachParaPduInfoIn->slotIndex;
    prachDspParaOut->cellIdx   = cellIndex;
    
    pduNum           = l1prachParaPduInfoIn->prachPduNum;
    tdOccasNum       = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachTdOcasNum;
    prachResCfgIdx   = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachResCfgIndex;
    preambleFormat   = l1prachParaPduInfoIn->l1PrachPduInfo[0].prachFormat;
    prachConfigIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachConfigIndex;
    prachLengthFlag  = l1PrachConfigInfoIn[prachResCfgIdx].prachSequenceLength;
    restrictedSetCfg = l1PrachConfigInfoIn[prachResCfgIdx].restrictedSetConfig;
    winNum = (restrictedSetCfg == 0) ?  1 : MAX_DET_WIN_NUM;
    
    prachDspParaOut->prachCellCfgPara.tdOccasNum    = tdOccasNum;
    prachDspParaOut->prachCellCfgPara.ulBwpPuschScs = l1PrachConfigInfoIn[prachResCfgIdx].ulBwpPuschScs;
    prachDspParaOut->prachCellCfgPara.restrictA     = restrictedSetCfg;
    prachDspParaOut->prachCellCfgPara.thAlpha       = 0x4CCD; /* 门限系数 浮点值为0.6 对应定点值为 19661*/
    prachDspParaOut->prachCellCfgPara.thSingleWin   = 0xFFFF; /* 算法配置值，待定 */
    prachDspParaOut->prachCellCfgPara.thMultiWin    = 0xFFFF; /* 算法配置值，待定 */
    if (prachLengthFlag == 0){
        prachDspParaOut->prachCellCfgPara.nFftSize  = PRACH_FFT_LEN_1536;
        zcSeqSize  = PRACH_ZC_LEN_839;
    }
    else{
        prachDspParaOut->prachCellCfgPara.nFftSize  = PRACH_FFT_LEN_256;
        zcSeqSize  = PRACH_ZC_LEN_139; 
    }
    prachDspParaOut->prachCellCfgPara.zcSeqSize = zcSeqSize; 
    
    fdOccasCnt = 0;
    for (pduIndex = 0; pduIndex < pduNum; pduIndex){
        ncsValue  = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].ncsValue;
        if (((ncsValue == 2)||(ncsValue == 4)) && (prachLengthFlag == 1)){
            excursionNum = 1;
        }
        else if (((ncsValue == 6)||(ncsValue == 8)||(ncsValue == 10)) && (prachLengthFlag == 1)){
            excursionNum = 2;
        }
        else{
            excursionNum = 5;
        }

        fdOcasNum = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachFdmNum;
        fdOcasIdx = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].PrachFdmIndex;
        for (fdLoop = fdOcasIdx; fdLoop < fdOcasIdx + fdOcasNum; fdLoop++){
            prachDspParaOut->prachFdOccasPara[fdOccasCnt].handle        = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].handle;      
            prachDspParaOut->prachFdOccasPara[fdOccasCnt].zcRootSeqNum  = l1PrachConfigInfoIn[prachResCfgIdx].rootSequenceNum[fdLoop];
            prachDspParaOut->prachFdOccasPara[fdOccasCnt].excursionNum  = excursionNum;
            
            rooSeqtNum   = l1PrachConfigInfoIn[prachResCfgIdx].rootSequenceNum[fdLoop];
            rootSeqIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachRootSequenceIndex[fdLoop];
            dataOutOffsetPerFdOccas = tdOccasNum * (zcSeqSize *  rooSeqtNum + 1);
            dataOutOffsetPerTdOccas = zcSeqSize * rooSeqtNum + 1;
            *(prachDspParaOut->prachFdOccasPara[fdOccasCnt].pdpSeqAddr) = 0x00000000 + fdLoop * dataOutOffsetPerFdOccas; /* 0x00000000为RPP输出基地址待定， 配置每个FD的首地址 */
            prachDspParaOut->prachFdOccasPara[fdOccasCnt].pdpSeqOffset  = dataOutOffsetPerTdOccas;
            
            prachRootDetWin = &prachDspParaOut->prachFdOccasPara[fdOccasCnt].prachRootDetWin[0];
            PrachWinStartAndEndCalc(restrictedSetCfg, rooSeqtNum, rootSeqIndex, zcSeqSize, ncsValue, prachRootDetWin);

            fdOccasCnt++;
        }    
        prachDspParaOut->prachCellCfgPara.fdOccasNum = fdOccasCnt;
    }

    return 0;
}

uint32_t L1PrachReport2Fapi(L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn, NrPrachDetectResult *prachDetectResult)
{
    uint16_t sfnInedx;
    uint8_t  slotInex, cellIndex, frameDuplexType;
    uint8_t  pduIndex, pduNum, tdOccasNum;
    uint8_t  fdOcasNum, fdOcasIdx, fdLoop, fdOccasCnt, fdOccasIndexInPdu;
    uint8_t  detectedOccasNum, occasIndex, duration;
    uint8_t  tdOccasIndex, fdOccasIndex;
    uint8_t  fdRaIndex[MAX_PRACH_FDM_NUM], startSymbol[MAX_PRACH_FDM_NUM][MAX_PRACH_TDM_NUM]; 
    uint8_t  preambleFormat, startSymb;
    uint8_t  preambleNum, preambleLoop, detectedIndex;
    uint16_t occasAgc; 
    uint16_t prembleSnrScale;
    uint16_t prachResCfgIdx, prachConfigIndex;
    NrPrachOccasResult    *prachOccasResult      = NULL;
    FapiNrPrachIndication *fapiNrPrachIndication = NULL;

    sfnInedx  = prachDetectResult->sfnIndex;
    slotInex  = prachDetectResult->slotIndex;
    cellIndex = prachDetectResult->cellIdx;
    detectedOccasNum = prachDetectResult->detectedOccasionNum;
    frameDuplexType  = g_cellConfigPara[cellIndex].frameDuplexType;
    fapiNrPrachIndication = &g_fapiNrPrachIndication[cellIndex][slotInex];
    fapiNrPrachIndication->numSFN  = sfnInedx;
    fapiNrPrachIndication->numSlot = slotInex;
    fapiNrPrachIndication->numPdus = detectedOccasNum;

    fdOccasCnt = 0;
    for (pduIndex = 0; pduIndex < pduNum; pduIndex){
        preambleFormat   = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachFormat;
        startSymb        = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachStartSymb;
        tdOccasNum       = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachTdOcasNum;
        prachResCfgIdx   = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachResCfgIndex;
        prachConfigIndex = l1PrachConfigInfoIn[prachResCfgIdx].prachConfigIndex; 
        duration = (frameDuplexType == 0) ? g_prachCfgTableFR1Fdd[prachConfigIndex].duration : g_prachCfgTableFR1Tdd[prachConfigIndex].duration;
        
        fdOcasNum = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].prachFdmNum;
        fdOcasIdx = l1prachParaPduInfoIn->l1PrachPduInfo[pduIndex].PrachFdmIndex;
        fdOccasIndexInPdu = 0;
        for (fdLoop = fdOcasIdx; fdLoop < fdOcasIdx + fdOcasNum; fdLoop++){
            fdRaIndex[fdOccasCnt] = fdOcasIdx + fdOccasIndexInPdu;
            for (tdOccasIndex = 0; tdOccasIndex < tdOccasNum; tdOccasIndex++){
                startSymbol[fdOccasCnt][tdOccasIndex] = startSymb + tdOccasIndex * duration;
            }
            fdOccasIndexInPdu++;
            fdOccasCnt++;
        }
    }

    for (occasIndex = 0; occasIndex < fapiNrPrachIndication->numPdus; occasIndex++){
        tdOccasIndex = prachDetectResult->detectedIndex[occasIndex][0];
        fdOccasIndex = prachDetectResult->detectedIndex[occasIndex][1];
        
        prachOccasResult = &prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex];
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].handle  = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].handle;
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].avgSnr  = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].avgSnr;
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].avgRssi = 0xFFFF; /* 待定 */
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].slotIndex = slotInex;
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].startSymbolIndex = startSymbol[fdOccasIndex][tdOccasIndex];
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].raIndex = fdRaIndex[fdOccasIndex];
        fapiNrPrachIndication->prachMeasPerPdu[occasIndex].numPreambles = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].numPreambles;
        occasAgc    = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].lowPhyAgc;
        preambleNum = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].numPreambles;
        
        for (preambleLoop = 0; preambleLoop < preambleNum; preambleLoop++){
            detectedIndex = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].preambleIndex[preambleLoop];
            fapiNrPrachIndication->prachMeasPerPdu[occasIndex].prachMesaPerId[preambleLoop].preambleIndex = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].preambleIndex[preambleLoop];
            fapiNrPrachIndication->prachMeasPerPdu[occasIndex].prachMesaPerId[preambleLoop].timingAdvance = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].timingAdvance[preambleLoop];
            fapiNrPrachIndication->prachMeasPerPdu[occasIndex].prachMesaPerId[preambleLoop].preamblePwr   = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].preamblePwr[preambleLoop];
            fapiNrPrachIndication->prachMeasPerPdu[occasIndex].prachMesaPerId[preambleLoop].preambleSnr   = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].preambleSnrBase[preambleLoop];
            prembleSnrScale = prachDetectResult->prachDspDetection[tdOccasIndex][fdOccasIndex].preambleSnrScale[preambleLoop];
        }
    }

    /* 待补充，线性转dB部分代码 */

    return 0;
}

uint32_t L1PrachParamCheck(uint8_t cellIndex, L1PrachParaPduInfo *l1prachParaPduInfoIn, L1PrachConfigInfo *l1PrachConfigInfoIn)
{
    // length 139 and 839(format0) preamble supported

    return 0;
}

uint32_t NrPhyUlPrachTask(uint8_t cellIndex, uint8_t slotIndex)
{
    PrachLowPhyHacPara  *prachLowPhyParaOut   = &g_prachLowPhyHacPara[cellIndex];
    PrachRPPHacPara     *prachRppParaOut      = &g_prachRppHacPara[cellIndex];
    PrachDetectDspPara  *prachDspParaOut      = &g_prachDetectDspPara[cellIndex];
    NrPrachDetectResult *prachDetectResult    = &g_prachDetectResult[cellIndex];
    L1PrachConfigInfo   *l1PrachConfigInfoIn  = &g_prachConfigInfo[cellIndex];
    L1PrachParaPduInfo  *l1prachParaPduInfoIn = &g_prachParaInfoOut[cellIndex][slotIndex];
    
    L1PrachParamCheck(cellIndex, l1prachParaPduInfoIn, l1PrachConfigInfoIn);

    L1PrachParaParse2LowPhyHac(cellIndex, l1prachParaPduInfoIn, l1PrachConfigInfoIn, prachLowPhyParaOut);

    L1PrachParaParse2RppHac(cellIndex, l1prachParaPduInfoIn, l1PrachConfigInfoIn, prachRppParaOut);
     
    L1PrachParaParse2Dsp(cellIndex, l1prachParaPduInfoIn, l1PrachConfigInfoIn, prachDspParaOut);

    L1PrachReport2Fapi(l1prachParaPduInfoIn, l1PrachConfigInfoIn, prachDetectResult);

    return 0;  
}
