#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../../../../common/inc/phy_ctrl_common.h"
#include "../inc/phyctrl_pucch.h"
#include "../inc/pucch_variable.h"
#include "../../../../common/src/common.c"

void PucchNcsandUVCalc(uint8_t SlotIdx, uint16_t PucchHoppingId,uint8_t GroupHopping);

void PucchFmt1Grouping(ArmPucParam *armPucParam);
void UlTtiRequestPucchPduparse(ArmPucParam *armPucParam, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex);

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

void PucchFmt01RptPreFill(FapiNrMsgPucchPduInfo *fapipucchpduInfo, FapiNrPucchFmt01Indication *fapiNrPucchFmt01Indication)
{
    uint8_t isSrExistFlag;
    uint8_t isHarqExistFlag;

    isSrExistFlag   = fapipucchpduInfo->srFlag;
    isHarqExistFlag = (0 == fapipucchpduInfo->bitLenHarq) ? 0:1;

    fapiNrPucchFmt01Indication->Handle      = fapipucchpduInfo->handle;
    fapiNrPucchFmt01Indication->pduBitmap   = (isHarqExistFlag<<1) + isSrExistFlag;
    fapiNrPucchFmt01Indication->PucchFormat = fapipucchpduInfo->formatType;
    fapiNrPucchFmt01Indication->RNTI        = fapipucchpduInfo->ueRnti;
    fapiNrPucchFmt01Indication->TA          = 0xffff;
    fapiNrPucchFmt01Indication->RSSI        = 0xffff;
    fapiNrPucchFmt01Indication->harqInfoFmt01.NumHarq = (fapipucchpduInfo->bitLenHarq);
}

void PucchFmt23RptPreFill(FapiNrMsgPucchPduInfo *fapipucchpduInfo, FapiNrPucchFmt23Indication *fapiNrPucchFmt23Indication)
{
    uint8_t isSrExistFlag;
    uint8_t isHarqExistFlag;
    uint8_t isCsiPart1ExistFlag;
    uint8_t isCsiPart2ExistFlag;

    isSrExistFlag       = (0 == fapipucchpduInfo->srFlag) ? 0:1;
    isHarqExistFlag     = (0 == fapipucchpduInfo->bitLenHarq) ? 0:1;
    isCsiPart1ExistFlag = (0 == fapipucchpduInfo->csiPart1BitLength) ? 0:1;
    isCsiPart2ExistFlag = (0 == fapipucchpduInfo->uciInfoAddInV3.part2sNum) ? 0:1;
    
    fapiNrPucchFmt23Indication->Handle                      = fapipucchpduInfo->handle;
    fapiNrPucchFmt23Indication->pduBitmap                   = (isCsiPart2ExistFlag<<3) + (isCsiPart1ExistFlag<<2) + (isHarqExistFlag<<1) + isSrExistFlag;
    fapiNrPucchFmt23Indication->RNTI                        = fapipucchpduInfo->ueRnti;
    fapiNrPucchFmt23Indication->PucchFormat                 = fapipucchpduInfo->formatType;
    fapiNrPucchFmt23Indication->TA                          = 0xffff;
    fapiNrPucchFmt23Indication->RSSI                        = 0xffff;
    fapiNrPucchFmt23Indication->srInfoFmt23.SrBitLen        = fapipucchpduInfo->srFlag;
    fapiNrPucchFmt23Indication->harqInfoFmt23.HarqBitLen    = fapipucchpduInfo->bitLenHarq;
    fapiNrPucchFmt23Indication->csipart1Info.CsiPart1BitLen = fapipucchpduInfo->csiPart1BitLength;
}

void PucchFmt0PduParse(PucParam *pucParam, FapiNrMsgPucchPduInfo *fapipucchpduInfo, uint8_t intraSlotFreqHopping, uint8_t groupOrSequenceHopping, uint16_t pduIndex, uint8_t slotNum)
{
    uint8_t SymbIdx;
    uint8_t srFlag;
    uint8_t bitLenHarq;
    PucFmt0Param *fmt0Param = NULL;

    srFlag     = fapipucchpduInfo->srFlag;
    bitLenHarq = fapipucchpduInfo->bitLenHarq;
    pucParam->hopParam[0].startSymIdx = fapipucchpduInfo->StartSymbolIndex;
    pucParam->hopParam[0].symNum      = (0 == intraSlotFreqHopping) ? (fapipucchpduInfo->numSymbols):1;
    pucParam->hopParam[0].dmrsSymNum  = 0;
    pucParam->hopParam[0].uciSymNum   = pucParam->hopParam[0].symNum;
    pucParam->hopParam[1].startSymIdx = (fapipucchpduInfo->StartSymbolIndex) + (pucParam->hopParam[0].symNum);
    pucParam->hopParam[1].symNum      = (fapipucchpduInfo->numSymbols) - (pucParam->hopParam[0].symNum);
    pucParam->hopParam[1].dmrsSymNum  = 0;
    pucParam->hopParam[1].uciSymNum   = pucParam->hopParam[1].symNum;

    fmt0Param = (PucFmt0Param *)(&(pucParam->formatxParam));
    fmt0Param->pduIdxInner = pduIndex;
    if(srFlag)//暂时不考虑抽头，即offset取值为0
    {
         if(0 == bitLenHarq)
        {
            fmt0Param->mcsBitMap = 0x1;//0
        }
        else if(1 == bitLenHarq)
        {
            fmt0Param->mcsBitMap = 0x249;//0,3,6,9
        }
        else if(2 == bitLenHarq)
        {
            fmt0Param->mcsBitMap = 0x6db;//0,1,3,4,6,7,9,10
        }
    }
    else
    {
        if(1 == bitLenHarq)
        {
            fmt0Param->mcsBitMap = 0x41;//0,6
        }
        else if(2 == bitLenHarq)
        {
            fmt0Param->mcsBitMap = 0x249;//0,3,6,9
        }
    }

    PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,fapipucchpduInfo->groupOrSequenceHopping);
    for (SymbIdx = 0; SymbIdx < SYM_NUM_PER_SLOT; SymbIdx++)
    {
        fmt0Param->cyclicShift[SymbIdx] = (fapipucchpduInfo->initCyclicShift + g_NcsValue[SymbIdx]) % SC_NUM_PER_RB;
    }

}

void PucchRMDecodeHacCfg(FapiNrMsgPucchPduInfo *fapipucchpduInfo, uint16_t uciLen ,uint8_t PduIdxInner, uint8_t msgType, uint32_t uciPart1llrNum, uint16_t sfnNum, uint8_t slotNum, uint8_t cellIndex)
{
    uint8_t  pduNum;
    uint8_t  totBit;
    uint8_t  part1Bit;
    uint8_t  maxCodeRate;
    uint8_t  valQm;
    HacCfgHead      *hacHead;
    RMDecodePduInfo *rmInfo;
    LlrSegInfo      *llrSegInfo;

    hacHead = &(g_pucchRMDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].hacCfgHead);
    pduNum  = hacHead->pduNum;
    if(0 == pduNum)
    {
        hacHead->sfn     = sfnNum;
        hacHead->slot    = slotNum;
        hacHead->cellIdx = cellIndex;
        hacHead->msgType = msgType;
    }

    rmInfo = &(g_pucchRMDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].rmPduInfo[pduNum]);
    rmInfo->uciBitNum  = uciLen;
    rmInfo->ueIdx      = PduIdxInner;
    rmInfo->codeMethod = 32;
    if(PUCCH_UCI_PART1 == msgType)
    {
        if(PUCCH_FORMAT_2 == (fapipucchpduInfo->formatType))
        {
            rmInfo->llrNum = 16 * (fapipucchpduInfo->numSymbols) * (fapipucchpduInfo->prbSize);
        }
        else if(PUCCH_FORMAT_3 == (fapipucchpduInfo->formatType))
        {
            totBit = 12 * (2 - (fapipucchpduInfo->pi2BpskFlag)) * (fapipucchpduInfo->numSymbols) * (fapipucchpduInfo->prbSize);
            if(!(fapipucchpduInfo->uciInfoAddInV3.part2sNum))
            {
                rmInfo->llrNum = totBit;
            }
            else
            {
                maxCodeRate    = fapipucchpduInfo->pucchParaAddInV3.maxCodeRate;
                valQm          = (2 - fapipucchpduInfo->pi2BpskFlag);
                part1Bit       = ((uint16_t)((totBit - 1) / ((maxCodeRateTab[maxCodeRate]) * valQm)) + 1)*valQm;
                rmInfo->llrNum = (totBit < part1Bit) ? totBit:part1Bit;
            }
        }
    }
    else if (PUCCH_PART2 == msgType)
    {
        totBit         = 12 * (2 - (fapipucchpduInfo->pi2BpskFlag)) * (fapipucchpduInfo->numSymbols) * (fapipucchpduInfo->prbSize);
        rmInfo->llrNum = totBit - uciPart1llrNum;
    }

    rmInfo->llrSegNum = 1;
    llrSegInfo = &rmInfo->llrSegInfo[0];
    //llrSegInfo->segStartAddr  = (PUCCH_UCI_PART1 == msgType) ? 0:0;//待修改,Part1和Part2紧排？并且与DSP解调接口赋值要一致
    llrSegInfo->segCycNum     = 1;
    llrSegInfo->segLlrNum     = rmInfo->llrNum;
    llrSegInfo->segPeriod     = rmInfo->llrNum;

    (hacHead->pduNum)++;
}

void PucchPolarDecodeHacCfg(FapiNrMsgPucchPduInfo *fapipucchpduInfo, uint16_t uciLen ,uint8_t PduIdxInner, uint8_t msgType, uint32_t uciPart1llrNum, uint16_t sfnNum, uint8_t slotNum, uint8_t cellIndex)
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
    HacCfgHead         *hacHead;
    PolarDecodePduInfo *polarInfo;
    LlrSegInfo         *llrSegInfo;

    hacHead = &(g_pucchPolarDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].hacCfgHead);
    pduNum  = hacHead->pduNum;
    if(0 == pduNum)
    {
        hacHead->sfn     = sfnNum;
        hacHead->slot    = slotNum;
        hacHead->cellIdx = cellIndex;
        hacHead->msgType = msgType;
    }

    polarInfo = &(g_pucchPolarDecodeHacCfgParaDDR[cellIndex][slotNum][msgType].polarPduInfo[pduNum]);
    if((12 <= uciLen) && (19 >= uciLen))
    {
        K = uciLen + 6;
    }
    else
    {
        K = uciLen + 11;
    }
    
    polarInfo->uciBitNum = uciLen;
    polarInfo->ueIdx     = PduIdxInner;
    if(PUCCH_UCI_PART1 == msgType)
    {
        if(PUCCH_FORMAT_2 == (fapipucchpduInfo->formatType))
        {
            polarInfo->llrNum = 16 * (fapipucchpduInfo->numSymbols) * (fapipucchpduInfo->prbSize);
        }
        else if(PUCCH_FORMAT_3 == (fapipucchpduInfo->formatType))
        {
            totBit = 12 * (2 - (fapipucchpduInfo->pi2BpskFlag)) * (fapipucchpduInfo->numSymbols) * (fapipucchpduInfo->prbSize);
            if(!(fapipucchpduInfo->uciInfoAddInV3.part2sNum))
            {
                polarInfo->llrNum = totBit;
            }
            else
            {
                maxCodeRate         = fapipucchpduInfo->pucchParaAddInV3.maxCodeRate;
                valQm               = (2 - fapipucchpduInfo->pi2BpskFlag);
                part1Bit            = ((uint16_t)((totBit - 1) / ((maxCodeRateTab[maxCodeRate]) * valQm)) + 1)*valQm;
                polarInfo->llrNum   = (totBit < part1Bit) ? totBit:part1Bit;
            }
        }
    }
    else if (PUCCH_PART2 == msgType)
    {
        totBit            = 12 * (2 - (fapipucchpduInfo->pi2BpskFlag)) * (fapipucchpduInfo->numSymbols) * (fapipucchpduInfo->prbSize);
        polarInfo->llrNum = totBit - uciPart1llrNum;
    }

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

    llrSegInfo = &(polarInfo->llrSegInfo[0]);
    //llrSegInfo->segStartAddr  = (PUCCH_UCI_PART1 == msgType) ? 0:0;//待修改,Part1和Part2紧排？并且与DSP解调接口赋值要一致
    llrSegInfo->segCycNum     = 1;
    llrSegInfo->segLlrNum     = polarInfo->llrNum;
    llrSegInfo->segPeriod     = polarInfo->llrNum;

    (hacHead->pduNum)++;
}

void PucchFmt2PduParse(PucParam *pucParam, FapiNrMsgPucchPduInfo *fapipucchpduInfo, uint8_t intraSlotFreqHopping, uint16_t pduIndex, uint16_t sfnNum, uint8_t slotNum, uint8_t cellIndex)
{
		uint8_t  StartSaveIdx;
        uint16_t uciPart1BitLength;
		uint32_t Cinit       = 0;
		uint32_t SequenceLen = 0;
		PucFmt2Param *fmt2Param = NULL;

        pucParam->hopParam[0].startSymIdx = fapipucchpduInfo->StartSymbolIndex;
        pucParam->hopParam[0].symNum      = (0 == intraSlotFreqHopping) ? (fapipucchpduInfo->numSymbols):1;
        pucParam->hopParam[0].dmrsSymNum  = pucParam->hopParam[0].symNum;
        pucParam->hopParam[0].uciSymNum   = pucParam->hopParam[0].symNum;
        pucParam->hopParam[1].startSymIdx = (fapipucchpduInfo->StartSymbolIndex) + (pucParam->hopParam[0].symNum);
        pucParam->hopParam[1].symNum      = (fapipucchpduInfo->numSymbols) - (pucParam->hopParam[0].symNum);
        pucParam->hopParam[1].dmrsSymNum  = pucParam->hopParam[1].symNum;
        pucParam->hopParam[1].uciSymNum   = pucParam->hopParam[1].symNum;

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
        
        uciPart1BitLength = (fapipucchpduInfo->srFlag) + (fapipucchpduInfo->bitLenHarq) + (fapipucchpduInfo->csiPart1BitLength);

        fmt2Param = (PucFmt2Param *)(&(pucParam->formatxParam));
        fmt2Param->pduIdxInner = pduIndex;
        fmt2Param->bitNum      = uciPart1BitLength;
        //fmt2Param->scrambSeqAddr;   /* 加扰序列在DDR中的存放地址,TODO:根据HAC存放确定是否需要2个hop的首地址 */ 
        //fmt2Param->llrAddr;         /* DSP处理完的LLR数据的存放地址，供HAC取数进行译码 */ 

        if((RM_BIT_LENGTH_MIN <= uciPart1BitLength) && (RM_BIT_LENGTH_MAX >= uciPart1BitLength))
        {
            PucchRMDecodeHacCfg(fapipucchpduInfo, uciPart1BitLength, pduIndex, PUCCH_UCI_PART1, 0, sfnNum, slotNum, cellIndex);
        }
        else if((POLAR_BIT_LENGTH_MIN <= uciPart1BitLength) && (POLAR_BIT_LENGTH_MAX >= uciPart1BitLength))
        {
            PucchPolarDecodeHacCfg(fapipucchpduInfo, uciPart1BitLength, pduIndex, PUCCH_UCI_PART1, 0, sfnNum, slotNum, cellIndex);
        }
}

void CalcPart1ReNum(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucFmt3Param *fmt3Param, uint16_t uciLen, uint8_t valQm)
{
    uint8_t  intraSlotFreqHopping;
    uint8_t  maxCodeRate;
    uint8_t  addDmrsFlag;
    uint8_t  uciSetNum = 0;
    uint8_t  numSymbols;
    uint8_t  uciSymNumSet[3]    = {0, 0, 0};
    uint8_t  uciSymIdxSet[3][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
    uint8_t  g_part1ReNum[SYM_NUM_PER_SLOT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t  part1SetNum = 0;
    uint8_t  uciSymNumPart1;
    uint8_t  SymbIdx;
    uint8_t  uciSetIdx;
    uint8_t  uciSymNumCnt;
    uint8_t  valN;
    uint8_t  valM;
    uint8_t  valL;
    uint16_t uciSymNum;
    uint16_t uciBitNum;
    uint16_t totE;
    uint16_t valG1;
    uint16_t valG2;

    intraSlotFreqHopping = fapipucchpduInfo->intraSlotFreqHopping;
    maxCodeRate          = fapipucchpduInfo->pucchParaAddInV3.maxCodeRate;
    addDmrsFlag          = fapipucchpduInfo->addDmrsFlag;

    if(uciLen < 12)
    {
        valL = 0;
    }
    else if(uciLen < 20)
    {
        valL = 6;
    }
    else
    {
        valL = 11;
    }
    numSymbols = fapipucchpduInfo->numSymbols;
    totE  = 12 * valQm * numSymbols *  (fapipucchpduInfo->prbSize);
    valG1 = (uint16_t)((totE + valL - 1) / ((maxCodeRateTab[maxCodeRate]) * valQm)) + 1;
    valG1 = (valG1 < totE) ? valG1:totE;
    valG2 = totE - valG1;

    switch (numSymbols)
    {
        case 4:
            if(!intraSlotFreqHopping)
            {
                uciSetNum = 2;
                uciSymNumSet[0]    = 2;
                uciSymNumSet[1]    = 1;
                uciSymIdxSet[0][0] = 0;
                uciSymIdxSet[0][1] = 2;
                uciSymIdxSet[1][0] = 3;
                g_part1ReNum[1]    = 0xff;
            }
            else
            {
                uciSetNum = 1;
                uciSymNumSet[0]    = 2;
                uciSymIdxSet[0][0] = 1;
                uciSymIdxSet[0][1] = 3;
                g_part1ReNum[0]    = 0xff;
                g_part1ReNum[2]    = 0xff;
            }
            break;
        case 5:
            uciSetNum = 1;
            uciSymNumSet[0]    = 3;
            uciSymIdxSet[0][0] = 1;
            uciSymIdxSet[0][1] = 2;
            uciSymIdxSet[0][2] = 4;
            g_part1ReNum[0]    = 0xff;
            g_part1ReNum[3]    = 0xff;
            break;
        case 6:
            uciSetNum = 1;
            uciSymNumSet[0]    = 4;
            uciSymIdxSet[0][0] = 0;
            uciSymIdxSet[0][1] = 2;
            uciSymIdxSet[0][2] = 3;
            uciSymIdxSet[0][3] = 5;
            g_part1ReNum[1]    = 0xff;
            g_part1ReNum[4]    = 0xff;
            break;
        case 7:
            uciSetNum = 2;
            uciSymNumSet[0]    = 4;
            uciSymNumSet[1]    = 1;
            uciSymIdxSet[0][0] = 0;
            uciSymIdxSet[0][1] = 2;
            uciSymIdxSet[0][2] = 3;
            uciSymIdxSet[0][3] = 5;
            uciSymIdxSet[1][0] = 6;
            g_part1ReNum[1]    = 0xff;
            g_part1ReNum[4]    = 0xff;
            break;
        case 8:
            uciSetNum = 2;
            uciSymNumSet[0]    = 4;
            uciSymNumSet[1]    = 2;
            uciSymIdxSet[0][0] = 0;
            uciSymIdxSet[0][1] = 2;
            uciSymIdxSet[0][2] = 4;
            uciSymIdxSet[0][3] = 6;
            uciSymIdxSet[1][0] = 3;
            uciSymIdxSet[1][1] = 7;
            g_part1ReNum[1]    = 0xff;
            g_part1ReNum[5]    = 0xff;
            break;
        case 9:
            uciSetNum = 2;
            uciSymNumSet[0]    = 4;
            uciSymNumSet[1]    = 3;
            uciSymIdxSet[0][0] = 0;
            uciSymIdxSet[0][1] = 2;
            uciSymIdxSet[0][2] = 5;
            uciSymIdxSet[0][3] = 7;
            uciSymIdxSet[1][0] = 3;
            uciSymIdxSet[1][1] = 4;
            uciSymIdxSet[1][2] = 8;
            g_part1ReNum[1]    = 0xff;
            g_part1ReNum[6]    = 0xff;
            break;
        case 10:
            if(!addDmrsFlag)
            {
                uciSetNum = 2;
                uciSymNumSet[0]    = 4;
                uciSymNumSet[1]    = 4;
                uciSymIdxSet[0][0] = 1;
                uciSymIdxSet[0][1] = 3;
                uciSymIdxSet[0][2] = 6;
                uciSymIdxSet[0][3] = 8;
                uciSymIdxSet[1][0] = 0;
                uciSymIdxSet[1][1] = 4;
                uciSymIdxSet[1][2] = 5;
                uciSymIdxSet[1][3] = 9;
                g_part1ReNum[2]    = 0xff;
                g_part1ReNum[7]    = 0xff;
            }
            else
            {
                uciSetNum = 1;
                uciSymNumSet[0]    = 6;
                uciSymIdxSet[0][0] = 0;
                uciSymIdxSet[0][1] = 2;
                uciSymIdxSet[0][2] = 4;
                uciSymIdxSet[0][3] = 5;
                uciSymIdxSet[0][4] = 7;
                uciSymIdxSet[0][5] = 9;
                g_part1ReNum[1]    = 0xff;
                g_part1ReNum[3]    = 0xff;
                g_part1ReNum[6]    = 0xff;
                g_part1ReNum[8]    = 0xff;
            }
            break;
        case 11:
            if(!addDmrsFlag)
            {
                uciSetNum = 3;
                uciSymNumSet[0]    = 4;
                uciSymNumSet[1]    = 4;
                uciSymNumSet[2]    = 1;
                uciSymIdxSet[0][0] = 1;
                uciSymIdxSet[0][1] = 3;
                uciSymIdxSet[0][2] = 6;
                uciSymIdxSet[0][3] = 8;
                uciSymIdxSet[1][0] = 0;
                uciSymIdxSet[1][1] = 4;
                uciSymIdxSet[1][2] = 5;
                uciSymIdxSet[1][3] = 9;
                uciSymIdxSet[2][0] = 10;
                g_part1ReNum[2]    = 0xff;
                g_part1ReNum[7]    = 0xff;
            }
            else
            {
                uciSetNum = 1;
                uciSymNumSet[0]    = 7;
                uciSymIdxSet[0][0] = 0;
                uciSymIdxSet[0][1] = 2;
                uciSymIdxSet[0][2] = 4;
                uciSymIdxSet[0][3] = 5;
                uciSymIdxSet[0][4] = 7;
                uciSymIdxSet[0][5] = 8;
                uciSymIdxSet[0][6] = 10;
                g_part1ReNum[1]    = 0xff;
                g_part1ReNum[3]    = 0xff;
                g_part1ReNum[6]    = 0xff;
                g_part1ReNum[9]    = 0xff;
            }
            break;
        case 12:
            if(!addDmrsFlag)
            {
                uciSetNum = 3;
                uciSymNumSet[0]    = 4;
                uciSymNumSet[1]    = 4;
                uciSymNumSet[2]    = 2;
                uciSymIdxSet[0][0] = 1;
                uciSymIdxSet[0][1] = 3;
                uciSymIdxSet[0][2] = 7;
                uciSymIdxSet[0][3] = 9;
                uciSymIdxSet[1][0] = 0;
                uciSymIdxSet[1][1] = 4;
                uciSymIdxSet[1][2] = 6;
                uciSymIdxSet[1][3] = 10;
                uciSymIdxSet[2][0] = 5;
                uciSymIdxSet[2][1] = 11;
                g_part1ReNum[2]    = 0xff;
                g_part1ReNum[8]    = 0xff;
            }
            else
            {
                uciSetNum = 1;
                uciSymNumSet[0]    = 8;
                uciSymIdxSet[0][0] = 0;
                uciSymIdxSet[0][1] = 2;
                uciSymIdxSet[0][2] = 3;
                uciSymIdxSet[0][3] = 5;
                uciSymIdxSet[0][4] = 6;
                uciSymIdxSet[0][5] = 8;
                uciSymIdxSet[0][6] = 9;
                uciSymIdxSet[0][7] = 11;
                g_part1ReNum[1]    = 0xff;
                g_part1ReNum[4]    = 0xff;
                g_part1ReNum[7]    = 0xff;
                g_part1ReNum[10]   = 0xff;
            }
            break;
        case 13:
            if(!addDmrsFlag)
            {
                uciSetNum = 3;
                uciSymNumSet[0]    = 4;
                uciSymNumSet[1]    = 4;
                uciSymNumSet[2]    = 3;
                uciSymIdxSet[0][0] = 1;
                uciSymIdxSet[0][1] = 3;
                uciSymIdxSet[0][2] = 8;
                uciSymIdxSet[0][3] = 10;
                uciSymIdxSet[1][0] = 0;
                uciSymIdxSet[1][1] = 4;
                uciSymIdxSet[1][2] = 7;
                uciSymIdxSet[1][3] = 11;
                uciSymIdxSet[2][0] = 5;
                uciSymIdxSet[2][1] = 6;
                uciSymIdxSet[2][2] = 12;
                g_part1ReNum[2]    = 0xff;
                g_part1ReNum[9]    = 0xff;
            }
            else
            {
                uciSetNum = 2;
                uciSymNumSet[0]    = 8;
                uciSymNumSet[1]    = 1;
                uciSymIdxSet[0][0] = 0;
                uciSymIdxSet[0][1] = 2;
                uciSymIdxSet[0][2] = 3;
                uciSymIdxSet[0][3] = 5;
                uciSymIdxSet[0][4] = 6;
                uciSymIdxSet[0][5] = 8;
                uciSymIdxSet[0][6] = 10;
                uciSymIdxSet[0][7] = 12;
                uciSymIdxSet[1][0] = 9;
                g_part1ReNum[1]    = 0xff;
                g_part1ReNum[4]    = 0xff;
                g_part1ReNum[7]    = 0xff;
                g_part1ReNum[11]   = 0xff;
            }
            break;
        case 14:
            if(!addDmrsFlag)
            {
                uciSetNum = 3;
                uciSymNumSet[0]    = 4;
                uciSymNumSet[1]    = 4;
                uciSymNumSet[2]    = 4;
                uciSymIdxSet[0][0] = 2;
                uciSymIdxSet[0][1] = 4;
                uciSymIdxSet[0][2] = 9;
                uciSymIdxSet[0][3] = 11;
                uciSymIdxSet[1][0] = 1;
                uciSymIdxSet[1][1] = 5;
                uciSymIdxSet[1][2] = 8;
                uciSymIdxSet[1][3] = 12;
                uciSymIdxSet[2][0] = 0;
                uciSymIdxSet[2][1] = 6;
                uciSymIdxSet[2][2] = 7;
                uciSymIdxSet[2][3] = 13;
                g_part1ReNum[3]    = 0xff;
                g_part1ReNum[10]   = 0xff;
            }
            else
            {
                uciSetNum = 2;
                uciSymNumSet[0]    = 8;
                uciSymNumSet[1]    = 2;
                uciSymIdxSet[0][0] = 0;
                uciSymIdxSet[0][1] = 2;
                uciSymIdxSet[0][2] = 4;
                uciSymIdxSet[0][3] = 6;
                uciSymIdxSet[0][4] = 7;
                uciSymIdxSet[0][5] = 9;
                uciSymIdxSet[0][6] = 11;
                uciSymIdxSet[0][7] = 13;
                uciSymIdxSet[1][0] = 3;
                uciSymIdxSet[1][1] = 10;
                uciSymIdxSet[1][2] = 8;
                uciSymIdxSet[1][3] = 12;
                g_part1ReNum[1]    = 0xff;
                g_part1ReNum[5]    = 0xff;
                g_part1ReNum[8]    = 0xff;
                g_part1ReNum[12]   = 0xff;
            }
            break;  
        default:
        break;
    }

    if(2 > uciSetNum)
    {
        part1SetNum = 1;
    }
    else
    {
        uciSymNumPart1 = 0;
        uciBitNum      = 0;
        for(uciSetIdx = 0; uciSetIdx < uciSetNum; uciSetIdx++)
        {
            uciSymNumPart1 += uciSymNumSet[uciSetIdx];
            if(valG1 <= (12 * valQm * uciSymNumPart1 * (fapipucchpduInfo->prbSize)))
            {
                part1SetNum = uciSetIdx + 1;
                uciSymNum   = uciSymNumSet[uciSetIdx];
                valN        = (valG1 - uciBitNum) / (uciSymNum * valQm);
                valM        = ((valG1 - uciBitNum) / valQm) % uciSymNum;
                for(uciSymNumCnt = 0; uciSymNumCnt < uciSymNumSet[uciSetIdx]; uciSymNumCnt++)
                {
                    SymbIdx = uciSymIdxSet[uciSetIdx][uciSymNumCnt];
                    g_part1ReNum[SymbIdx] = (0 < valM) ? (valN + 1):(valN);
                    valM--;
                }
                break;
            }
            else
            {
                for(uciSymNumCnt = 0; uciSymNumCnt < uciSymNumSet[uciSetIdx]; uciSymNumCnt++)
                {
                    SymbIdx = uciSymIdxSet[uciSetIdx][uciSymNumCnt];
                    g_part1ReNum[SymbIdx] = 12 * (fapipucchpduInfo->prbSize);
                }
                uciBitNum += (12 * valQm * uciSymNumSet[uciSetIdx] * (fapipucchpduInfo->prbSize));
            }
        }
    }
}

void CalcLlrBlockInfo(FapiNrMsgPucchPduInfo *fapipucchpduInfo, PucFmt3Param *fmt3Param, uint16_t uciLen, uint8_t valQm, uint8_t uciSymNum, uint16_t uciPart2Enable)
{	
    uint8_t  symbIdx;
	uint8_t  part1BlockNum;
    uint8_t  part2BlockNum;
    uint16_t isPreSymPart1;
    uint16_t isPreSymPart2;
    uint16_t llrNumPerSym;
    uint16_t llrNumCnt;

	part1BlockNum = 0;
	part2BlockNum = 0;
	isPreSymPart1 = 0;
	isPreSymPart2 = 0;
	llrNumCnt     = 0;
	llrNumPerSym  = (12 * (fapipucchpduInfo->prbSize) * valQm);
	if(uciPart2Enable)
    {   
        part1BlockNum = 0;
        part2BlockNum = 0;
        isPreSymPart1 = 0;
        isPreSymPart2 = 0;
        llrNumCnt     = 0;
        llrNumPerSym  = (12 * (fapipucchpduInfo->prbSize) * valQm);

        memset(fmt3Param->part1LlrPos, 0, sizeof(Fmt3UciLlrPos) * MAX_PUCCH_3_BLOCK_NUM);
        memset(fmt3Param->part2LlrPos, 0, sizeof(Fmt3UciLlrPos) * MAX_PUCCH_3_BLOCK_NUM);

        CalcPart1ReNum(fapipucchpduInfo, fmt3Param, uciLen, valQm);

        for(symbIdx = 0; symbIdx < SYM_NUM_PER_SLOT; symbIdx++)
        {
            if(0xff != g_part1ReNum[symbIdx])
            {
                if(0 == g_part1ReNum[symbIdx])//仅part2
                {
                    if(!isPreSymPart2)
                    {
                        fmt3Param->part2LlrPos[part2BlockNum].llrStart  = llrNumCnt;
                        part2BlockNum++;
                    }
                    fmt3Param->part2LlrPos[part2BlockNum].llrDuration += llrNumPerSym;
                    isPreSymPart1 = 0;
                    isPreSymPart2 = 1;
                    llrNumCnt += llrNumPerSym;
                }
                else if((12 * (fapipucchpduInfo->prbSize)) == g_part1ReNum[symbIdx])//仅part1
                {
                    if(!isPreSymPart1)
                    {
                        fmt3Param->part1LlrPos[part1BlockNum].llrStart  = llrNumCnt;
                        part1BlockNum++;
                    }
                    fmt3Param->part1LlrPos[part1BlockNum].llrDuration += llrNumPerSym;
                    isPreSymPart1 = 1;
                    isPreSymPart2 = 0;
                    llrNumCnt += llrNumPerSym;
                }
                else//part1+part2
                {
                    if(!isPreSymPart1)
                    {
                        fmt3Param->part1LlrPos[part1BlockNum].llrStart  = llrNumCnt;
                        part1BlockNum++;
                    }
                    fmt3Param->part1LlrPos[part1BlockNum].llrDuration += ((g_part1ReNum[symbIdx]) * valQm);
                    llrNumCnt += ((g_part1ReNum[symbIdx]) * valQm);

                    if(!isPreSymPart2)
                    {
                        fmt3Param->part2LlrPos[part2BlockNum].llrStart  = llrNumCnt;
                        part2BlockNum++;
                    }
                    fmt3Param->part1LlrPos[part2BlockNum].llrDuration +=  (llrNumPerSym - ((g_part1ReNum[symbIdx]) * valQm));
                    isPreSymPart1 = 0;
                    isPreSymPart2 = 1;
                    llrNumCnt +=  (llrNumPerSym - ((g_part1ReNum[symbIdx]) * valQm));
                }
            }
        }
        fmt3Param->part1BlockNum = part1BlockNum;
        fmt3Param->part2BlockNum = part2BlockNum;
    }
    else
    {
        fmt3Param->part1BlockNum = 1;
        fmt3Param->part1LlrPos[0].llrStart    = 0;
        fmt3Param->part1LlrPos[0].llrDuration = llrNumPerSym * uciSymNum;
    }
}

void CalcFmt3SymInfo(PucParam *pucParam, PucFmt3Param *fmt3Param, uint8_t m0, uint8_t numSymbols, uint8_t startSymbolIndex, uint8_t intraSlotFreqHopping, uint8_t addDmrsFlag)
{
    uint8_t symNum[HOP_NUM];
    uint8_t dmrsSymNum[HOP_NUM];
    uint8_t symNumCnt;
    uint8_t hopIdx;
    
    switch(numSymbols) 
    {
        case 4:
            {
                pucParam->hopParam[0].dmrsSymNum = 1;
                pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0:1;
                fmt3Param->dmrsSymIdx[0][0]  = (0 == intraSlotFreqHopping) ? 1:0;
                fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:2;
                fmt3Param->cyclicShift[0][0] = (m0 + *(g_NcsValue +(fmt3Param->dmrsSymIdx[0][0])) % SC_NUM_PER_RB);
                fmt3Param->cyclicShift[1][0] = (m0 + *(g_NcsValue +(fmt3Param->dmrsSymIdx[1][0])) % SC_NUM_PER_RB);
                break;
            }
        case 5:
            {
                pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2:1;
                pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0:1;
                fmt3Param->dmrsSymIdx[0][0]  = 0;
                fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 3:0;
                fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:3;  
                break;     
            }
        case 6:
        case 7:
            {
                pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2:1;
                pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0:1;
                fmt3Param->dmrsSymIdx[0][0]  = 1;
                fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 4:0;
                fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:4;
                break;
            }
        case 8:
            {
                pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2:1;
                pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0:1;
                fmt3Param->dmrsSymIdx[0][0]  = 1;
                fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 5:0;
                fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:5;
                break;
            }
        case 9:
            {
                pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2:1;
                pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0:1;
                fmt3Param->dmrsSymIdx[0][0]  = 1;
                fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 6:0;
                fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:6; 
                break;
            }
        case 10:
            {
                if(addDmrsFlag)
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 4 : 2;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 2;
                    fmt3Param->dmrsSymIdx[0][0]  = 1;
                    fmt3Param->dmrsSymIdx[0][1]  = 3;
                    fmt3Param->dmrsSymIdx[0][2]  = (0 == intraSlotFreqHopping) ? 6:0;
                    fmt3Param->dmrsSymIdx[0][3]  = (0 == intraSlotFreqHopping) ? 8:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:6;
                    fmt3Param->dmrsSymIdx[1][1]  = (0 == intraSlotFreqHopping) ? 0:8;
                }
                else
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2 : 1;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 1;
                    fmt3Param->dmrsSymIdx[0][0]  = 2;
                    fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 7:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:7;
                }
                break;
            }
        case 11:
            {
                if(addDmrsFlag)
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 4 : 2;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 2;
                    fmt3Param->dmrsSymIdx[0][0]  = 1;
                    fmt3Param->dmrsSymIdx[0][1]  = 3;
                    fmt3Param->dmrsSymIdx[0][2]  = (0 == intraSlotFreqHopping) ? 6:0;
                    fmt3Param->dmrsSymIdx[0][3]  = (0 == intraSlotFreqHopping) ? 9:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:6;
                    fmt3Param->dmrsSymIdx[1][1]  = (0 == intraSlotFreqHopping) ? 0:9;
                }
                else
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2 : 1;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 1;
                    fmt3Param->dmrsSymIdx[0][0]  = 2;
                    fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 7:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:7;
                }	
                break;
            }
        case 12:
            {
                if(addDmrsFlag)
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 4 : 2;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 2;
                    fmt3Param->dmrsSymIdx[0][0]  = 1;
                    fmt3Param->dmrsSymIdx[0][1]  = 4;
                    fmt3Param->dmrsSymIdx[0][2]  = (0 == intraSlotFreqHopping) ? 7:0;
                    fmt3Param->dmrsSymIdx[0][3]  = (0 == intraSlotFreqHopping) ? 10:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:7;
                    fmt3Param->dmrsSymIdx[1][1]  = (0 == intraSlotFreqHopping) ? 0:10;
                }
                else
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2 : 1;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 1;
                    fmt3Param->dmrsSymIdx[0][0]  = 2;
                    fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 8:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:8;
                }	
                break;
            }
        case 13:
            {
                if(addDmrsFlag)
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 4 : 2;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 2;
                    fmt3Param->dmrsSymIdx[0][0]  = 1;
                    fmt3Param->dmrsSymIdx[0][1]  = 4;
                    fmt3Param->dmrsSymIdx[0][2]  = (0 == intraSlotFreqHopping) ? 7:0;
                    fmt3Param->dmrsSymIdx[0][3]  = (0 == intraSlotFreqHopping) ? 11:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:7;
                    fmt3Param->dmrsSymIdx[1][1]  = (0 == intraSlotFreqHopping) ? 0:11;
                }
                else
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2 : 1;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 1;
                    fmt3Param->dmrsSymIdx[0][0]  = 2;
                    fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 9:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:9;
                }	
                break;
            }
        case 14:
            {
                if(addDmrsFlag)
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 4 : 2;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 2;
                    fmt3Param->dmrsSymIdx[0][0]  = 1;
                    fmt3Param->dmrsSymIdx[0][1]  = 5;
                    fmt3Param->dmrsSymIdx[0][2]  = (0 == intraSlotFreqHopping) ? 8:0;
                    fmt3Param->dmrsSymIdx[0][3]  = (0 == intraSlotFreqHopping) ? 12:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:8;
                    fmt3Param->dmrsSymIdx[1][1]  = (0 == intraSlotFreqHopping) ? 0:12;
                }
                else
                {
                    pucParam->hopParam[0].dmrsSymNum = (0 == intraSlotFreqHopping) ? 2 : 1;
                    pucParam->hopParam[1].dmrsSymNum = (0 == intraSlotFreqHopping) ? 0 : 1;
                    fmt3Param->dmrsSymIdx[0][0]  = 3;
                    fmt3Param->dmrsSymIdx[0][1]  = (0 == intraSlotFreqHopping) ? 10:0;
                    fmt3Param->dmrsSymIdx[1][0]  = (0 == intraSlotFreqHopping) ? 0:10;
                }	
                break;
            }
        default:
            break;
    }

    symNum[0]  = (0 == intraSlotFreqHopping) ? numSymbols : (numSymbols>>1);
	symNum[1]  = (numSymbols - symNum[0]);
    pucParam->hopParam[0].startSymIdx = startSymbolIndex;
    pucParam->hopParam[1].startSymIdx = (0 == intraSlotFreqHopping) ? 0 : (startSymbolIndex + symNum[0]);
    for(hopIdx = 0; hopIdx < (intraSlotFreqHopping + 1); hopIdx++)
    {
        dmrsSymNum[hopIdx] = pucParam->hopParam[hopIdx].dmrsSymNum;
        pucParam->hopParam[hopIdx].symNum    = symNum[hopIdx];
        pucParam->hopParam[hopIdx].uciSymNum = (symNum[hopIdx] - dmrsSymNum[hopIdx]);
        for(symNumCnt = 0; symNumCnt < dmrsSymNum[hopIdx]; symNumCnt++)
        {
            fmt3Param->cyclicShift[hopIdx][symNumCnt] = (m0 + *(g_NcsValue +(fmt3Param->dmrsSymIdx[hopIdx][symNumCnt])))% SC_NUM_PER_RB;
        }
    }
}

void PucchFmt3PduParse(PucParam *pucParam, FapiNrMsgPucchPduInfo *fapipucchpduInfo, uint8_t intraSlotFreqHopping, uint16_t pduIndex, uint8_t groupOrSequenceHopping, uint16_t sfnNum, uint8_t slotNum, uint8_t cellIndex)
{
	uint8_t  SymbIdx;
    uint8_t  SymbNumCnt;
	uint8_t  addDmrsFlag;
    uint8_t  maxCodeRate;
    uint8_t  numSymbols;
    uint8_t  uciSymNum;
    uint8_t  uciPart2Enable;
    uint8_t  valQm;
	uint8_t  symNum[HOP_NUM];
    uint8_t  part1BlockNum;
    uint8_t  part2BlockNum;
    uint16_t uciLen;
    uint16_t isPreSymPart1;
    uint16_t isPreSymPart2;
    uint16_t llrNumPerSym;
    uint16_t llrNumCnt;
	uint32_t Cinit 		 = 0;
	uint32_t SequenceLen = 0;
	PucFmt3Param *fmt3Param = NULL;
    
    valQm          = 2 - (fapipucchpduInfo->pi2BpskFlag);
    uciLen         = (fapipucchpduInfo->srFlag) + (fapipucchpduInfo->bitLenHarq) + (fapipucchpduInfo->csiPart1BitLength);
	addDmrsFlag    = fapipucchpduInfo->addDmrsFlag;
    maxCodeRate    = (fapipucchpduInfo->pucchParaAddInV3.maxCodeRate)&0x7;
    uciPart2Enable = (0 == (fapipucchpduInfo->uciInfoAddInV3.part2sNum)) ? 0:1;

    fmt3Param = (PucFmt3Param *)(&(pucParam->formatxParam));
    fmt3Param->pduIdxInner    = pduIndex;
	fmt3Param->pi2bpsk        = (1 == fapipucchpduInfo->pi2BpskFlag) ? 1:0;
	fmt3Param->addDmrsEnable  = (1 == fapipucchpduInfo->addDmrsFlag) ? 1:0;
    fmt3Param->uciPart2Enable = uciPart2Enable;

    g_pucchCsiPart2Flag[cellIndex][slotNum] = uciPart2Enable;

	PucchNcsandUVCalc(slotNum,fapipucchpduInfo->nIdPucchHopping,groupOrSequenceHopping);

	Cinit         = ((fapipucchpduInfo->ueRnti)<<15) + (fapipucchpduInfo->nIdPucchHopping);
	SequenceLen   = 12*(2 - (fapipucchpduInfo->pi2BpskFlag))*(fapipucchpduInfo->numSymbols)*(fapipucchpduInfo->prbSize);
	PseudoRandomSeqGen(g_fmt23dataScrambuff, Cinit, SequenceLen, 0);

    numSymbols = fapipucchpduInfo->numSymbols;
    CalcFmt3SymInfo(pucParam, fmt3Param, (fapipucchpduInfo->initCyclicShift), numSymbols, (fapipucchpduInfo->StartSymbolIndex), intraSlotFreqHopping, addDmrsFlag);

    uciSymNum  = (numSymbols - (pucParam->hopParam[0].dmrsSymNum) - (pucParam->hopParam[0].dmrsSymNum));
    CalcLlrBlockInfo(fapipucchpduInfo, fmt3Param, uciLen, valQm, uciSymNum, uciPart2Enable);
	
    //fmt3Param->scrambSeqAddr[HOP_NUM];           /* 加扰序列在DDR中的存放地址，TODO:根据HAC存放确定是否需要2个hop的首地址 */

    if((RM_BIT_LENGTH_MIN <= uciLen) && (RM_BIT_LENGTH_MAX >= uciLen))
    {
        PucchRMDecodeHacCfg(fapipucchpduInfo, uciLen, pduIndex, PUCCH_UCI_PART1, 0, sfnNum, slotNum, cellIndex);
    }
    else if((POLAR_BIT_LENGTH_MIN <= uciLen) && (POLAR_BIT_LENGTH_MAX >= uciLen))
    {
        PucchPolarDecodeHacCfg(fapipucchpduInfo, uciLen, pduIndex, PUCCH_UCI_PART1, 0, sfnNum, slotNum, cellIndex);
    }

}

void UlTtiRequestPucchFmt023PduParse(ArmPucParam *armPucParam, PucParam *pucParam, uint16_t sfnNum, uint16_t slotNum, uint16_t pduIndex, uint8_t cellIndex)
{
    uint8_t  formatType;
    uint8_t  EndSymbolIndex;
    uint8_t  pucchNumpersym;
    uint8_t  pucchindex;
    uint8_t  groupOrSequenceHopping;
    uint8_t  intraSlotFreqHopping; 
    uint8_t  rptIndex;
    FapiNrMsgPucchPduInfo *fapipucchpduInfo;

    fapipucchpduInfo = &(armPucParam->FapiPucchPduInfo[pduIndex]);
    formatType      = fapipucchpduInfo->formatType;
    EndSymbolIndex  = fapipucchpduInfo->StartSymbolIndex + fapipucchpduInfo->numSymbols;
    pucchindex      = armPucParam->pucchNum;
    pucchNumpersym  = armPucParam->pucchNumpersym[EndSymbolIndex]++;
    armPucParam->pucchIndex[EndSymbolIndex][pucchNumpersym] = pucchindex;

    groupOrSequenceHopping = fapipucchpduInfo->groupOrSequenceHopping;
    intraSlotFreqHopping   = fapipucchpduInfo->intraSlotFreqHopping;
    /* 所在小区的小区级参数 */
    pucParam->pucFormat    = fapipucchpduInfo->formatType;
    pucParam->rxAntNum     = g_cellConfigPara[cellIndex].rxAntNum;

    /* 时隙跳频参数 */
    pucParam->hopNum  = (intraSlotFreqHopping + 1);
    pucParam->hopParam[0].prbStart    = fapipucchpduInfo->prbStart;
    pucParam->hopParam[0].prbSize     = fapipucchpduInfo->prbSize;
    pucParam->hopParam[1].prbStart    = fapipucchpduInfo->secondHopPRB;
    pucParam->hopParam[1].prbSize     = fapipucchpduInfo->prbSize;

    /* PUC data在DDR中的存放地址 */
    //pucParam->hopParam[0].dataStartAddr
    //pucParam->hopParam[1].dataStartAddr

    /* ZC基序列或PN序列在DDR中的存放地址:
       fmt0数据/fmt1数据和导频/fmt3导频使用ZC序列,某hop内,各符号天线上的ZC基序列相同;
       fmt2导频使用PN序列,数组的2个元素分别为2个符号的PN序列的地址 */
    //pucParam->hopParam[0].baseSeqAddr
    //pucParam->hopParam[1].baseSeqAddr
    
	switch(formatType)
	{
		case PUCCH_FORMAT_0:
			PucchFmt0PduParse(pucParam, fapipucchpduInfo, intraSlotFreqHopping, groupOrSequenceHopping, pduIndex, slotNum);

            rptIndex = g_pucchFmt01RptNum[cellIndex][slotNum];
            PucchFmt01RptPreFill(fapipucchpduInfo, &(g_pucchFmt01Rst[cellIndex][slotNum].fapiNrPucchFmt01Indication[rptIndex]));
            g_pucchFmt01RptIndex[cellIndex][slotNum][pduIndex] = g_pucchFmt01RptNum[cellIndex][slotNum]++;
			break;
		case PUCCH_FORMAT_2:
			PucchFmt2PduParse(pucParam, fapipucchpduInfo, intraSlotFreqHopping, pduIndex, sfnNum, slotNum, cellIndex);
            
            rptIndex = g_pucchFmt23RptNum[cellIndex][slotNum];
            PucchFmt23RptPreFill(fapipucchpduInfo, &(g_pucchFmt23Rst[cellIndex][slotNum].fapiNrPucchFmt23Indication[rptIndex]));
            g_pucchFmt23RptIndex[cellIndex][slotNum][pduIndex] = g_pucchFmt23RptNum[cellIndex][slotNum]++;
			break;
		case PUCCH_FORMAT_3:
			PucchFmt3PduParse(pucParam, fapipucchpduInfo, intraSlotFreqHopping, pduIndex, groupOrSequenceHopping, sfnNum, slotNum, cellIndex);

            rptIndex = g_pucchFmt23RptNum[cellIndex][slotNum];
            PucchFmt23RptPreFill(fapipucchpduInfo, &(g_pucchFmt23Rst[cellIndex][slotNum].fapiNrPucchFmt23Indication[rptIndex]));
            g_pucchFmt23RptIndex[cellIndex][slotNum][pduIndex] = g_pucchFmt23RptNum[cellIndex][slotNum]++;
			break;
		default:
		break;
	}
}

void UlTtiRequestPucchFmt1PduParse(ArmPucParam *armPucParam, PucParam *pucParam, uint8_t pucchpduGroupCnt, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex)
{
    uint8_t  EndSymbolIndex;
    uint8_t  pucchNumpersym;
    uint8_t  pucchindex;
    uint8_t  pucchpduIndex;
    uint8_t  intraSlotFreqHopping;
    uint8_t  numSymbols;
    uint8_t  symNum[HOP_NUM];
    uint8_t  SymbIdx;
    uint8_t  OccIdx;
    uint8_t  OccNumCnt;
    uint8_t  MinUserOcc;
    uint8_t  MinUserOccIdx;
    uint8_t  pucchuserNumPerOcc;
    uint8_t  rptIndex;
    FapiNrMsgPucchPduInfo *fapipucchpduInfo = NULL;
    PucFmt1Param          *fmt1Param        = NULL;
    Fmt1ParamOcc          *fmt1ParamOcc     = NULL;
    Fmt1UEParam           *fmt1UEParam      = NULL;                     

    fapipucchpduInfo = &(armPucParam->FapiPucchPduInfo[pucchpduGroupCnt]);//待修改
    /* fmt1 UE common */
    fmt1Param = (PucFmt1Param *)(&(pucParam->formatxParam));
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
        pucchuserNumPerOcc = armPucParam->pucchuserNumPerOcc[pucchpduGroupCnt][OccIdx];
        if(0 == pucchuserNumPerOcc)
        {
            continue;
        }

        fmt1ParamOcc = &fmt1Param->fmt1ParamOcc[OccNumCnt];
        fmt1ParamOcc->ueTapBitMap       = 0;
        fmt1ParamOcc->timeDomainOccIdx  = OccIdx;
        fmt1ParamOcc->userNumPerOcc     = pucchuserNumPerOcc;

        for(pucchindex = 0; pucchindex < pucchuserNumPerOcc; pucchindex++)
        {
            pucchpduIndex    = armPucParam->pucchpduIndexinGroup[pucchpduGroupCnt][OccIdx][pucchindex];
            fapipucchpduInfo = &(armPucParam->FapiPucchPduInfo[pucchpduIndex]);
            rptIndex = g_pucchFmt01RptNum[cellIndex][slotNum];
            PucchFmt01RptPreFill(fapipucchpduInfo, &(g_pucchFmt01Rst[cellIndex][slotNum].fapiNrPucchFmt01Indication[rptIndex]));
            g_pucchFmt01RptIndex[cellIndex][slotNum][pucchpduIndex] = g_pucchFmt01RptNum[cellIndex][slotNum]++;
            for(SymbIdx = (fapipucchpduInfo->StartSymbolIndex); SymbIdx < ((fapipucchpduInfo->StartSymbolIndex) + (fapipucchpduInfo->numSymbols)); SymbIdx++)
            {   
                fmt1ParamOcc->ueTapBitMap |= (1<<((N_SC_PER_PRB - (fapipucchpduInfo->initCyclicShift)) % N_SC_PER_PRB));
            }

            fmt1UEParam = &fmt1ParamOcc->fmt1UEParam[pucchindex];
            fmt1UEParam->pduIdxInner  = pucchpduIndex;
            fmt1UEParam->m0           = (fapipucchpduInfo->initCyclicShift);
            fmt1UEParam->uciBitNum    = (0 == (fapipucchpduInfo->bitLenHarq)) ? (fapipucchpduInfo->srFlag):(fapipucchpduInfo->bitLenHarq);
        }

        if(MinUserOcc > pucchuserNumPerOcc)
        {
            MinUserOcc     = pucchuserNumPerOcc;
            MinUserOccIdx  = OccNumCnt;
        }
        OccNumCnt++;
    }
    fmt1Param->occNum        = OccNumCnt;
    fmt1Param->minUserOccIdx = MinUserOccIdx;

    EndSymbolIndex   = fapipucchpduInfo->StartSymbolIndex + fapipucchpduInfo->numSymbols;
    pucchNumpersym   = armPucParam->pucchNumpersym[EndSymbolIndex]++;
    armPucParam->pucchIndex[EndSymbolIndex][pucchNumpersym] = armPucParam->pucchNum;

    intraSlotFreqHopping = fapipucchpduInfo->intraSlotFreqHopping;
    /* 所在小区的小区级参数 */
    pucParam->pucFormat    = fapipucchpduInfo->formatType;
    pucParam->rxAntNum     = g_cellConfigPara[cellIndex].rxAntNum;
    pucParam->hopNum       = (intraSlotFreqHopping + 1);

    /* 时隙跳频参数 */
    pucParam->hopNum  = (intraSlotFreqHopping + 1);
    pucParam->hopParam[0].prbStart    = fapipucchpduInfo->prbStart;
    pucParam->hopParam[0].prbSize     = fapipucchpduInfo->prbSize;
    pucParam->hopParam[1].prbStart    = fapipucchpduInfo->secondHopPRB;
    pucParam->hopParam[1].prbSize     = fapipucchpduInfo->prbSize;
    
    /* time domain */
    numSymbols = fapipucchpduInfo->numSymbols;
    symNum[0]                         = (0 == intraSlotFreqHopping) ? numSymbols : (numSymbols>>1);
    symNum[1]                         = numSymbols - symNum[0];
    pucParam->hopParam[0].uciSymNum   = (0 == intraSlotFreqHopping) ? (numSymbols>>1) : (numSymbols>>2);
    pucParam->hopParam[1].uciSymNum   = (0 == intraSlotFreqHopping) ? 0 : ((numSymbols + 2)>>2);
    pucParam->hopParam[0].dmrsSymNum  = symNum[0] - pucParam->hopParam[0].uciSymNum;
    pucParam->hopParam[1].dmrsSymNum  = symNum[1] - pucParam->hopParam[1].uciSymNum;

    pucParam->hopParam[0].startSymIdx = fapipucchpduInfo->StartSymbolIndex;
    pucParam->hopParam[1].startSymIdx = (0 == intraSlotFreqHopping) ? 0 : ((fapipucchpduInfo->StartSymbolIndex) + symNum[0]);
    pucParam->hopParam[0].symNum      = symNum[0];
    pucParam->hopParam[1].symNum      = symNum[1];
    
    /* PUC data在DDR中的存放地址 */
    //uint32_t *dataStartAddr[HOP_NUM];  

    /* ZC基序列或PN序列在DDR中的存放地址:
       fmt0数据/fmt1数据和导频/fmt3导频使用ZC序列,某hop内,各符号天线上的ZC基序列相同;
       fmt2导频使用PN序列,数组的2个元素分别为2个符号的PN序列的地址 */
    //uint32_t *baseSeqAddr[HOP_NUM]; 
}

void PucchFmt1Grouping(ArmPucParam *armPucParam)
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

    for(pucchpduNumCnt1 = 0; pucchpduNumCnt1 < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_1]; pucchpduNumCnt1++)
    {
        if(1 == pucchfmt1pduflag[pucchpduNumCnt1])
        {
            continue;
        }
        pucchpduIndex1      = armPucParam->pucchfmtpduIdxInner[PUCCH_FORMAT_1][pucchpduNumCnt1];
        prbStart1           = armPucParam->FapiPucchPduInfo[pucchpduIndex1].prbStart;
        StartSymbolIndex1   = armPucParam->FapiPucchPduInfo[pucchpduIndex1].StartSymbolIndex;
        tdOccIdx1           = armPucParam->FapiPucchPduInfo[pucchpduIndex1].tdOccIdx;

        pucchpduGroupNum    = armPucParam->pucchpduGroupNum;
        pucchueidxInOcc     = armPucParam->pucchuserNumPerOcc[pucchpduGroupNum][tdOccIdx1]++;
        armPucParam->pucchpduIndexinGroup[pucchpduGroupNum][tdOccIdx1][pucchueidxInOcc] = pucchpduIndex1;
        armPucParam->pucchpduNumPerGroup[pucchpduGroupNum]++;

        pucchfmt1pduflag[pucchpduNumCnt1] = 1;
        for(pucchpduNumCnt2 = (pucchpduIndex1 + 1); pucchpduNumCnt2 < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_1]; pucchpduNumCnt2++)
        {
            pucchpduIndex2      = armPucParam->pucchfmtpduIdxInner[PUCCH_FORMAT_1][pucchpduNumCnt2];
            prbStart2           = armPucParam->FapiPucchPduInfo[pucchpduIndex2].prbStart;
            StartSymbolIndex2   = armPucParam->FapiPucchPduInfo[pucchpduIndex2].StartSymbolIndex;
            tdOccIdx2           = armPucParam->FapiPucchPduInfo[pucchpduIndex2].tdOccIdx;
            if((0 == pucchfmt1pduflag[pucchpduNumCnt2]) && (prbStart1 == prbStart2) && (StartSymbolIndex1 == StartSymbolIndex2))
            {
                armPucParam->pucchpduNumPerGroup[pucchpduGroupNum]++;
                pucchueidxInOcc = armPucParam->pucchuserNumPerOcc[pucchpduGroupNum][tdOccIdx2]++;
                armPucParam->pucchpduIndexinGroup[pucchpduGroupNum][tdOccIdx2][pucchueidxInOcc] = pucchpduIndex2;
                pucchfmt1pduflag[pucchpduNumCnt2] = 1;
            }
        }
        armPucParam->pucchpduGroupNum++;
    }
}

void UlTtiRequestPucchPduparse(ArmPucParam *armPucParam, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex)
{
    uint16_t pduIndex;
    uint8_t  pduNumCnt;
    uint8_t  pucchpduGroupCnt;
    uint8_t  fmtIdx;
    FapiNrMsgPucchPduInfo *fapipucchpduInfo = NULL;
    PucchPara             *pucchPara        = NULL;
    PucParam              *pucParam         = NULL;
    PucFmt0AlgoParam      *fmt0AlgoParam    = NULL;
    PucFmt1AlgoParam      *fmt1AlgoParam    = NULL;
    PucFmt23AlgoParam     *fmt2AlgoParam    = NULL;
    PucFmt23AlgoParam     *fmt3AlgoParam    = NULL;

    fapipucchpduInfo = &(armPucParam->FapiPucchPduInfo[0]);
    pucchPara = &(g_PucchPara[cellIndex][slotNum]);
    pucParam  = &(g_PucchPara[cellIndex][slotNum].pucParam);

    if(0 < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_2])
    {
        for(pduNumCnt = 0; pduNumCnt < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_2]; pduNumCnt++)
        {
            pucParam = (pucParam + (armPucParam->pucchNum));
            pduIndex = armPucParam->pucchfmtpduIdxInner[PUCCH_FORMAT_2][pduNumCnt];
            g_pucchFmt23RptIndex[cellIndex][slotNum][pduIndex] = pduNumCnt;
            UlTtiRequestPucchFmt023PduParse(armPucParam, pucParam, sfnNum, slotNum, pduIndex, cellIndex); 
            armPucParam->pucchNum++;
        }
    }

    if(0 < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_3])
    {
        for(pduNumCnt = 0; pduNumCnt <armPucParam->pucchfmtpdunum[PUCCH_FORMAT_3]; pduNumCnt++)
        {
            pucParam = (pucParam + (armPucParam->pucchNum));
            pduIndex = armPucParam->pucchfmtpduIdxInner[PUCCH_FORMAT_3][pduNumCnt];
            g_pucchFmt23RptIndex[cellIndex][slotNum][pduIndex] = (armPucParam->pucchfmtpdunum[PUCCH_FORMAT_2]) + pduNumCnt;
            UlTtiRequestPucchFmt023PduParse(armPucParam, pucParam, sfnNum, slotNum, pduIndex, cellIndex); 
            armPucParam->pucchNum++; 
        }
    }

    if(0 < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_0])
    {
        for(pduNumCnt = 0; pduNumCnt < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_0]; pduNumCnt++)
        {
            pucParam = (pucParam + (armPucParam->pucchNum));
            pduIndex = armPucParam->pucchfmtpduIdxInner[PUCCH_FORMAT_0][pduNumCnt];
            g_pucchFmt23RptIndex[cellIndex][slotNum][pduIndex] = pduNumCnt;
            UlTtiRequestPucchFmt023PduParse(armPucParam, pucParam, sfnNum, slotNum, pduIndex, cellIndex); 
            armPucParam->pucchNum++;
        }
    }

    /* pucch fmt1,将复用的PDU先分组，再解析*/
    if(0 < armPucParam->pucchfmtpdunum[PUCCH_FORMAT_1])
    {
        PucchFmt1Grouping(armPucParam);
        for(pucchpduGroupCnt = 0; pucchpduGroupCnt < armPucParam->pucchpduGroupNum; pucchpduGroupCnt++)
        {
            pucParam = (pucParam + (armPucParam->pucchNum));
            UlTtiRequestPucchFmt1PduParse(armPucParam, pucParam, pucchpduGroupCnt, sfnNum, slotNum, cellIndex);
            armPucParam->pucchNum++;
        }
    }  

    pucchPara->pucchNum = armPucParam->pucchNum;
    pucchPara->fmt0Num  = armPucParam->pucchfmtpdunum[PUCCH_FORMAT_0];
    pucchPara->fmt1Num  = armPucParam->pucchfmtpdunum[PUCCH_FORMAT_1];
    pucchPara->fmt2Num  = armPucParam->pucchfmtpdunum[PUCCH_FORMAT_2];
    pucchPara->fmt3Num  = armPucParam->pucchfmtpdunum[PUCCH_FORMAT_3];

    /*算法参数待接口OAM确认后赋值
    fmt0AlgoParam  = pucchPara->fmt0AlgoParam;
    fmt1AlgoParam  = pucchPara->fmt1AlgoParam;
    fmt23AlgoParam = pucchPara->fmt23AlgoParam;

    fmt0AlgoParam->deltaOffset  = ;
    fmt0AlgoParam->noiseTapNum  = ;
    fmt0AlgoParam->threshold    = ;

    fmt1AlgoParam->noiseTapNum  = ;
    fmt1AlgoParam->threshold    = ;

    fmt23AlgoParam->MrcIrcFlag  = ;
    fmt23AlgoParam->Epsilon     = ;
    fmt23AlgoParam->beta        = ;
    fmt23AlgoParam->segNum      = ;
    fmt23AlgoParam->threshold   = ;
    */

}

void PucchPart1ParaCfgHandler()//计算DSP解调参数时顺便计算
{
    printf("配置Part1译码参数\n");
    
    //UlTtiRequestPucchPduparse(fapipucchpduInfo, sfnNum, slotNum, cellIndex); 

}

void PucchUCIPart1Part2Parse()
{
    uint16_t sfn;     
    uint16_t slot;
    uint16_t csiPart2BitLength; 
    uint8_t  cellIdx;
    uint8_t  pduNum;
    uint8_t  pduNumCnt;
    uint8_t  codeType;
    uint8_t  msgType;
    uint8_t  pduIndex;
    uint8_t  ueIdx;
    uint8_t  rptIdx;
    uint8_t  numPart1Params;
    uint16_t *map              = NULL;
    uint8_t  *sizesPart1Params = NULL;
    PucchPolarDecodeHacCfgPara   *pucchPolarDecodeHacCfgPara   = NULL;
    PucchRMDecodeHacCfgPara      *pucchRMDecodeHacCfgPara      = NULL;
    HacCfgHead                   *hacCfgHead                   = NULL;
    PolarDecodePduInfo           *polarDecodePduInfo           = NULL;
    RMDecodePduInfo              *rmDecodePduInfo              = NULL;
    RMUeDecodeOut                *rmUeDecodeOut                = NULL;
    PolarUeDecodeOut             *polarUeDecodeOut             = NULL;
    PucchFmt23Rst                *pucchFmt23Rst                = NULL;
    FapiNrPucchFmt23Indication   *fapiNrPucchFmt23Indication   = NULL;
    HARQInfoFmt23                *harqInfoFmt23                = NULL;
	SRInfoFmt23                  *srInfoFmt23                  = NULL;
	CSIpart1Info                 *csipart1Info                 = NULL;
	CSIpart2Info                 *csipart2Info                 = NULL;
    ArmPucParam                  *armPucParam                  = NULL;
    FapiNrMsgPucchPduInfo        *fapipucchpduInfo             = NULL;
	
    //先解析消息头？  
    sfn      =  0; //待从消息头获取    
    slot     =  0; //待从消息头获取
    cellIdx  =  0; //待从消息头获取  
    pduNum   =  0; //待从消息头获取 
    codeType =  0; //待从消息头获取，确认是RM还是Polar译码结果
    msgType  =  0; //待从消息头获取，确认是UCI Part1还是CSI Part2  

    armPucParam   = &(g_armPucParam[cellIdx][slot]);
    pucchFmt23Rst = &(g_pucchFmt23Rst[cellIdx][slot]);//UCI译码结果回写目的地址
    if(0 == codeType)//RM
    {
        pucchRMDecodeHacCfgPara = &(g_pucchRMDecodeHacCfgParaDDR[cellIdx][slot][msgType]);//读取RM译码结果
        hacCfgHead = &(pucchRMDecodeHacCfgPara->hacCfgHead);

        for(pduNumCnt = 0; pduNumCnt < pduNum; pduNumCnt++)
        {
            rmDecodePduInfo = &(pucchRMDecodeHacCfgPara->rmPduInfo[pduNumCnt]);
            //rmUeDecodeOut   = (rmDecodePduInfo->OutputAddr);//UE输出结果地址
            
            pduIndex  = rmDecodePduInfo->ueIdx;
            rptIdx    = g_pucchFmt23RptIndex[cellIdx][slot][pduIndex];
			if(1 == msgType)//UCI Part1
			{
                fapipucchpduInfo = &(armPucParam->FapiPucchPduInfo[pduIndex]);
				harqInfoFmt23    = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].harqInfoFmt23);
				srInfoFmt23      = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].srInfoFmt23);
				csipart1Info     = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].csipart1Info);

				harqInfoFmt23->HarqCrc             = 0;//待补充，rmUeDecodeOut->
				harqInfoFmt23->HarqBitLen          = fapipucchpduInfo->bitLenHarq;
				harqInfoFmt23->HarqPayload[0]      = 0;//L2D到DDR的拷贝,待补充,rmUeDecodeOut->

				srInfoFmt23->SrBitLen              = fapipucchpduInfo->srFlag;
				srInfoFmt23->SrPayload             = 0;//待补充,rmUeDecodeOut->

				csipart1Info->CsiPart1BitLen       = fapipucchpduInfo->csiPart1BitLength;//待补充
				csipart1Info->CsiPart1Crc          = 0;//待补充,rmUeDecodeOut->
				csipart1Info->CsiPart1Payload[0]   = 0;//L2D到DDR的拷贝,待补充,rmUeDecodeOut->

                if(fapipucchpduInfo->uciInfoAddInV3.part2sNum)//如果存在CSI Part2,顺便配置CSI Part2译码参数
                {
                    //sizesPart1Params = ;//待补充
                    //map              = ;//待补充
                    //numPart1Params   = ;//待补充
                    csiPart2BitLength = CalcCsiPart2BitLength(&(fapipucchpduInfo->uciInfoAddInV3), csipart1Info->CsiPart1Payload, sizesPart1Params, map, csipart1Info->CsiPart1BitLen, numPart1Params);
                    g_pucchCsiPart2BitLength[cellIdx][slot][ueIdx] = csiPart2BitLength;
                    if((RM_BIT_LENGTH_MIN <= csiPart2BitLength) && (RM_BIT_LENGTH_MAX >= csiPart2BitLength))
                    {
                        PucchRMDecodeHacCfg(fapipucchpduInfo, csiPart2BitLength, pduIndex, PUCCH_PART2, rmDecodePduInfo->llrNum, sfn, slot, cellIdx);
                    }
                    else if((POLAR_BIT_LENGTH_MIN <= csiPart2BitLength) && (POLAR_BIT_LENGTH_MAX >= csiPart2BitLength))
                    {
                        PucchPolarDecodeHacCfg(fapipucchpduInfo, csiPart2BitLength, pduIndex, PUCCH_PART2, rmDecodePduInfo->llrNum, sfn, slot, cellIdx);
                    }
                }
			}
			else if(2 == msgType)//CSI Part2
			{
				csipart2Info = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].csipart2Info);
				csipart2Info->CsiPart2BitLen       = g_pucchCsiPart2BitLength[cellIdx][slot][pduIndex];
				csipart2Info->CsiPart2Crc          = 0;//待补充，rmUeDecodeOut->
				csipart2Info->CsiPart2Payload[0]   = 0;//L2D到DDR的拷贝,待补充，rmUeDecodeOut->
			}
			else
			{
				;//异常处理
			}    
        }
    }
    else if(1 == codeType)//Polar
    {
        pucchPolarDecodeHacCfgPara = &(g_pucchPolarDecodeHacCfgParaDDR[cellIdx][slot][msgType]);//读取Polar译码结果
        hacCfgHead = &(pucchPolarDecodeHacCfgPara->hacCfgHead);
        //hacCfgHead->增加校验？

        for(pduNumCnt = 0; pduNumCnt < pduNum; pduNumCnt++)
        {
            polarDecodePduInfo = &(pucchPolarDecodeHacCfgPara->polarPduInfo[pduNumCnt]);
            //polarUeDecodeOut   = (polarDecodePduInfo->OutputAddr);//UE输出结果地址
            
            pduIndex  = rmDecodePduInfo->ueIdx;
            rptIdx    = g_pucchFmt23RptIndex[cellIdx][slot][pduIndex];
			if(PUCCH_PART1 == msgType)//UCI Part1
			{
                fapipucchpduInfo = &(armPucParam->FapiPucchPduInfo[pduIndex]);
				harqInfoFmt23    = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].harqInfoFmt23);
				srInfoFmt23      = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].srInfoFmt23);
				csipart1Info     = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].csipart1Info);

				harqInfoFmt23->HarqCrc            = 0;//待补充，rmUeDecodeOut->
				harqInfoFmt23->HarqBitLen         = fapipucchpduInfo->bitLenHarq;
				harqInfoFmt23->HarqPayload[0]     = 0;//L2D到DDR的拷贝,待补充,rmUeDecodeOut->

				srInfoFmt23->SrBitLen             = fapipucchpduInfo->srFlag;
				srInfoFmt23->SrPayload            = 0;//待补充,rmUeDecodeOut->

				csipart1Info->CsiPart1BitLen      = fapipucchpduInfo->csiPart1BitLength;//待补充
				csipart1Info->CsiPart1Crc         = 0;//待补充,rmUeDecodeOut->
				csipart1Info->CsiPart1Payload[0]  = 0;//L2D到DDR的拷贝,待补充,rmUeDecodeOut->

                if(fapipucchpduInfo->uciInfoAddInV3.part2sNum)//如果存在CSI Part2,顺便配置CSI Part2译码参数
                {
                    //sizesPart1Params = ;//待补充
                    //map              = ;//待补充
                    //numPart1Params   = ;//待补充
                    csiPart2BitLength = CalcCsiPart2BitLength(&(fapipucchpduInfo->uciInfoAddInV3), csipart1Info->CsiPart1Payload, sizesPart1Params, map, csipart1Info->CsiPart1BitLen, numPart1Params);
                    g_pucchCsiPart2BitLength[cellIdx][slot][pduIndex] = csiPart2BitLength;
                    if((RM_BIT_LENGTH_MIN <= csiPart2BitLength) && (RM_BIT_LENGTH_MAX >= csiPart2BitLength))
                    {
                        PucchRMDecodeHacCfg(fapipucchpduInfo, csiPart2BitLength, pduIndex, PUCCH_PART2, polarDecodePduInfo->llrNum, sfn, slot, cellIdx);
                    }
                    else if((POLAR_BIT_LENGTH_MIN <= csiPart2BitLength) && (POLAR_BIT_LENGTH_MAX >= csiPart2BitLength))
                    {
                        PucchPolarDecodeHacCfg(fapipucchpduInfo, csiPart2BitLength, pduIndex, PUCCH_PART2, polarDecodePduInfo->llrNum, sfn, slot, cellIdx);
                    }
                }
			}
			else if(PUCCH_PART2 == msgType)//CSI Part2
			{
				csipart2Info = &(pucchFmt23Rst->fapiNrPucchFmt23Indication[rptIdx].csipart2Info);
				csipart2Info->CsiPart2BitLen     = g_pucchCsiPart2BitLength[cellIdx][slot][pduIndex];
				csipart2Info->CsiPart2Crc        = 0;//待补充,rmUeDecodeOut->
				csipart2Info->CsiPart2Payload[0] = 0;//L2D到DDR的拷贝,待补充,rmUeDecodeOut->
			}
			else
			{
				;//异常处理
			}
        }
    }
    else
    {
        ;//异常处理
    }
    
    //return 0;
}

void PucchPart1ParsePart2ParaCfgHandler()
{
    printf("解析Part1译码结果&&配置Part2译码参数并trigger HAC启动译码\n");
    PucchUCIPart1Part2Parse();//解析Part1译码结果,顺便配置Part2译码参数

    //待确认接口后补充trigger HAC启动CSI Part2译码

}

void PucchPart2ParseHandler()
{
    printf("解析Part2译码结果\n");
    PucchUCIPart1Part2Parse();//解析Part2译码结果
}

void PucchUciSendHandler()//待设计，收齐结果后发送(格式23的译码结果，DSP上报结果（HAC做DTX判决）)？
{
    printf("UCI发送\n");
    
    //return 0;
}

uint32_t PucchUciFsmProc(uint32_t event, uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex)
{

    /*待挪至slot任务启动时初始化
    FSM_Regist(g_pucchFSM[cellIndex][slotNum&0x1],g_puschUciTable);待挪至slot任务启动时初始化
    g_pucchFSM[cellIndex][slotNum&0x1].curState  = Pucch_Uci_Idle_State;
    g_pucchFSM[cellIndex][slotNum&0x1].size      = sizeof(g_pucchTable)/sizeof(FsmTable);
    g_pucchCsiPart2Flag[cellIndex][slotNum] = 0;
    */

    if(g_pucchCsiPart2Flag[cellIndex][slotNum])//本slot本小区含CSI Part2的UE，进入状态机流程
    {
        while(1)//待修改为上报结果收集完成即退出
        {
            printf("state:%d\n",g_pucchFSM[cellIndex][slotNum&0x1].curState);
            //scanf("%d", &event);
            switch (event)
            {
                case Pucch_Slot_Tast_Start_Event:
                case Pucch_Part1_Result_Trigger_Event:
                case Pucch_Part2_Result_Trigger_Event:
                case Pucch_UCI_Packing_Over_Event:
                {
                    FSM_EventHandle(&g_pucchFSM[cellIndex][slotNum&0x1], event);//状态机
                    break;
                }
                default:
                    break;
            }
        }
    }
    else
    {
        while(1)//待修改为上报结果收集完成即退出
        {
            printf("state:%d\n",g_pucchFSM[cellIndex][slotNum&0x1].curState);
            //scanf("%d", &event);
            switch (event)
            {
                case Pucch_Slot_Tast_Start_Event:
                    PucchPart1ParaCfgHandler();
                    break;
                case Pucch_Part1_Result_Trigger_Event:
                    PucchPart1ParsePart2ParaCfgHandler();
                    break;
                case Pucch_UCI_Packing_Over_Event:
                    PucchUciSendHandler();
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}

void pucchFmt0Pack(FapiNrPucchFmt01Indication *fapiNrPucchFmt01Indication, PucchFmt0Rpt  *pucchFmt0Rpt, FapiNrMsgPucchPduInfo *fapiPucchPduInfo)
{
    uint8_t pduBitmap;
    uint8_t mcsValue;
    uint8_t numHarq;
    uint8_t srIndication;
    uint8_t harqValue[2];
    uint8_t srExistFlag;
    uint8_t harqExistFlag;
    uint8_t isDtx;
    SRInfoFmt01   *srInfoFmt01   = NULL;
	HARQInfoFmt01 *harqInfoFmt01 = NULL;

    pduBitmap     = (fapiNrPucchFmt01Indication->pduBitmap);
    numHarq       = (fapiPucchPduInfo->bitLenHarq);
    srExistFlag   = pduBitmap&0x1;
    harqExistFlag = (pduBitmap>>1)&0x1;
    
    isDtx    = pucchFmt0Rpt->isDtx;
    mcsValue = pucchFmt0Rpt->mcsValue;
    if(srExistFlag)//暂时不考虑抽头，即offset取值为0
    {
		if(0 == numHarq)
		{
			srIndication = (1 - isDtx);
		}
		else if(1 == numHarq)
		{
			switch (mcsValue)
			{
				case 0:
					srIndication = 0;
					harqValue[0] = 0;
					break;
				case 6:
					srIndication = 0;
					harqValue[0] = 1;
					break;
				case 3:
					srIndication = 1;
					harqValue[0] = 0;
					break;
				case 9:
					srIndication = 1;
					harqValue[0] = 1;
		
					break;
				default:
					break;
			}
		}
		else if(2 == numHarq)
		{
			switch (mcsValue)
			{
				case 0:
					srIndication = 0;
					harqValue[0] = 0;
					harqValue[1] = 0;
					break;
				case 3:
					srIndication = 0;
					harqValue[0] = 0;
					harqValue[1] = 1;
					break;
				case 6:
					srIndication = 0;
					harqValue[0] = 1;
					harqValue[1] = 1;
					break;
				case 9:
					srIndication = 0;
					harqValue[0] = 1;
					harqValue[1] = 0;
					break;
				case 1:
					srIndication = 1;
					harqValue[0] = 0;
					harqValue[1] = 0;
					break;
				case 4:
					srIndication = 1;
					harqValue[0] = 0;
					harqValue[1] = 1;
					break;
				case 7:
					srIndication = 1;
					harqValue[0] = 1;
					harqValue[1] = 1;
					break;
				case 10:
					srIndication = 1;
					harqValue[0] = 1;
					harqValue[1] = 0;
					break;
				default:
					break;
			}
		}
    }
    else
    {
		switch(mcsValue)
		{
			case 0:
				harqValue[0] = 0;
				harqValue[1] = 0;
				break;
			case 3:
				harqValue[0] = 0;//顺序待确认
				harqValue[1] = 1;
				break;
			case 6:
				harqValue[0] = 1;
				harqValue[1] = 1;
				break;
			case 9:
				harqValue[0] = 1;//顺序待确认
				harqValue[1] = 0;
				break;
			default:
				break;
		}
    }

    if(srExistFlag)
    {
        srInfoFmt01 = &(fapiNrPucchFmt01Indication->srInfoFmt01);
        srInfoFmt01->SRconfidenceLevel = 0xff;
        srInfoFmt01->SRindication      = (0 == isDtx) ? srIndication:0;
    }

    if(harqExistFlag)
    {
        harqInfoFmt01 = &(fapiNrPucchFmt01Indication->harqInfoFmt01);
        harqInfoFmt01->HarqconfidenceLevel  = 0xff;
        harqInfoFmt01->HarqValue[0]         = (0 == isDtx) ? harqValue[0]:2;
        harqInfoFmt01->HarqValue[1]         = (0 == isDtx) ? harqValue[1]:2;
        harqInfoFmt01->NumHarq              = numHarq;
    }
    fapiNrPucchFmt01Indication->UL_CQI = 0;//pucchFmt0Rpt->snr;线性转dB并转换，待算法方案确认后修改
   
}

void pucchFmt1Pack(FapiNrPucchFmt01Indication *fapiNrPucchFmt01Indication, PucchFmt1Rpt  *pucchFmt1Rpt, FapiNrMsgPucchPduInfo *fapiPucchPduInfo)
{
    uint8_t pduBitmap;
    uint8_t numHarq;
    uint8_t srIndication;
    uint8_t harqValue[2];
    uint8_t srExistFlag;
    uint8_t harqExistFlag;
    uint8_t isDtx;
    SRInfoFmt01   *srInfoFmt01   = NULL;
	HARQInfoFmt01 *harqInfoFmt01 = NULL;

    pduBitmap     = (fapiNrPucchFmt01Indication->pduBitmap);
    numHarq       = (fapiPucchPduInfo->bitLenHarq);
    srExistFlag   = pduBitmap&0x1;
    harqExistFlag = (pduBitmap>>1)&0x1;
    
    isDtx = pucchFmt1Rpt->isDtx;
    if(srExistFlag)
    {
        srInfoFmt01 = &(fapiNrPucchFmt01Indication->srInfoFmt01);
        srInfoFmt01->SRconfidenceLevel = 0xff;
        srInfoFmt01->SRindication      = (0 == isDtx) ? 1:0;
    }

    if(harqExistFlag)
    {
        harqValue[0] = pucchFmt1Rpt->uciDecodeValue[0];
        harqValue[1] = pucchFmt1Rpt->uciDecodeValue[1];
        harqInfoFmt01 = &(fapiNrPucchFmt01Indication->harqInfoFmt01);
        harqInfoFmt01->HarqconfidenceLevel  = 0xff;
        harqInfoFmt01->HarqValue[0]         = (0 == isDtx) ? harqValue[0]:2;
        harqInfoFmt01->HarqValue[1]         = (0 == isDtx) ? harqValue[1]:2;
        harqInfoFmt01->NumHarq              = pucchFmt1Rpt->uciBitNum;
    }
    fapiNrPucchFmt01Indication->UL_CQI = 0;//pucchFmt0Rpt->snr;线性转dB并转换，待算法方案确认后修改
}

void pucchFmt01Rpt(uint16_t sfnNum, uint16_t slotNum, uint8_t cellIndex)
{
	uint8_t  uciNum;
	uint8_t  uciNumCnt;
    uint8_t  pucchFmt0Num;
    uint8_t  pucchFmt1Num;
    uint8_t  pduIndex;
    uint8_t  rptIndex;
	PucchFmt01Rst              *pucchFmt01Rst              = NULL;
    FapiNrPucchFmt01Indication *fapiNrPucchFmt01Indication = NULL;
	PucchRpt                   *pucchRpt                   = NULL;
	PucchFmt0Rpt               *pucchFmt0Rpt               = NULL;
    PucchFmt1Rpt               *pucchFmt1Rpt               = NULL;
    FapiNrMsgPucchPduInfo      *fapiPucchPduInfo           = NULL;
	
	pucchFmt01Rst = &g_pucchFmt01Rst[cellIndex][slotNum];
    //pucchRpt    = ;待赋值
    uciNum        = 0;
    pucchFmt0Num  = pucchRpt->pucchFmt0Num;
    pucchFmt1Num  = pucchRpt->pucchFmt1Num;
	for(uciNumCnt = 0; uciNumCnt < pucchFmt0Num; uciNumCnt++)
	{
        pucchFmt0Rpt      = &(pucchRpt->pucchFmt0Rpt[uciNumCnt]);
        pduIndex          = pucchFmt0Rpt->pduIdxInner;
        fapiPucchPduInfo  = &(g_armPucParam[cellIndex][slotNum].FapiPucchPduInfo[pduIndex]);
        rptIndex          = g_pucchFmt01RptIndex[cellIndex][slotNum][pduIndex];
        fapiNrPucchFmt01Indication = &(pucchFmt01Rst->fapiNrPucchFmt01Indication[rptIndex]);
        pucchFmt0Pack(fapiNrPucchFmt01Indication, pucchFmt0Rpt, fapiPucchPduInfo);
        //转换
        SendTlvMsg(&(g_pucchTlvMsg[cellIndex][slotNum][PUCCH_PART1]), g_pucchRptBuffer[cellIndex][slotNum][PUCCH_PART1], (uint8_t *)fapiNrPucchFmt01Indication);
	}

    for(uciNumCnt = 0; uciNumCnt < pucchFmt1Num; uciNumCnt++)
	{
		fapiNrPucchFmt01Indication = &(pucchFmt01Rst->fapiNrPucchFmt01Indication[uciNum++]); 
        pucchFmt1Rpt      = &(pucchRpt->pucchFmt1Rpt[uciNumCnt]);
        pduIndex          = pucchFmt1Rpt->pduIdxInner;
        fapiPucchPduInfo  = &(g_armPucParam[cellIndex][slotNum].FapiPucchPduInfo[pduIndex]);
        rptIndex          = g_pucchFmt01RptIndex[cellIndex][slotNum][pduIndex];
        fapiNrPucchFmt01Indication = &(pucchFmt01Rst->fapiNrPucchFmt01Indication[rptIndex]);
        pucchFmt1Pack(fapiNrPucchFmt01Indication, pucchFmt1Rpt, fapiPucchPduInfo);
        //转换
        SendTlvMsg(&(g_pucchTlvMsg[cellIndex][slotNum][PUCCH_PART1]), g_pucchRptBuffer[cellIndex][slotNum][PUCCH_PART1], (uint8_t *)fapiNrPucchFmt01Indication);
	}

    //发送

}

FsmTable g_pucchTable[] =
{
    //{当前状态S，                                  trigger事件E，                              handler函数H，                           转移后的状态S}
	{Pucch_Uci_Idle_State,                         Pucch_Slot_Tast_Start_Event,                PucchPart1ParaCfgHandler,               Pucch_Wait_Part1_Result_State},//S0{E0,H0}->S1
	
	{Pucch_Wait_Part1_Result_State,                Pucch_Part1_Result_Trigger_Event,           PucchPart1ParsePart2ParaCfgHandler,     Pucch_Wait_Part2_Result_State},//S1{E1,H1}->S2
	
	{Pucch_Wait_Part2_Result_State,                Pucch_Part2_Result_Trigger_Event,           PucchPart2ParseHandler,                 Pucch_Uci_Packing_State},//S2{E2,H2}->S3
	
    {Pucch_Uci_Packing_State,                      Pucch_UCI_Packing_Over_Event,               PucchUciSendHandler,                    Pucch_Uci_Idle_State},//S3{E3,H3}->S0
};