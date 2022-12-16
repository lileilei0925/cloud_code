#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../../../../common/inc/phy_ctrl_common.h"
#include "../../../../common/src/common.c"
#include "../inc/phyctrl_pusch.h"
#include "../inc/pusch_variable.h"

uint32_t L1PuschTbParaCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschDemapParaTemp *puschDemapParaTemp);
uint32_t L1PuschUciCrcLengthCalculate(uint16_t ucibitlength, uint8_t *crcLength);
uint32_t L1PuschUciEncodeBitCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo  *puschResourceInfo, PuschDemapParaTemp *puschDemapParaTemp);
uint32_t L1PuschUciReLocationCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, PuschAckAndCsiInfo *puschAckAndCsiInfo);
uint32_t L1PuschLdpcCbPartRowCalculate(PuschLdpcUePara *puschLdpcUePara, uint8_t baseGraphType, uint16_t *cbPartRow);
uint32_t L1PuschCsiPart2ResCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, PuschAckAndCsiInfo *puschAckAndCsiInfo, CsiPart2ParaInfo *csiPart2ParaInfo);
uint32_t L1PuschDataTypeCalculate(L1PuschPduInfo *l1PuschUeInfo, uint8_t *dataFlag);
uint32_t L1PuschCsiPart2AndDataExtract(uint8_t dataFlag, L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, CsiPart2ParaInfo *csiPart2ParaInfo, LlrSegInfo *llrSegInfo);

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

    for (symbIndex = 0; symbIndex < SYM_NUM_PER_SLOT; symbIndex++){
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

        if(validUeNum > 0){
            g_puschUeNumInSymb[cellIndex][symbIndex] = validUeNum;
            nrPuschSymPara->validUeNum = validUeNum;
            puschDcePara->taskBitmap |= (1 << symbIndex ); 

            /* 输入数据地址，待确定 */
            *nrPuschSymPara->puschFreqData  = 0x00000000;
            *nrPuschSymPara->puschFreqAgc   = 0x00000001;   
            /* 输出数据数据，待确定 */
            *nrPuschSymPara->puschCheResult = 0x00000003;
            *nrPuschSymPara->puschRuuResult = 0x00000004;
            *nrPuschSymPara->puschCheAgc    = 0x00000005;
        }   
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
    uint16_t dataReInSymb, rbNum;
    PuschDemapParaTemp puschDemapParaTemp[16];
    PuschResourceInfo  puschResourceInfo[16];
    PuschDeqAndDemapSymPara *puschDeqAndDemapSymPara = NULL;
    PuschDeqAndDemapUePara  *puschDeqAndDemapUePara  = NULL;
    PuschAckAndCsiInfo      *puschAckAndCsiInfo      = NULL;    
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
    memset(&g_puschAckAndCsiInfo[cellIndex][slotIndex][0][0], 0x0, sizeof(PuschAckAndCsiInfo) * NR_PUSCH_MAX_UE_NUM_PER_SLOT * SYM_NUM_PER_SLOT);
    for(ueIndex = 0; ueIndex < puschDeqAndDemapHacPara->ueNum; ueIndex++){
        l1PuschUeInfo = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
        puschAckAndCsiInfo = &g_puschAckAndCsiInfo[cellIndex][slotIndex][ueIndex][0];
        
        L1PuschTbParaCalculate(l1PuschUeInfo, &puschDemapParaTemp[ueIndex]); /* TB参数计算 */
        
        ueSymbNum   = l1PuschUeInfo->nrOfSymbols;
        dmrsSymbNum = g_puschDmrsSymbNum[cellIndex][slotIndex][ueIndex];
        dataSymbNum = ueSymbNum - dmrsSymbNum;
        puschResourceInfo[ueIndex].dmrsSymbNum     = dmrsSymbNum;
        puschResourceInfo[ueIndex].dataSymbNum     = dataSymbNum;
        puschResourceInfo[ueIndex].firstDmrsIndex  = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0];
        puschResourceInfo[ueIndex].secondDmrsIndex = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][1];
        puschResourceInfo[ueIndex].thirdDmrsIndex  = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][2];
        puschResourceInfo[ueIndex].rbNumAllRbg     = g_puschUeRbsize[cellIndex][slotIndex][ueIndex];

        L1PuschUciEncodeBitCalculate(l1PuschUeInfo,  &puschResourceInfo[ueIndex], &puschDemapParaTemp[ueIndex]);

        L1PuschUciReLocationCalculate(l1PuschUeInfo, &puschResourceInfo[ueIndex], puschAckAndCsiInfo);
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
            
            rbNum    = puschResourceInfo[ueIndex].rbNumAllRbg;
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

            puschDeqAndDemapUePara->palcehoderFlag     = puschAckAndCsiInfo[symbIndex].ackRvdExsitFlag;
            puschDeqAndDemapUePara->uciReStart         = puschAckAndCsiInfo[symbIndex].ackRvdStartRe;
            puschDeqAndDemapUePara->uciReDistance      = puschAckAndCsiInfo[symbIndex].ackRvdDistance;
            puschDeqAndDemapUePara->uciReNum           = puschAckAndCsiInfo[symbIndex].ackRvdReNum;
            
            puschDeqAndDemapUePara->ackFlag            = puschAckAndCsiInfo[symbIndex].ackExsitFlag;
            puschDeqAndDemapUePara->ackReStart         = puschAckAndCsiInfo[symbIndex].ackStartRe;
            puschDeqAndDemapUePara->ackReDistance      = puschAckAndCsiInfo[symbIndex].ackDistance;
            puschDeqAndDemapUePara->ackReNum           = puschAckAndCsiInfo[symbIndex].ackReNum;

            puschDeqAndDemapUePara->csiPart1Flag       = puschAckAndCsiInfo[symbIndex].csiPart1ExsitFlag;
            puschDeqAndDemapUePara->csiPart1ReStart    = puschAckAndCsiInfo[symbIndex].csiPart1StartRe;
            puschDeqAndDemapUePara->csiPart1ReDistance = puschAckAndCsiInfo[symbIndex].csiPart1Distance;
            puschDeqAndDemapUePara->csiPart1ReNum      = puschAckAndCsiInfo[symbIndex].ackReNum;
            
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
    uint8_t  cbRmLen0, cbRmLen1, qamModer, layerNum, cbNum, segmNum;
    uint8_t  betaOffsetPart2, alphaScaling, csiPart2UlschFlag,crcLengthPart2;
    uint16_t rbSize, ulNref, ulResourceG;
    uint16_t csiPart2Flag, ulKr;
    uint16_t csiPart2BitLen;
    uint16_t ueUCIReNum, qPart2P0, qPart2P1;
    uint16_t iLoop, cbPartRow[3];
    uint16_t enCodeHarqAckReNum, enCodeCsiPart1ReNum, enCodeulschReNum;
    PuschResourceInfo  puschResourceInfo;
    PuschDemapParaTemp puschDemapParaTemp;
    LlrSegInfo   puschLlrSegmInfo[5];
    L1PuschPduInfo     *l1PuschUeInfo      = NULL;
    PuschLdpcUePara    *puschLdpcUePara    = NULL;
    PuschAckAndCsiInfo *puschAckAndCsiInfo = NULL;
    CsiPart2ParaInfo   *csiPart2ParaInfo   = NULL;
    uint32_t *csiPart2AndDataBaseAddr = NULL;

    ldpcDecoderHacPara->sfnIndex  = l1PuschParaPduInfo->sfnIndex;
    ldpcDecoderHacPara->slotIndex = l1PuschParaPduInfo->slotIndex;
    ldpcDecoderHacPara->ueNum     = l1PuschParaPduInfo->puschPduNum;
    ldpcDecoderHacPara->maxIterNum       = 10;  /* 算法配置参数待定 */
    ldpcDecoderHacPara->reviseFactor     = 192; /* 修正因子，默认值为0.75 */
    ldpcDecoderHacPara->harqCompressType = 0;   /* 压缩模式，需要Demap按照相同压缩模式输出 */
    ldpcDecoderHacPara->decodeAlgType    = 0;   /* 译码算法选择 */
    ueNum     = l1PuschParaPduInfo->puschPduNum;
    slotIndex = l1PuschParaPduInfo->slotIndex;

    l1PuschUeInfo   = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
    puschLdpcUePara = &ldpcDecoderHacPara->puschLdpcUePara[ueIndex];
    for (ueIndex = 0; ueIndex < ueNum; ueIndex++){
        baseGraphType   = l1PuschUeInfo->ldpcBaseGraph - 1;
      
        L1PuschTbParaCalculate(l1PuschUeInfo, &puschDemapParaTemp);   /* TB参数计算 */
        
        memset(&cbPartRow[0], 0x0, sizeof(uint16_t) * 3);
        puschLdpcUePara->ueIndex      = ueIndex;
        puschLdpcUePara->tbCrcType    = puschDemapParaTemp.tbCrcType;
        puschLdpcUePara->cbNum        = puschDemapParaTemp.cbNum;
        puschLdpcUePara->ldpcKd       = puschDemapParaTemp.ulKd;
        puschLdpcUePara->cbFillBitLen = puschDemapParaTemp.cbFillingLen;
        puschLdpcUePara->ldpcZc       = puschDemapParaTemp.ulZc;
        puschLdpcUePara->ldpcBgId     = g_puschLdpcBgId[baseGraphType][puschDemapParaTemp.iLs];
        puschLdpcUePara->qamMode      = l1PuschUeInfo->qamModOrder;
        puschLdpcUePara->layerNum     = l1PuschUeInfo->nrOfLayers;
        puschLdpcUePara->tbSize       = l1PuschUeInfo->puschDataPara.tbSize;

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
        
        puschResourceInfo.dmrsSymbNum     = dmrsSymbNum;
        puschResourceInfo.dataSymbNum     = ueSymbNum - dmrsSymbNum;
        puschResourceInfo.firstDmrsIndex  = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][0];
        puschResourceInfo.secondDmrsIndex = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][1];
        puschResourceInfo.thirdDmrsIndex  = g_puschDmrsSymbIndex[cellIndex][slotIndex][ueIndex][2];
        puschResourceInfo.rbNumAllRbg     = g_puschUeRbsize[cellIndex][slotIndex][ueIndex];
        puschResourceInfo.ulResourceG     = ulResourceG;
        L1PuschUciEncodeBitCalculate(l1PuschUeInfo, &puschResourceInfo, &puschDemapParaTemp);
        
        /* CSI-Part2占用Re数计算 */ 
        csiPart2BitLen  = 0xFFFF; /* 根据part1译码结果获取part2的长度 */
        L1PuschUciCrcLengthCalculate(csiPart2BitLen, &crcLengthPart2);

        ulKr            = puschDemapParaTemp.ulKr;
        ueUCIReNum      = puschResourceInfo.ueUCIReNum;
        betaOffsetPart2 = l1PuschUeInfo->puschUciPara.betaOffsetCsi2;
        qPart2P0        = ceilDiv((csiPart2BitLen + crcLengthPart2) * betaOffsetPart2 * ueUCIReNum, cbNum * ulKr);
        qPart2P1        = ceilDiv(alphaScaling * ueUCIReNum, 1) - puschResourceInfo.enCodeAckRe - puschResourceInfo.enCodeCsiPart1Re; 
        puschResourceInfo.enCodeCsiPart2Re = (qPart2P0 < qPart2P1) ? qPart2P0 : qPart2P1;
        puschResourceInfo.csiPart2bit      = csiPart2BitLen;
        
        csiPart2Flag = l1PuschUeInfo->puschUciPara.flagCsiPart2;
        if ((l1PuschUeInfo->pduBitMap & 0x3) == 1){        /* DATA Only */
            csiPart2UlschFlag = NR_ULSCH_WITHOUT_UCI;
        }
        else if ((l1PuschUeInfo->pduBitMap & 0x3) == 3){   /* Data + UCI */
            if (csiPart2Flag != 0){ /* 0: No CSI part 2, 65535: Determine CSI Part2 length based on Table3-77 */
                csiPart2UlschFlag = NR_ULSCH_WITH_PART2;   /* DATA + UCI 存在 CSI-Part2 */
            }
            else{
                csiPart2UlschFlag = NR_ULSCH_WITHOUT_PART2;/* DATA + UCI 不存在 CSI-Part2 */
            }
        }
        
        csiPart2ParaInfo   = &g_CsiPart2ParaInfo[cellIndex][slotIndex][ueIndex];
        puschAckAndCsiInfo = &g_puschAckAndCsiInfo[cellIndex][slotIndex][ueIndex][0];
        memset(csiPart2ParaInfo, 0x0, sizeof(CsiPart2ParaInfo));
        L1PuschCsiPart2ResCalculate(l1PuschUeInfo, &puschResourceInfo, puschAckAndCsiInfo, csiPart2ParaInfo);

        enCodeHarqAckReNum  = puschResourceInfo.enCodeAckRe;   
        enCodeCsiPart1ReNum = puschResourceInfo.enCodeCsiPart1Re;
        csiPart2AndDataBaseAddr = &g_UePart2AndDataAddr[cellIndex][ueIndex];
        if ((csiPart2UlschFlag == NR_ULSCH_WITHOUT_UCI) || (csiPart2UlschFlag == NR_ULSCH_WITHOUT_PART2)){/* 没有CSI-Part2时， Data只有一段 */
            if (puschResourceInfo.hackAckbit > 2){
                enCodeulschReNum = ulResourceG - enCodeCsiPart1ReNum - enCodeHarqAckReNum;
            }
            else {
                enCodeulschReNum = ulResourceG - enCodeCsiPart1ReNum;
            }

            segmNum = 1;
            puschLlrSegmInfo[0].segStartAddr = csiPart2AndDataBaseAddr; /* ue data数据地址 */
            puschLlrSegmInfo[0].segLlrNum = enCodeulschReNum;
            puschLlrSegmInfo[0].segPeriod = enCodeulschReNum;
            puschLlrSegmInfo[0].segCycNum = 1;
        }
        else if (csiPart2UlschFlag == NR_ULSCH_WITH_PART2){/* Data with Part2 需要抽取 Part2 + Data */    
            /* CSI-Part2&Data 抽取 */
            segmNum = L1PuschCsiPart2AndDataExtract(NR_PUSCH_ULSCH_EXT, l1PuschUeInfo, &puschResourceInfo, csiPart2ParaInfo, &puschLlrSegmInfo[0]);
        }
  
        puschLdpcUePara->llrSegNum = segmNum;
        for(iLoop = 0; iLoop < puschLdpcUePara->llrSegNum; iLoop++){
            puschLdpcUePara->llrSegInfo[iLoop].segStartAddr = puschLlrSegmInfo[iLoop].segStartAddr;
            puschLdpcUePara->llrSegInfo[iLoop].segLlrNum    = puschLlrSegmInfo[iLoop].segLlrNum;
            puschLdpcUePara->llrSegInfo[iLoop].segPeriod    = puschLlrSegmInfo[iLoop].segPeriod;
            puschLdpcUePara->llrSegInfo[iLoop].segCycNum    = puschLlrSegmInfo[iLoop].segCycNum;
        }

        l1PuschUeInfo++;
        puschLdpcUePara++;
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
        puschDemapParaTemp->cbNum = 1;
    }
    else{
        crcLength = 24;
        puschDemapParaTemp->cbNum = ceilDiv(tbSizeB, kCb - crcLength);
    }
    tbSizeBp = tbSizeB + (puschDemapParaTemp->cbNum) * crcLength;
    ulKd     = tbSizeBp / (puschDemapParaTemp->cbNum); /* 每个CB中的bit数 */
    puschDemapParaTemp->ulKd = ulKd;

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

uint32_t L1PuschUciEncodeBitCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, PuschDemapParaTemp *puschDemapParaTemp)
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
    puschResourceInfo->hackAckbit  = hackAckBitLength;
    puschResourceInfo->csiPart1bit = csiPart1BitLength;
    
    L1PuschUciCrcLengthCalculate(hackAckBitLength, &crcLengthAck);
    L1PuschUciCrcLengthCalculate(csiPart1BitLength, &crcLengthPart1);
    
    rbSize        = puschResourceInfo->rbNumAllRbg;
    dmrsSymb      = puschResourceInfo->dmrsSymbNum;
    dataSymb      = puschResourceInfo->dataSymbNum;
    dmrsNoDataNum = l1PuschUeInfo->numCdmGrpsNoData;
    dmrsRePerRB   = l1PuschUeInfo->dmrsCfgType == 1 ? NR_DMRS_TYPE1_SC_PER_RB : NR_DMRS_TYPE2_SC_PER_RB;
    ulKr          = puschDemapParaTemp->ulKr;
    cbNum         = puschDemapParaTemp->cbNum;
    alphaScaling  = l1PuschUeInfo->puschUciPara.alphaScaling;
    codeRate      = l1PuschUeInfo->targetCodeRate;
    qamMode       = l1PuschUeInfo->qamModOrder;
    ueSCHReNum    = (dmrsSymb + dataSymb) * rbSize * N_SC_PER_PRB - (dmrsSymb * rbSize * dmrsNoDataNum * dmrsRePerRB); /* 暂时不考虑PTRS：SCH可以在除dmrs no data之外的re上传输 */
    ueUCIReNum    = dataSymb * rbSize * N_SC_PER_PRB;  /* 暂时不考虑PTRS：UCI可在除DMRS符号之外的符号上传输 */
    ueUCIReNumAck = ueUCIReNum - puschResourceInfo->firstDmrsIndex * rbSize * N_SC_PER_PRB;
    puschResourceInfo->ulResourceG = ueSCHReNum;
    puschResourceInfo->ueUCIReNum  = ueUCIReNum;
    puschResourceInfo->dmrsRePerRB = dmrsRePerRB;

    if(l1PuschUeInfo->pduBitMap == 1){
        /* PUSCH Only *///只需要区分DMRS即可
       
    }
    else if(l1PuschUeInfo->pduBitMap == 2){/* UCI only */
        /* HARQ-ACK */
        if(hackAckBitLength < 2){
            hackAckBitLength = 2;
        }
        qAckP0   = ceilDiv((hackAckBitLength + crcLengthAck) * betaOffsetAck, codeRate*qamMode/1024);
        qAckP1   = ceilDiv(alphaScaling * ueUCIReNumAck, 1); /* 向上取整待修改 */
        puschResourceInfo->enCodeAckRe = (qAckP0 < qAckP1) ? qAckP0 : qAckP1; 
        /* CSI-Part1 */
        qPart1P0 = ceilDiv((csiPart1BitLength + crcLengthPart1) * betaOffsetPart1, codeRate*qamMode/1024);
        qPart1P1 = ueUCIReNum - puschResourceInfo->enCodeAckRe; 
        puschResourceInfo->enCodeCsiPart1Re = (qPart1P0 < qPart1P1) ? qPart1P0 : qPart1P1; 
    }
    else if(l1PuschUeInfo->pduBitMap == 3){/* PSUCH + UCI */
        /* HARQ-ACK */
        qAckP0   = ceilDiv((hackAckBitLength + crcLengthAck) * betaOffsetAck * ueUCIReNum, cbNum * ulKr);
        qAckP1   = ceilDiv(alphaScaling * ueUCIReNumAck, 1); /* 向上取整待修改 */
        puschResourceInfo->enCodeAckRe = (qAckP0 < qAckP1) ? qAckP1 : qAckP1; 
        /* CSI-Part1 */
        qPart1P0 = ceilDiv((csiPart1BitLength + crcLengthPart1) * betaOffsetPart1 * ueUCIReNum, cbNum * ulKr);
        qPart1P1 = ceilDiv(alphaScaling * ueUCIReNum, 1) - puschResourceInfo->enCodeAckRe; 
        puschResourceInfo->enCodeCsiPart1Re = (qPart1P0 < qPart1P1) ? qPart1P0 : qPart1P1; 
    }

    return 0;
}

uint32_t L1PuschUciReLocationCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, PuschAckAndCsiInfo *puschAckAndCsiInfo)
{
    uint8_t  ackReRvdDistance, ackReDistance, csiPart1Distance;
    uint8_t  symbIndex, qamMode, layerNum;
    uint8_t  symbStart, symbNum;
    uint16_t dmrsSymBitmap;
    uint16_t mAckCnt, mReCnt, reIndex;
    uint16_t nMUciSc, nGAck, nGAckRvd;
    uint16_t ackReNumRvd[SYM_NUM_PER_SLOT] = { 0 };
    uint16_t ackReNum[SYM_NUM_PER_SLOT]    = { 0 };
    uint16_t nMUciScBar[SYM_NUM_PER_SLOT]  = { 0 };
    uint16_t mCsiPart1Cnt, nGCsiPart1, nMCsiPart1Sc;

    memset(puschAckAndCsiInfo, 0x0, sizeof(PuschAckAndCsiInfo) * SYM_NUM_PER_SLOT);
    qamMode       = l1PuschUeInfo->qamModOrder;
    layerNum      = l1PuschUeInfo->nrOfLayers;
    symbStart     = l1PuschUeInfo->startSymbIndex;
    symbNum       = l1PuschUeInfo->nrOfSymbols;
    dmrsSymBitmap = l1PuschUeInfo->ulDmrsSymbPos;
    
    nMUciSc       = puschResourceInfo->rbNumAllRbg * N_SC_PER_PRB; /* 每个符号上的有效RE数，UCI不能在DMRS符号上传送 */
    nGAckRvd      = puschResourceInfo->enCodeAckRe * qamMode * layerNum;
    nGAck         = nGAckRvd;
    mAckCnt       = 0;
    symbIndex     = puschResourceInfo->firstDmrsIndex + 1; /* ACK 从第一个DMRS之后的数据符号开始映射, 当前只会配置单列导频 */
    if (puschResourceInfo->hackAckbit <= 2){/* step1: 计算ACK预留资源 */
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

                puschAckAndCsiInfo[symbIndex].ackRvdExsitFlag = 1;
                puschAckAndCsiInfo[symbIndex].ackRvdStartRe   = 0;
                puschAckAndCsiInfo[symbIndex].ackRvdDistance  = ackReRvdDistance;
                puschAckAndCsiInfo[symbIndex].ackRvdReNum     = mReCnt;
                ackReNumRvd[symbIndex] = mReCnt;
                symbIndex = symbIndex + 1;

                if (((uint16_t)1<< symbIndex) & dmrsSymBitmap != 0){
                    symbIndex = symbIndex + 1;
                }
            }   
        }

        mAckCnt   = 0;
        symbIndex = puschResourceInfo->firstDmrsIndex + 1; /* ACK 从第一个DMRS之后的数据符号开始映射 */
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

                for(reIndex = 0; reIndex < mReCnt; reIndex++){
                    //ackRE[symbIndex][reIndex] = ackRvdRE[symbIndex][reIndex * ackReDistance];
                    mAckCnt = mAckCnt + layerNum * qamMode;
                }

                puschAckAndCsiInfo[symbIndex].ackExsitFlag = 1;
                puschAckAndCsiInfo[symbIndex].ackStartRe   = 0;
                puschAckAndCsiInfo[symbIndex].ackDistance  = ackReRvdDistance * ackReDistance;
                puschAckAndCsiInfo[symbIndex].ackReNum     = mReCnt;
                symbIndex = symbIndex + 1;

                if (((uint16_t)1<< symbIndex) & dmrsSymBitmap != 0){
                    symbIndex = symbIndex + 1;
                }
            }
        }
    } 
    else{ /* step2: ACK资源直接映射 */
        mAckCnt = 0;
        while (mAckCnt < nGAck) 
        {
            nMUciScBar[symbIndex] = nMUciSc;
            if (nMUciScBar[symbIndex] > 0){
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
                //nMUciScBar[symbIndex] = nMUciScBar[symbIndex] - mReCnt;
                puschAckAndCsiInfo[symbIndex].ackExsitFlag = 1;
                puschAckAndCsiInfo[symbIndex].ackStartRe   = 0;
                puschAckAndCsiInfo[symbIndex].ackDistance  = ackReDistance;
                puschAckAndCsiInfo[symbIndex].ackReNum     = mReCnt;
                ackReNum[symbIndex] = mReCnt;
                symbIndex = symbIndex + 1;

                if (((uint16_t)1<< symbIndex) & dmrsSymBitmap != 0){
                    symbIndex = symbIndex + 1;
                }
            }
        }
    }

    if (puschResourceInfo->csiPart1bit > 0){  /* step3: 第一部分part1资源位置计算 */
        nGCsiPart1   = puschResourceInfo->enCodeCsiPart1Re * layerNum * qamMode;
        
        /* 找到第一个可用于承载CSI-Part1的符号索引 */
        for (symbIndex = symbStart; symbIndex < (symbStart + symbNum); symbIndex++){
            if (((uint16_t)1<< symbIndex) & dmrsSymBitmap != 0){ /* UCI不能映射到dmrs符号, 如果调度的符号为dmrs符号，顺移到下一个符号 */
                nMUciSc = 0;
            }

            nMCsiPart1Sc = nMUciSc - ackReNumRvd[symbIndex] - ackReNum[symbIndex];
            if (nMCsiPart1Sc > 0){ /* 小于0表示当前符号无可用RE给csiPart1，顺移到下一个符号 */
                break; 
            }
        }

        mCsiPart1Cnt = 0;
        while (mCsiPart1Cnt < nGCsiPart1)
        {
            nMCsiPart1Sc = nMUciSc - ackReNumRvd[symbIndex] - ackReNum[symbIndex]; /* 当前符号去除ACK后可用于传输CSI-Part2的RE数 */
            if (nMCsiPart1Sc > 0){
                if ((nGCsiPart1 - mCsiPart1Cnt) >= nMCsiPart1Sc * layerNum * qamMode){
                    csiPart1Distance = 1;
                    mReCnt           = nMCsiPart1Sc;
                }
                else{
                    csiPart1Distance = nMCsiPart1Sc * layerNum * qamMode / (nGCsiPart1 - mCsiPart1Cnt);
                    mReCnt           = ceilDiv((nGCsiPart1 - mCsiPart1Cnt), layerNum * qamMode);
                }

                for (reIndex = 0; reIndex < mReCnt; reIndex++){
                    //csiPart1RE[symbIndex][reIndex] = reIndex * ackReDistance;
                    mCsiPart1Cnt =+ layerNum * qamMode;
                }

                puschAckAndCsiInfo[symbIndex].csiPart1ExsitFlag = 1;
                puschAckAndCsiInfo[symbIndex].csiPart1Distance  = csiPart1Distance;
                puschAckAndCsiInfo[symbIndex].csiPart1ReNum     = mReCnt;
                if (puschAckAndCsiInfo[symbIndex].ackRvdExsitFlag == 1){
                    if (puschAckAndCsiInfo[symbIndex].ackRvdDistance == 1){
                        puschAckAndCsiInfo[symbIndex].csiPart1StartRe  = puschAckAndCsiInfo[symbIndex].ackRvdStartRe + puschAckAndCsiInfo[symbIndex].ackRvdReNum;
                    }
                    else{ /* puschAckAndCsiInfo[symbIndex].ackRvdDistance > 1 */
                        puschAckAndCsiInfo[symbIndex].csiPart1StartRe  = puschAckAndCsiInfo[symbIndex].ackRvdStartRe + 1;
                    }
                }

                if (puschAckAndCsiInfo[symbIndex].ackExsitFlag == 1){
                    if (puschAckAndCsiInfo[symbIndex].ackDistance == 1){
                        puschAckAndCsiInfo[symbIndex].csiPart1StartRe  = puschAckAndCsiInfo[symbIndex].ackStartRe + puschAckAndCsiInfo[symbIndex].ackReNum;
                    }
                    else{
                        puschAckAndCsiInfo[symbIndex].csiPart1StartRe  = puschAckAndCsiInfo[symbIndex].ackStartRe + 1;
                    }
                }

                symbIndex = symbIndex + 1;
                if (((uint16_t)1<< symbIndex) & dmrsSymBitmap != 0){
                    symbIndex = symbIndex + 1;
                }
            }
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

uint32_t L1PuschDataTypeCalculate(L1PuschPduInfo *l1PuschUeInfo, uint8_t *dataFlag)
{
    uint16_t pduBitmap, harqAckBitLen, csiPart1BitLen, csiPart2Flag;

    pduBitmap      = l1PuschUeInfo->pduBitMap;
    harqAckBitLen  = l1PuschUeInfo->puschUciPara.harqAckBitLength;
    csiPart1BitLen = l1PuschUeInfo->puschUciPara.csiPart1BitLength;
    csiPart2Flag   = l1PuschUeInfo->puschUciPara.flagCsiPart2;

    if ((pduBitmap & 0x3) == 1){      /* Data only */
        *dataFlag = NR_ULSCH_WITHOUT_UCI;
    }
    else if ((pduBitmap & 0x3) == 2){ /* UCI only */
        if (harqAckBitLen > 0) { /* HARQ-ACK 存在 */
            if (csiPart1BitLen == 0){ /* 不存在Part 1，也不会有Part2，即 */
                *dataFlag = NR_UCI_HARQ_ACK;
            }
            else { /* 存在Part1 需要再判断是否有part2 */
                if(csiPart2Flag > 0){
                    *dataFlag = NR_UCI_WITH_ALL;         /* HARQ-ACK CSI-Part1 + CSI-Part2 */
                }
                else{
                    *dataFlag = NR_UCI_WITHOUT_PART2;    /*  HARQ-ACK + CSI-Part1 */
                }
            }
        }
        else { /* HARQ-ACK 不存在, UCI only下一定有CSI-Part1，需要判断是否存在CSI-Part2 */
            if (csiPart1BitLen > 0){
                if(csiPart2Flag > 0){
                    *dataFlag = NR_UCI_WITHOUT_HARQ_ACK; /* CSI-Part1 + CSI-Part2 */
                }
                else{
                    *dataFlag = NR_UCI_CSI_PART1;        /* CSI-Part1 */
                }
            }
        }
    }
    else if ((pduBitmap & 0x3) == 3){ /* Data + UCI */
        if (csiPart2Flag > 0){
            *dataFlag = NR_ULSCH_WITH_PART2;             /* DATA + UCI 存在 CSI-Part2 */
        }
        else{
            *dataFlag = NR_ULSCH_WITHOUT_PART2;          /* DATA + UCI 不存在 CSI-Part2 */
        }
    }
    else{
        return ERROR_CODE;
    }

    return 0;
}

uint32_t L1PuschCsiPart2ResCalculate(L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, PuschAckAndCsiInfo *puschAckAndCsiInfo, CsiPart2ParaInfo *csiPart2ParaInfo)
{
    uint8_t  symbNum, startSymIndex, symbIndex, symIndexBitmap;
    uint8_t  dmrsSymBitmap, qamModer, layerNum;
    uint8_t  cdmNumNoData,dmrsRePerRB, csiPart2StartSymb, csiPart2EndSymb;
    uint8_t  csiPart2Distance, harqAckBitLen;
    uint16_t rbsize, dmrsDataReNum, mReCnt, reIndex;
    uint16_t nMUciSc, nGCsiPart2,nMCsiPart2Sc, mCsiPart2Cnt;
   
    rbsize        = puschResourceInfo->rbNumAllRbg;
    startSymIndex = l1PuschUeInfo->startSymbIndex;
    symbNum       = l1PuschUeInfo->nrOfSymbols;
    dmrsSymBitmap = l1PuschUeInfo->ulDmrsSymbPos;
    qamModer      = l1PuschUeInfo->qamModOrder;
    layerNum      = l1PuschUeInfo->nrOfLayers;
    cdmNumNoData  = l1PuschUeInfo->numCdmGrpsNoData;
    dmrsRePerRB   = l1PuschUeInfo->dmrsCfgType == 1 ? NR_DMRS_TYPE1_SC_PER_RB : NR_DMRS_TYPE2_SC_PER_RB;
    harqAckBitLen = l1PuschUeInfo->puschUciPara.harqAckBitLength;
  
    nMUciSc = rbsize * N_SC_PER_PRB; /* 每个符号上的有效RE数，UCI不能在DMRS符号上传送 */
    csiPart2StartSymb = startSymIndex;
    /* 获取CSI-Part2的起始符号 */
    for (symbIndex = startSymIndex; symbIndex < startSymIndex + symbNum; symbIndex++){
        mCsiPart2Cnt = 0;
        nGCsiPart2   = puschResourceInfo->enCodeCsiPart2Re * layerNum * qamModer;

        if (harqAckBitLen > 2){
            nMCsiPart2Sc = nMUciSc - puschAckAndCsiInfo[symbIndex].ackReNum - puschAckAndCsiInfo[symbIndex].csiPart1ReNum;
        }
        else{
            nMCsiPart2Sc = nMUciSc - puschAckAndCsiInfo[symbIndex].csiPart1ReNum;
        }
        
        symIndexBitmap = (1 << symbIndex);
        if ((symIndexBitmap & dmrsSymBitmap) != 0){ /* Dmrs Symbol */
            nMCsiPart2Sc = 0;
            csiPart2ParaInfo->dmrsNumBfPart2++;
        }

        if (nMCsiPart2Sc > 0){ /* 找到UE的可用于CSI-Part2的第一个符号 */
            break;
        }
        csiPart2StartSymb++;  /* 碰到 DMRS符号 符号数加1， nMCsiPart2Sc <= 0 符号数加1 */
    }
    
    /* 获取CSI-Part2的截止符号，和每个符号上所占的RE数，以及每个符号上的RE间隔 */
    csiPart2EndSymb = csiPart2StartSymb;
    while (mCsiPart2Cnt < nGCsiPart2)
    {
        if (harqAckBitLen > 2){
            nMCsiPart2Sc = nMUciSc - puschAckAndCsiInfo[csiPart2EndSymb].ackReNum - puschAckAndCsiInfo[csiPart2EndSymb].csiPart1ReNum;
        }
        else{
            nMCsiPart2Sc = nMUciSc - puschAckAndCsiInfo[csiPart2EndSymb].csiPart1ReNum;
        }
        
        if (nMCsiPart2Sc > 0){
            if ((nGCsiPart2 - mCsiPart2Cnt) >= nMCsiPart2Sc * layerNum * qamModer){
                csiPart2Distance = 1;
                mReCnt           = nMCsiPart2Sc;
            }
            else{
                csiPart2Distance = nMCsiPart2Sc * layerNum * qamModer / (nGCsiPart2 - mCsiPart2Cnt);
                mReCnt           = ceilDiv((nGCsiPart2 - mCsiPart2Cnt), layerNum * qamModer);
            }    
        } 

        for (reIndex = 0; reIndex < mReCnt; reIndex++){ /*  */
            mCsiPart2Cnt = mCsiPart2Cnt + layerNum * qamModer;
        }
        
        csiPart2ParaInfo->reDistance[csiPart2EndSymb]     = csiPart2Distance;  
        csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] = mReCnt;
        csiPart2EndSymb++;

        symIndexBitmap = (1 << csiPart2EndSymb);
        if ((symIndexBitmap & dmrsSymBitmap) != 0){ /* Dmrs Symbol */
            csiPart2EndSymb++;
        }
    } 

    csiPart2ParaInfo->csiPart2StartSymb = csiPart2StartSymb;
    csiPart2ParaInfo->csiPart2EndSymb   = csiPart2EndSymb - 1;

    return 0;
}

uint32_t L1PuschCsiPart2AndDataExtract(uint8_t dataFlag, L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo, CsiPart2ParaInfo *csiPart2ParaInfo, LlrSegInfo *llrSegInfo)
{
    uint8_t  segmIndex, segmNum, segmNumCnt, cdmGrpsNodata;
    uint8_t  symIndexBitmap, symbIndex, ueLastSymbIndex;
    uint8_t  csiPart2StartSymb ,csiPart2EndSymb;
    uint16_t rbSize, dmrsReNumPerRB, dataReInDmrsSymb;
    uint16_t dataReNumTemp, csiPart2ReNumTemp;
    uint16_t dmrsSymBitmap;
    uint16_t nMUciSc;
    uint16_t csiPart2ReCnt[6]     = { 0 };
    uint16_t segCycNum[6]         = { 0 };
    uint16_t segPeriod[6]         = { 0 };
    uint16_t segLlrNum[6]         = { 0 };
    uint16_t segmAddrOffSet[6]    = { 0 };
    uint16_t dataReNumPerSymb[14] = { 0 };
    uint32_t *csiPart2AndDataBaseAddr = NULL;
    
    rbSize            = puschResourceInfo->rbNumAllRbg;
    dmrsReNumPerRB    = puschResourceInfo->dmrsRePerRB;
    dmrsSymBitmap     = l1PuschUeInfo->ulDmrsSymbPos;
    cdmGrpsNodata     = l1PuschUeInfo->numCdmGrpsNoData;
    ueLastSymbIndex   = l1PuschUeInfo->startSymbIndex + l1PuschUeInfo->nrOfSymbols - 1;
    dataReInDmrsSymb  = rbSize * (N_SC_PER_PRB - dmrsReNumPerRB * cdmGrpsNodata);
    csiPart2StartSymb = csiPart2ParaInfo->csiPart2StartSymb;
    csiPart2EndSymb   = csiPart2ParaInfo->csiPart2EndSymb;
    
    /* 计算获取CSI-Part2&Data的分段和每段中的资源信息 */
    nMUciSc = rbSize * N_SC_PER_PRB; /* 每个符号上的有效RE数，UCI不能在DMRS符号上传送 */  // 需要考虑符号上有CSI-Part1和Harq-Ack
    if (dataFlag == NR_PUSCH_CSIPART2_EXT){
        if (dataReInDmrsSymb == 0){/* DMRS符号无数据 */
            if (csiPart2ParaInfo->reDistance[csiPart2EndSymb] > 1){ /* 最后一个符号为一类，其余符号为一类  */
                csiPart2ReNumTemp = puschResourceInfo->enCodeCsiPart2Re - csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] /* 除最后一个符号外其余RE全部连续,最后一个符号单独讨论 */;
                segmNumCnt = 2; 
                llrSegInfo[0].segStartAddr = csiPart2AndDataBaseAddr;  /* 基地址起始就是Part2起始 */
                llrSegInfo[0].segCycNum    = 1;
                llrSegInfo[0].segPeriod    = csiPart2ReNumTemp;
                llrSegInfo[0].segLlrNum    = csiPart2ReNumTemp;   

                llrSegInfo[1].segStartAddr = (uint32_t *)((uint8_t *)csiPart2AndDataBaseAddr + csiPart2ReNumTemp);   
                llrSegInfo[1].segCycNum    = csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb]; /*  */
                llrSegInfo[1].segPeriod    = csiPart2ParaInfo->reDistance[csiPart2EndSymb];
                llrSegInfo[1].segLlrNum    = 1;
            }
            else {/* reDistance[csiPart2EndSymb] == 1 , 所有csi-Part2 RE连续 */
                segmNumCnt = 1;
                llrSegInfo[0].segStartAddr = csiPart2AndDataBaseAddr;  /* 基地址起始就是Part2起始 */
                llrSegInfo[0].segCycNum    = 1;
                llrSegInfo[0].segPeriod    = puschResourceInfo->enCodeCsiPart2Re;
                llrSegInfo[0].segLlrNum    = puschResourceInfo->enCodeCsiPart2Re;   
            }
        }
        else {/* DMRS符号有数据 */
            segmNumCnt = 0;
            segmAddrOffSet[0] = csiPart2ParaInfo->dmrsNumBfPart2 * dataReInDmrsSymb;
            //csiPart2AndDataBaseAddr = (uint32_t *)((uint8_t *)csiPart2AndDataBaseAddr + segmAddrOffSet[0]);  /* 基地址 + 数据偏移为Part2起始 */  
            for (symbIndex = csiPart2StartSymb; symbIndex < csiPart2EndSymb; symbIndex++){
                csiPart2ReCnt[segmNumCnt] += csiPart2ParaInfo->part2ReNumInSymb[symbIndex];
                if (csiPart2ParaInfo->part2ReNumInSymb[symbIndex] == 0){ /* DMRS 符号, 根据统计规则, 首尾符号不会是DMRS符号 */  
                    segCycNum[segmNumCnt] = 1;
                    segPeriod[segmNumCnt] = csiPart2ReCnt[segmNumCnt];
                    segLlrNum[segmNumCnt] = csiPart2ReCnt[segmNumCnt];
                    segmNumCnt++;
                    segmAddrOffSet[segmNumCnt] = dataReInDmrsSymb + csiPart2ReCnt[segmNumCnt - 1];
                }
            }    

            if (csiPart2ReCnt[segmNumCnt] > 0){
                if (csiPart2ParaInfo->reDistance[csiPart2EndSymb] > 1){
                    segCycNum[segmNumCnt] = 1;
                    segPeriod[segmNumCnt] = csiPart2ReCnt[segmNumCnt];
                    segLlrNum[segmNumCnt] = csiPart2ReCnt[segmNumCnt];
                    
                    segmAddrOffSet[segmNumCnt + 1] = csiPart2ReCnt[segmNumCnt];
                    segCycNum[segmNumCnt + 1]     = csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                    segPeriod[segmNumCnt + 1]     = csiPart2ParaInfo->reDistance[csiPart2EndSymb];
                    segLlrNum[segmNumCnt + 1]     = 1;
                }
                else { /* csiPart2ParaInfo->reDistance[csiPart2EndSymb] == 1 */
                    segCycNum[segmNumCnt] = 1;
                    segPeriod[segmNumCnt] = csiPart2ReCnt[segmNumCnt] + csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                    segLlrNum[segmNumCnt] = csiPart2ReCnt[segmNumCnt] + csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                }
            }
            else { /* csiPart2ReCnt[segmNumCnt] == 0 */ 
                if (csiPart2ParaInfo->reDistance[csiPart2EndSymb] > 1){
                    segCycNum[segmNumCnt] = csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                    segPeriod[segmNumCnt] = csiPart2ParaInfo->reDistance[csiPart2EndSymb];
                    segLlrNum[segmNumCnt] = 1;
                }
                else { /* csiPart2ParaInfo->reDistance[csiPart2EndSymb] == 1 */
                    segCycNum[segmNumCnt] = 1;
                    segPeriod[segmNumCnt] = csiPart2ReCnt[segmNumCnt] + csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                    segLlrNum[segmNumCnt] = csiPart2ReCnt[segmNumCnt] + csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                }
            }
            
            segmNum = segmNumCnt + 1; 
            llrSegInfo[0].segStartAddr = csiPart2AndDataBaseAddr; /* Data数据的第一个RE位置 */
            for (segmIndex = 0; segmIndex < segmNum; segmIndex++){
                llrSegInfo[segmIndex].segStartAddr = (uint32_t *)((uint8_t *)llrSegInfo[segmIndex].segStartAddr + segmAddrOffSet[segmIndex]);
                llrSegInfo[segmIndex].segCycNum = segCycNum[segmIndex];
                llrSegInfo[segmIndex].segPeriod = segPeriod[segmIndex];
                llrSegInfo[segmIndex].segLlrNum = segLlrNum[segmIndex]; 
            }
        }   
    }
    else {/* (dataFlag == NR_PUSCH_ULSCH_EXT) */
        if (dataReInDmrsSymb == 0){/* DMRS 符号无数据 */
            if (csiPart2ParaInfo->reDistance[csiPart2EndSymb] > 1){ /* CSIPart2最后一个符号的REdistance大于1，最后一个符号为一类，其余符号为一类 */
                csiPart2ReNumTemp = puschResourceInfo->enCodeCsiPart2Re - csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb]; /* 除最后一个符号外其余RE全部连续,最后一个符号单独讨论 */;
                if (csiPart2EndSymb == ueLastSymbIndex){ /* CsiPart2的最后一个符号也是ue调度的最后一个符号，只有一个segment */
                    segmNum = 1; 
                    llrSegInfo[0].segStartAddr = (uint32_t *)((uint8_t *)csiPart2AndDataBaseAddr + csiPart2ReNumTemp + 1);  /* Data数据的第一个RE位置 */
                    llrSegInfo[0].segCycNum    = csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] - 1;
                    llrSegInfo[0].segPeriod    = csiPart2ParaInfo->reDistance[csiPart2EndSymb];
                    llrSegInfo[0].segLlrNum    = csiPart2ParaInfo->reDistance[csiPart2EndSymb] - 1;   
                }
                else{ /* CsiPart2的最后一个符号不是ue调度的最后一个符号 ， 有两个segment */
                    segmNum = 2; 
                    llrSegInfo[0].segStartAddr = (uint32_t *)((uint8_t *)csiPart2AndDataBaseAddr + csiPart2ReNumTemp + 1);  /* Data数据的第一个RE位置 */
                    llrSegInfo[0].segCycNum    = csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] - 1;
                    llrSegInfo[0].segPeriod    = csiPart2ParaInfo->reDistance[csiPart2EndSymb];
                    llrSegInfo[0].segLlrNum    = csiPart2ParaInfo->reDistance[csiPart2EndSymb] - 1 ;  
                    
                    dataReNumTemp = (nMUciSc - llrSegInfo[0].segCycNum * llrSegInfo[0].segPeriod - 1) + nMUciSc * (ueLastSymbIndex - csiPart2EndSymb);
                    llrSegInfo[1].segStartAddr = (uint32_t *)((uint8_t *)llrSegInfo[0].segStartAddr + llrSegInfo[0].segCycNum * llrSegInfo[0].segPeriod);  
                    llrSegInfo[1].segCycNum    = 1;
                    llrSegInfo[1].segPeriod    = dataReNumTemp;
                    llrSegInfo[1].segLlrNum    = dataReNumTemp;  
                }
            }
            else {/* reDistance[csiPart2EndSymb] == 1 , 所有csi-Part2 RE连续 */
                dataReNumTemp = nMUciSc * (ueLastSymbIndex - csiPart2EndSymb + 1) - csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                segmNum = 1;
                llrSegInfo[0].segStartAddr = (uint32_t *)((uint8_t *)csiPart2AndDataBaseAddr + puschResourceInfo->enCodeCsiPart2Re);  /* 基地址起始就是Part2起始 */
                llrSegInfo[0].segCycNum    = 1;
                llrSegInfo[0].segPeriod    = dataReNumTemp;
                llrSegInfo[0].segLlrNum    = dataReNumTemp;   
            }
        }
        else { /* DMRS符号有数据 */
            segmNumCnt = 1;
            //csiPart2AndDataBaseAddr = (uint32_t *)((uint8_t *)csiPart2AndDataBaseAddr + segmAddrOffSet[0]);  /* 基地址 + Part2偏移为Data起始 */
            if (csiPart2ParaInfo->dmrsNumBfPart2 > 0){ /* CSI-part2前面存在DMRS符号，即存在DATA数据，起始地址即Data的首地址 */
                segCycNum[segmNumCnt]     = 1;
                segPeriod[segmNumCnt]     = csiPart2ParaInfo->dmrsNumBfPart2 * dataReInDmrsSymb;
                segLlrNum[segmNumCnt]     = csiPart2ParaInfo->dmrsNumBfPart2 * dataReInDmrsSymb;
                segmAddrOffSet[segmNumCnt] = 0;
                segmNumCnt++;   
            }
            
            for (symbIndex = csiPart2StartSymb; symbIndex < csiPart2EndSymb; symbIndex++){
                csiPart2ReCnt[segmNumCnt] += csiPart2ParaInfo->part2ReNumInSymb[symbIndex];
                if (csiPart2ParaInfo->part2ReNumInSymb[symbIndex] == 0){ /* DMRS 符号, 根据统计规则, 首尾符号不会是DMRS符号 */  
                    segCycNum[segmNumCnt] = 1;
                    segPeriod[segmNumCnt] = dataReInDmrsSymb;
                    segLlrNum[segmNumCnt] = dataReInDmrsSymb;
                    segmAddrOffSet[segmNumCnt] = segLlrNum[segmNumCnt - 1] + csiPart2ReCnt[segmNumCnt];  
                    segmNumCnt++; 
                }    
            }   
            
            if (csiPart2ParaInfo->reDistance[csiPart2EndSymb] > 1){
                segCycNum[segmNumCnt]     = csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] - 1;
                segPeriod[segmNumCnt]     = csiPart2ParaInfo->reDistance[csiPart2EndSymb];
                segLlrNum[segmNumCnt]     = csiPart2ParaInfo->reDistance[csiPart2EndSymb] - 1;
                segmAddrOffSet[segmNumCnt] = segLlrNum[segmNumCnt - 1] + csiPart2ReCnt[segmNumCnt] + 1;

                segCycNum[segmNumCnt + 1] = 1;
                segPeriod[segmNumCnt + 1] = nMUciSc * (ueLastSymbIndex - csiPart2EndSymb + 1) - csiPart2ParaInfo->reDistance[csiPart2EndSymb] * (csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] - 1) + 1;
                segLlrNum[segmNumCnt + 1] = nMUciSc * (ueLastSymbIndex - csiPart2EndSymb + 1) - csiPart2ParaInfo->reDistance[csiPart2EndSymb] * (csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] - 1) + 1;
                segmAddrOffSet[segmNumCnt + 1] = csiPart2ParaInfo->reDistance[csiPart2EndSymb] * (csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb] - 1) + 1;                
            }
            else { /* csiPart2ParaInfo->reDistance[csiPart2EndSymb] == 1 */
                segCycNum[segmNumCnt]     = 1;
                segPeriod[segmNumCnt]     = nMUciSc * (ueLastSymbIndex - csiPart2EndSymb + 1) - csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                segLlrNum[segmNumCnt]     = nMUciSc * (ueLastSymbIndex - csiPart2EndSymb + 1) - csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
                segmAddrOffSet[segmNumCnt] = segLlrNum[segmNumCnt - 1] + csiPart2ReCnt[segmNumCnt] + csiPart2ParaInfo->part2ReNumInSymb[csiPart2EndSymb];
            }
            
            segmNum = segmNumCnt; 
            llrSegInfo[0].segStartAddr = csiPart2AndDataBaseAddr; /* Data数据的第一个RE位置 */
            for (segmIndex = 0; segmIndex < segmNum; segmIndex++){
                llrSegInfo[segmIndex].segStartAddr = (uint32_t *)((uint8_t *)llrSegInfo[segmIndex].segStartAddr + segmAddrOffSet[segmIndex]);
                llrSegInfo[segmIndex].segCycNum    = segCycNum[segmIndex + 1];
                llrSegInfo[segmIndex].segPeriod    = segPeriod[segmIndex + 1];
                llrSegInfo[segmIndex].segLlrNum    = segLlrNum[segmIndex + 1]; 
            }    
        }
    }

    return segmNum;
}

uint32_t L1PuschHarqAckDecoder2Bit(uint8_t cellIndex, uint8_t slotIndex, uint8_t ueIndex, L1PuschPduInfo *l1PuschUeInfo, PuschResourceInfo *puschResourceInfo)
{   
    uint8_t  qamModer, layerNum,enCodeBitN, deMultiLlrE;
    uint8_t  symbIndex, harqAckBitLen;
    uint16_t nGHarqAck, loopIndex, index;
    int8_t   *ackSoftBitAddr = NULL;
    int16_t  ackSoftBit[24] = { 0 }; 
    int16_t  disMet[4] = { 0 };
    int16_t  maxDisMet;
    uint8_t  tempAckOut;
    int8_t   *encodeTableSelect = NULL;
    uint16_t thresholdAck = 0;

    qamModer  = l1PuschUeInfo->qamModOrder;
    layerNum  = l1PuschUeInfo->nrOfLayers;
    nGHarqAck = puschResourceInfo->enCodeAckRe * qamModer * layerNum;
    
    deMultiLlrE = 0;
    for (symbIndex = 0; symbIndex < SYM_NUM_PER_SLOT; symbIndex++){
        deMultiLlrE += g_puschAckAndCsiInfo[cellIndex][slotIndex][ueIndex][symbIndex].ackReNum;
    }
    
    /* E>N: 重复模式合并，E=N 直接输出，E<N 填0 */
    harqAckBitLen = l1PuschUeInfo->puschUciPara.harqAckBitLength;
    enCodeBitN    = (harqAckBitLen == 1) ? qamModer : 3 * qamModer;
    nGHarqAck     = deMultiLlrE * qamModer * layerNum;
    for (loopIndex = 0; loopIndex < nGHarqAck;  loopIndex++){ 
        index      = loopIndex % enCodeBitN;
        ackSoftBit[index] += ackSoftBitAddr[loopIndex];
    }
    
    if (harqAckBitLen == 1){
        for (index = 0; index < 2; index++){
            for (loopIndex = 0; loopIndex < enCodeBitN; loopIndex++){
                disMet[index] += g_lut1BitEncodeTable[loopIndex][0] * ackSoftBit[loopIndex] + g_lut1BitEncodeTable[loopIndex][1] * ackSoftBit[loopIndex];
            }
        }
    
        tempAckOut = 0;
        maxDisMet  = disMet[0];
        if (disMet[1] > disMet[0]){
            tempAckOut = 1;
            maxDisMet  = disMet[1];
            disMet[2]  = disMet[0];
            disMet[0]  = disMet[1];
            disMet[1]  = disMet[2];
        }
        /* disMet[0],  disMet[1],取绝对值即算法文档的Metric[0],Metric[1], 分别表示最大和次大打分值 */
    }
    else{ 
        if (qamModer == 1){
            encodeTableSelect = &g_lut2BitEncodeTableQm1[0][0];
        }
        else if (qamModer == 2){
            encodeTableSelect = &g_lut2BitEncodeTableQm2[0][0]; 
        } 
        else if (qamModer == 4){
            encodeTableSelect = &g_lut2BitEncodeTableQm4[0][0]; 
        } 
        else if (qamModer == 6){
            encodeTableSelect = &g_lut2BitEncodeTableQm6[0][0]; 
        }
        else if (qamModer == 8){
            encodeTableSelect = &g_lut2BitEncodeTableQm8[0][0]; 
        }  

        for (index = 0; index < 4; index++){
            for (loopIndex = 0; loopIndex < enCodeBitN; loopIndex++){
                encodeTableSelect = encodeTableSelect + loopIndex * 4;
                disMet[index] += encodeTableSelect[0] * ackSoftBit[loopIndex] + encodeTableSelect[1] * ackSoftBit[loopIndex]
                                 + encodeTableSelect[2] * ackSoftBit[loopIndex] + encodeTableSelect[3] * ackSoftBit[loopIndex];
            }
        }

        tempAckOut = 0;
        maxDisMet  = disMet[0];
        for (index = 1; index < 4; index++){
            if (disMet[index] > disMet[0]){
                maxDisMet  = disMet[index];
                tempAckOut = index;
                disMet[1] = disMet[0];
                disMet[0] = disMet[index];

            }
            else if (disMet[index] > disMet[1]){
                 disMet[1] = disMet[index];
            }
        }
        /* disMet[0],  disMet[1],取绝对值即算法文档的Metric[0],Metric[1], 分别表示最大和次大打分值 */
    }
    
    return 0;
}
void PuschRMDecodeHacCfg(L1PuschPduInfo *l1PuschPduInfo, uint16_t uciLen ,uint8_t pduIdxInner, uint8_t msgType, uint16_t sfnNum, uint8_t slotNum, uint8_t cellIndex)
{
    uint8_t  pduNum;
    uint8_t  totBit;
    uint8_t  part1Bit;
    uint8_t  maxCodeRate;
    uint8_t  valQm;
    uint8_t  dataFlag;
    PuschResourceInfo   *puschResourceInfo  = NULL;
    PuschUciPara        *puschUciPara       = NULL;
    CsiPart2ParaInfo    *csiPart2ParaInfo   = NULL;
    HacCfgHead          *hacHead            = NULL;
    RMDecodePduInfo     *rmInfo             = NULL;
    LlrSegInfo          *llrSegInfo         = NULL;

    hacHead = &(g_puschRMDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].hacCfgHead);
    pduNum  = hacHead->pduNum;
    if(0 == pduNum)
    {
        hacHead->sfn     = sfnNum;
        hacHead->slot    = slotNum;
        hacHead->cellIdx = cellIndex;
        hacHead->msgType = msgType;
    }
    puschUciPara = &(l1PuschPduInfo->puschUciPara);
    rmInfo = &(g_puschRMDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].rmPduInfo[pduNum]);
    rmInfo->uciBitNum  = uciLen;
    rmInfo->ueIdx      = pduIdxInner;
    rmInfo->codeMethod = 32;
    rmInfo->uciBitNum  = uciLen;

    if(Pusch_Uci_CsiPart2 == msgType)
    {   
        dataFlag  = ((l1PuschPduInfo->pduBitMap)&0x1);
        csiPart2ParaInfo  = &(g_CsiPart2ParaInfo[cellIndex][sfnNum][pduIdxInner]);
        puschResourceInfo = &(g_puschResourceInfo[cellIndex][sfnNum][pduIdxInner]);
        rmInfo->llrSegNum = L1PuschCsiPart2AndDataExtract(dataFlag, l1PuschPduInfo, puschResourceInfo, csiPart2ParaInfo, &(rmInfo->llrSegInfo[0]));
	}
	else
	{
		rmInfo->llrSegNum = 1;
		llrSegInfo = &rmInfo->llrSegInfo[0];
		llrSegInfo->segStartAddr = 0;//待修改
		llrSegInfo->segCycNum    = 1;
		llrSegInfo->segLlrNum    = rmInfo->llrNum;
		llrSegInfo->segPeriod    = rmInfo->llrNum;
	}

    (hacHead->pduNum)++;
}

void PuschPolarDecodeHacCfg(L1PuschPduInfo *l1PuschPduInfo, uint16_t uciLen ,uint8_t pduIdxInner, uint8_t msgType, uint16_t sfnNum, uint8_t slotNum, uint8_t cellIndex)
{
    uint8_t  pduNum;
    uint8_t  totBit;
    uint8_t  part1Bit;
    uint8_t  maxCodeRate;
    uint8_t  valQm;
    uint16_t K;
    uint16_t n;
    uint16_t n1;
    uint16_t n2;
     uint8_t dataFlag;
    PuschResourceInfo  *puschResourceInfo = NULL;
	PuschUciPara       *puschUciPara      = NULL;
    CsiPart2ParaInfo   *csiPart2ParaInfo  = NULL;
    HacCfgHead         *hacHead           = NULL;
    PolarDecodePduInfo *polarInfo         = NULL;
    LlrSegInfo         *llrSegInfo        = NULL;

    hacHead = &(g_puschPolarDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].hacCfgHead);
    pduNum  = hacHead->pduNum;
    if(0 == pduNum)
    {
        hacHead->sfn     = sfnNum;
        hacHead->slot    = slotNum;
        hacHead->cellIdx = cellIndex;
        hacHead->msgType = msgType;
    }

    polarInfo = &(g_puschPolarDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].polarPduInfo[pduNum]);
    if((12 <= uciLen) && (19 >= uciLen))
    {
        K = uciLen + 6;
    }
    else
    {
        K = uciLen + 11;
    }
    polarInfo->uciBitNum = uciLen;
    polarInfo->ueIdx     = pduIdxInner;
    
    if(((8 * (polarInfo->llrNum)) <= (9 * (2>>(log2Ceiling(polarInfo->llrNum) - 1))))
        && ((16 * K) < (9 * (polarInfo->llrNum))))//E<=(9/8)*2^(Ceil(log2E)-1) and K/E<9/16
    {
        n1 = log2Ceiling(polarInfo->llrNum) - 1;
    }
    else
    {
        n1 = log2Ceiling(polarInfo->llrNum);
    }
    n2 = log2Ceiling(8 * K);
    n = (n1 < n2) ? n1:n2;
    n = (n < 10) ? n:10;
    n = (n < 5) ? 5:n;
    polarInfo->nVal = n;
    polarInfo->sizeT = ceil((sqrt(8 * (polarInfo->llrNum) + 1) - 1)/2);

	if(Pusch_Uci_CsiPart2 == msgType)
	{
        dataFlag  = ((l1PuschPduInfo->pduBitMap)&0x1);
        csiPart2ParaInfo  = &(g_CsiPart2ParaInfo[cellIndex][sfnNum][pduIdxInner]);
        puschResourceInfo = &(g_puschResourceInfo[cellIndex][sfnNum][pduIdxInner]);
        polarInfo->llrSegNum = L1PuschCsiPart2AndDataExtract(dataFlag, l1PuschPduInfo, puschResourceInfo, csiPart2ParaInfo, &(polarInfo->llrSegInfo[0]));
	}
	else
	{
		llrSegInfo = &polarInfo->llrSegInfo[0];
		llrSegInfo->segStartAddr  = 0;//待修改
		llrSegInfo->segCycNum     = 1;
		llrSegInfo->segLlrNum     = polarInfo->llrNum;
		llrSegInfo->segPeriod     = polarInfo->llrNum;
	}

    (hacHead->pduNum)++;
}


void PuschUciRptPreFill(L1PuschPduInfo *l1PuschPduInfo, FapiNrPushUciIndication *fapiNrPushUciIndication)
{
    uint8_t isHarqExistFlag;
    uint8_t isCsiPart1ExistFlag;
    uint8_t isCsiPart2ExistFlag;
    PuschUciPara *puschUciPara = NULL;

    puschUciPara = &(l1PuschPduInfo->puschUciPara);
    isHarqExistFlag     = (0 == (puschUciPara->harqAckBitLength))  ? 0:1;
    isCsiPart1ExistFlag = (0 == (puschUciPara->csiPart1BitLength)) ? 0:1;
    isCsiPart2ExistFlag = (0 == (puschUciPara->flagCsiPart2))      ? 0:1;
    
    fapiNrPushUciIndication->pduBitmap = (isCsiPart2ExistFlag<<3) + (isCsiPart1ExistFlag<<2) + (isHarqExistFlag<<1);
    fapiNrPushUciIndication->Handle    = l1PuschPduInfo->handle;
    fapiNrPushUciIndication->RNTI      = l1PuschPduInfo->ueRnti;
    fapiNrPushUciIndication->UL_CQI    = 0xff;
    fapiNrPushUciIndication->TA        = 0xffff;
    fapiNrPushUciIndication->RSSI      = 0xffff;

    fapiNrPushUciIndication->harqInfoFmt23.HarqBitLen    = (puschUciPara->harqAckBitLength);
    fapiNrPushUciIndication->csipart1Info.CsiPart1BitLen = (puschUciPara->csiPart1BitLength);
}

void PuschUciAndDataHacParaCfg(L1PuschParaPduInfo *l1PuschParaPduInfo, uint8_t cellIndex)//ACK大于2比特&&CSI part1&&不含CSI Part2的Data(LDPC)
{
    uint8_t  ueNum;
    uint8_t  ueIndex;
    uint8_t  slot;
    uint8_t  rptIndex;
    uint16_t sfn;
    uint16_t pduBitmap;
    uint16_t harqAckBitLength;
    uint16_t csiPart1BitLength;
    uint16_t flagCsiPart2;
    L1PuschPduInfo *l1PuschUeInfo = NULL;

    ueNum = l1PuschParaPduInfo->puschPduNum;
    sfn   = l1PuschParaPduInfo->sfnIndex;
    slot  = l1PuschParaPduInfo->slotIndex;

    for(ueIndex = 0; ueIndex < ueNum; ueIndex++)
    {
        l1PuschUeInfo = &l1PuschParaPduInfo->l1PuschPduInfo[ueIndex];
        pduBitmap         = (l1PuschUeInfo->pduBitMap);
        harqAckBitLength  = (l1PuschUeInfo->puschUciPara.harqAckBitLength);
        csiPart1BitLength = (l1PuschUeInfo->puschUciPara.csiPart1BitLength);
        flagCsiPart2      = (l1PuschUeInfo->puschUciPara.flagCsiPart2);

        if((pduBitmap&0x1) && (!flagCsiPart2))//不含CSI Part2的Data
        {
            //L1PuschPara2LdpcDecoderHac(cellIndex, l1PuschParaPduInfo, ldpcDecoderHacPara);
        }

        if((pduBitmap>>1)&0x1)//UCI
        {
            if((1 == harqAckBitLength) || (2 == harqAckBitLength))//ACK 1/2比特
            {
                ;//待补充？
            }
            else if((RM_BIT_LENGTH_MIN <= harqAckBitLength) && (RM_BIT_LENGTH_MAX >= harqAckBitLength))
            {
                PuschRMDecodeHacCfg(l1PuschUeInfo, harqAckBitLength, ueIndex, Pusch_Uci_Ack, sfn, slot, cellIndex);
            }
            else if((POLAR_BIT_LENGTH_MIN <= harqAckBitLength) && (POLAR_BIT_LENGTH_MAX >= harqAckBitLength))
            {
                PuschPolarDecodeHacCfg(l1PuschUeInfo, harqAckBitLength, ueIndex, Pusch_Uci_Ack, sfn, slot, cellIndex);
            }

            if((RM_BIT_LENGTH_MIN <= csiPart1BitLength) && (RM_BIT_LENGTH_MAX >= csiPart1BitLength))
            {
                PuschRMDecodeHacCfg(l1PuschUeInfo, csiPart1BitLength, ueIndex, Pusch_Uci_Ack, sfn, slot, cellIndex);
            }
            else if((POLAR_BIT_LENGTH_MIN <= csiPart1BitLength) && (POLAR_BIT_LENGTH_MAX >= csiPart1BitLength))
            {
                PuschPolarDecodeHacCfg(l1PuschUeInfo, csiPart1BitLength, ueIndex, Pusch_Uci_Ack, sfn, slot, cellIndex);
            }

            rptIndex = g_puschUciRptNum[cellIndex][slot];
            PuschUciRptPreFill(l1PuschUeInfo, &(g_puschUciRst[cellIndex][slot].fapiNrPushUciIndication[rptIndex]));//UCI上报预填
            g_puschUciRptNum[cellIndex][slot] = g_puschUciRptNum[cellIndex][slot]++;
        }
    }
}

uint32_t PuschACK1or2BitDecodeHandler()
{

    //L1PuschHarqAckDecoder2Bit(cellIndex, slotIndex, ueIndex, l1PuschUeInfo, puschResourceInfo);

    return 0;
}

uint32_t PuschUCIParser()//ACK大于2比特，CSI Part1和CSI Part2解析
{
    uint16_t sfn;     
    uint16_t slot;
    uint16_t csiPart2BitLen;  
    uint8_t  cellIdx;
    uint8_t  pduNum;
    uint8_t  codeType;
    uint8_t  msgType;
    uint8_t  pduIndex;
    uint8_t  ueIdx;
    uint8_t  rptIdx;
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
    L1PuschPduInfo               *l1PuschPduInfo               = NULL;

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
            
            ueIdx  = rmDecodePduInfo->ueIdx;
            rptIdx = g_puschUciRptIndex[cellIdx][slot][ueIdx];
            l1PuschPduInfo = &(g_puschParaInfoOut[cellIdx][slot].l1PuschPduInfo[ueIdx]);
			if(0 == msgType)//ACK大于2比特
			{
				harqInfoFmt23 = &(puschUciRst->fapiNrPushUciIndication[rptIdx].harqInfoFmt23);
				harqInfoFmt23->HarqCrc        = 0;//待补充
				harqInfoFmt23->HarqBitLen     = rmDecodePduInfo->uciBitNum;
				harqInfoFmt23->HarqPayload[0] = 0;//L2D到DDR的拷贝,待补充
			}
			else if(1 == msgType)//CSI Part1
			{
				csipart1Info = &(puschUciRst->fapiNrPushUciIndication[rptIdx].csipart1Info);
				csipart1Info->CsiPart1Crc        = 0;//待补充
				csipart1Info->CsiPart1BitLen     = rmDecodePduInfo->uciBitNum;
				csipart1Info->CsiPart1Payload[0] = 0;//L2D到DDR的拷贝,待补充
                if(l1PuschPduInfo->puschUciPara.flagCsiPart2)//如果存在CSI Part2
                {
                    //l1PuschPduInfo = ;//待补充
                    //sizesPart1Params = ;//待补充
                    //map              = ;//待补充
                    //numPart1Params   = ;//待补充
                    csiPart2BitLen = CalcCsiPart2BitLength(&(l1PuschPduInfo->part2InfoAddInV3), csipart1Info->CsiPart1Payload, sizesPart1Params, map, csipart1Info->CsiPart1BitLen, numPart1Params);
                    if((RM_BIT_LENGTH_MIN <= csiPart2BitLen) && (RM_BIT_LENGTH_MAX >= csiPart2BitLen))
                    {
                        PuschRMDecodeHacCfg(l1PuschPduInfo, csiPart2BitLen, pduIndex, Pusch_Uci_CsiPart2, sfn, slot, cellIdx);
                    }
                    else if((POLAR_BIT_LENGTH_MIN <= csiPart2BitLen) && (POLAR_BIT_LENGTH_MAX >= csiPart2BitLen))
                    {
                        PuschPolarDecodeHacCfg(l1PuschPduInfo, csiPart2BitLen, pduIndex, Pusch_Uci_CsiPart2, sfn, slot, cellIdx);
                    }

                    if((l1PuschPduInfo->pduBitMap)&0x1)//配置含Part2的UE的LDPC编码
                    {
                        //L1PuschPara2LdpcDecoderHac(cellIdx, l1PuschPduInfo, ldpcDecoderHacPara);
                    }
                }
			}
			else if(2 == msgType)//CSI Part2
			{
				csipart2Info = &(puschUciRst->fapiNrPushUciIndication[rptIdx].csipart2Info);
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
            
            ueIdx  = polarDecodePduInfo->ueIdx;
            rptIdx = g_puschUciRptIndex[cellIdx][slot][ueIdx];
            l1PuschPduInfo = &(g_puschParaInfoOut[cellIdx][slot].l1PuschPduInfo[ueIdx]);
			if(0 == msgType)//ACK大于2比特
			{
				harqInfoFmt23 = &(puschUciRst->fapiNrPushUciIndication[rptIdx].harqInfoFmt23);
				harqInfoFmt23->HarqCrc        = 0;//polarUeDecodeOut->待补充
				harqInfoFmt23->HarqBitLen     = polarDecodePduInfo->uciBitNum;
				harqInfoFmt23->HarqPayload[0] = 0;//polarUeDecodeOut,L2D到DDR的拷贝,待补充
			}
			else if(1 == msgType)//CSI Part1
			{
				csipart1Info = &(puschUciRst->fapiNrPushUciIndication[rptIdx].csipart1Info);
				csipart1Info->CsiPart1Crc        = 0;//polarUeDecodeOut->待补充
				csipart1Info->CsiPart1BitLen     = polarDecodePduInfo->uciBitNum;
				csipart1Info->CsiPart1Payload[0] = 0;//polarUeDecodeOut,L2D到DDR的拷贝,待补充
                if(l1PuschPduInfo->puschUciPara.flagCsiPart2)//如果存在CSI Part2
                {
                    //sizesPart1Params = ;//待接口确定后补充
                    //map              = ;//待接口确定后补充
                    //numPart1Params   = ;//待接口确定后补充
                    csiPart2BitLen = CalcCsiPart2BitLength(&(l1PuschPduInfo->part2InfoAddInV3), csipart1Info->CsiPart1Payload, sizesPart1Params, map, csipart1Info->CsiPart1BitLen, numPart1Params);
                    if((RM_BIT_LENGTH_MIN <= csiPart2BitLen) && (RM_BIT_LENGTH_MAX >= csiPart2BitLen))
                    {
                        PuschRMDecodeHacCfg(l1PuschPduInfo, csiPart2BitLen, pduIndex, Pusch_Uci_CsiPart2, sfn, slot, cellIdx);
                    }
                    else if((POLAR_BIT_LENGTH_MIN <= csiPart2BitLen) && (POLAR_BIT_LENGTH_MAX >= csiPart2BitLen))
                    {
                        PuschPolarDecodeHacCfg(l1PuschPduInfo, csiPart2BitLen, pduIndex, Pusch_Uci_CsiPart2, sfn, slot, cellIdx);
                    } 
                }
			}
			else if(2 == msgType)//CSI Part2
			{
				csipart2Info = &(puschUciRst->fapiNrPushUciIndication[rptIdx].csipart2Info);
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

void PuschPart1AndLDPCParaCfgHandler()//是否与DSP参数计算放在一起待确认？
{
    printf("配置Part1译码参数&&配置无Part2 UE的LDPC译码参数\n");
    
    //配置CSI Part1译码参数
    //配置不含CSI Part2的UE的LDPC译码参数
    //PuschUciAndDataHacParaCfg(L1PuschParaPduInfo *l1PuschParaPduInfo, uint8_t cellIndex)//ACK大于2比特&&CSI part1&&不含CSI Part2的Data(LDPC)

    //return 0;
}

void PuschPart1ParsePart2AndLDPCParaCfgHandler()
{
    printf("解析Part1&&配置Part2译码参数&&配置含Part2的UE的LDPC译码参数\n");
    
    PuschUCIParser();//解析CSI Part1，顺便配置CSI Part2参数(如有)
    
    //配置含CSI Part2的UE的LDPC译码参数

    //return 0;
}

void PuschTriggerPart2AndLDPCDecodeHandler()//待确认接口后补充
{
    printf("trigger Polar/RM HAC启动Part2译码&&trigger LDPC HAC启动UL-Data译码\n");//
    //return 0;
}

void PuschParseCfgTriggerHandler()
{
    PuschPart1ParsePart2AndLDPCParaCfgHandler();

    PuschTriggerPart2AndLDPCDecodeHandler();
    
    //return 0;
}

void PuschPart2ParseHandler()
{
    printf("解析PUSCH Part2\n");//PUSCH CSI part2
    
    PuschUCIParser();

    //return 0;
}

void PuschUciSendHandler()//待设计，收齐结果后发送(UCI HAC的译码结果)？
{
    printf("给L2发送UCI\n");
    
    //return 0;
}

uint32_t PuschUciFsmProc(uint32_t event, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex)
{

    /*待挪至slot任务启动时初始化
    FSM_Regist(g_puschUciFSM[cellIndex][slotNum&0x1],g_puschUciTable);待挪至slot任务启动时初始化
    g_puschUciFSM[cellIndex][slotNum&0x1].curState = Pusch_Uci_Idle_State;
    g_puschUciFSM[cellIndex][slotNum&0x1].size      = sizeof(g_puschUciTable)/sizeof(FsmTable);
    */

    if(g_puschCsiPart2Flag[cellIndex][slotNum])//本slot本小区含CSI Part2的UE，进入状态机流程
    {
        while(1)//待修改为上报结果收集完成即退出
        {
            printf("state:%d\n",g_puschUciFSM[cellIndex][slotNum&0x1].curState);
            //scanf("%d", &event);
            switch (event)
            {
                case Pusch_Slot_Tast_Start_Event:
                case Pusch_Part1_Result_Trigger_Event:
                case Pusch_Part2_And_Data_Demap_Trigger_Event:
                case Pusch_Part2_Result_Trigger_Event:
                case Pusch_UCI_Packing_Over_Event:
                    FSM_EventHandle(&g_puschUciFSM[cellIndex][slotNum&0x1], event);//状态机
                    break;
                case ACK_1or2_Bit_Data_Trigger_Event:
                    PuschACK1or2BitDecodeHandler();
                    break;
                case ACK_Over2_Bit_Result_Trigger_Event:
                    PuschACKOver2BitParseHandler();
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        while(1)//待修改为上报结果收集完成即退出
        {
            printf("state:%d\n",g_puschUciFSM[cellIndex][slotNum&0x1].curState);
            //scanf("%d", &event);
            switch (event)
            {
                case Pusch_Slot_Tast_Start_Event:
                    PuschPart1AndLDPCParaCfgHandler();
                    break;
                case Pusch_Part1_Result_Trigger_Event:
                    PuschPart1ParsePart2AndLDPCParaCfgHandler();//不含CSI Part2,函数内部控制不会配置包含CSI Part2的UE的UCI译码和LPDC译码参数
                    break;
                case Pusch_UCI_Packing_Over_Event:
                    PuschUciSendHandler();
                    break;
                case ACK_1or2_Bit_Data_Trigger_Event:
                    PuschACK1or2BitDecodeHandler();
                    break;
                case ACK_Over2_Bit_Result_Trigger_Event:
                    PuschACKOver2BitParseHandler();
                    break;
                default:
                    break;
            }

        }
    }

    return 0;
}

#if 1

FsmTable g_puschUciTable[] =
{
    //{当前状态S，                                  trigger事件E，                              handler函数H，                               转移后的状态S}
	{Pusch_Uci_Idle_State,                         Pusch_Slot_Tast_Start_Event,                PuschPart1AndLDPCParaCfgHandler,             Pusch_Wait_Part1_Result_State},//S0{E0,H0}->S1
	
	{Pusch_Wait_Part1_Result_State,                Pusch_Part1_Result_Trigger_Event,           PuschPart1ParsePart2AndLDPCParaCfgHandler,   Pusch_Wait_Part2_Data_With_Para_Ready_State},//S1{E1,H1}->S2
	{Pusch_Wait_Part2_Data_With_Para_Ready_State,  Pusch_Part2_And_Data_Demap_Trigger_Event,   PuschTriggerPart2AndLDPCDecodeHandler,       Pusch_Wait_Part2_Result_State},//S2{E2,H2}->S4
	
	{Pusch_Wait_Part1_Result_State,                Pusch_Part2_And_Data_Demap_Trigger_Event,   NULL,                                        Pusch_Wait_Part2_Para_With_Data_Ready_State},//S1{E2,NULL}->S3
	{Pusch_Wait_Part2_Para_With_Data_Ready_State,  Pusch_Part1_Result_Trigger_Event,           PuschParseCfgTriggerHandler,                 Pusch_Wait_Part2_Result_State},//S3{E1,H1+H2}->S4
	
	{Pusch_Wait_Part2_Result_State,                Pusch_Part2_Result_Trigger_Event,           PuschPart2ParseHandler,                      Pusch_Uci_Packing_State},//S4{E3,H3}->S5
	{Pusch_Uci_Packing_State,                      Pusch_UCI_Packing_Over_Event,               PuschUciSendHandler,                         Pusch_Uci_Idle_State},//S4{E4,H4}->S0
};

#endif