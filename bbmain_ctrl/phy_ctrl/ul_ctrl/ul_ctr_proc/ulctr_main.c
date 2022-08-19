#include "../prach/src/phyctrl_prach.c"
#include "../pusch/src/phyctrl_pusch.c"
#include "../pucch/src/phyctrl_pucch.c"

uint32_t MessageUlTtiRequestParse(uint8_t cellIndex, uint8_t *srcUlSlotMesagesBuff);
uint32_t UlTtiRequestMessageSizeCalc (uint8_t *srcUlSlotMesagesBuff);
uint32_t UlTtiRequestPrachPduparse(FapiNrMsgPrachPduInfo *fapiPrachPduInfoIn, L1PrachPduInfo *l1PrachPduOut, uint16_t pudIndex);
uint32_t UlTtiRequestPuschPduparse(FapiNrMsgPuschPduInfo *fapiPuschPduInfoIn, L1PuschPduInfo *l1PuschPduOut, uint16_t pudIndex);

#if 1
int main(void)
{
  uint16_t a = 16;
  uint16_t b = 16;
  uint32_t c = 0;

  printf("c = %d;\n",c);
  printf("___Hello World___;\n");

  return 0;
}
#endif


/* UL_TTI.request slot messages parsing */
uint32_t MessageUlTtiRequestParse(uint8_t cellIndex, uint8_t *srcUlSlotMesagesBuff)
{
    FapiNrMsgPrachPduInfo *fapiPrachPduParaIn = NULL;
    FapiNrMsgPuschPduInfo *fapiPuschPduParaIn = NULL;
    FapiNrMsgPucchPduInfo *fapipucchPduParaIn  = NULL;
    FapiNrMsgPucchPduInfo  *fapipucchpduInfo   = NULL;
    L1PrachPduInfo        *l1PrachPduInfo      = NULL;
    L1PuschPduInfo        *l1PuschPduInfo      = NULL;
    ArmToDspPucParam      *armTodspPucParam    = NULL;
    PucParam              *pucParam            = NULL;

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
    uint8_t  pucchpduGroupCnt;
    uint32_t ulTtirequestMessageSize;
    
    if (srcUlSlotMesagesBuff != NULL){
        /******************** UL_TTI.request Slot Message 的总大小计算 ********************/
        ulTtirequestMessageSize = UlTtiRequestMessageSizeCalc (srcUlSlotMesagesBuff);
        memcpy(&g_ulTtiMessageTempBuff[0], srcUlSlotMesagesBuff, ulTtirequestMessageSize); /* Slot Messages Ul_TTI.request信息从共享DDR copy到Arm核内,后期改DMA搬移 */
		
		/* 本小区pucch相关变量初始化 */
        PucchParaInit(cellIndex);
    
        /******************** Slot Messages Ul_TTI.request信息 parsing *******************/
        UlTtiRequestHeadInfo *ulRequestHead = (UlTtiRequestHeadInfo *)g_ulTtiMessageTempBuff;
        sfnNum     = ulRequestHead->sfnNum;
        slotNum    = ulRequestHead->slotNum;
        ulPduNum   = ulRequestHead->pduNum;
        ulPduTypes = ulRequestHead->ulPduTypes;
        ueGroupNum = ulRequestHead->ueGroupNum;
        memcpy(&pduNumPerType[0], &ulRequestHead->pduNumPerType[0], sizeof(uint16_t) * MAX_UL_PDU_TYPES);

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
                    fapiPuschPduParaIn = (FapiNrMsgPuschPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    l1PuschPduInfo     =  &g_puschParaInfoOut[cellIndex].l1PuschPduInfo[pduCntPerType[1]];
                    UlTtiRequestPuschPduparse(fapiPuschPduParaIn, l1PuschPduInfo, pduIndex);
                    pduCntPerType[1]++;
                    break;

                case UL_PDU_TYPE_PUCCH:
                    /* code */
                    fapipucchPduParaIn = (FapiNrMsgPucchPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    if(PUCCH_FORMAT_1 == fapipucchPduParaIn->formatType)
                    {
                        fapipucchpduInfo = g_FapiPucchfmt1PduInfo[cellIndex] + g_pucchfmt1pdunum;
                        memcpy(fapipucchpduInfo, fapipucchPduParaIn, sizeof(FapiNrMsgPucchPduInfo));
                        g_pucchfmt1pdunum++;
                    }
                    else
                    {    
                        pucParam = (g_armtodspPucParam.pucPerCellParam[cellIndex].pucParam + g_pucchfmt023pdunum);
                        UlTtiRequestPucchFmt023Pduparse(fapipucchPduParaIn, pucParam, sfnNum, slotNum, pduIndex, cellIndex);
                        g_pucchfmt023pdunum++;
                    }
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
        
        /* pucch fmt1,将复用的PDU先分组，再解析*/
        if(0 < g_pucchfmt1pdunum)
        {
            PucchFmt1Grouping(cellIndex);
            for(pucchpduGroupCnt = 0; pucchpduGroupCnt < g_pucchpduGroupNum; pucchpduGroupCnt++)
            {
                pucParam = (g_armtodspPucParam.pucPerCellParam[cellIndex].pucParam + g_pucchfmt023pdunum + pucchpduGroupCnt);
                UlTtiRequestPucchFmt1Pduparse(pucParam, pucchpduGroupCnt, sfnNum, slotNum, pduIndex, cellIndex);
            }
        }
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

uint32_t UlTtiRequestPuschPduparse(FapiNrMsgPuschPduInfo *fapiPuschPduInfoIn, L1PuschPduInfo *l1PuschPduOut, uint16_t pudIndex)
{
    uint8_t digitalBfNum, digitalBfIndex, prgIndex;
    uint8_t cbPresentNum, part2Idx, numPart1Params, part1ParaIdx;
    uint8_t           *dataTempPtr0      = NULL;
    uint16_t          *dataTempPtr1      = NULL;
    uint16_t          *beamIndex         = NULL;
    PuschUciInfo      *puschUciInfo      = NULL;
    PuschPtrsInfo     *puschPtrsInfo     = NULL;
    PuschDftOfdmInfo  *puschDftOfdmInfo  = NULL;
    RxBeamformingInfo *rxBeamformingInfo = NULL;
    PuschParaAddInV3  *puschParaAddInV3  = NULL;
    UciInfoAddInV3    *uciInfoAddInV3    = NULL;

    l1PuschPduOut->pduIndex           = pudIndex;
    l1PuschPduOut->pduBitMap          = fapiPuschPduInfoIn->pduBitmap;
    l1PuschPduOut->ueRnti             = fapiPuschPduInfoIn->ueRnti;
    l1PuschPduOut->handle             = fapiPuschPduInfoIn->handle;
    /*BWP*/
    l1PuschPduOut->bwpSize            = fapiPuschPduInfoIn->bwpSize;
    l1PuschPduOut->bwpStart           = fapiPuschPduInfoIn->bwpStart;
    l1PuschPduOut->subCarrierSpacing  = fapiPuschPduInfoIn->subCarrierSpacing;
    l1PuschPduOut->cyclicPrefix       = fapiPuschPduInfoIn->cyclicPrefix;
    /*PUSCH information always included*/
    l1PuschPduOut->targetCodeRate     = fapiPuschPduInfoIn->targetCodeRate;
    l1PuschPduOut->qamModOrder        = fapiPuschPduInfoIn->qamModOrder;
    l1PuschPduOut->mcsIndex           = fapiPuschPduInfoIn->mcsIndex;
    l1PuschPduOut->mcsTable           = fapiPuschPduInfoIn->mcsTable;
    l1PuschPduOut->transformPrecoding = fapiPuschPduInfoIn->transformPrecoding;
    l1PuschPduOut->nIdPusch           = fapiPuschPduInfoIn->nIdPusch;
    l1PuschPduOut->nrOfLayers         = fapiPuschPduInfoIn->nrOfLayers;
    /*DMRS*/
    l1PuschPduOut->ulDmrsSymbPos      = fapiPuschPduInfoIn->ulDmrsSymbPos;
    l1PuschPduOut->dmrsCfgType        = fapiPuschPduInfoIn->dmrsCfgType;
    l1PuschPduOut->dmrsScrambleId     = fapiPuschPduInfoIn->dmrsScrambleId;
    l1PuschPduOut->puschDmrsId        = fapiPuschPduInfoIn->puschDmrsId;
    l1PuschPduOut->nSCID              = fapiPuschPduInfoIn->nSCID;
    l1PuschPduOut->numCdmGrpsNoData   = fapiPuschPduInfoIn->numDmrsCdmGrpsNoData;
    l1PuschPduOut->dmrsPort           = fapiPuschPduInfoIn->dmrsPort;
    /*Pusch Allocation in frequency domain*/
    l1PuschPduOut->resourceAlloc      = fapiPuschPduInfoIn->resourceAlloc;
    memcpy(&l1PuschPduOut->rbBitmap[0], &fapiPuschPduInfoIn->rbBitmap[0], sizeof(uint8_t)*36);
    l1PuschPduOut->rbStart            = fapiPuschPduInfoIn->rbStart;
    l1PuschPduOut->rbSize             = fapiPuschPduInfoIn->rbSize;
    l1PuschPduOut->vrbToPrbMapping    = fapiPuschPduInfoIn->vrbToPrbMapping;
    l1PuschPduOut->intraSlotHopping   = fapiPuschPduInfoIn->intraSlotFreqhopping;
    l1PuschPduOut->txDCLocation       = fapiPuschPduInfoIn->txDCLocation;
    l1PuschPduOut->ulFreqShift7p5Khz  = fapiPuschPduInfoIn->ulFreqShift7p5Khz;
    /*Resource Allocation in time domain*/
    l1PuschPduOut->startSymbIndex     = fapiPuschPduInfoIn->startSymbIndex;
    l1PuschPduOut->nrOfSymbols        = fapiPuschPduInfoIn->nrOfSymbols;
    
    puschUciInfo = (PuschUciInfo *)((uint8_t *)&fapiPuschPduInfoIn->nrOfSymbols + 1);
    memset(&l1PuschPduOut->puschDataPara.rvIndex, 0, sizeof(PuschDataPara));        /* 清零 puschData 参数 */
    if(((l1PuschPduOut->pduBitMap >> 0) & 0x0001) == 1){
        l1PuschPduOut->puschDataPara.rvIndex       = fapiPuschPduInfoIn->puschDataInfo.rvIndex;
        l1PuschPduOut->puschDataPara.harqProcessId = fapiPuschPduInfoIn->puschDataInfo.harqProcessId;
        l1PuschPduOut->puschDataPara.newData       = fapiPuschPduInfoIn->puschDataInfo.newData;
        l1PuschPduOut->puschDataPara.numCb         = fapiPuschPduInfoIn->puschDataInfo.numCb;
        cbPresentNum = ceil_div(l1PuschPduOut->puschDataPara.numCb, 8);
        memcpy(&l1PuschPduOut->puschDataPara.cbPresentAndPos[0], &fapiPuschPduInfoIn->puschDataInfo.cbPresentAndPose[0], sizeof(uint8_t)*cbPresentNum);
        puschUciInfo = (PuschUciInfo *)((uint8_t *)&fapiPuschPduInfoIn->puschDataInfo.cbPresentAndPose[0] + sizeof(uint8_t)*cbPresentNum);
    }

    puschPtrsInfo = (PuschPtrsInfo *)puschUciInfo;
    memset(&l1PuschPduOut->puschUciPara.harqAckBitLength, 0, sizeof(PuschUciPara)); /* 清零 puschUci 参数 */
    if(((l1PuschPduOut->pduBitMap >> 1) & 0x0001) == 1){
        l1PuschPduOut->puschUciPara.harqAckBitLength  = puschUciInfo->harqAckBitLength;
        l1PuschPduOut->puschUciPara.csiPart1BitLength = puschUciInfo->csiPart1BitLength;
        l1PuschPduOut->puschUciPara.flagCsiPart2      = puschUciInfo->flagCsiPart2;
        l1PuschPduOut->puschUciPara.alphaScaling      = puschUciInfo->alphaScaling;
        l1PuschPduOut->puschUciPara.betaOffsetHarqAck = puschUciInfo->betaOffsetHarqAck;
        l1PuschPduOut->puschUciPara.betaOffsetCsi1    = puschUciInfo->betaOffsetCsi1;
        l1PuschPduOut->puschUciPara.betaOffsetCsi2    = puschUciInfo->betaOffsetCsi2;
        puschPtrsInfo = (PuschPtrsInfo *)((uint8_t *)puschUciInfo + sizeof(PuschUciInfo));
    }
    
    puschDftOfdmInfo = (PuschDftOfdmInfo *)puschPtrsInfo;
    memset(&l1PuschPduOut->puschPtrsPara.numPtrsPorts, 0, sizeof(PuschPtrsPara)); /* 清零 puschPtrs 参数 */
    if(((l1PuschPduOut->pduBitMap >> 2) & 0x0001) == 1){
        l1PuschPduOut->puschPtrsPara.numPtrsPorts    = puschPtrsInfo->numPtrsPorts;
        memcpy(&l1PuschPduOut->puschPtrsPara.ptrsPortInfo[0], &puschPtrsInfo->ptrsPortInfo[0], sizeof(PtrsPortPara)*puschPtrsInfo->numPtrsPorts);
        puschPtrsInfo = (PuschPtrsInfo *) ((uint8_t *)puschPtrsInfo + sizeof(PtrsPortPara)*puschPtrsInfo->numPtrsPorts);
        l1PuschPduOut->puschPtrsPara.ptrsTimeDensity = puschPtrsInfo->ptrsTimeDensity;
        l1PuschPduOut->puschPtrsPara.ptrsFreqDensity = puschPtrsInfo->ptrsFreqDensity;
        l1PuschPduOut->puschPtrsPara.ulPtrsPower     = puschPtrsInfo->ulPtrsPower;
        puschDftOfdmInfo = (PuschDftOfdmInfo *)((uint8_t *)&puschPtrsInfo->ulPtrsPower + 1);
    }

    rxBeamformingInfo = (RxBeamformingInfo *)puschDftOfdmInfo;
    memset(&l1PuschPduOut->puschDftOfdmPara.lowPaprGrpNum, 0, sizeof(PuschDftOfdmPara)); /* 清零 puschPtrs 参数 */
    if(((l1PuschPduOut->pduBitMap >> 3) & 0x0001) == 1)
    {
        l1PuschPduOut->puschDftOfdmPara.lowPaprGrpNum = puschDftOfdmInfo->lowPaprGrpNum;
        l1PuschPduOut->puschDftOfdmPara.lowPaprSeqNum = puschDftOfdmInfo->lowPaprSeqNum;
        l1PuschPduOut->puschDftOfdmPara.ulptrsSampleDensity = puschDftOfdmInfo->ulptrsSampleDensity;
        l1PuschPduOut->puschDftOfdmPara.ulptrsTimeDensity = puschDftOfdmInfo->ulptrsTimeDensity;
        rxBeamformingInfo = (RxBeamformingInfo *)((uint8_t *)&puschDftOfdmInfo->ulptrsTimeDensity + 1);
    }

    l1PuschPduOut->trpScheme          = rxBeamformingInfo->trpScheme;
    l1PuschPduOut->prgNum             = rxBeamformingInfo->numPRGs;
    l1PuschPduOut->prgSize            = rxBeamformingInfo->prgSize;
    l1PuschPduOut->digitalBfNum       = rxBeamformingInfo->digBfInterface;
    digitalBfNum = l1PuschPduOut->digitalBfNum;
    for (prgIndex = 0; prgIndex < l1PuschPduOut->prgNum; prgIndex++){
        beamIndex = (uint16_t *)(&rxBeamformingInfo->beamIndex[0]) + prgIndex * digitalBfNum;
        for (digitalBfIndex = 0; digitalBfIndex < digitalBfNum; digitalBfIndex++){
            l1PuschPduOut->beamIndex[prgIndex][digitalBfIndex] = *beamIndex;
            beamIndex++;
        }
    }

    puschParaAddInV3 = (PuschParaAddInV3 *)((uint8_t *)&rxBeamformingInfo->beamIndex[0] + sizeof(uint16_t)*prgIndex*digitalBfNum);
    l1PuschPduOut->puschTransType         = puschParaAddInV3->puschTransType;
    l1PuschPduOut->deltabwp0fromActiveBwp = puschParaAddInV3->deltabwp0fromActiveBwp;
    l1PuschPduOut->initialUlBwpSize       = puschParaAddInV3->initialUlBwpSize;
    l1PuschPduOut->groupOrSequenceHopping = puschParaAddInV3->groupOrSequenceHopping;
    l1PuschPduOut->puschSecondHopPRB      = puschParaAddInV3->puschSecondHopPRB;
    l1PuschPduOut->ldpcBaseGraph          = puschParaAddInV3->ldpcBaseGraph;
    l1PuschPduOut->tbSizeLbrmBytes        = puschParaAddInV3->tbSizeLbrmBytes;

    uciInfoAddInV3 = (UciInfoAddInV3 *)((uint8_t *)&puschParaAddInV3->tbSizeLbrmBytes + sizeof(uint32_t)); 
    memset(&l1PuschPduOut->part2InfoAddInV3.numPart2s, 0, sizeof(Part2InfoAddInV3)); /* 清零 Part2InfoAddInV3 参数 */
    if(((l1PuschPduOut->pduBitMap >> 1) & 0x0001) == 1)
    {
        l1PuschPduOut->part2InfoAddInV3.numPart2s = uciInfoAddInV3->numPart2s;
        for(part2Idx = 0; part2Idx < uciInfoAddInV3->numPart2s; part2Idx++)
        {
            l1PuschPduOut->part2InfoAddInV3.part2ReportPara[part2Idx].priority       = uciInfoAddInV3->part2ReportInfo[part2Idx].priority;
            numPart1Params  = uciInfoAddInV3->part2ReportInfo[part2Idx].numPart1Params;
            l1PuschPduOut->part2InfoAddInV3.part2ReportPara[part2Idx].numPart1Params = numPart1Params;
            memcpy(&l1PuschPduOut->part2InfoAddInV3.part2ReportPara[part2Idx].paramOffsets[0], &uciInfoAddInV3->part2ReportInfo[part2Idx].paramOffsets[0], sizeof(uint16_t)*numPart1Params);
            dataTempPtr0 = (uint8_t *)&uciInfoAddInV3->part2ReportInfo[part2Idx].paramOffsets[0] + sizeof(uint16_t)*numPart1Params;
            memcpy(&l1PuschPduOut->part2InfoAddInV3.part2ReportPara[part2Idx].paramSizes[0], dataTempPtr0, sizeof(uint8_t)*numPart1Params);
            dataTempPtr1 = (uint16_t *)(dataTempPtr0 + sizeof(uint8_t)*numPart1Params);
            l1PuschPduOut->part2InfoAddInV3.part2ReportPara[part2Idx].part2SizeMapIndex = *dataTempPtr1;
        }
    }

    return 0;
}
