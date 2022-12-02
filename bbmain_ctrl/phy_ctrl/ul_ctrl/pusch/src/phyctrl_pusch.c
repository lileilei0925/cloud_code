#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../../../../common/inc/phy_ctrl_common.h"
#include "../../../../common/src/common.c"

#include "../inc/phyctrl_pusch.h"
#include "../inc/pusch_variable.h"

uint32_t L1PuschTbParaCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschDemapParaTemp *puschDemapParaTemp);
uint32_t L1PuschUciCrcLengthCalculate(uint16_t ucibitlength, uint8_t *crcLength);
uint32_t L1PuschUciEncodeBitCalculate(L1PuschPduInfo *l1PuschUeInfo,  PuschDemapParaTemp *puschDemapParaTemp);
uint32_t L1PuschUciReLocationCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschDemapParaTemp *puschDemapParaTemp);
uint32_t L1PuschLdpcCbPartRowCalculate(PuschLdpcUePara *puschLdpcUePara, uint8_t baseGraphType, uint16_t *cbPartRow);

#if 1
int main(void)
{
  uint32_t a = 40;
  uint32_t b = 16;
  uint32_t c = 0;
  uint32_t d = 0;
  uint16_t aa;
  uint16_t bb[16] = { 0 };
  uint8_t  rbgNum = 0;
  uint8_t  rbgStartAndsize[137*2] = { 0 };
  
  a = sizeof(NrPuschMeasPara);
  b = sizeof(PuschLowPhyCellReport);
  
  //findRbgNumAndSize(&rbBitmap[0], 36, &rbgNum, &g_puschUeRbgInfo[0][0]);
  //findBit1NumAndIndex(rbBitmap1, &aa, &bb[0]); 
  printf("c = %d;\n",a+b);
  printf("___Hello World___;\n");

  return 0;
}
#endif


uint32_t L1PuschDmrsPara2PnGenHac(int8_t cellIndex, L1PuschParaPduInfo *l1PuschParaPduInfo, PuschPnGenPara *dmrsPnGenPara)
{
    uint8_t  ueIndex, ueNum;
    uint8_t  dmrsIdx, dmrsNumAllUe, dmrsPerRB;
    uint8_t  slotIndex, resourceAlloc, seqLength;
    uint8_t  rbgIdx;
    uint8_t  cdmGrpNumCnt[3], loopIndex;
    uint8_t  uePortNum[NR_PUSCH_MAX_UE_NUM_PER_SLOT], uePortIndex[NR_PUSCH_MAX_UE_NUM_PER_SLOT][NR_PUSCH_UE_PORT_NUM]; 
    uint16_t nSCIDNID, nSCID, addrOffset, bwpStart;
    L1PuschPduInfo  *l1PuschUeInfo   = NULL;
    PnGenParaPerSeq *pnGenParaPerSeq = NULL;

    uint32_t *dmrsBaseAddr = (uint32_t *)&cellIndex; //根据小区获取存储的基地址

    dmrsNumAllUe = 0;
    dmrsPnGenPara->sfnIndex   = l1PuschParaPduInfo->sfnIndex;
    dmrsPnGenPara->slotIndex  = l1PuschParaPduInfo->slotIndex;
    //dmrsPnGenPara->taskType = 0; /* 0: PUSCH DMRS 序列 */
    ueNum     = l1PuschParaPduInfo->puschPduNum;
    slotIndex = l1PuschParaPduInfo->slotIndex;
    pnGenParaPerSeq = &dmrsPnGenPara->PnGenParaPerSeq[0];

    for (ueIndex = 0; ueIndex < ueNum; ueIndex++){
        l1PuschUeInfo   = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];

        if (l1PuschUeInfo->transformPrecoding == 0){/* 0：传输预编码使能，dmrs使用ZC序列 */
            continue; 
        }

        bwpStart = l1PuschUeInfo->bwpStart;
        findBit1NumAndIndex(l1PuschUeInfo->ulDmrsSymbPos, (uint16_t*)&g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex],  (uint16_t*)&g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0]);/* 获取DMRS数，和dmrs的符号索引 */
        findBit1NumAndIndex(l1PuschUeInfo->dmrsPorts, (uint16_t*)&uePortNum[ueIndex], (uint16_t*)&uePortIndex[ueIndex][0]);         /* 获取port数，和portIndex索引 */

        cdmGrpNumCnt[0]  = 0;
        cdmGrpNumCnt[1]  = 0;
        cdmGrpNumCnt[2]  = 0;
        g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][0] = 0;
        g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][1] = 0;
        g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][2] = 0;
        for (loopIndex = 0; loopIndex < uePortNum[ueIndex]; loopIndex++){
            if ((uePortIndex[ueIndex][loopIndex] >> 1) == 0){
                cdmGrpNumCnt[0] = 1;
                g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][0]++;
            }
            else if ((uePortIndex[ueIndex][loopIndex] >> 1) == 1){
                cdmGrpNumCnt[1] = 1;
                g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][1]++;
            }
            else if ((uePortIndex[ueIndex][loopIndex] >> 1) == 2){
                cdmGrpNumCnt[2] = 1;
                g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][2]++;
            }
            else {
                return ERROR_CODE;
            }
        }
        g_puschUeCdmNum[cellIndex][slotIndex][ueIndex] = cdmGrpNumCnt[0] + cdmGrpNumCnt[1] + cdmGrpNumCnt[2];
        g_puschUeStartCdmIdx[cellIndex][slotIndex][ueIndex] = uePortIndex[ueIndex][0] >> 1;  /* 对第一个portIndex 除 2 获取当前的cdmIndex*/

        dmrsPerRB = (l1PuschUeInfo->dmrsCfgType == 1) ? NR_DMRS_TYPE1_SC_PER_RB : NR_DMRS_TYPE2_SC_PER_RB;
        resourceAlloc   = l1PuschUeInfo->resourceAlloc;
        if (resourceAlloc == 0){
            findRbgNumAndSize(&l1PuschUeInfo->rbBitmap[0], 36, &g_puschUeRbgNum[ueIndex], &g_puschUeRbgInfo[ueIndex][0]); /* 资源分配Type0时使用 */
            seqLength = bwpStart + g_puschUeRbgInfo[ueIndex][(g_puschUeRbgNum[ueIndex] - 1)].rbStart + g_puschUeRbgInfo[ueIndex][(g_puschUeRbgNum[ueIndex] - 1)].rbNum; /* 最后一个RBG的起始加上最后一个RBG的size */
        }
        else if (resourceAlloc == 1){
            g_puschUeRbgNum[ueIndex]  = 1;
            g_puschUeRbgInfo[ueIndex][0].rbStart = l1PuschUeInfo->rbStart;
            g_puschUeRbgInfo[ueIndex][1].rbNum = l1PuschUeInfo->rbSize;
            seqLength = bwpStart + g_puschUeRbgInfo[ueIndex][0].rbStart + g_puschUeRbgInfo[ueIndex][1].rbNum;
        }
        seqLength = seqLength * dmrsPerRB * 2;  /* 2：QPSK调制 */

        nSCIDNID = l1PuschUeInfo->dmrsScrambleId;
        nSCID    = l1PuschUeInfo->nSCID;         

        for (dmrsIdx = 0; dmrsIdx < g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex]; dmrsIdx++){
            for (loopIndex = g_puschUeStartCdmIdx[cellIndex][slotIndex][ueIndex]; loopIndex < g_puschUeCdmNum[cellIndex][slotIndex][ueIndex]; loopIndex++){
                pnGenParaPerSeq->cinit = ((1 << 17) * (14 * (uint32_t)slotIndex + (uint32_t)g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][dmrsIdx] + 1) * (2 * (uint32_t)nSCIDNID + 1) + (1 << 17) * (loopIndex >> 1)  + 2 * (uint32_t)nSCIDNID + (uint32_t)nSCID) % (1 << 31);
                pnGenParaPerSeq->seqStart = g_puschUeRbgInfo[ueIndex][0].rbStart;
                pnGenParaPerSeq->seqLen   = seqLength;
                addrOffset = ceilDiv(seqLength, 32);
                pnGenParaPerSeq->outPutAddr = dmrsBaseAddr + (dmrsIdx * g_puschUeCdmNum[cellIndex][slotIndex][ueIndex] * addrOffset + (loopIndex - g_puschUeStartCdmIdx[cellIndex][slotIndex][ueIndex]) * addrOffset); 
                pnGenParaPerSeq++;
                dmrsNumAllUe++;
            }
        }
    }

    dmrsPnGenPara->pnSeqNum = dmrsNumAllUe;
    return 0;
}

uint32_t L1PuschParaParse2DceDsp(uint8_t cellIndex, L1PuschParaPduInfo *l1PuschParaPduInfo, L1CellConfigInfo l1CellConfigInfo, PuschDcePara *puschDcePara)
{
    uint8_t  symbIndex, ueIndex, validUeNum, dmrsCnt[NR_PUSCH_MAX_UE_NUM_PER_SLOT];
    uint8_t  ueNum, symbStart, rbStart, rbNum, loopIndex, slotIndex;
    uint8_t  uePortNum[NR_PUSCH_MAX_UE_NUM_PER_SLOT], uePortIndex[NR_PUSCH_MAX_UE_NUM_PER_SLOT][NR_PUSCH_UE_PORT_NUM]; 
    uint8_t  rbgStartIndex[NR_PUSCH_MAX_UE_NUM_PER_SLOT], rbgStartCnt, rbgIndex;
    uint16_t pduBitMap, dmrsSymbPos; 

    L1PuschPduInfo *l1PuschUeInfo  = NULL;
    NrPuschSymPara *nrPuschSymPara = NULL; 

    memset(&puschDcePara->sfnIndex, 0x0, sizeof(PuschDcePara));  /* 对接口先清零 */

    /* 算法开关类参数，待接口和传参方式确认 */
    puschDcePara->puschCellPara.foeSwitch      = 1;
    puschDcePara->puschCellPara.minCPT         = 1;
    puschDcePara->puschCellPara.nfft           = 1;
    puschDcePara->puschCellPara.puschEqMode    = 1;
    puschDcePara->puschCellPara.ruuAvgRbGrpNum = 1;
    puschDcePara->puschCellPara.ruuAvgTimeFlag = 1;
    puschDcePara->puschCellPara.rxAntNum       = l1CellConfigInfo.rxAntNum;
    puschDcePara->puschCellPara.rxAntIdx[0]    = 1;
    puschDcePara->puschCellPara.targetAgc      = 0;
    puschDcePara->sfnIndex  = l1PuschParaPduInfo->sfnIndex;
    puschDcePara->slotIndex = l1PuschParaPduInfo->slotIndex;
    slotIndex = l1PuschParaPduInfo->slotIndex;

    rbgStartCnt = 0;
    ueNum = l1PuschParaPduInfo->puschPduNum;
    for(ueIndex = 0; ueIndex < ueNum; ueIndex++){
        l1PuschUeInfo   = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
        findBit1NumAndIndex(l1PuschUeInfo->dmrsPorts, (uint16_t*)&uePortNum[ueIndex], (uint16_t*)&uePortIndex[ueIndex][0]);         /* 获取port数，和portIndex索引 */
        g_puschRbgStartIndex[cellIndex][slotIndex][ueIndex] = rbgStartCnt;
        rbgStartCnt += g_puschUeRbgNum[ueIndex];
        dmrsCnt[ueIndex] = 0;  /* 清零 后面使用 */
    }

    for(symbIndex = 0; symbIndex < SYM_NUM_PER_SLOT; symbIndex++){
        validUeNum = 0;
        nrPuschSymPara = &puschDcePara->puschSymPara[symbIndex];
        for(ueIndex = 0; ueIndex < ueNum; ueIndex++){
            l1PuschUeInfo = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
            symbStart     = l1PuschUeInfo->startSymbIndex;

            if(symbStart > symbIndex){
                continue;
            }

            if((symbIndex == symbStart) 
            && (symbIndex != g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0]) 
            && (symbIndex != g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][1]) 
            && (symbIndex != g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][2])){
                nrPuschSymPara->puschUePara[ueIndex].userTaskType = 1; /* 表示调度的是第1个符号但不是DMRS符号，需要做频域dagc */ 
            }
            else if((symbIndex == symbStart) 
            && (symbIndex == g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0])){
                nrPuschSymPara->puschUePara[ueIndex].userTaskType = 3; /* 表示调度的是第1个符号且是DMRS符号，需要做频域dagc和信道估计 */
                dmrsCnt[ueIndex]++;
            }
            else if((symbIndex != symbStart) && 
            ( (symbIndex == g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0]) 
            ||(symbIndex == g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][1]) 
            ||(symbIndex == g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][2]))){
                nrPuschSymPara->puschUePara[ueIndex].userTaskType = 2; /* 表示不是第一个符号，但是是DMRS符号，需要做信道估计 */
                dmrsCnt[ueIndex]++;
            }
            else{
                continue;
            }

            pduBitMap = l1PuschUeInfo->pduBitMap;
            nrPuschSymPara->puschUePara[ueIndex].ueIndex          = ueIndex;
            nrPuschSymPara->puschUePara[ueIndex].dftPrecEn        = ((pduBitMap >> 3) & 0x1);  /* pduBitmap第三bit指示dft-s-ofdm */
            nrPuschSymPara->puschUePara[ueIndex].dmrsCfgType      = l1PuschUeInfo->dmrsCfgType;
            nrPuschSymPara->puschUePara[ueIndex].totalDmrsSymbNum = g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex];
            nrPuschSymPara->puschUePara[ueIndex].dmrsPositionIdx  = dmrsCnt[ueIndex]; /* UE DMRS符号索引，1:首符号，2：中间符号，3：第三个符号 */
            nrPuschSymPara->puschUePara[ueIndex].sinr             = 255;         /* 待确认来源 */
            nrPuschSymPara->puschUePara[ueIndex].cheRbgNum        = g_puschUeRbgNum[ueIndex];
            nrPuschSymPara->puschUePara[ueIndex].startRbgIdx      = rbgStartIndex[ueIndex];
            nrPuschSymPara->puschUePara[ueIndex].validCdmNum      = g_puschUeCdmNum[cellIndex][slotIndex][ueIndex];
            
            if(l1PuschUeInfo->transformPrecoding == 0){
                *nrPuschSymPara->puschUePara[ueIndex].puschBaseSeq = 0x0000000A; // PN序列 来源PNGen HAC
            }
            else{
                *nrPuschSymPara->puschUePara[ueIndex].puschBaseSeq = 0x0000000A; // ZC序列 来源DDR
            }
            
            for(loopIndex = 0; loopIndex < g_puschUeCdmNum[cellIndex][slotIndex][ueIndex]; loopIndex++){
                nrPuschSymPara->puschUePara[ueIndex].puschCdmPara[loopIndex].cdmIdx       = g_puschUeCdmNum[cellIndex][slotIndex][ueIndex];
                nrPuschSymPara->puschUePara[ueIndex].puschCdmPara[loopIndex].portNum      = g_puschPortNumPerCdm[cellIndex][slotIndex][ueIndex][loopIndex];
                nrPuschSymPara->puschUePara[ueIndex].puschCdmPara[loopIndex].startPortIdx = uePortIndex[ueIndex][0];
            }
            
            nrPuschSymPara->puschUePara[ueIndex].rbsize = 0;
            for(rbgIndex = 0; rbgIndex < g_puschUeRbgNum[ueIndex]; rbgIndex++){
                nrPuschSymPara->puschCePara[rbgIndex + rbgStartIndex[ueIndex]].rbStart = g_puschUeRbgInfo[ueIndex][rbgIndex].rbStart;
                nrPuschSymPara->puschCePara[rbgIndex + rbgStartIndex[ueIndex]].rbNum   = g_puschUeRbgInfo[ueIndex][rbgIndex].rbNum;
                 nrPuschSymPara->puschUePara[ueIndex].rbsize += g_puschUeRbgInfo[ueIndex][rbgIndex].rbNum;
            }
            g_puschUeRbsize[cellIndex][slotIndex][ueIndex] = nrPuschSymPara->puschUePara[ueIndex].rbsize;
       
            validUeNum ++;
        }   
        g_puschUeNumInSymb[cellIndex][symbIndex] = validUeNum;
        nrPuschSymPara->validUeNum = validUeNum;
        /* 输入数据地址，待确定 */
        *nrPuschSymPara->puschFreqData  = 0x00000000;
        *nrPuschSymPara->puschFreqAgc   = 0x00000001;   
        /* 输出数据数据，待确定 */
        *nrPuschSymPara->puschCheResult = 0x00000003;
        *nrPuschSymPara->puschRuuResult = 0x00000004;
        *nrPuschSymPara->puschCheAgc    = 0x00000005;
    }    
    return 0;
}

uint32_t L1PuschDescramblePara2PnGenHac(uint8_t cellIndex, L1PuschParaPduInfo *l1PuschParaPduInfo, PuschPnGenPara *descramblePnGenPara)
{
    uint8_t  ueIndex, ueNum, rbgIdx;
    uint8_t  symbIndex, startSymbIndex, endSymbIndex, qamModOrder, layerNum;
    uint8_t  slotIndex, resourceAlloc, reNum, cdmGrpsNoData, puschReNumPreDmrsRb, dmrsCfgType;
    uint16_t nIDPusch, ueVaildBitNum, addrOffset; 
    L1PuschPduInfo  *l1PuschUeInfo   = NULL;
    uint32_t *dmrsBaseAddr = (uint32_t *)&cellIndex; // 根据小区获取存储的基地址

    ueNum   = l1PuschParaPduInfo->puschPduNum;
    slotIndex = l1PuschParaPduInfo->slotIndex;
    descramblePnGenPara->sfnIndex   = l1PuschParaPduInfo->sfnIndex;
    descramblePnGenPara->slotIndex  = l1PuschParaPduInfo->slotIndex;
    //descramblePnGenPara->taskType = 1; /* 1: PUSCH 解扰序列 */
    descramblePnGenPara->pnSeqNum = ueNum;

    for (ueIndex = 0; ueIndex < ueNum; ueIndex++){
        l1PuschUeInfo  = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
        startSymbIndex = l1PuschUeInfo->startSymbIndex;
        endSymbIndex   = startSymbIndex + l1PuschUeInfo->nrOfSymbols;
        cdmGrpsNoData  = l1PuschUeInfo->numCdmGrpsNoData - 1;/* numCdmGrpsNoData 值从1 2 3转化为0,1,2*/
        dmrsCfgType    = l1PuschUeInfo->dmrsCfgType;
        qamModOrder    = l1PuschUeInfo->qamModOrder;
        layerNum       = l1PuschUeInfo->nrOfLayers;
        nIDPusch       = l1PuschUeInfo->nIdPusch;

        descramblePnGenPara->PnGenParaPerSeq->cinit = ((uint32_t)1 << 15) * l1PuschUeInfo->ueRnti + nIDPusch;
        ueVaildBitNum = 0;
        puschReNumPreDmrsRb = g_puschReNumPerDmrsRb[dmrsCfgType][cdmGrpsNoData];
        for (symbIndex  = startSymbIndex; symbIndex < endSymbIndex; symbIndex++){
            reNum = 0;
            if ((symbIndex!= g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0])
            && (symbIndex != g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][1]) 
            && (symbIndex != g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][2])){/* 非DMRS符号 */
                for(rbgIdx = 0; rbgIdx < g_puschUeRbgNum[ueIndex]; rbgIdx++){// Type0 可能有多个RBG， Type1只有1个
                    reNum += g_puschUeRbgInfo[ueIndex][rbgIdx].rbNum * N_SC_PER_PRB; /* 最后一个RBG的起始加上最后一个RBG的size */
                } 
            }
            else{ /* DMRS symbol */
                for(rbgIdx = 0; rbgIdx < g_puschUeRbgNum[ueIndex]; rbgIdx++){// Type0 可能有多个RBG， Type1只有1个
                    reNum += g_puschUeRbgInfo[ueIndex][rbgIdx].rbNum * puschReNumPreDmrsRb; /* 最后一个RBG的起始加上最后一个RBG的size */
                } 
            }
            g_puschUeReNumPerSymb[cellIndex][slotIndex][ueIndex][symbIndex] = reNum;
            ueVaildBitNum += (reNum * layerNum * qamModOrder);
        }
        addrOffset = ceilDiv(ueVaildBitNum, 32);
        descramblePnGenPara->PnGenParaPerSeq->seqStart   = 0;
        descramblePnGenPara->PnGenParaPerSeq->seqLen     = ueVaildBitNum;
        descramblePnGenPara->PnGenParaPerSeq->outPutAddr = dmrsBaseAddr + addrOffset; 
    }

    return 0;
}

uint32_t L1PuschPara2DeqAndDemapHac(uint8_t cellIndex, L1PuschParaPduInfo *l1PuschParaPduInfo, PuschDeqAndDemapHacPara *puschDeqAndDemapHacPara)
{
    uint8_t  slotIndex;
    uint8_t  ueIndex, validUeNum, rbgIndex, layerNum;
    uint8_t  symbIndex, symbStart, symbEnd, ueSymbNum, qamMode;
    uint8_t  dmrsSymbNum, dataSymbNum; 
    uint16_t rbSize, dataReInSymb, rbNum;
    PuschDemapParaTemp puschDemapParaTemp[16];
    PuschDeqAndDemapSymPara *puschDeqAndDemapSymPara = NULL;
    PuschDeqAndDemapUePara  *puschDeqAndDemapUePara  = NULL;
    L1PuschPduInfo          *l1PuschUeInfo           = NULL;

    memset(&puschDeqAndDemapHacPara->sfnIndex, 0x0, sizeof(PuschDeqAndDemapHacPara));  /* 对接口先清零 */
    puschDeqAndDemapHacPara->xCut           = 3;
    puschDeqAndDemapHacPara->ySat           = 5;
    puschDeqAndDemapHacPara->demodThreshold = 40;
    puschDeqAndDemapHacPara->freqCompEn     = 1; /* 频偏补偿功能使能 */
    puschDeqAndDemapHacPara->cellIndex      = cellIndex;
    puschDeqAndDemapHacPara->ueNum          = l1PuschParaPduInfo->puschPduNum;
    puschDeqAndDemapHacPara->sfnIndex       = l1PuschParaPduInfo->sfnIndex;
    puschDeqAndDemapHacPara->slotIndex      = l1PuschParaPduInfo->slotIndex;
    puschDeqAndDemapHacPara->rxAntNum       = g_cellConfigPara[cellIndex].rxAntNum;
    puschDeqAndDemapHacPara->ruuAvgRbGrpNum = g_cellConfigPara[cellIndex].ruuAvgRbGrpNum;
    slotIndex = l1PuschParaPduInfo->sfnIndex;
    
    /* ue级参数提前计算 */
    for(ueIndex = 0; ueIndex < puschDeqAndDemapHacPara->ueNum; ueIndex++){
        rbSize = g_puschUeRbsize[cellIndex][slotIndex][ueIndex];
        l1PuschUeInfo = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
        puschDemapParaTemp[ueIndex].rbNumAllRbg = rbSize;

        L1PuschTbParaCalculate(l1PuschUeInfo, &puschDemapParaTemp[ueIndex]); /* TB参数计算 */
        
        ueSymbNum       = l1PuschUeInfo->nrOfSymbols;
        dmrsSymbNum     = g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex];
        dataSymbNum     = ueSymbNum - dmrsSymbNum;
        puschDemapParaTemp[ueIndex].dmrsSymbNum     = dmrsSymbNum;
        puschDemapParaTemp[ueIndex].dataSymbNum     = dataSymbNum;
        puschDemapParaTemp[ueIndex].firstDmrsIndex  = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0];
        puschDemapParaTemp[ueIndex].secondDmrsIndex = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][1];
        puschDemapParaTemp[ueIndex].thirdDmrsIndex  = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][2];

        L1PuschUciEncodeBitCalculate(l1PuschUeInfo,  &puschDemapParaTemp[ueIndex]);
        L1PuschUciReLocationCalculate(l1PuschUeInfo, &puschDemapParaTemp[ueIndex]);
    }
    
    /* 按照符号循环，配置每个符号的参数 */
    for (symbIndex = 0; symbIndex < SYM_NUM_PER_SLOT; symbIndex++){
        puschDeqAndDemapSymPara = &puschDeqAndDemapHacPara->puschDeqAndDemapSymPara[symbIndex];
        validUeNum = 0;
        for (ueIndex = 0; ueIndex < puschDeqAndDemapHacPara->ueNum; ueIndex++){
            l1PuschUeInfo = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
            symbStart = l1PuschUeInfo->startSymbIndex;
            symbEnd   = symbStart + l1PuschUeInfo->nrOfSymbols;

            if ((symbStart > symbIndex) && (symbEnd < symbIndex)){  /* 当前符号不在UE调度的符号范围内 */
                continue;
            }

            puschDeqAndDemapUePara = &puschDeqAndDemapSymPara->puschDeqAndDemapUePara[ueIndex];
            puschDeqAndDemapUePara->ueIndex        = ueIndex;
            puschDeqAndDemapUePara->codeWordNum    = 1;  /* NR上行1码字，LTE上行对大2码字，NR FAPI接口中无上行码字数 */
            puschDeqAndDemapUePara->layerNum       = l1PuschUeInfo->nrOfLayers;
            puschDeqAndDemapUePara->deModType      = l1PuschUeInfo->qamModOrder;
            puschDeqAndDemapUePara->dmrsSymbBitMap = l1PuschUeInfo->ulDmrsSymbPos;
            puschDeqAndDemapUePara->freqCompCoeff  = g_puschFreqCompCoeff[cellIndex][slotIndex][ueIndex][symbIndex]; /* 频偏相位值来源测量模块 */
            puschDeqAndDemapUePara->transPrecodeEn = l1PuschUeInfo->transformPrecoding;
            puschDeqAndDemapUePara->betaRuu        = 0xFFFF;   /* 待定 */
            memcpy(&puschDeqAndDemapUePara->rbBitmap[0], &l1PuschUeInfo->rbBitmap[0], sizeof(uint8_t)*35); /* 通过RB位图告诉频域数据位置 */
         
            /* DMRS符号标志识别，DMRS配置信道估计输入地址，DATA配置插值系数 */
            if(((symbIndex + 1) & puschDeqAndDemapUePara->dmrsSymbBitMap) == 1){ /* DMRS symbol */
                puschDeqAndDemapUePara->dmrsSymbFlag = 1; /* 当前符号是dmrs 符号 */
                if (l1PuschUeInfo->dmrsCfgType == 1){
                    if (l1PuschUeInfo->numCdmGrpsNoData == 2){
                        puschDeqAndDemapUePara->dmrsReBitmap     = 0x0FFF;       /* DMRS CDMGroup0/1，均有DMRS，无DATA RE*/
                        dataReInSymb = 0;
                    } 
                    else if (l1PuschUeInfo->numCdmGrpsNoData == 1){
                        puschDeqAndDemapUePara->dmrsReBitmap = 0x0555;           /* DMRS CDMGroup0: port(0 1),K = 0,2,4,6,8,10 */
                        dataReInSymb = 6;
                    }
                    else {
                        return 0xFFFFFFFF;
                    }
                }
                else if (l1PuschUeInfo->dmrsCfgType == 2){
                    if (l1PuschUeInfo->numCdmGrpsNoData == 3){
                        puschDeqAndDemapUePara->dmrsReBitmap  = 0x0FFF;          /* DMRS CDMGroup0/1/2，均有DMRS，无DATA RE*/
                        dataReInSymb  = 0;
                    }
                    else if (l1PuschUeInfo->numCdmGrpsNoData == 2){
                        puschDeqAndDemapUePara->dmrsReBitmap  = 0x03CF;          /* DMRS CDMGroup0/1，均有DMRS，无DATA RE*/
                        dataReInSymb  = 4;
                    }
                    else if (l1PuschUeInfo->numCdmGrpsNoData == 1){
                        puschDeqAndDemapUePara->dmrsReBitmap = 0x00C3;           /* DMRS CDMGroup0，均有DMRS，无DATA RE*/
                        dataReInSymb  = 8;
                    }
                    else {
                        return 0xFFFFFFFF;
                    }
                }
                else {
                    return 0xFFFFFFFF;
                }

                *puschDeqAndDemapUePara->dmrsHInBaseAddr1 = 0x00000000; /* DMRS 地址每个ue的每个DMRS符号一个地址，再通过调度RB数确定信号估计数据长度 */
                *puschDeqAndDemapUePara->dmrsHInBaseAddr2 = 0x00000001;
                *puschDeqAndDemapUePara->dmrsHInBaseAddr3 = 0x00000002;
                *puschDeqAndDemapUePara->ruuInBaseAddr    = 0x00000003; 
            }
            else{
                puschDeqAndDemapUePara->dmrsSymbFlag     = 0;
                puschDeqAndDemapUePara->dmrsReBitmap     = 0;
                dataReInSymb                             = N_SC_PER_PRB;
                /* ue的每个数据符号，配置差值系数 差值系数待定 */
                puschDeqAndDemapUePara->equInterCoeff[0] = 0; 
                puschDeqAndDemapUePara->equInterCoeff[1] = 0;
            }
            
            rbNum    = puschDemapParaTemp[ueIndex].rbNumAllRbg;
            layerNum = puschDeqAndDemapUePara->layerNum;
            qamMode  = puschDeqAndDemapUePara->deModType;
            puschDeqAndDemapUePara->scrambleReOffset += rbNum * dataReInSymb * layerNum * qamMode;
            
            /* 插值模式选择 */
            if ((g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex] == 1) || (g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex] == 3)){
                puschDeqAndDemapUePara->timeInterMode = 0; /* 1符号DMRS和3符号DMRS 均采用平推 */
            }
            else if (g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex] == 2){
                if (symbIndex < ((symbStart + symbEnd) >> 1)){
                    puschDeqAndDemapUePara->timeInterMode = 0; 
                }
                else{
                    puschDeqAndDemapUePara->timeInterMode = 1; /*0：平推，1：差值； 两符号DMRS前一半平推，后一半差值，如果调度符号是奇数，前一半占用的符号少 */
                }
            }

            puschDeqAndDemapUePara->palcehoderFlag      = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackRvdExsitFlag;
            puschDeqAndDemapUePara->uciReStart          = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackRvdStartRe;
            puschDeqAndDemapUePara->uciReDistance       = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackRvdDistance;
            puschDeqAndDemapUePara->uciReNum            = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackRvdReNum;
            
            puschDeqAndDemapUePara->ackFlag             = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackExsitFlag;
            puschDeqAndDemapUePara->ackReStart          = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackStartRe;
            puschDeqAndDemapUePara->ackReDistance       = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackDistance;
            puschDeqAndDemapUePara->ackReNum            = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackReNum;

            puschDeqAndDemapUePara->csiPart1Flag        = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].csiPart1ExsitFlag;
            puschDeqAndDemapUePara->csiPart1ReStart     = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].csiPart1StartRe;
            puschDeqAndDemapUePara->csiPart1ReDistance  = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].csiPart1Distance;
            puschDeqAndDemapUePara->csiPart1ReNum       = puschDemapParaTemp[ueIndex].puschAckAndCsiPart1Info[symbIndex].ackReNum;
            
            /* 地址待定 */
            *puschDeqAndDemapUePara->scrambBaseAddrC0In = 0x00000000;
            *puschDeqAndDemapUePara->scrambBaseAddrC1In = 0x00000001;
            *puschDeqAndDemapUePara->postSinrAverOut    = 0x00000002;
            *puschDeqAndDemapUePara->demuxAckOut        = 0x00000003;
            *puschDeqAndDemapUePara->demuxpart1Out      = 0x00000004;
            *puschDeqAndDemapUePara->demuxpart2DataOut  = 0x00000005;
            validUeNum++;
        }
        puschDeqAndDemapSymPara->validUeNum = validUeNum;
    }
    return 0;
}

uint32_t L1PuschPara2LdpcDecoderHac(uint8_t cellIndex, L1PuschParaPduInfo *l1PuschParaPduInfo, PuschLdpcDecoderHacPara *ldpcDecoderHacPara)
{
    uint8_t  ueIndex, ueNum, slotIndex, baseGraphType, rvIndex, harqId;
    uint8_t  dmrsSymbNum, ueSymbNum, dmrsNoDataNum, dmrsRePerRB;
    uint8_t  cbRmLen0, cbRmLen1;
    uint8_t  qamModer, layerNum, cbNum;
    uint16_t rbSize, ulNref, ulResourceG;
    uint16_t iLoop, cbPartRow[3];
    PuschDemapParaTemp puschDemapParaTemp;
    L1PuschPduInfo  *l1PuschUeInfo   = NULL;
    PuschLdpcUePara *puschLdpcUePara = NULL;
   
    ldpcDecoderHacPara->sfnIndex  = l1PuschParaPduInfo->sfnIndex;
    ldpcDecoderHacPara->slotIndex = l1PuschParaPduInfo->slotIndex;
    ldpcDecoderHacPara->ueNum     = l1PuschParaPduInfo->puschPduNum;
    ldpcDecoderHacPara->maxIterNum       = 10;  /* 算法配置参数待定 */
    ldpcDecoderHacPara->reviseFactor     = 192; /* 修正因子，默认值为0.75 */
    ldpcDecoderHacPara->harqCompressType = 0;   /* 压缩模式，需要Demap按照相同压缩模式输出 */
    ldpcDecoderHacPara->decodeAlgType    = 0;   /* 译码算法选择 */
    ueNum     = l1PuschParaPduInfo->puschPduNum;
    slotIndex = l1PuschParaPduInfo->slotIndex;

    for (ueIndex = 0; ueIndex < ueNum; ueIndex++){
        l1PuschUeInfo   = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
        puschLdpcUePara = &ldpcDecoderHacPara->puschLdpcUePara[ueIndex];
        baseGraphType   = l1PuschUeInfo->ldpcBaseGraph - 1;

        L1PuschTbParaCalculate(l1PuschUeInfo, &puschDemapParaTemp);   /* TB参数计算 */
        
        memset(&cbPartRow[0], 0x0, sizeof(uint16_t) * 3);
        puschLdpcUePara->ueIndex          = ueIndex;
        puschLdpcUePara->tbCrcType        = puschDemapParaTemp.tbCrcType;
        puschLdpcUePara->cbNum            = puschDemapParaTemp.cbNum;
        puschLdpcUePara->ldpcKd           = puschDemapParaTemp.ulKd;
        puschLdpcUePara->cbFillBitLen     = puschDemapParaTemp.cbFillingLen;
        puschLdpcUePara->ldpcZc           = puschDemapParaTemp.ulZc;
        puschLdpcUePara->ldpcBgId         = g_puschLdpcBgId[baseGraphType][puschDemapParaTemp.iLs];
        puschLdpcUePara->qamMode          = l1PuschUeInfo->qamModOrder;
        puschLdpcUePara->layerNum         = l1PuschUeInfo->nrOfLayers;
        puschLdpcUePara->tbSize           = l1PuschUeInfo->puschDataPara.tbSize;

        rbSize        = g_puschUeRbsize[cellIndex][slotIndex][ueIndex];
        dmrsSymbNum   = g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex];
        ueSymbNum     = l1PuschUeInfo->nrOfSymbols;
        dmrsNoDataNum = l1PuschUeInfo->numCdmGrpsNoData;
        dmrsRePerRB   = l1PuschUeInfo->dmrsCfgType == 1 ? NR_DMRS_TYPE1_SC_PER_RB : NR_DMRS_TYPE2_SC_PER_RB;
        ulResourceG   = ueSymbNum * rbSize * N_SC_PER_PRB - (dmrsSymbNum * rbSize * dmrsNoDataNum * dmrsRePerRB); /* 暂时不考虑PTRS：SCH可以在除dmrs no data之外的re上传输 */
        qamModer      = puschLdpcUePara->qamMode;
        layerNum      = puschLdpcUePara->layerNum;
        cbNum         = puschLdpcUePara->cbNum;
        cbRmLen0      = qamModer * layerNum * (ulResourceG / (qamModer * layerNum * cbNum));
        cbRmLen1      = qamModer * layerNum * ceilDiv(ulResourceG, (qamModer * layerNum * cbNum));
        puschLdpcUePara->cbRmLen0 = cbRmLen0;
        puschLdpcUePara->cbRmLen1 = cbRmLen1;
        puschLdpcUePara->cbRmLen0Num = cbNum - (ulResourceG % (qamModer * layerNum)) - 1;

        if (l1PuschUeInfo->puschDataPara.newData == 0){ /* 0:retransmission, 1: new Data */
            puschLdpcUePara->harqCombFlag = 1;
            memset(&puschLdpcUePara->rBgUsedBitmap[0], 0x0, sizeof(uint16_t) * 3);
        }
        else{
            puschLdpcUePara->harqCombFlag = 0;
        }

        if (l1PuschUeInfo->tbSizeLbrmBytes == 0){ /* FAPI使用tbSizeLbrm指示I-LBRM, I-LBRM为0 */
            puschLdpcUePara->ldpcNcb = puschDemapParaTemp.ulN;
        }
        else{/* I-LBRM 非0 */
            ulNref = (l1PuschUeInfo->tbSizeLbrmBytes * 8 * 3) / (puschLdpcUePara->cbNum * 2);
            puschLdpcUePara->ldpcNcb = (ulNref < puschDemapParaTemp.ulN) ? ulNref : puschDemapParaTemp.ulN;
        } 
        
        rvIndex = l1PuschUeInfo->puschDataPara.rvIndex;
        puschLdpcUePara->ldpcK0 = ((g_puschK0Coeff[baseGraphType][rvIndex] * puschLdpcUePara->ldpcNcb) / puschDemapParaTemp.ulN) * puschLdpcUePara->ldpcZc;

        L1PuschLdpcCbPartRowCalculate(puschLdpcUePara, baseGraphType, &cbPartRow[0]);
        
        for (iLoop = 0; iLoop < 3; iLoop++){
            puschLdpcUePara->rBgUsedBitmap[iLoop] |= cbPartRow[iLoop];
        }
    }

    return 0;
}

uint32_t L1PuschTbParaCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschDemapParaTemp *puschDemapParaTemp)
{
    uint8_t  iLoop, jLoop, liftNumCnt, iLs, liftSizeIndex;
    uint8_t  crcLength, tbCrcType, baseGraph;
    uint16_t minValue, minValueIndex;
    uint16_t tbSizeA, tbSizeB, tbSizeBp, ulKd, Zc;
    uint16_t kCb, kb, tempZc, fillingLen;
    uint16_t liftingInfo[8][3]; //0: Size value; 1 : iLs; 2: liftSizeIdx
     
    tbSizeA = l1PuschUeInfo->puschDataPara.tbSize * 8; /* puschDataPara.tbSize为Byte 转换为bit */
    if (3824 < tbSizeA){
        crcLength = 24;
        tbCrcType = 1;  /* 1:24A，2:24B，3:24C，4:16，5:11，6:6，7:8 */
    }
    else{
        crcLength = 16;
        tbCrcType = 4;  /* 1:24A，2:24B，3:24C，4:16，5:11，6:6，7:8 */
    }
    tbSizeB = tbSizeA + crcLength;
    puschDemapParaTemp->tbCrcType = tbCrcType;
    
    if (l1PuschUeInfo->ldpcBaseGraph == 2)
    {
        kCb = 3840;
        if (tbSizeB > 640){
            kb = 10;     
        }
        else if(tbSizeB > 560){
            kb = 9;    
        }
        else if(tbSizeB > 192){
            kb = 8;    
        }
        else{
            kb = 6;    
        }
    }
    else{
        kCb = 8448;
        kb  = 22;
    }

    if(tbSizeB <= kCb){
        crcLength = 0;
        puschDemapParaTemp->cbNum     = 1;
    }
    else{
        crcLength = 24;
        puschDemapParaTemp->cbNum     = ceilDiv(tbSizeB, kCb - crcLength);
    }
    tbSizeBp = tbSizeB + (puschDemapParaTemp->cbNum) * crcLength;
    ulKd     = tbSizeBp / (puschDemapParaTemp->cbNum); /* 每个CB中的bit数 */
    puschDemapParaTemp->ulKd         = ulKd;

    /*get Zc from Table 5.3.2-1: Sets of LDPC lifting size  */
    tempZc = ceilDiv(ulKd, kb);
    liftNumCnt = 0;
    for (iLoop = 0; iLoop < 8; iLoop++){
        for (jLoop = 0; jLoop < 8; jLoop++){
            if (tempZc <= g_liftSizeTable[iLoop][jLoop]){
                liftingInfo[liftNumCnt][0] = g_liftSizeTable[iLoop][jLoop];
                liftingInfo[liftNumCnt][1] = iLoop;
                liftingInfo[liftNumCnt][2] = jLoop;
                liftNumCnt++;
                break;
            }
        }
    }
    
    /* 获取Zc最小值 */
    minValue = liftingInfo[0][0];
    minValueIndex = 0;
    for (iLoop = 1; iLoop < liftNumCnt; iLoop++){
        if(minValue > liftingInfo[iLoop][0]){
            minValue      = liftingInfo[iLoop][0];
            minValueIndex = iLoop;
        }
    }

    puschDemapParaTemp->ulZc = liftingInfo[minValueIndex][0];
    puschDemapParaTemp->iLs  = liftingInfo[minValueIndex][1];
    puschDemapParaTemp->ulKr = (l1PuschUeInfo->ldpcBaseGraph == 1) ? (22 * Zc) : (10 * Zc);
    puschDemapParaTemp->ulN  = (l1PuschUeInfo->ldpcBaseGraph == 1) ? (66 * Zc) : (50 * Zc);
    puschDemapParaTemp->cbFillingLen = puschDemapParaTemp->ulKr - puschDemapParaTemp->ulKd;

    return 0;
}

uint32_t L1PuschUciCrcLengthCalculate(uint16_t ucibitlength, uint8_t *crcLength)
{
    if(ucibitlength > 0)
    {
        if(ucibitlength > 360){
            *crcLength = 11;
        }
        else if((ucibitlength < 360)&&(ucibitlength >=20)){
            *crcLength = 11;
        }
        else if((ucibitlength <= 19)&&(ucibitlength >= 12)){
            *crcLength = 6;
        }
        else{
            *crcLength = 0;
        }
    }
    return 0;
}

uint32_t L1PuschUciEncodeBitCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschDemapParaTemp *puschDemapParaTemp)
{
    uint8_t  crcLengthAck, crcLengthPart1;
    uint8_t  alphaScaling, betaOffsetAck, betaOffsetPart1;
    uint8_t  qamMode, cbNum, dmrsRePerRB, dmrsNoDataNum;
    uint8_t  dmrsSymb, dataSymb;
    uint16_t hackAckBitLength, csiPart1BitLength;
    uint16_t qAckP0, qAckP1, qPart1P0, qPart1P1;
    uint16_t codeRate, rbSize;
    uint16_t ulKr, ueSCHReNum, ueUCIReNum, ueUCIReNumAck;

    hackAckBitLength  = l1PuschUeInfo->puschUciPara.harqAckBitLength;
    csiPart1BitLength = l1PuschUeInfo->puschUciPara.csiPart1BitLength;
    puschDemapParaTemp->hackAckbit  = hackAckBitLength;
    puschDemapParaTemp->csiPart1bit = csiPart1BitLength;
    
    L1PuschUciCrcLengthCalculate(hackAckBitLength, &crcLengthAck);
    L1PuschUciCrcLengthCalculate(csiPart1BitLength, &crcLengthPart1);
    
    rbSize        = puschDemapParaTemp->rbNumAllRbg;
    dmrsSymb      = puschDemapParaTemp->dmrsSymbNum;
    dataSymb      = puschDemapParaTemp->dataSymbNum;
    dmrsNoDataNum = l1PuschUeInfo->numCdmGrpsNoData;
    dmrsRePerRB   = l1PuschUeInfo->dmrsCfgType == 1 ? NR_DMRS_TYPE1_SC_PER_RB : NR_DMRS_TYPE2_SC_PER_RB;
    ulKr          = puschDemapParaTemp->ulKr;
    cbNum         = puschDemapParaTemp->cbNum;
    alphaScaling  = l1PuschUeInfo->puschUciPara.alphaScaling;
    codeRate      = l1PuschUeInfo->targetCodeRate;
    qamMode       = l1PuschUeInfo->qamModOrder;
    ueSCHReNum    = (dmrsSymb + dataSymb) * rbSize * N_SC_PER_PRB - (dmrsSymb * rbSize * dmrsNoDataNum * dmrsRePerRB); /* 暂时不考虑PTRS：SCH可以在除dmrs no data之外的re上传输 */
    ueUCIReNum    = dataSymb * rbSize * N_SC_PER_PRB;  /* 暂时不考虑PTRS：UCI可在除DMRS符号之外的符号上传输 */
    ueUCIReNumAck = ueUCIReNum - puschDemapParaTemp->firstDmrsIndex * rbSize * N_SC_PER_PRB;

    if(l1PuschUeInfo->pduBitMap == 1){
        /* PUSCH Only *///只需要区分DMRS即可
       
    }
    else if(l1PuschUeInfo->pduBitMap == 2){/* UCI only */
        /* HARQ-ACK */
        qAckP0   = ceilDiv((hackAckBitLength + crcLengthAck) * betaOffsetAck, codeRate*qamMode/1024);
        qAckP1   = ceilDiv(alphaScaling * ueUCIReNumAck, 1); /* 向上取整待修改 */
        puschDemapParaTemp->enCodeAckRe = (qAckP0 < qAckP1) ? qAckP0 : qAckP1; 
        /* CSI-Part1 */
        qPart1P0 = ceilDiv((csiPart1BitLength + crcLengthPart1) * betaOffsetPart1, codeRate*qamMode/1024);
        qPart1P1 = ueUCIReNum - puschDemapParaTemp->enCodeAckRe; 
        puschDemapParaTemp->enCodeCsiPart1Re = (qPart1P0 < qPart1P1) ? qPart1P0 : qPart1P1; 
    }
    else if(l1PuschUeInfo->pduBitMap == 3){/* PSUCH + UCI */
        /* HARQ-ACK */
        qAckP0   = ceilDiv((hackAckBitLength + crcLengthAck) * betaOffsetAck * ueUCIReNum, cbNum * ulKr);
        qAckP1   = ceilDiv(alphaScaling * ueUCIReNumAck, 1); /* 向上取整待修改 */
        puschDemapParaTemp->enCodeAckRe = (qAckP0 < qAckP1) ? qAckP1 : qAckP1; 
        /* CSI-Part1 */
        qPart1P0 = ceilDiv((csiPart1BitLength + crcLengthPart1) * betaOffsetPart1 * ueUCIReNum, cbNum * ulKr);
        qPart1P1 = ceilDiv(alphaScaling * ueUCIReNum, 1) - puschDemapParaTemp->enCodeAckRe; 
        puschDemapParaTemp->enCodeCsiPart1Re = (qPart1P0 < qPart1P1) ? qPart1P0 : qPart1P1; 
    }

    return 0;
}

uint32_t L1PuschUciReLocationCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschDemapParaTemp *puschDemapParaTemp)
{
    uint8_t  qamMode, layerNum, ackReRvdDistance, ackReDistance, csiPart1Distance;
    uint8_t  symbIndex;
    uint16_t mAckCnt, nGAck, nGAckRvd, mReCnt, reIndex;
    uint16_t nMUciSc, ackReNumRvd[SYM_NUM_PER_SLOT], nMUciScBar[SYM_NUM_PER_SLOT];
    uint16_t mCsiPart1Cnt, nGCsiPart1, nMCsiPart1Sc;

    qamMode  = l1PuschUeInfo->qamModOrder;
    layerNum = l1PuschUeInfo->nrOfLayers;
    nGAckRvd = puschDemapParaTemp->enCodeAckRe * qamMode * layerNum;
    nGAck    = nGAckRvd;
    nMUciSc  = puschDemapParaTemp->rbNumAllRbg * N_SC_PER_PRB; /* 每个符号上的有效RE数，UCI不能在DMRS符号上传送 */

    mAckCnt  = 0;
    symbIndex = puschDemapParaTemp->firstDmrsIndex + 1; /* ACK 从第一个DMRS之后的数据符号开始映射, 当前只会配置单列导频 */
    if(puschDemapParaTemp->hackAckbit <= 2){/* step1: 计算ACK预留资源 */
        while (mAckCnt < nGAckRvd) 
        {
            nMUciScBar[symbIndex] = nMUciSc;
            if(nMUciScBar[symbIndex] > 0){
                if((nGAckRvd - mAckCnt) >= (nMUciScBar[symbIndex] * layerNum * qamMode)){
                    ackReRvdDistance = 1;
                    mReCnt           = nMUciScBar[symbIndex];
                }
                else{
                    ackReRvdDistance = (nMUciScBar[symbIndex] * layerNum * qamMode) / (nGAckRvd - mAckCnt);
                    mReCnt           = ceilDiv((nGAckRvd - mAckCnt), layerNum * qamMode);
                }

                for (reIndex = 0; reIndex < mReCnt; reIndex++){
                    //ackRvdRE[symbIndex][reIndex] = reIndex * reDistance; /* 资源位置可以通过 reDistance, mReCnt联合确认，不需要提前存储 */
                    mAckCnt = mAckCnt + layerNum * qamMode;
                }
            
                symbIndex = symbIndex + 1;
                if ((symbIndex == puschDemapParaTemp->secondDmrsIndex) || (symbIndex == puschDemapParaTemp->thirdDmrsIndex)){
                    symbIndex = symbIndex + 1;
                }

                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackRvdExsitFlag = 1;
                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackRvdStartRe   = 0;
                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackRvdDistance  = ackReRvdDistance;
                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackRvdReNum     = mReCnt;
                ackReNumRvd[symbIndex] = mReCnt;
            }   
        }

        mAckCnt   = 0;
        symbIndex = puschDemapParaTemp->firstDmrsIndex + 1; /* ACK 从第一个DMRS之后的数据符号开始映射 */
        while (mAckCnt < nGAck) /* step5: 计算ACK实际占用资源 */
        {
            if (ackReNumRvd[symbIndex] > 0){
                if((nGAck - mAckCnt) >= ackReNumRvd[symbIndex] * layerNum * qamMode){
                    ackReDistance = 1;
                    mReCnt        = ackReNumRvd[symbIndex];
                }
                else{
                    ackReDistance = (ackReNumRvd[symbIndex] * layerNum * qamMode) / (nGAck - mAckCnt);
                    mReCnt        = ceilDiv((nGAck - mAckCnt) , (layerNum * qamMode));
                }

                for(reIndex = 0; reIndex < mReCnt; reIndex++)
                {
                    //ackRE[symbIndex][reIndex] = ackRvdRE[symbIndex][reIndex * ackReDistance];
                    mAckCnt = mAckCnt + layerNum * qamMode;
                }

                symbIndex = symbIndex + 1;
                if ((symbIndex == puschDemapParaTemp->secondDmrsIndex)||(symbIndex == puschDemapParaTemp->thirdDmrsIndex)){
                    symbIndex = symbIndex + 1;
                }

                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackExsitFlag = 1;
                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackStartRe   = 0;
                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackDistance  = ackReRvdDistance * ackReDistance;
                puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackReNum     = mReCnt;
            }
        }
    } 
    else  /* step2: ACK资源直接映射 */
    {
        while (mAckCnt < nGAck) 
        {
            nMUciScBar[symbIndex] = nMUciSc;
            if( nMUciScBar[symbIndex] > 0){
                if((nGAck - mAckCnt) >= nMUciScBar[symbIndex] * layerNum * qamMode){
                    ackReDistance = 1;
                    mReCnt        = nMUciScBar[symbIndex];
                }
                else{
                    ackReDistance = (nMUciScBar[symbIndex] * layerNum * qamMode) / (nGAck - mAckCnt);
                    mReCnt        = ceilDiv((nGAck - mAckCnt) , (layerNum * qamMode));
                }

                for(reIndex = 0; reIndex < mAckCnt; reIndex++){
                    //ackRE[symbIndex][reIndex] = reIndex * ackReDistance;
                    mAckCnt = mAckCnt + layerNum * qamMode;
                }
                nMUciScBar[symbIndex] = nMUciScBar[symbIndex] - mReCnt;
            }

            symbIndex = symbIndex + 1;
            if ((symbIndex == puschDemapParaTemp->secondDmrsIndex)||(symbIndex == puschDemapParaTemp->thirdDmrsIndex)){
                symbIndex = symbIndex + 1;
            }

            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackExsitFlag = 1;
            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackStartRe   = 0;
            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackDistance  = ackReDistance;
            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackReNum     = mReCnt;
        }
    }

    if(puschDemapParaTemp->csiPart1bit > 0){  /* step3: 第一部分part1资源位置计算 */
        nGCsiPart1   = puschDemapParaTemp->enCodeCsiPart1Re * layerNum * qamMode;
        
        mCsiPart1Cnt = 0;
        symbIndex    = l1PuschUeInfo->startSymbIndex; /* symbIndex 不能为dmrs符号 */
        nMCsiPart1Sc = nMUciSc - puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].ackRvdReNum;
        while (nMCsiPart1Sc <= 0) {
            symbIndex = symbIndex + 1; 
        }
        
        while (mCsiPart1Cnt < nGCsiPart1)
        {
            if(nMCsiPart1Sc > 0){
                if((nGCsiPart1 - mCsiPart1Cnt) >= nMCsiPart1Sc * layerNum * qamMode){
                    csiPart1Distance = 1;
                    mReCnt           = nMCsiPart1Sc;
                }
                else{
                    csiPart1Distance = nMCsiPart1Sc * layerNum * qamMode / (nGCsiPart1 - mCsiPart1Cnt);
                    mReCnt           = ceilDiv((nGCsiPart1 - mCsiPart1Cnt), layerNum * qamMode);
                }
            }

            for(reIndex = 0; reIndex < mReCnt; reIndex++){
                //csiPart1RE[symbIndex][reIndex] = reIndex * ackReDistance;
                mCsiPart1Cnt = mCsiPart1Cnt + layerNum * qamMode;
            }

            symbIndex = symbIndex + 1;
            if ((symbIndex == puschDemapParaTemp->firstDmrsIndex) || (symbIndex == puschDemapParaTemp->secondDmrsIndex) || (symbIndex == puschDemapParaTemp->thirdDmrsIndex)){
                symbIndex = symbIndex + 1;
            }

            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].csiPart1ExsitFlag = 1;
            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].csiPart1StartRe   = 0;
            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].csiPart1Distance  = csiPart1Distance;
            puschDemapParaTemp->puschAckAndCsiPart1Info[symbIndex].csiPart1ReNum     = mReCnt;
        }
    }

    return 0; 
}

uint32_t L1PuschLdpcCbPartRowCalculate(PuschLdpcUePara *puschLdpcUePara, uint8_t baseGraphType, uint16_t *cbPartRow)
{
    uint16_t nSys, nSysCp;
    uint16_t ldpcK0, ldpcKd;
    uint16_t ldpcZc, fillingLen, ldpcNcb;
    uint16_t expStart, sysCorep, cbRmLen1;
    uint16_t startRow, endRow;
    uint16_t iLoop, intPart, remaindPart;
     
    ldpcZc = puschLdpcUePara->ldpcZc;
    if (baseGraphType == 0){
        nSys   = 20 * ldpcZc;
        nSysCp = 24 * ldpcZc;
    }
    else {
        nSys   = 8 * ldpcZc;
        nSysCp = 12 * ldpcZc;
    }  
    
    ldpcK0     = puschLdpcUePara->ldpcK0;
    ldpcKd     = puschLdpcUePara->ldpcKd;
    ldpcKd     = ldpcKd - 2 * ldpcZc;
    fillingLen = nSysCp - nSys;
    if (ldpcK0 <= ldpcKd){
        expStart = nSysCp;
        sysCorep = nSysCp - ldpcK0 - fillingLen;
    }
    else if (ldpcKd < ldpcK0 <= nSys){
        expStart = nSysCp;
        sysCorep = nSysCp - nSys;
    }
    else if (nSys < ldpcK0 <= nSysCp){
        expStart = nSysCp;
        sysCorep = nSysCp - ldpcK0;
    }
    else{ /* ldpcK0 > nSysCp */
        expStart = ldpcK0;
        sysCorep = 0; 
    }

    ldpcNcb  = puschLdpcUePara->ldpcNcb;
    cbRmLen1 = puschLdpcUePara->cbRmLen1;
    
    if ((cbRmLen1 - sysCorep) <= (ldpcNcb - expStart)){
        startRow = (expStart - nSysCp) / ldpcZc;
        endRow   = startRow + ceilDiv(cbRmLen1 - sysCorep, ldpcZc);
    }
    else if ((ldpcNcb - expStart) < (cbRmLen1 - sysCorep) <= (ldpcNcb - expStart + nSysCp - fillingLen)){
        startRow = (expStart - nSysCp) / ldpcZc;
        endRow   = startRow + ceilDiv(ldpcNcb - expStart, ldpcZc);
    }
    else if ((ldpcNcb - expStart + nSysCp - fillingLen) < (cbRmLen1 - sysCorep) <= (ldpcNcb - fillingLen)){
        startRow = 0;
        endRow   = startRow + ceilDiv(cbRmLen1 - (ldpcNcb - expStart + sysCorep + nSysCp - fillingLen), ldpcZc);
    }
    else{
        startRow = 0;
        endRow   = startRow + ceilDiv((ldpcNcb - nSysCp), ldpcZc);
    }
    
    for (iLoop = startRow; iLoop < endRow; iLoop++){
        intPart     = iLoop / 16;
        remaindPart = iLoop % 16;
        cbPartRow[intPart] |= (uint16_t)1 << remaindPart;
    }

    return 0;
}

uint32_t PuschACK1or2BitDecodeHandler()//待李雷雷提供ACK 1/2比特译码函数
{
    
    
    return 0;
}

uint32_t PuschUCIParser()//ACK大于2比特，CSI Part1和CSI Part2解析
{
    uint16_t sfn;     
    uint16_t slot;
    uint16_t uciLen;  
    uint8_t  cellIdx;
    uint8_t  pduNum;
    uint8_t  codeType;
    uint8_t  msgType;
    uint8_t  pduIndex;
    uint8_t  ueIdx;
    uint8_t  numPart1Params;
    uint16_t *map              = NULL;
    uint8_t  *sizesPart1Params = NULL;
    PuschPolarDecodeHacCfgPara   *puschPolarDecodeHacCfgPara   = NULL;
    PuschRMDecodeHacCfgPara      *puschRMDecodeHacCfgPara      = NULL;
    HacCfgHead                   *hacCfgHead                   = NULL;
    PolarDecodePduInfo           *polarDecodePduInfo           = NULL;
    RMDecodePduInfo              *rmDecodePduInfo              = NULL;
    RMUeDecodeOut                *rmUeDecodeOut                = NULL;
    PolarUeDecodeOut             *polarUeDecodeOut             = NULL;
    PuschUciRst                  *puschUciRst                  = NULL;
    FapiNrPushUciIndication      *fapiNrPushUciIndication      = NULL;
    HARQInfoFmt23                *harqInfoFmt23                = NULL;
	CSIpart1Info                 *csipart1Info                 = NULL;
	CSIpart2Info                 *csipart2Info                 = NULL;
    FapiNrMsgPuschPduInfo        *fapipuschpduInfo             = NULL;

    //先解析消息头？  
    sfn      =  0; //待从消息头获取    
    slot     =  0; //待从消息头获取
    cellIdx  =  0; //待从消息头获取  
    pduNum   =  0; //待从消息头获取 
    codeType =  0; //待从消息头获取，确认是RM还是Polar译码结果
    msgType  =  0; //待从消息头获取，确认是ACK,CSI Part1还是CSI Part2  
    
    puschUciRst = &(g_puschUciRst[cellIdx][slot]);//UCI译码结果回写目的地址

    if(0 == codeType)//RM
    {
        puschRMDecodeHacCfgPara = &(g_puschRMDecodeHacCfgParaDDR[cellIdx][slot][msgType]);//读取RM译码结果
        hacCfgHead = &(puschRMDecodeHacCfgPara->hacCfgHead);
        //hacCfgHead->增加校验？

        for(pduIndex = 0; pduIndex < pduNum; pduIndex++)
        {
            rmDecodePduInfo = &(puschRMDecodeHacCfgPara->rmPduInfo[pduIndex]);
            rmUeDecodeOut   = (RMUeDecodeOut *)(rmDecodePduInfo->OutputAddr);//UE输出结果地址
            
            ueIdx = rmDecodePduInfo->ueIdx;//待从配置参数获取,还需要转换为UCI上报对应的UE索引
			if(0 == msgType)//ACK大于2比特
			{
				harqInfoFmt23 = &(puschUciRst->fapiNrPushUciIndication[ueIdx].harqInfoFmt23);
				harqInfoFmt23->HarqCrc        = 0;//待补充
				harqInfoFmt23->HarqBitLen     = rmDecodePduInfo->uciBitNum;
				harqInfoFmt23->HarqPayload[0] = 0;//L2D到DDR的拷贝,待补充
			}
			else if(1 == msgType)//CSI Part1
			{
				csipart1Info = &(puschUciRst->fapiNrPushUciIndication[ueIdx].csipart1Info);
				csipart1Info->CsiPart1Crc        = 0;//待补充
				csipart1Info->CsiPart1BitLen     = rmDecodePduInfo->uciBitNum;
				csipart1Info->CsiPart1Payload[0] = 0;//L2D到DDR的拷贝,待补充
                if(1)//如果存在CSI Part2
                {
                    //fapipuschpduInfo = ;//待补充
                    //sizesPart1Params = ;//待补充
                    //map              = ;//待补充
                    //numPart1Params   = ;//待补充
                    uciLen = CalcPucchCsipart2Size(&(fapipuschpduInfo->uciInfoAddInV3), csipart1Info->CsiPart1Payload, sizesPart1Params, map, csipart1Info->CsiPart1BitLen, numPart1Params);
                    if((3 <= uciLen) && (11 >= uciLen))
                    {
                        //PuschRMDecodeHacCfg(fapipuschpduInfo, uciLen, pduIndex, PUCCH_UCI_PART2, sfn, slot, cellIdx);
                    }
                    else if(360 > uciLen)
                    {
                        //PuschPolarDecodeHacCfg(fapipuschpduInfo, uciLen, pduIndex, PUCCH_UCI_PART2, sfn, slot, cellIdx);
                    } 
                }
			}
			else if(2 == msgType)//CSI Part2
			{
				csipart2Info = &(puschUciRst->fapiNrPushUciIndication[ueIdx].csipart2Info);
				csipart2Info->CsiPart2Crc        = 0;//待补充
				csipart2Info->CsiPart2BitLen     = rmDecodePduInfo->uciBitNum;
				csipart2Info->CsiPart2Payload[0] = 0;//L2D到DDR的拷贝,待补充
			}
			else
			{
				;//异常处理
			}
        }
    }
    else if(1 == codeType)//Polar
    {
        puschPolarDecodeHacCfgPara = &(g_puschPolarDecodeHacCfgParaDDR[cellIdx][slot][msgType]);//读取Polar译码结果
        hacCfgHead = &(puschPolarDecodeHacCfgPara->hacCfgHead);
        //hacCfgHead->增加校验？

        for(pduIndex = 0; pduIndex < pduNum; pduIndex++)
        {
            polarDecodePduInfo = &(puschPolarDecodeHacCfgPara->polarPduInfo[pduIndex]);
            polarUeDecodeOut   = (PolarUeDecodeOut *)(polarDecodePduInfo->OutputAddr);//UE输出结果地址
            
            ueIdx = polarDecodePduInfo->ueIdx;//待从配置参数获取,还需要转换为UCI上报对应的UE索引
			if(0 == msgType)//ACK大于2比特
			{
				harqInfoFmt23 = &(puschUciRst->fapiNrPushUciIndication[ueIdx].harqInfoFmt23);
				harqInfoFmt23->HarqCrc        = 0;//polarUeDecodeOut->待补充
				harqInfoFmt23->HarqBitLen     = polarDecodePduInfo->uciBitNum;
				harqInfoFmt23->HarqPayload[0] = 0;//polarUeDecodeOut,L2D到DDR的拷贝,待补充
			}
			else if(1 == msgType)//CSI Part1
			{
				csipart1Info = &(puschUciRst->fapiNrPushUciIndication[ueIdx].csipart1Info);
				csipart1Info->CsiPart1Crc        = 0;//polarUeDecodeOut->待补充
				csipart1Info->CsiPart1BitLen     = polarDecodePduInfo->uciBitNum;
				csipart1Info->CsiPart1Payload[0] = 0;//polarUeDecodeOut,L2D到DDR的拷贝,待补充
                if(1)//如果存在CSI Part2
                {
                    //fapipuschpduInfo = ;//待补充
                    //sizesPart1Params = ;//待补充
                    //map              = ;//待补充
                    //numPart1Params   = ;//待补充
                    uciLen = CalcPucchCsipart2Size(&(fapipuschpduInfo->uciInfoAddInV3), csipart1Info->CsiPart1Payload, sizesPart1Params, map, csipart1Info->CsiPart1BitLen, numPart1Params);
                    if((3 <= uciLen) && (11 >= uciLen))
                    {
                        //PuschRMDecodeHacCfg(fapipuschpduInfo, uciLen, pduIndex, PUCCH_UCI_PART2, sfn, slot, cellIdx);
                    }
                    else if(360 > uciLen)
                    {
                        //PuschPolarDecodeHacCfg(fapipuschpduInfo, uciLen, pduIndex, PUCCH_UCI_PART2, sfn, slot, cellIdx);
                    } 
                }
			}
			else if(2 == msgType)//CSI Part2
			{
				csipart2Info = &(puschUciRst->fapiNrPushUciIndication[ueIdx].csipart2Info);
				csipart2Info->CsiPart2Crc        = 0;//polarUeDecodeOut->待补充
				csipart2Info->CsiPart2BitLen     = polarDecodePduInfo->uciBitNum;
				csipart2Info->CsiPart2Payload[0] = 0;//polarUeDecodeOut,L2D到DDR的拷贝,待补充
			}
			else
			{
				;//异常处理
			}
        }
    }
    else
    {
        ;//异常
    }
    return 0;
}


uint32_t PuschACKOver2BitParseHandler()
{
    printf("解析大于2比特的ACK译码结果\n");//ACK大于2比特
    
    PuschUCIParser();
    
    return 0;
}

uint32_t PuschPart1AndLDPCParaCfgHandler()
{
    printf("配置Part1译码参数&&配置无Part2 UE的LDPC译码参数\n");//
    
    //配置CSI Part1译码参数

    //配置无CSI Part2的UE的LDPC译码参数

    return 0;
}

uint32_t PuschPart1ParsePart2AndLDPCParaCfgHandler()
{
    printf("解析Part1&&配置Part2译码参数&&配置含Part2的UE的LDPC译码参数\n");
    
    PuschUCIParser();//解析CSI Part1，顺便配置CSI Part2参数
    
    //配置含CSI Part2的UE的LDPC译码参数

    return 0;
}

uint32_t PuschTriggerPart2AndLDPCDecodeHandler()//待确认接口后补充
{
    printf("trigger Polar/RM HAC启动Part2译码&&trigger LDPC HAC启动UL-Data译码\n");//
    return 0;
}

uint32_t PuschParseCfgTriggerHandler()
{
    PuschPart1ParsePart2AndLDPCParaCfgHandler();

    PuschTriggerPart2AndLDPCDecodeHandler();//待确认接口后补充
    
    return 0;
}

uint32_t PuschPart2ParseHandler()
{
    printf("解析PUSCH Part2\n");//PUSCH CSI part2
    
    PuschUCIParser();

    return 0;
}

uint32_t PuschUciSendHandler()//待设计
{
    printf("给L2发送UCI\n");
    
    return 0;
}