#if 1
#include <stdio.h>
#include <string.h>
#include "../prach/src/phyctrl_prach.c"
#include "../pucch/src/phyctrl_pucch.c"

uint32_t MessageUlTtiRequestParse(uint8_t cellIndex, uint8_t *srcUlSlotMesagesBuff);

/* UL_TTI.request slot messages parsing */
uint32_t MessageUlTtiRequestParse(uint8_t cellIndex, uint8_t *srcUlSlotMesagesBuff)
{
    FapiNrMsgPrachPduInfo *fapiprachPduParaIn  = NULL;
    FapiNrMsgPucchPduInfo *fapipucchPduParaIn  = NULL;
    FapiNrMsgPucchPduInfo  *fapipucchpduInfo   = NULL;
    L1PrachPduInfo        *l1prachPduInfo      = NULL;
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
    
    if (srcUlSlotMesagesBuff != NULL)
    {
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
        memcpy(&pduNumPerType[0], ulRequestHead->pduNumPerType, sizeof(uint16_t) * MAX_UL_PDU_TYPES);
      
        L1PrachParaPduInfo  *l1prachParaPduInfoOut = &g_prachParaInfoOut[cellIndex];
        l1prachParaPduInfoOut->sfnNum  = sfnNum;
        l1prachParaPduInfoOut->slotNum = slotNum;

        PduHeadInfo *pduHead = (PduHeadInfo *)((uint8_t *)&g_ulTtiMessageTempBuff[0] + sizeof(UlTtiRequestHeadInfo));
        for (pduIndex = 0; pduIndex < ulPduNum; pduIndex++)
        {   
            pduType = pduHead->pduType;
            switch (pduType)
            {
                case UL_PDU_TYPE_PRACH:
                    fapiprachPduParaIn = (FapiNrMsgPrachPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    l1prachPduInfo     =  &g_prachParaInfoOut[cellIndex].l1PrachPduInfo[pduCntPerType[0]];
                    UlTtiRequestPrachPduparse (fapiprachPduParaIn, l1prachPduInfo, pduIndex);
                    pduCntPerType[0]++;
                    break;

                case UL_PDU_TYPE_PUSCH:
                    /* code */
                    pduCntPerType[1]++;
                    break;

                case UL_PDU_TYPE_PUCCH:
                    /* code */
                    fapipucchPduParaIn = (FapiNrMsgPucchPduInfo *)((uint8_t *)pduHead + sizeof(PduHeadInfo));
                    if(PUCCH_FORMAT_1 == fapipucchPduParaIn->formatType)
                    {
                        fapipucchpduInfo = g_FapiPucchPduInfo[cellIndex] + g_pucchfmt1pdunum[cellIndex];
                        memcpy(fapipucchpduInfo, fapipucchPduParaIn, sizeof(FapiNrMsgPucchPduInfo));
                        g_pucchfmt1pdunum[cellIndex]++;
                    }
                    else
                    {    
                        pucParam = (g_armtodspPucParam.pucPerCellParam[cellIndex].pucParam + g_pucchfmt023pdunum[cellIndex]);
                        UlTtiRequestPucchFmt023Pduparse(fapipucchPduParaIn, pucParam, cellIndex);
                        g_pucchfmt023pdunum[cellIndex]++;
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
        if(0 < g_pucchfmt1pdunum[cellIndex])
        {
            PucchFmt1Grouping(cellIndex);
            for(pucchpduGroupCnt = 0; pucchpduGroupCnt < g_pucchpduGroupNum[cellIndex]; pucchpduGroupCnt++)
            {
                pucParam = (g_armtodspPucParam.pucPerCellParam[cellIndex].pucParam + g_pucchfmt023pdunum[cellIndex] + pucchpduGroupCnt);
                UlTtiRequestPucchFmt1Pduparse(pucParam, pucchpduGroupCnt, cellIndex);
            }
        }
        /************** pduIndex mapping relation with UE **************/
        UlueGoupNumInfo  *ulUeGoupNumInfo = (UlueGoupNumInfo *)((uint8_t *)pduHead); /* sizeof(uint8_t) * (ulUeGoupNumInfo->ueNum + 1) per Group */
        UlPduMappingInfo *ulPduMappingInfo = &g_ulPduMappingInfo[cellIndex][0];
        for (groupIndex = 0; groupIndex < ueGroupNum; groupIndex++)
        {
            ueNumInGroup = ulUeGoupNumInfo->ueNum;
            for (ueIndex = 0; ueIndex < ueNumInGroup; ueIndex++)
            {
                pduIndex = ulUeGoupNumInfo->pduIdx[ueIndex];
                ulPduMappingInfo[pduIndex].ueIndex = ueIndex;
                ulPduMappingInfo[pduIndex].groupIndex = groupIndex;
            }
            ulUeGoupNumInfo = ulUeGoupNumInfo +  (sizeof(uint8_t) * (ulUeGoupNumInfo->ueNum + 1));
        }
    }
    return 0;
}
#endif