#include <stdio.h>
#include <string.h>
#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../../../../common/inc/phy_ctrl_common.h"
#include "../inc/phyctrl_pucch.h"
#include "../inc/pucch_variable.h"
#include "../../../../common/src/common.c"

void PucchParaInit(uint8_t cellIndex);
void PucchNcsandUVCalc(uint8_t SlotIdx, uint16_t PucchHoppingId,uint8_t GroupHopping);

void UlTtiRequestPucchFmt023Pduparse(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucParam *pucParam, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex);
void UlTtiRequestPucchFmt1Pduparse(PucParam *pucParam, uint8_t pucchpduGroupCnt, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex);
void PucchFmt1Grouping(uint8_t cellIndex);

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

void PucchParaInit(uint8_t cellIndex)
{
    g_pucchfmt1pdunum   =  0;
    g_pucchfmt023pdunum =  0; 
    g_pucchpduGroupNum  =  0;

    memset(g_FapiPucchfmt1PduInfo[cellIndex],       0, MAX_PUCCH_NUM * sizeof(FapiNrMsgPucchPduInfo));
    memset(g_pucchNumpersym  ,       0, MAX_PUCCH_NUM);
    memset(g_pucchIndex      ,       0, (SYM_NUM_PER_SLOT * MAX_PUCCH_NUM));
    memset(g_pucchpduNumPerGroup,    0, MAX_PUCCH_NUM);
    memset(g_pucchpduIndexinGroup,   0, (MAX_PUCCH_NUM * MAX_USER_NUM_PER_OCC));
}

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
    uint8_t  SymbIdx = 0;
    uint8_t  NcsTemp = 0;
    
    FssPucch = PucchHoppingId % 30;
    NIDdiv30 = PucchHoppingId / 30;

    Cinit = NIDdiv30;
    SequenceLen = 8 * (SlotIdx + 1) * 2;
    PseudoRandomSeqGen((uint8_t*)g_NghData, Cinit, SequenceLen);

    Cinit = (NIDdiv30 << 5) + FssPucch;
    SequenceLen = (SlotIdx + 1) * 2;
    PseudoRandomSeqGen((uint8_t*)g_NvData, Cinit, SequenceLen);

    Cinit = PucchHoppingId;
    SequenceLen = 8 * 14 * (SlotIdx + 1);
    PseudoRandomSeqGen((uint8_t*)g_NcsData, Cinit, SequenceLen);

    FghPucch = 0;
    if ((1 == GroupHopping))   /*group hopping is enable */
    {
        /*calculate first hop u,v*/
        SlotIdx1 = (8 * 2 * SlotIdx) >> 5;
        SlotBits = (8 * 2 * SlotIdx) & 0x1F;  /* %32 */
        TempData = do_brev(g_NghData[SlotIdx1]);   /* 位反转 */
        FghPucch = (_extu(TempData, (24 - SlotBits), 24)) % 30;
        g_NuValue[SlotIdx][0] = (FssPucch + FghPucch) % 30;
        g_NvValue[SlotIdx][0] = 0;

        /*calculate second hop u,v*/
        SlotIdx1 = (8 * (2 * SlotIdx + 1)) >> 5;
        SlotBits = (8 * (2 * SlotIdx + 1)) & 0x1F;  /* %32 */
        TempData = do_brev(g_NghData[SlotIdx1]);   /* 位反转 */
        FghPucch = (_extu(TempData, (24 - SlotBits), 24)) % 30;
        g_NuValue[SlotIdx][1] = (FssPucch + FghPucch) % 30;
        g_NvValue[SlotIdx][1] = 0;
    }
    else if (2 == GroupHopping) /*group hopping is disabled,sequence hopping is enable */
    {
        /*calculate first hop u,v*/
        SlotIdx1 = (2 * SlotIdx) >> 5;
        SlotBits = (2 * SlotIdx) & 0x1F;  /* %32 */
        TempData = g_NvData[SlotIdx1];
        g_NuValue[SlotIdx][0] = (FssPucch + FghPucch) % 30;
        g_NvValue[SlotIdx][0] = (TempData >> (31 - SlotBits)) & 0x1;

        /*calculate second hop u,v*/
        SlotIdx1 = (2 * SlotIdx + 1) >> 5;// Ts 38.211 6.3.2.2.1  nhop
        SlotBits = (2 * SlotIdx + 1) & 0x1F;  /* %32 */
        TempData = g_NvData[SlotIdx1];
        g_NuValue[SlotIdx][1] = (FssPucch + FghPucch) % 30;
        g_NvValue[SlotIdx][1] = (TempData >> (31 - SlotBits)) & 0x1;
    }
    else /*group hopping is disabled,sequence hopping is disabled, */
    {
        /*calculate first hop u,v*/
        g_NuValue[SlotIdx][0] = (FssPucch + FghPucch) % 30;
        g_NvValue[SlotIdx][0] = 0;

        /*calculate second hop u,v*/
        g_NuValue[SlotIdx][1] = (FssPucch + FghPucch) % 30;
        g_NvValue[SlotIdx][1] = 0;
    }

    for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
    {
        TempData = 8 * SYM_NUM_PER_SLOT * SlotIdx + 8 * SymbIdx;
        SlotIdx1 = TempData >> 5;
        SlotBits = TempData & 0x1F; /* %32 */
        TempData = do_brev(g_NcsData[SlotIdx1]);
        NcsTemp = _extu(TempData, (24 - SlotBits), 24);
        g_NcsValue[SlotIdx * SYM_NUM_PER_SLOT + SymbIdx] = NcsTemp;
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
    uint32_t Cinit = 0;
    uint32_t SequenceLen = 0;
    PucFmt0Param *fmt0Param = NULL;
    PucFmt2Param *fmt2Param = NULL;
    PucFmt3Param *fmt3Param = NULL;

    formatType      = fapipucchpduInfo->formatType;
    EndSymbolIndex  = fapipucchpduInfo->StartSymbolIndex + fapipucchpduInfo->numSymbols;
    pucchindex      = g_pucchfmt023pdunum;
    pucchNumpersym  = g_pucchNumpersym[EndSymbolIndex]++;
    g_pucchIndex[EndSymbolIndex][pucchNumpersym] = pucchindex;

    groupOrSequenceHopping = fapipucchpduInfo->groupOrSequenceHopping;
    intraSlotFreqHopping   = fapipucchpduInfo->intraSlotFreqHopping;
    /* 所在小区的小区级参数 */
    pucParam->cellId    = g_cellConfigPara[cellIndex].cellIndex;
    pucParam->sfn       = sfnNum;
    pucParam->slot      = slotNum;
    pucParam->pduIndex  = pduIndex;
    pucParam->rxAntNum  = g_cellConfigPara[cellIndex].rxAntNum;
    pucParam->BW        = g_cellConfigPara[cellIndex].bandWidthUl;
    pucParam->scs       = fapipucchpduInfo->subcarrierSpacing;
    pucParam->pucFormat = fapipucchpduInfo->formatType;

    /* BWP parameter */
    pucParam->bwpStart = fapipucchpduInfo->bwpStart;
    pucParam->bwpSize  = fapipucchpduInfo->bwpSize;

    /* frequency domain */
    pucParam->prbStart              = fapipucchpduInfo->prbStart;
    pucParam->prbSize               = fapipucchpduInfo->prbSize;
    pucParam->intraSlotFreqHopping  = fapipucchpduInfo->intraSlotFreqHopping;
    pucParam->secondHopPrb          = fapipucchpduInfo->secondHopPRB;
    //pucParam->secondHopSymIdx     = ;////

    /* time domain */
    pucParam->startSymIdx = fapipucchpduInfo->StartSymbolIndex;
    pucParam->symNum = fapipucchpduInfo->numSymbols;

     /* PUC data在DDR中的存放地址 */
    //int32 *dataAddr[SYM_NUM_PER_SLOT][MAX_RX_ANT_NUM];  
    
    /* PUC DAGC因子在DDR中的存放地址 */
    //int16 *dagcAddr[SYM_NUM_PER_SLOT][MAX_RX_ANT_NUM];  

    /* sequence address */
    //int32 *baseSeqAddr[HOP_NUM];  			/* ZC基序列或PN序列在DDR中的存放地址，fmt0/1/3使用ZC序列，fmt2使用PN序列*/
    
    if(PUCCH_FORMAT_0 == formatType)
    {
        pucParam->dmrsSymNum[0] = 0;
        pucParam->dmrsSymNum[1] = 0;
        pucParam->uciSymNum[0]  = (0 == intraSlotFreqHopping) ? (pucParam->symNum):1;
        pucParam->uciSymNum[1]  = pucParam->symNum - pucParam->uciSymNum[0];

        fmt0Param = (PucFmt0Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
        fmt0Param->srbitlen = fapipucchpduInfo->srFlag;
        fmt0Param->harqBitLength = fapipucchpduInfo->bitLenHarq;
        fmt0Param->deltaOffset = 0;
        fmt0Param->noiseTapNum = 6;

        PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
        for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
        {
            fmt0Param->cyclicShift[SymbIdx] = (fapipucchpduInfo->initCyclicShift + g_NcsValue[slotNum * SYM_NUM_PER_SLOT + SymbIdx]) % SC_NUM_PER_RB;
        }

        fmt0Param->rnti = fapipucchpduInfo->ueRnti;
        //fmt0Param->threshold = ;////算法参数，待定
    }
    else if(PUCCH_FORMAT_2 == formatType)
    {
        pucParam->uciSymNum[0]  = (0 == intraSlotFreqHopping) ? (pucParam->symNum):1;
        pucParam->uciSymNum[1]  = pucParam->symNum - pucParam->uciSymNum[0];
        pucParam->dmrsSymNum[0] = pucParam->uciSymNum[0];
        pucParam->dmrsSymNum[1] = pucParam->uciSymNum[1];

        Cinit 		= ((fapipucchpduInfo->ueRnti)<<15)  + fapipucchpduInfo->nIdPucchHopping;
        SequenceLen = 16*(fapipucchpduInfo->numSymbols)*((fapipucchpduInfo->prbSize));
        PseudoRandomSeqGen((uint8_t*)g_fmt23dataScrambuff, Cinit, SequenceLen);

        SequenceLen = ((fapipucchpduInfo->prbStart) * SC_NUM_PER_RB * 2 * 1/3);
	    Cinit 		= ((((SYM_NUM_PER_SLOT * slotNum + fapipucchpduInfo->StartSymbolIndex + 1)*(2 * fapipucchpduInfo->dmrsScramblingId + 1))<<17)  + (2 * fapipucchpduInfo->dmrsScramblingId))&0x7fffffff;//超过32位
        PseudoRandomSeqGen((uint8_t*)g_fmt2pilotScrambuff[0], Cinit, SequenceLen);
	    Cinit 		= ((((SYM_NUM_PER_SLOT * slotNum + fapipucchpduInfo->StartSymbolIndex + 2)*(2 * fapipucchpduInfo->dmrsScramblingId + 1))<<17)  + (2 * fapipucchpduInfo->dmrsScramblingId))&0x7fffffff;;//超过32位
	    PseudoRandomSeqGen((uint8_t*)g_fmt2pilotScrambuff[1], Cinit, SequenceLen);
        
        fmt2Param = (PucFmt2Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
        fmt2Param->MrcIrcFlag = 0;
        fmt2Param->srbitlen = fapipucchpduInfo->srFlag;
        fmt2Param->rnti = fapipucchpduInfo->ueRnti;
        fmt2Param->harqBitLength = fapipucchpduInfo->bitLenHarq;
        fmt2Param->csipart1BitLength = fapipucchpduInfo->csiPart1BitLength;
        fmt2Param->nid  = fapipucchpduInfo->nIdPucchScrambling;
        fmt2Param->nid0 = fapipucchpduInfo->dmrsScramblingId;
        //fmt2Param->beta = ;////算法参数，待定
        fmt2Param->segNum = 4;
        //fmt2Param->threshold = ;////算法参数，待定(根据UCI比特数，确定是基于RM译码还是基于SINR的门限)
        
    }
    else if(PUCCH_FORMAT_3 == formatType)
    {
        fmt3Param = (PucFmt3Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
        fmt3Param->MrcIrcFlag = 0;
        fmt3Param->srbitlen = fapipucchpduInfo->srFlag;
        fmt3Param->pi2bpsk = fapipucchpduInfo->pi2BpskFlag;
        fmt3Param->adddmrsflag = fapipucchpduInfo->addDmrsFlag;
        fmt3Param->harqBitLength = fapipucchpduInfo->bitLenHarq;
        fmt3Param->csipart1BitLength = fapipucchpduInfo->csiPart1BitLength;
        fmt3Param->nid = fapipucchpduInfo->nIdPucchScrambling;

        PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
        for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
        {
            fmt3Param->cyclicShift[SymbIdx] = (fapipucchpduInfo->initCyclicShift + g_NcsValue[slotNum * SYM_NUM_PER_SLOT + SymbIdx]) % SC_NUM_PER_RB;
        }

        Cinit = ((fapipucchpduInfo->ueRnti)<<15)  + fapipucchpduInfo->nIdPucchHopping;
        SequenceLen = 12*(2-fapipucchpduInfo->pi2BpskFlag)*(fapipucchpduInfo->numSymbols)*((fapipucchpduInfo->prbSize));;
        PseudoRandomSeqGen((uint8_t*)g_fmt23dataScrambuff, Cinit, SequenceLen);

        fmt3Param->rnti = fapipucchpduInfo->ueRnti;
        //fmt3Param->beta = ;////算法参数，待定
        fmt3Param->segNum = 4;
        //fmt3Param->threshold = ;////算法参数，待定(根据UCI比特数，确定是基于RM译码还是基于SINR的门限)

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
            pucParam->dmrsSymNum[0] = (0 == intraSlotFreqHopping) ? (2*(1 + fmt3Param->adddmrsflag)) : (1 + fmt3Param->adddmrsflag);
            pucParam->dmrsSymNum[1] = (0 == intraSlotFreqHopping) ? 0 : (1 + fmt3Param->adddmrsflag);
        }
        pucParam->uciSymNum[0] = symNum[0] - pucParam->dmrsSymNum[0];
        pucParam->uciSymNum[1] = symNum[1] - pucParam->dmrsSymNum[1];
    }
}

void UlTtiRequestPucchFmt1Pduparse(PucParam *pucParam, uint8_t pucchpduGroupCnt, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex)
{
    uint8_t  EndSymbolIndex;
    uint8_t  pucchNumpersym;
    uint8_t  pucchindex;
    uint8_t  pucchpduIndex;
    uint8_t  intraSlotFreqHopping;
    uint8_t  symNum[HOP_NUM];
    uint8_t  SlotIdx;//待用fapi接口替换
    uint8_t  SymbIdx;
    FapiNrMsgPucchPduInfo *fapipucchpduInfo = NULL;
    PucFmt1Param          *fmt1Param        = NULL;
    PucFmt1UEParam        *fmt1UEParam      = NULL;                     

    pucchpduIndex    = g_pucchpduIndexinGroup[pucchpduGroupCnt][0];
    fapipucchpduInfo = &g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex]; 

    EndSymbolIndex   = fapipucchpduInfo->StartSymbolIndex + fapipucchpduInfo->numSymbols;
    pucchindex       = g_pucchfmt023pdunum + pucchpduGroupCnt;
    
    pucchNumpersym   = g_pucchNumpersym[EndSymbolIndex]++;
    g_pucchIndex[EndSymbolIndex][pucchNumpersym] = pucchindex;

    intraSlotFreqHopping = fapipucchpduInfo->intraSlotFreqHopping;

    /* 所在小区的小区级参数 */
    pucParam->cellId    = g_cellConfigPara[cellIndex].cellIndex;
    pucParam->sfn       = sfnNum;
    pucParam->slot      = slotNum;
    pucParam->pduIndex  = pduIndex;
    pucParam->rxAntNum  = g_cellConfigPara[cellIndex].rxAntNum;
    pucParam->BW        = g_cellConfigPara[cellIndex].bandWidthUl;
    pucParam->scs       = fapipucchpduInfo->subcarrierSpacing;
    pucParam->pucFormat = fapipucchpduInfo->formatType;

    /* BWP parameter */
    pucParam->bwpStart              = fapipucchpduInfo->bwpStart;
    pucParam->bwpSize               = fapipucchpduInfo->bwpSize;

    /* frequency domain */
    pucParam->prbStart              = fapipucchpduInfo->prbStart;
    pucParam->prbSize               = fapipucchpduInfo->prbSize;
    pucParam->intraSlotFreqHopping  = fapipucchpduInfo->intraSlotFreqHopping;
    pucParam->secondHopPrb          = fapipucchpduInfo->secondHopPRB;
    //pucParam->secondHopSymIdx     = ;////

    /* time domain */
    pucParam->startSymIdx           = fapipucchpduInfo->StartSymbolIndex;
    pucParam->symNum                = fapipucchpduInfo->numSymbols;

    symNum[0]                       = (0 == intraSlotFreqHopping) ? (pucParam->symNum) : ((pucParam->symNum)>>1);
    symNum[1]                       = pucParam->symNum - symNum[0];
    pucParam->uciSymNum[0]          = (0 == intraSlotFreqHopping) ? ((pucParam->symNum)>>1) : ((pucParam->symNum)>>2);
    pucParam->uciSymNum[1]          = (0 == intraSlotFreqHopping) ? 0 : ((pucParam->symNum + 2)>>2);
    pucParam->dmrsSymNum[0]         = symNum[0] - pucParam->uciSymNum[0];
    pucParam->dmrsSymNum[1]         = symNum[1] - pucParam->uciSymNum[1];

     /* PUC data在DDR中的存放地址 */
    //int32 *dataAddr[SYM_NUM_PER_SLOT][MAX_RX_ANT_NUM];  
    
    /* PUC DAGC因子在DDR中的存放地址 */
    //int16 *dagcAddr[SYM_NUM_PER_SLOT][MAX_RX_ANT_NUM];  

    /* sequence address */
    //int32 *baseSeqAddr[HOP_NUM];  			/* ZC基序列或PN序列在DDR中的存放地址，fmt0/1/3使用ZC序列，fmt2使用PN序列*/

    /* fmt1 UE common */
    fmt1Param                       =  (PucFmt1Param *)((uint8_t *)pucParam  + sizeof(PucParam) - sizeof(PucFmt1Param));
    fmt1Param->timeDomainOccIdx     = fapipucchpduInfo->tdOccIdx;
    fmt1Param->userNumPerOcc        = g_pucchpduNumPerGroup[pucchpduGroupCnt];
    //fmt1Param->noiseTapNum       = 6;////算法参数
    //fmt1Param->threshold         = ;////算法参数,待定

    /* fmt1 UE*/
    for(pucchpduIndex = 0; pucchpduIndex < g_pucchpduNumPerGroup[pucchpduGroupCnt]; pucchpduIndex++)
    {
        fapipucchpduInfo             = &g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex];
        fmt1UEParam                  = &fmt1Param->fmt1UEParam[pucchpduIndex];
        fmt1UEParam->srbitlen        = fapipucchpduInfo->srFlag;
        fmt1UEParam->harqBitLength   = fapipucchpduInfo->bitLenHarq;
        
        PucchNcsandUVCalc(SlotIdx,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
        for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
        {
            fmt1UEParam->cyclicShift[SymbIdx] = (fapipucchpduInfo->initCyclicShift + g_NcsValue[SlotIdx * SYM_NUM_PER_SLOT + SymbIdx]) % SC_NUM_PER_RB;
        }

        fmt1UEParam->rnti  = fapipucchpduInfo->ueRnti;
    }
}

void PucchFmt1Grouping(uint8_t cellIndex)
{
    uint8_t  pucchfmt1pduflag[MAX_PUCCH_NUM] = {0};
    uint16_t prbStart1,prbStart2;
    uint8_t  StartSymbolIndex1,StartSymbolIndex2;
    uint8_t  tdOccIdx1,tdOccIdx2;
    uint8_t  pucchpduIndex1,pucchpduIndex2;
    uint8_t  pucchpduNumcnt;
    uint8_t  pucchpduGroupNum;
    uint8_t  pucchpduGroupCnt;
    uint8_t  groupIndex;
    uint8_t  ueIndex; 
    uint8_t  ueNumPerGroup;

    for(pucchpduIndex1 = 0; pucchpduIndex1 < g_pucchfmt1pdunum; pucchpduIndex1++)
    {
        if(1 == pucchfmt1pduflag[pucchpduIndex1])
        {
            continue;
        }
        pucchpduGroupNum = g_pucchpduGroupNum;
        pucchpduNumcnt   = g_pucchpduNumPerGroup[g_pucchpduGroupNum]++;
        g_pucchpduIndexinGroup[pucchpduGroupNum][pucchpduNumcnt] = pucchpduIndex1;
        pucchfmt1pduflag[pucchpduIndex1] = 1;
        for(pucchpduIndex2 = (pucchpduIndex1 + 1); pucchpduIndex2 < g_pucchfmt1pdunum; pucchpduIndex2++)
        {
            prbStart1           = g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex1].prbStart;
            prbStart2           = g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex2].prbStart;
            StartSymbolIndex1   = g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex1].StartSymbolIndex;
            StartSymbolIndex2   = g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex2].StartSymbolIndex;
            tdOccIdx1           = g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex1].tdOccIdx;
            tdOccIdx2           = g_FapiPucchfmt1PduInfo[cellIndex][pucchpduIndex2].tdOccIdx;
            if((0 == pucchfmt1pduflag[pucchpduIndex2]) 
                &&(prbStart1 == prbStart2) && (StartSymbolIndex1 == StartSymbolIndex2) && (tdOccIdx1 == tdOccIdx2))
            {
                pucchpduNumcnt = g_pucchpduNumPerGroup[g_pucchpduGroupNum]++;
                g_pucchpduIndexinGroup[pucchpduGroupNum][pucchpduNumcnt] = pucchpduIndex2;
                pucchfmt1pduflag[pucchpduIndex2] = 1;
            }
        }
        g_pucchpduGroupNum++;
    }
}