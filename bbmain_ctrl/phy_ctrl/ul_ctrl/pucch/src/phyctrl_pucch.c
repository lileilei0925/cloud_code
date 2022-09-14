#include <stdio.h>
#include <string.h>
#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../../../../common/inc/phy_ctrl_common.h"
#include "../inc/phyctrl_pucch.h"
#include "../inc/pucch_variable.h"
#include "../../../../common/src/common.c"

void PucchNcsandUVCalc(uint8_t SlotIdx, uint16_t PucchHoppingId,uint8_t GroupHopping);

void UlTtiRequestPucchFmt023Pduparse(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucParam *pucParam, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex);
void UlTtiRequestPucchFmt1Pduparse(PucParam *pucParam, uint8_t pucchpduGroupCnt, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex);
void PucchFmt1Grouping();
void UlTtiRequestPucchPduparse(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucParam *pucParam, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex);

/*
int main(void)
{
  uint16_t a = 16;
  uint16_t b = 16;
  uint16_t c = 0;

  printf("c = %d;\n",c);
  printf("___Hello World___;\n");

  return 0;
}
*/

void PucchNcsandUVCalc(uint8_t SlotIdx, uint16_t PucchHoppingId,uint8_t GroupHopping)
{
    uint32_t Cinit = 0;
    uint32_t SequenceLen = 0;
    uint32_t TempData = 0;
    uint16_t NIDdiv30 = 0;
    uint8_t  SlotIdx1 = 0;
    uint8_t  FssPucch = 0;
    uint8_t  FghPucch = 0;
    uint8_t  SlotBits = 0;
    uint8_t  StartSaveIdx = 0;
    uint8_t  SymbIdx = 0;
    uint8_t  NcsTemp = 0;
    
    FghPucch = 0;
    FssPucch = PucchHoppingId % 30;
    NIDdiv30 = PucchHoppingId / 30;

    if ((1 == GroupHopping))   /*group hopping is enable */
    {
        /*calculate first hop u,v*/
        Cinit        = NIDdiv30;
        SequenceLen  = 8 * (SlotIdx + 1) * 2;
        StartSaveIdx = (8 * 2 * SlotIdx) >> 5;
        PseudoRandomSeqGen(g_NghNvNcsBuff, Cinit, SequenceLen, StartSaveIdx);

        SlotBits = (8 * 2 * SlotIdx) & 0x1F;  /* %32 */
        TempData = do_brev(g_NghNvNcsBuff[0]);   /* 位反转 */
        FghPucch = (_extu(TempData, (24 - SlotBits), 24)) % 30;
        g_NuValue[0] = (FssPucch + FghPucch) % 30;
        g_NvValue[0] = 0;

        /*calculate second hop u,v*/
        SlotBits = (8 * (2 * SlotIdx + 1)) & 0x1F;  /* %32 */
        TempData = do_brev(g_NghNvNcsBuff[0]);   /* 位反转 */
        FghPucch = (_extu(TempData, (24 - SlotBits), 24)) % 30;
        g_NuValue[1] = (FssPucch + FghPucch) % 30;
        g_NvValue[1] = 0;
    }
    else if (2 == GroupHopping) /*group hopping is disabled,sequence hopping is enable */
    {
        /*calculate first hop u,v*/
        Cinit        = (NIDdiv30 << 5) + FssPucch;
        SequenceLen  = (SlotIdx + 1) * 2;
        StartSaveIdx = (2 * SlotIdx) >> 5;
        PseudoRandomSeqGen(g_NghNvNcsBuff, Cinit, SequenceLen, StartSaveIdx);
        
        SlotBits = (2 * SlotIdx) & 0x1F;  /* %32 */
        TempData = g_NghNvNcsBuff[SlotIdx1 - StartSaveIdx];
        g_NuValue[0] = (FssPucch + FghPucch) % 30;
        g_NvValue[0] = (TempData >> (31 - SlotBits)) & 0x1;

        /*calculate second hop u,v*/
        SlotBits = (2 * SlotIdx + 1) & 0x1F;  /* %32 */
        TempData = g_NghNvNcsBuff[0];
        g_NuValue[1] = (FssPucch + FghPucch) % 30;
        g_NvValue[1] = (TempData >> (31 - SlotBits)) & 0x1;
    }
    else /*group hopping is disabled,sequence hopping is disabled, */
    {
        /*calculate first hop u,v*/
        g_NuValue[0] = (FssPucch + FghPucch) % 30;
        g_NvValue[0] = 0;

        /*calculate second hop u,v*/
        g_NuValue[1] = (FssPucch + FghPucch) % 30;
        g_NvValue[1] = 0;
    }

    /*calculate ncs of current slot in TS 38.211 6.3.2.2.2*/
    Cinit        = PucchHoppingId;
    SequenceLen  = 8 * SYM_NUM_PER_SLOT * (SlotIdx + 1);
    StartSaveIdx = (8 * SYM_NUM_PER_SLOT * SlotIdx)>>5;
    PseudoRandomSeqGen(g_NghNvNcsBuff, Cinit, SequenceLen, StartSaveIdx);
    for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
    {
        TempData = 8 * SYM_NUM_PER_SLOT * SlotIdx + 8 * SymbIdx;
        SlotIdx1 = TempData >> 5;
        SlotBits = TempData & 0x1F; /* %32 */
        TempData = do_brev(g_NghNvNcsBuff[SlotIdx1 - StartSaveIdx]);
        NcsTemp  = _extu(TempData, (24 - SlotBits), 24);
        g_NcsValue[SymbIdx] = NcsTemp;
    }
}

void UlTtiRequestPucchFmt023Pduparse(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucParam *pucParam, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex)
{
    uint8_t  formatType;
    uint8_t  EndSymbolIndex;
    uint8_t  pucchNumpersym;
    uint8_t  pucchindex;
    uint8_t  groupOrSequenceHopping;
    uint8_t  intraSlotFreqHopping; 
    uint8_t  symNum[HOP_NUM];
    uint8_t  SymbIdx;
    uint8_t  StartSaveIdx;
    uint32_t Cinit = 0;
    uint32_t SequenceLen = 0;
    PucFmt0Param *fmt0Param = NULL;
    PucFmt2Param *fmt2Param = NULL;
    PucFmt3Param *fmt3Param = NULL;

    formatType      = fapipucchpduInfo->formatType;
    EndSymbolIndex  = fapipucchpduInfo->StartSymbolIndex + fapipucchpduInfo->numSymbols;
    pucchindex      = g_armPucParam.pucchNum;
    pucchNumpersym  = g_armPucParam.pucchNumpersym[EndSymbolIndex]++;
    g_armPucParam.pucchIndex[EndSymbolIndex][pucchNumpersym] = pucchindex;

    groupOrSequenceHopping = fapipucchpduInfo->groupOrSequenceHopping;
    intraSlotFreqHopping   = fapipucchpduInfo->intraSlotFreqHopping;
    /* 所在小区的小区级参数 */
    pucParam->PduIdxInner  = pduIndex;
    pucParam->pucFormat    = fapipucchpduInfo->formatType;
    pucParam->rxAntNum     = g_cellConfigPara[cellIndex].rxAntNum;

    /* frequency domain */
    pucParam->prbStart              = fapipucchpduInfo->prbStart;
    pucParam->prbSize               = fapipucchpduInfo->prbSize;
    pucParam->intraSlotFreqHopping  = fapipucchpduInfo->intraSlotFreqHopping;
    pucParam->secondHopPrb          = fapipucchpduInfo->secondHopPRB;

    /* time domain */
    pucParam->startSymIdx = fapipucchpduInfo->StartSymbolIndex;
    pucParam->symNum      = fapipucchpduInfo->numSymbols;

    /* PUC data在DDR中的存放地址 */
    //uint32_t *dataStartAddr[HOP_NUM];  
    
    /* ZC基序列或PN序列在DDR中的存放地址:
       fmt0数据/fmt1数据和导频/fmt3导频使用ZC序列,某hop内,各符号天线上的ZC基序列相同;
       fmt2导频使用PN序列,数组的2个元素分别为2个符号的PN序列的地址 */
    //uint32_t *baseSeqAddr[HOP_NUM]; 
    
    if(PUCCH_FORMAT_0 == formatType)
    {
        pucParam->dmrsSymNum[0] = 0;
        pucParam->dmrsSymNum[1] = 0;
        pucParam->uciSymNum[0]  = (0 == intraSlotFreqHopping) ? (pucParam->symNum):1;
        pucParam->uciSymNum[1]  = pucParam->symNum - pucParam->uciSymNum[0];

        fmt0Param = (PucFmt0Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
        fmt0Param->srFlag        = fapipucchpduInfo->srFlag;
        fmt0Param->harqBitLength = fapipucchpduInfo->bitLenHarq;
        PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
        for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
        {
            fmt0Param->cyclicShift[SymbIdx] = (fapipucchpduInfo->initCyclicShift + g_NcsValue[SymbIdx]) % SC_NUM_PER_RB;
        }

        fmt0Param->rnti = fapipucchpduInfo->ueRnti;

    }
    else if(PUCCH_FORMAT_2 == formatType)
    {
        pucParam->uciSymNum[0]  = (0 == intraSlotFreqHopping) ? (pucParam->symNum):1;
        pucParam->uciSymNum[1]  = pucParam->symNum - pucParam->uciSymNum[0];
        pucParam->dmrsSymNum[0] = pucParam->uciSymNum[0];
        pucParam->dmrsSymNum[1] = pucParam->uciSymNum[1];

        Cinit 		 = ((fapipucchpduInfo->ueRnti)<<15)  + fapipucchpduInfo->nIdPucchHopping;
        SequenceLen  = 16*(fapipucchpduInfo->numSymbols)*((fapipucchpduInfo->prbSize));
        PseudoRandomSeqGen(g_fmt23dataScrambuff, Cinit, SequenceLen, 0);

	    Cinit 		 = ((((SYM_NUM_PER_SLOT * slotNum + fapipucchpduInfo->StartSymbolIndex + 1)*(2 * fapipucchpduInfo->dmrsScramblingId + 1))<<17)  + (2 * fapipucchpduInfo->dmrsScramblingId))&0x7fffffff;//超过32位
        SequenceLen  = ((fapipucchpduInfo->prbStart + fapipucchpduInfo->prbSize) * SC_NUM_PER_RB * 2 * 1/3);
        StartSaveIdx = (fapipucchpduInfo->prbStart * SC_NUM_PER_RB * 2 * 1/3) >> 5;
        PseudoRandomSeqGen(g_fmt2pilotScrambuff[0], Cinit, SequenceLen, StartSaveIdx);
	    
        Cinit 		 = ((((SYM_NUM_PER_SLOT * slotNum + fapipucchpduInfo->StartSymbolIndex + 2)*(2 * fapipucchpduInfo->dmrsScramblingId + 1))<<17)  + (2 * fapipucchpduInfo->dmrsScramblingId))&0x7fffffff;;//超过32位
        if(fapipucchpduInfo->intraSlotFreqHopping)
        {
            SequenceLen  = (fapipucchpduInfo->secondHopPRB + fapipucchpduInfo->prbSize) * SC_NUM_PER_RB * 2 * 1/3;
	        StartSaveIdx = (fapipucchpduInfo->secondHopPRB * SC_NUM_PER_RB * 2 * 1/3) >> 5;
        }
        PseudoRandomSeqGen(g_fmt2pilotScrambuff[1], Cinit, SequenceLen, StartSaveIdx);
        
        fmt2Param = (PucFmt2Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
        fmt2Param->srBitLen = fapipucchpduInfo->srFlag;
        fmt2Param->harqBitLength = fapipucchpduInfo->bitLenHarq;
        fmt2Param->csiPart1BitLength = fapipucchpduInfo->csiPart1BitLength;
        fmt2Param->rnti              = fapipucchpduInfo->ueRnti;
        //fmt2Param->scrambSeqAddr[HOP_NUM];   /* 加扰序列在DDR中的存放地址,TODO:根据HAC存放确定是否需要2个hop的首地址 */ 
    }
    else if(PUCCH_FORMAT_3 == formatType)
    {
        fmt3Param = (PucFmt3Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
        fmt3Param->pi2bpsk = fapipucchpduInfo->pi2BpskFlag;
        fmt3Param->addDmrsFlag = fapipucchpduInfo->addDmrsFlag;
        fmt3Param->srBitLen = fapipucchpduInfo->srFlag;
        fmt3Param->harqBitLength = fapipucchpduInfo->bitLenHarq;
        fmt3Param->csiPart1BitLength = fapipucchpduInfo->csiPart1BitLength;
        fmt3Param->rnti = fapipucchpduInfo->ueRnti;

        PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
        for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
        {
            fmt3Param->cyclicShift[SymbIdx] = (fapipucchpduInfo->initCyclicShift + g_NcsValue[SymbIdx]) % SC_NUM_PER_RB;
        }

        Cinit         = ((fapipucchpduInfo->ueRnti)<<15)  + fapipucchpduInfo->nIdPucchHopping;
        SequenceLen   = 12*(2-fapipucchpduInfo->pi2BpskFlag)*(fapipucchpduInfo->numSymbols)*((fapipucchpduInfo->prbSize));
        PseudoRandomSeqGen(g_fmt23dataScrambuff, Cinit, SequenceLen, 0);
        //fmt3Param->scrambSeqAddr[HOP_NUM];           /* 加扰序列在DDR中的存放地址，TODO:根据HAC存放确定是否需要2个hop的首地址 */

        symNum[0]  = (0 == intraSlotFreqHopping) ? (pucParam->symNum) : ((pucParam->symNum)>>1);
        symNum[1]  = pucParam->symNum - symNum[0];
        if(5 > pucParam->symNum)
        {
            pucParam->dmrsSymNum[0] = 1;
            pucParam->dmrsSymNum[1] = (0 == intraSlotFreqHopping) ? 0:1;;
        }
        else if(10 > pucParam->symNum)
        {
            pucParam->dmrsSymNum[0] = (0 == intraSlotFreqHopping) ? 2:1;
            pucParam->dmrsSymNum[1] = (0 == intraSlotFreqHopping) ? 0:1;
        }
        else
        {
            pucParam->dmrsSymNum[0] = (0 == intraSlotFreqHopping) ? (2*(1 + fmt3Param->addDmrsFlag)) : (1 + fmt3Param->addDmrsFlag);
            pucParam->dmrsSymNum[1] = (0 == intraSlotFreqHopping) ? 0 : (1 + fmt3Param->addDmrsFlag);
        }
        pucParam->uciSymNum[0] = symNum[0] - pucParam->dmrsSymNum[0];
        pucParam->uciSymNum[1] = symNum[1] - pucParam->dmrsSymNum[1];
        pucParam->secondHopSymIdx  = (0 == intraSlotFreqHopping) ? 0 : (pucParam->startSymIdx + symNum[0]);
    }
}

void UlTtiRequestPucchFmt1Pduparse(PucParam *pucParam, uint8_t pucchpduGroupCnt, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex)
{
    uint8_t  EndSymbolIndex;
    uint8_t  pucchNumpersym;
    uint8_t  pucchindex;
    uint8_t  pucchpduIndex;
    uint8_t  intraSlotFreqHopping;
    uint8_t  symNum[HOP_NUM];
    uint8_t  SymbIdx;
    uint8_t  OccIdx;
    uint8_t  OccNumCnt;
    uint8_t  MinUserOcc;
    uint8_t  MinUserOccIdx;
    uint8_t  pucchuserNumPerOcc;
    FapiNrMsgPucchPduInfo *fapipucchpduInfo = NULL;
    PucFmt1Param          *fmt1Param        = NULL;
    Fmt1ParamOcc          *fmt1ParamOcc     = NULL;
    Fmt1UEParam           *fmt1UEParam      = NULL;                     

    /* fmt1 UE common */
    fmt1Param                       =  (PucFmt1Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
    PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
    for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
    {
        fmt1Param->cyclicShift[SymbIdx]  = (g_NcsValue[SymbIdx]) % SC_NUM_PER_RB;//DSP算法和实现要求，fmt1不需要加初始循环移位m0
    }

    /* fmt1 UE*/
    MinUserOcc    = MAX_USER_NUM_PER_OCC;
    MinUserOccIdx = 0;
    OccNumCnt     = 0;
    for(OccIdx = 0; OccIdx < MAX_OCC_NUM_FMT1; OccIdx++)
    {
        pucchuserNumPerOcc = g_armPucParam.pucchuserNumPerOcc[pucchpduGroupCnt][OccIdx];
        if(0 == pucchuserNumPerOcc)
        {
            continue;
        }

        fmt1ParamOcc = &fmt1Param->fmt1ParamOcc[OccNumCnt];
        memset(fmt1ParamOcc->ueTapBitMap, 0, SYM_NUM_PER_SLOT);
        for(SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
        {   
            fmt1ParamOcc->ueTapBitMap[SymbIdx] |= (1<<(fmt1Param->cyclicShift[SymbIdx]));
        }
        fmt1ParamOcc->timeDomainOccIdx  = OccIdx;
        fmt1ParamOcc->userNumPerOcc     = pucchuserNumPerOcc;

        for(pucchindex = 0; pucchindex < pucchuserNumPerOcc; pucchindex++)
        {
            pucchpduIndex    = g_armPucParam.pucchpduIndexinGroup[pucchpduGroupCnt][OccIdx][pucchindex];
            fapipucchpduInfo = &(g_armPucParam.FapiPucchPduInfo[pucchpduIndex]);
            fmt1UEParam = &fmt1ParamOcc->fmt1UEParam[pucchindex];
            fmt1UEParam->m0            = fapipucchpduInfo->initCyclicShift;
            fmt1UEParam->srFlag        = fapipucchpduInfo->srFlag;
            fmt1UEParam->harqBitLength = fapipucchpduInfo->bitLenHarq;
            fmt1UEParam->rnti          = fapipucchpduInfo->ueRnti;   
        }

        if(MinUserOcc > pucchuserNumPerOcc)
        {
            MinUserOcc     = pucchuserNumPerOcc;
            MinUserOccIdx  = OccNumCnt;
        }
        OccNumCnt++;
    }
    fmt1Param->occNum        = OccNumCnt;
    fmt1Param->MinUserOccIdx = MinUserOccIdx;

    EndSymbolIndex   = fapipucchpduInfo->StartSymbolIndex + fapipucchpduInfo->numSymbols;
    pucchNumpersym   = g_armPucParam.pucchNumpersym[EndSymbolIndex]++;
    g_armPucParam.pucchIndex[EndSymbolIndex][pucchNumpersym] = g_armPucParam.pucchNum;

    intraSlotFreqHopping = fapipucchpduInfo->intraSlotFreqHopping;
    /* 所在小区的小区级参数 */
    //pucParam->PduIdxInner  = pduIndex;需要修改，塞到各种格式下
    pucParam->pucFormat    = fapipucchpduInfo->formatType;
    pucParam->rxAntNum     = g_cellConfigPara[cellIndex].rxAntNum;
    /* frequency domain */
    pucParam->prbStart              = fapipucchpduInfo->prbStart;
    pucParam->prbSize               = fapipucchpduInfo->prbSize;
    pucParam->intraSlotFreqHopping  = intraSlotFreqHopping;
    pucParam->secondHopPrb          = fapipucchpduInfo->secondHopPRB;
    /* time domain */
    pucParam->startSymIdx = fapipucchpduInfo->StartSymbolIndex;
    pucParam->symNum      = fapipucchpduInfo->numSymbols;
    symNum[0]                 = (0 == intraSlotFreqHopping) ? (pucParam->symNum) : ((pucParam->symNum)>>1);
    symNum[1]                 = pucParam->symNum - symNum[0];
    pucParam->uciSymNum[0]    = (0 == intraSlotFreqHopping) ? ((pucParam->symNum)>>1) : ((pucParam->symNum)>>2);
    pucParam->uciSymNum[1]    = (0 == intraSlotFreqHopping) ? 0 : ((pucParam->symNum + 2)>>2);
    pucParam->dmrsSymNum[0]   = symNum[0] - pucParam->uciSymNum[0];
    pucParam->dmrsSymNum[1]   = symNum[1] - pucParam->uciSymNum[1];
    pucParam->secondHopSymIdx = (0 == intraSlotFreqHopping) ? 0 : (pucParam->startSymIdx + symNum[0]);
    /* PUC data在DDR中的存放地址 */
    //uint32_t *dataStartAddr[HOP_NUM];  

    /* ZC基序列或PN序列在DDR中的存放地址:
       fmt0数据/fmt1数据和导频/fmt3导频使用ZC序列,某hop内,各符号天线上的ZC基序列相同;
       fmt2导频使用PN序列,数组的2个元素分别为2个符号的PN序列的地址 */
    //uint32_t *baseSeqAddr[HOP_NUM]; 
}

void PucchFmt1Grouping()
{
    uint8_t  pucchfmt1pduflag[MAX_PUCCH_NUM] = {0};
    uint16_t prbStart1,prbStart2;
    uint8_t  StartSymbolIndex1,StartSymbolIndex2;
    uint8_t  tdOccIdx1,tdOccIdx2;
    uint8_t  pucchpduIndex1,pucchpduIndex2;
    uint8_t  pucchpduNumCnt1,pucchpduNumCnt2;
    uint8_t  pucchueidxInOcc;
    uint8_t  pucchpduGroupNum;
    uint8_t  pucchpduGroupCnt;
    uint8_t  groupIndex;
    uint8_t  ueIndex; 
    uint8_t  ueNumPerGroup;

    for(pucchpduNumCnt1 = 0; pucchpduNumCnt1 < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_1]; pucchpduNumCnt1++)
    {
        if(1 == pucchfmt1pduflag[pucchpduNumCnt1])
        {
            continue;
        }
        pucchpduIndex1      = g_armPucParam.pucchfmtpduIdxInner[PUCCH_FORMAT_1][pucchpduNumCnt1];
        prbStart1           = g_armPucParam.FapiPucchPduInfo[pucchpduIndex1].prbStart;
        StartSymbolIndex1   = g_armPucParam.FapiPucchPduInfo[pucchpduIndex1].StartSymbolIndex;
        tdOccIdx1           = g_armPucParam.FapiPucchPduInfo[pucchpduIndex1].tdOccIdx;

        pucchpduGroupNum    = g_armPucParam.pucchpduGroupNum;
        pucchueidxInOcc     = g_armPucParam.pucchuserNumPerOcc[pucchpduGroupNum][tdOccIdx1]++;
        g_armPucParam.pucchpduIndexinGroup[pucchpduGroupNum][tdOccIdx1][pucchueidxInOcc] = pucchpduIndex1;
        g_armPucParam.pucchpduNumPerGroup[pucchpduGroupNum]++;

        pucchfmt1pduflag[pucchpduNumCnt1] = 1;
        for(pucchpduNumCnt2 = (pucchpduIndex1 + 1); pucchpduNumCnt2 < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_1]; pucchpduNumCnt2++)
        {
            pucchpduIndex2      = g_armPucParam.pucchfmtpduIdxInner[PUCCH_FORMAT_1][pucchpduNumCnt2];
            prbStart2           = g_armPucParam.FapiPucchPduInfo[pucchpduIndex2].prbStart;
            StartSymbolIndex2   = g_armPucParam.FapiPucchPduInfo[pucchpduIndex2].StartSymbolIndex;
            tdOccIdx2           = g_armPucParam.FapiPucchPduInfo[pucchpduIndex2].tdOccIdx;
            if((0 == pucchfmt1pduflag[pucchpduNumCnt2]) && (prbStart1 == prbStart2) && (StartSymbolIndex1 == StartSymbolIndex2))
            {
                g_armPucParam.pucchpduNumPerGroup[pucchpduGroupNum]++;
                pucchueidxInOcc = g_armPucParam.pucchuserNumPerOcc[pucchpduGroupNum][tdOccIdx2]++;
                g_armPucParam.pucchpduIndexinGroup[pucchpduGroupNum][tdOccIdx2][pucchueidxInOcc] = pucchpduIndex2;
                pucchfmt1pduflag[pucchpduNumCnt2] = 1;
            }
        }
        g_armPucParam.pucchpduGroupNum++;
    }
}

void UlTtiRequestPucchPduparse(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucParam *pucParam, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex)
{
    uint8_t pduNumCnt;
    uint8_t pucchpduGroupCnt;

    if(0 < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_2])
    {
        for(pduNumCnt = 0; pduNumCnt < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_2]; pduNumCnt++)
        {
            pucParam = (g_PucchPara[cellIndex].pucParam + g_armPucParam.pucchNum);
            pduIndex = g_armPucParam.pucchfmtpduIdxInner[PUCCH_FORMAT_2][pduNumCnt];
            UlTtiRequestPucchFmt023Pduparse(fapipucchpduInfo, pucParam, sfnNum, slotNum, pduIndex, cellIndex); 
            g_armPucParam.pucchNum++;
        }
    }

    if(0 < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_3])
    {
        for(pduNumCnt = 0; pduNumCnt < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_3]; pduNumCnt++)
        {
            pucParam = (g_PucchPara[cellIndex].pucParam + g_armPucParam.pucchNum);
            pduIndex = g_armPucParam.pucchfmtpduIdxInner[PUCCH_FORMAT_3][pduNumCnt];
            UlTtiRequestPucchFmt023Pduparse(fapipucchpduInfo, pucParam, sfnNum, slotNum, pduIndex, cellIndex); 
            g_armPucParam.pucchNum++; 
        }
    }

    if(0 < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_0])
    {
        for(pduNumCnt = 0; pduNumCnt < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_0]; pduNumCnt++)
        {
            pucParam = (g_PucchPara[cellIndex].pucParam + g_armPucParam.pucchNum);
            pduIndex = g_armPucParam.pucchfmtpduIdxInner[PUCCH_FORMAT_0][pduNumCnt];
            UlTtiRequestPucchFmt023Pduparse(fapipucchpduInfo, pucParam, sfnNum, slotNum, pduIndex, cellIndex); 
            g_armPucParam.pucchNum++;
        }
    }

    /* pucch fmt1,将复用的PDU先分组，再解析*/
    if(0 < g_armPucParam.pucchfmtpdunum[PUCCH_FORMAT_1])
    {
        PucchFmt1Grouping();
        for(pucchpduGroupCnt = 0; pucchpduGroupCnt < g_armPucParam.pucchpduGroupNum; pucchpduGroupCnt++)
        {
            pucParam = (g_PucchPara[cellIndex].pucParam + g_armPucParam.pucchNum);
            UlTtiRequestPucchFmt1Pduparse(pucParam, pucchpduGroupCnt, sfnNum, slotNum, cellIndex);
            g_armPucParam.pucchNum++;
        }
    }
}

uint16_t CalcPucchCsipart2Size(FapiNrMsgPucchPduInfo *fapipucchpduInfo, uint8_t *CsiPart1Payload, uint8_t *sizesPart1Params, uint16_t *map ,uint8_t numPart1Params)
{
    Part2ReportInfo *part2ReportInfo = NULL;
    uint16_t csiPart1BitLength;
    uint16_t numPart2s;
    uint8_t  Part1ParamsIdx;
    uint16_t paramOffsets;
    uint8_t  paramSizes;
    uint16_t ParamValue;
    uint8_t  bitNum;
    uint16_t mapIndex;
     
    csiPart1BitLength = fapipucchpduInfo->csiPart1BitLength;

    numPart2s         = 1;//fapipucchpduInfo->uciInfoAddInV3.numPart2s;暂时只支持1个CSI report
    part2ReportInfo   = fapipucchpduInfo->uciInfoAddInV3.part2ReportInfo;
    if(numPart1Params != part2ReportInfo->numPart1Params)
    {
        return 0;
    }

    mapIndex = 0;
    for(Part1ParamsIdx = 0; Part1ParamsIdx < numPart1Params; Part1ParamsIdx++)
    {
        paramOffsets = part2ReportInfo->paramOffsets[Part1ParamsIdx];
        paramSizes   = part2ReportInfo->paramSizes[Part1ParamsIdx];  
        ParamValue   = InterceptData(CsiPart1Payload, paramOffsets, paramSizes);//CsiPart1Payload,paramOffsets,paramSizes?从CsiPart1Payload截取值
        bitNum       = sizesPart1Params[Part1ParamsIdx];
        ParamValue   = ParamValue&genbitmask(bitNum);//二次截取
        mapIndex     = (mapIndex<<bitNum) + ParamValue;
    }

    return map[mapIndex];
}