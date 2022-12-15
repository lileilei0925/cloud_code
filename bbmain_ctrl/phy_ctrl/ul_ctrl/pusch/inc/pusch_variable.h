#pragma once
#include "phyctrl_pusch.h"
#include "../src/phyctrl_pusch.c"

L1PuschParaPduInfo  g_puschParaInfoOut[MAX_CELL_NUM][SLOT_NUM_PER_FRAME] = { 0 };/* Pusch 参数本地buffer */
NrPuschCePara       g_puschUeRbgInfo[NR_PUSCH_MAX_UE_NUM_PER_SLOT][NR_PUSCH_MAX_PRG_NUM] = { 0 }; /* 16个ue，137个Rbg */
PuschResourceInfo   g_puschResourceInfo[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][MAX_PUSCH_PDU_NUM];//放到DDR

uint8_t g_puschCsiPart2Flag[MAX_CELL_NUM][SLOT_NUM_PER_FRAME] = { 0 };

/* PUSCH UCI译码参数 */
PuschPolarDecodeHacCfgPara  g_puschPolarDecodeHacCfgParaDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][Pusch_Uci_Type_Num];//放到DDR
PuschRMDecodeHacCfgPara     g_puschRMDecodeHacCfgParaDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][Pusch_Uci_Type_Num];//放到DDR

PuschPolarDecodeHacCfgPara  g_puschPolarDecodeHacCfgParal2[MAX_CELL_NUM][PUSCH_UCI_PING_PONG_NUM][Pusch_Uci_Type_Num];//放到L2
PuschRMDecodeHacCfgPara     g_puschRMDecodeHacCfgParaL2[MAX_CELL_NUM][PUSCH_UCI_PING_PONG_NUM][Pusch_Uci_Type_Num];//放到L2
/* PUSCH UCI译码输入数据 */
//PUSCH Demap输出规划和如何摆放？待规划

/* PUSCH UCI译码输出 */
PolarDecodeOut       g_puschPolarDecodeOutDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][Pusch_Uci_Type_Num];//放到DDR
RMDecodeOut          g_puschRMDecodeOutDDR[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][Pusch_Uci_Type_Num];//放到DDR

PolarDecodeOut       g_puschPolarDecodeOutL2[MAX_CELL_NUM][PUSCH_UCI_PING_PONG_NUM][Pusch_Uci_Type_Num];//放到L2
RMDecodeOut          g_puschRMDecodeOutL2[MAX_CELL_NUM][PUSCH_UCI_PING_PONG_NUM][Pusch_Uci_Type_Num];//放到L2

CsiPart2ParaInfo    g_CsiPart2ParaInfo[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
PuschAckAndCsiInfo  g_puschAckAndCsiInfo[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][SYM_NUM_PER_SLOT] = { 0 };

FSM g_puschUciFSM[MAX_CELL_NUM][PUSCH_UCI_PING_PONG_NUM];
/* PUSCH UCI译码组包结果 */
PuschUciRst     g_puschUciRst[MAX_CELL_NUM][SLOT_NUM_PER_FRAME];//放到DDR

uint8_t  g_puschUeRbgNum[NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t  g_puschRbgStartIndex[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t  g_puschUeRbsize[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint16_t g_puschUeReNumPerSymb[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][SYM_NUM_PER_SLOT] = { 0 };
uint8_t  g_puschPortNumPerCdm[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][3] = { 0 }; /* 3表示最多3个CDM */
uint8_t  g_puschUeCdmNum[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t  g_puschUeNumInSymb[MAX_CELL_NUM][SLOT_NUM_PER_FRAME] = { 0 };
uint8_t  g_puschUeStartCdmIdx[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 };
uint8_t  g_puschReNumPerDmrsRb[2][3] = {{6,0,0},{8,4,0}}; /* 两种dmrs分配类型，3种cdmNodata值 */
uint16_t g_puschFreqCompCoeff[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][SYM_NUM_PER_SLOT] = { 0 };
uint8_t  g_puschDmrsSymbNum[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT];
uint8_t  g_puschDmrsSymbIndex[MAX_CELL_NUM][SLOT_NUM_PER_FRAME][NR_PUSCH_MAX_UE_NUM_PER_SLOT][NR_PUSCH_MAX_DRMS_NUM] = { 0xFF };
uint8_t  g_puschK0Coeff[NR_BASE_GRAPH_NUM][NR_RVID_NUM] = {{0,17,33,56}, {0,13,25,43}};
uint32_t g_UePart2AndDataAddr[MAX_CELL_NUM][NR_PUSCH_MAX_UE_NUM_PER_SLOT] = { 0 }; /* 暂定，临时使用 */

/*38.212 Table 5.3.2-1: Sets of LDPC lifting size  */
uint16_t g_liftSizeTable[8][8] = {{2,  4,  8,  16,  32,  64,  128, 256},
                                  {3,  6,  12, 24,  48,  96,  192, 384},
                                  {5,  10, 20, 40,  80,  160, 320,   0},
                                  {7,  14, 28, 56,  112, 224, 0,     0},
                                  {9,  18, 36, 72,  144, 288, 0,     0},
                                  {11, 22, 44, 88,  176, 352, 0,     0},
                                  {13, 26, 52, 104, 208, 0,   0,     0},
                                  {15, 30, 60, 120, 240, 0,   0,     0}};

uint8_t g_puschLdpcBgId[2][8] = {{0, 1, 2, 3, 4, 5, 6, 7}, {8, 9, 10, 11, 12, 13, 14, 15}};

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