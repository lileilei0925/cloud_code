#pragma once
#include "phyctrl_pusch.h"

L1PuschParaPduInfo  g_puschParaInfoOut[MAX_CELL_NUM] = { 0 };/* Pusch 参数本地buffer */

uint8_t         g_puschUeRbgNum[NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t         g_puschRbgStartIndex[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t         g_puschUeRbsize[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
NrPuschCePara   g_puschUeRbgInfo[NR_PUSCH_MAX_UE_NUM_PER_SLOT][NR_PUSCH_MAX_PRG_NUM] = { 0 }; /* 16个ue，137个Rbg */

uint16_t        g_puschUeReNumPerSymb[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][SYM_NUM_PER_SLOT] = { 0 };
uint8_t         g_puschPortNumPerCdm[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][3] = { 0 }; /* 3表示最多3个CDM */
uint8_t         g_puschUeCdmNum[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t         g_puschUeNumInSymb[MAX_CELL_NUM][SLOT_NUM_PER_FRAME] = { 0 };
uint8_t         g_puschUeStartCdmIdx[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t         g_puschReNumPerDmrsRb[2][3] = {{6,0,0},{8,4,0}}; /* 两种dmrs分配类型，3种cdmNodata值 */
uint16_t        g_puschFreqCompCoeff[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][SYM_NUM_PER_SLOT] = { 0 };
uint8_t         g_puschDmrsSymbNum[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT];
uint8_t         g_puschDmrsSymbIndex[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][NR_PUSCH_MAX_DRMS_NUM] = { 0xFF };
uint8_t         g_puschK0Coeff[NR_BASE_GRAPH_NUM][NR_RVID_NUM] = {{0,17,33,56}, {0,13,25,43}};

/*38.212 Table 5.3.2-1: Sets of LDPC lifting size  */
uint16_t        g_liftSizeTable[8][8] = {{2,  4,  8,  16,  32,  64,  128, 256},
                                         {3,  6,  12, 24,  48,  96,  192, 384},
                                         {5,  10, 20, 40,  80,  160, 320,   0},
                                         {7,  14, 28, 56,  112, 224, 0,     0},
                                         {9,  18, 36, 72,  144, 288, 0,     0},
                                         {11, 22, 44, 88,  176, 352, 0,     0},
                                         {13, 26, 52, 104, 208, 0,   0,     0},
                                         {15, 30, 60, 120, 240, 0,   0,     0}};

uint8_t         g_puschLdpcBgId[2][8] = {{0, 1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};