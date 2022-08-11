#include <stdio.h>
#include <string.h>
#include "../../../../common/inc/fapi_mac2phy_interface.h"
#include "../inc/phyctrl_pusch.h"
//#include "../inc/prach_variable.h"

int main(void)
{
  uint16_t a = 16;
  uint16_t b = 16;
  uint16_t c = 0;

  printf("c = %d;\n",c);
  printf("___Hello World___;\n");

  return 0;
}

#if 0
uint32_t L1PuschRxParasInit(L1PuschParaPduInfo *l1puschParaPduInfoIn, L1PuschConfigInfo *l1PuschConfigInfoIn, PuschRxParaLocal *puschRxParaLocal)
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

#endif

